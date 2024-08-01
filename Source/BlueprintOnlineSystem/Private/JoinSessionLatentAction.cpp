// Fill out your copyright notice in the Description page of Project Settings.

#include "JoinSessionLatentAction.h"

#include "LogOnlineSystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

FJoinSessionLatentAction::FJoinSessionLatentAction(
    const FLatentActionInfo&             InLatentInfo,
    const APlayerController&             InPlayerController,
    const FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
    FString& OutSessionConnectInfo, FString& OutBeaconConnectInfo,
    EJoinSessionResult& OutResult)
    : OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateRaw(
          this, &FJoinSessionLatentAction::OnJoinSessionComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget),
      OnlineSessionSearchResult(
          BlueprintSessionSearchResult.OnlineSessionSearchResult),
      SessionConnectInfo(OutSessionConnectInfo),
      BeaconConnectInfo(OutBeaconConnectInfo), Result(OutResult) {
	// get World
	const auto World = InPlayerController.GetWorld();
	check(World != nullptr);

	// get session interface
	const auto OnlineSubsystem = Online::GetSubsystem(World);
	check(OnlineSubsystem != nullptr);

	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// get local player
	const auto LocalPlayer = InPlayerController.GetLocalPlayer();
	check(LocalPlayer != nullptr);

	// get index of player controller
	const auto PlayerIndex = LocalPlayer->GetLocalPlayerIndex();

	// set callback
	OnJoinSessionCompleteDelegateHandle =
	    OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
	        OnJoinSessionCompleteDelegate);

	// log
	UE_LOG(LogOnlineSystem, Log, TEXT("Try to join session"));

	// change state to running
	IsRunning = true;

	// start joining session
	const bool bJoinSessionSuccessfullyStarted =
	    OnlineSessionInterface->JoinSession(PlayerIndex, NAME_GameSession,
	                                        OnlineSessionSearchResult);

	// if failed to start joining session
	if (!bJoinSessionSuccessfullyStarted) {
		// log
		UE_LOG(LogOnlineSystem, Error, TEXT("Failed to start joining session"));

		// unbind callback
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
		    OnJoinSessionCompleteDelegateHandle);
		OnJoinSessionCompleteDelegateHandle.Reset();

		// finish
		Finish(EJoinSessionResult::UnknownError);
		return;
	}
}

void FJoinSessionLatentAction::UpdateOperation(FLatentResponse& Response) {
	Response.FinishAndTriggerIf(IsRunning == false, ExecutionFunction, OutputLink,
	                            CallbackTarget);
}

void FJoinSessionLatentAction::Finish(const EJoinSessionResult& InResult) {
	Result    = InResult;
	IsRunning = false;
}

void FJoinSessionLatentAction::OnJoinSessionComplete(
    FName                              SessionName,
    EOnJoinSessionCompleteResult::Type OnJoinSessionCompleteResult) {
	// unbind callback
	OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
	    OnJoinSessionCompleteDelegateHandle);
	OnJoinSessionCompleteDelegateHandle.Reset();

	// when failed to join session
	if (OnJoinSessionCompleteResult != EOnJoinSessionCompleteResult::Success) {
		// finish
		Finish(EOnJoinSessionCompleteResult::SessionIsFull ==
		               OnJoinSessionCompleteResult
		           ? EJoinSessionResult::SessionIsFull
		       : EOnJoinSessionCompleteResult::SessionDoesNotExist ==
		               OnJoinSessionCompleteResult
		           ? EJoinSessionResult::SessionDoesNotExist
		       : EOnJoinSessionCompleteResult::CouldNotRetrieveAddress ==
		               OnJoinSessionCompleteResult
		           ? EJoinSessionResult::CouldNotRetrieveAddress
		       : EOnJoinSessionCompleteResult::AlreadyInSession ==
		               OnJoinSessionCompleteResult
		           ? EJoinSessionResult::AlreadyInSession
		           : EJoinSessionResult::UnknownError);
		return;
	}

	// get connect string of session
	const bool bSuccessfullyGetConnectInfoOfSession =
	    OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession,
	                                                     SessionConnectInfo);

	// if failed to get connect info of session
	if (!bSuccessfullyGetConnectInfoOfSession) {
		// finish
		Finish(EJoinSessionResult::ErrorInGetResolvedConnectStringOfSession);
		return;
	}

	// log connect string of session
	UE_LOG(LogOnlineSystem, Log,
	       TEXT("JoinSession success. Connect Info of session: %s"),
	       *SessionConnectInfo);

	// get connect string of beacon
	const bool bSuccessfullyGetConnectInfoOfBeacon =
	    OnlineSessionInterface->GetResolvedConnectString(
	        OnlineSessionSearchResult, NAME_BeaconPort, BeaconConnectInfo);

	// if success to get connect info of beacon
	if (bSuccessfullyGetConnectInfoOfBeacon) {
		// log connect string of beacon
		UE_LOG(LogOnlineSystem, Log, TEXT("Connect Info of beacon: %s"),
		       *BeaconConnectInfo);
	}
	// if failed to get connect info of beacon
	else {
		// warn that you failed to get connect info of beacon
		UE_LOG(LogOnlineSystem, Warning,
		       TEXT("Failed to get connect info of beacon."));

		// set BeaconConnectInfo string to empty string
		BeaconConnectInfo = "";
	}

	// finish
	Finish(EJoinSessionResult::Success);
}
