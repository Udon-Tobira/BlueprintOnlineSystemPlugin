// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintOnlineSystem.h"

#include "LogOnlineSystem.h"
#include "OnlineSubsystemUtils.h"

#define LOCTEXT_NAMESPACE "FBlueprintOnlineSystemModule"

void FBlueprintOnlineSystemModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact
	// timing is specified in the .uplugin file per-module
}

void FBlueprintOnlineSystemModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For
	// modules that support dynamic reloading, we call this function before
	// unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintOnlineSystemModule, BlueprintOnlineSystem)

void UBlueprintOnlineSystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);
}

void UBlueprintOnlineSystem::Deinitialize() {
	[this]() {
		// get World
		const auto World = this->GetWorld();

		// if World is nullptr
		if (nullptr == World) {
			// finish
			return;
		}

		// get online subsystem
		const auto OnlineSubsystem = Online::GetSubsystem(World);

		// if OnlineSubsystem is nullptr
		if (nullptr == OnlineSubsystem) {
			// finish
			return;
		}

		// get session interface
		const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		// if OnlineSessionInterface is nullptr
		if (nullptr == OnlineSessionInterface) {
			// finish
			return;
		}

		// if already destroyed
		if (!OnlineSessionInterface->GetNamedSession(NAME_GameSession)) {
			// finish
			return;
		}

		// log
		UE_LOG(LogOnlineSystem, Log, TEXT("Try to destroy session"));

		// start destroying session
		// fire and forget
		const bool bDestroySessionSuccessfullyStarted =
		    OnlineSessionInterface->DestroySession(NAME_GameSession);

		// if failed to start destroying session
		if (!bDestroySessionSuccessfullyStarted) {
			// log
			UE_LOG(LogOnlineSystem, Error,
			       TEXT("Failed to start destroying session"));

			// finish
			return;
		}
	}();

	Super::Deinitialize();
}

FName UBlueprintOnlineSystem::GetOnlineSubsystemName() const {
	// get World
	const auto World = this->GetWorld();
	check(World != nullptr);

	// get online subsystem
	const auto OnlineSubsystem = Online::GetSubsystem(World);
	check(OnlineSubsystem != nullptr);

	return OnlineSubsystem->GetSubsystemName();
}

void UBlueprintOnlineSystem::Login(const UObject*           WorldContextObject,
                                   FLatentActionInfo        LatentActionInfo,
                                   const APlayerController* PlayerController,
                                   const FString&           AuthType,
                                   ELoginResult&            LoginResult) {
	check(WorldContextObject != nullptr);
	check(PlayerController != nullptr);

	const auto World = GEngine->GetWorldFromContextObject(
	    WorldContextObject, EGetWorldErrorMode::Assert);
	check(World != nullptr);

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	LatentActionManager.AddNewAction(
	    LatentActionInfo.CallbackTarget, LatentActionInfo.UUID,
	    new FLoginLatentAction(LatentActionInfo, *PlayerController, AuthType,
	                           LoginResult));
}

void UBlueprintOnlineSystem::CreateSession(
    const UObject* WorldContextObject, FLatentActionInfo LatentActionInfo,
    const APlayerController* PlayerController, int32 MaxConnections,
    FString& SessionId, ECreateSessionResult& CreateSessionResult) {
	check(WorldContextObject != nullptr);
	check(PlayerController != nullptr);

	const auto World = GEngine->GetWorldFromContextObject(
	    WorldContextObject, EGetWorldErrorMode::Assert);
	check(World != nullptr);

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	LatentActionManager.AddNewAction(
	    LatentActionInfo.CallbackTarget, LatentActionInfo.UUID,
	    new FCreateSessionLatentAction(LatentActionInfo, *PlayerController,
	                                   MaxConnections, SessionId,
	                                   CreateSessionResult));
}

void UBlueprintOnlineSystem::DestroySession(
    const UObject* WorldContextObject, FLatentActionInfo LatentActionInfo,
    EDestroySessionResult& DestroySessionResult) {
	check(WorldContextObject != nullptr);

	const auto World = GEngine->GetWorldFromContextObject(
	    WorldContextObject, EGetWorldErrorMode::Assert);
	check(World != nullptr);

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	LatentActionManager.AddNewAction(
	    LatentActionInfo.CallbackTarget, LatentActionInfo.UUID,
	    new FDestroySessionLatentAction(*WorldContextObject, LatentActionInfo,
	                                    DestroySessionResult));
}

void UBlueprintOnlineSystem::FindSession(
    const UObject* WorldContextObject, FLatentActionInfo LatentActionInfo,
    const APlayerController* PlayerController, const FString& SessionId,
    FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
    EFindSessionResult&            FindSessionResult) {
	check(WorldContextObject != nullptr);
	check(PlayerController != nullptr);

	const auto World = GEngine->GetWorldFromContextObject(
	    WorldContextObject, EGetWorldErrorMode::Assert);
	check(World != nullptr);

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	LatentActionManager.AddNewAction(
	    LatentActionInfo.CallbackTarget, LatentActionInfo.UUID,
	    new FFindSessionLatentAction(LatentActionInfo, *PlayerController,
	                                 SessionId, BlueprintSessionSearchResult,
	                                 FindSessionResult));
}

void UBlueprintOnlineSystem::JoinSession(
    const UObject* WorldContextObject, FLatentActionInfo LatentActionInfo,
    const APlayerController*             PlayerController,
    const FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
    FString& SessionConnectInfo, FString& BeaconConnectInfo,
    EJoinSessionResult& JoinSessionResult) {
	check(WorldContextObject != nullptr);
	check(PlayerController != nullptr);

	const auto World = GEngine->GetWorldFromContextObject(
	    WorldContextObject, EGetWorldErrorMode::Assert);
	check(World != nullptr);

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	LatentActionManager.AddNewAction(
	    LatentActionInfo.CallbackTarget, LatentActionInfo.UUID,
	    new FJoinSessionLatentAction(
	        LatentActionInfo, *PlayerController, BlueprintSessionSearchResult,
	        SessionConnectInfo, BeaconConnectInfo, JoinSessionResult));
}
