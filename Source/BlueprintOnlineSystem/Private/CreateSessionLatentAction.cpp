// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateSessionLatentAction.h"

#include "LogOnlineSystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

FCreateSessionLatentAction::FCreateSessionLatentAction(
    const FLatentActionInfo& InLatentInfo,
    const APlayerController& InPlayerController, int32 MaxConnections,
    FString& OutSessionId, ECreateSessionResult& OutResult)
    : OnCreateSessionCompleteDelegate(
          FOnCreateSessionCompleteDelegate::CreateRaw(
              this, &FCreateSessionLatentAction::OnCreateSessionComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget), SessionId(OutSessionId),
      Result(OutResult) {
	// get session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// make session settings
	FOnlineSessionSettings OnlineSessionSettings;
	OnlineSessionSettings.bIsLANMatch =
	    OnlineSubsystem->GetSubsystemName() == "NULL" ? true : false;
	OnlineSessionSettings.NumPublicConnections   = MaxConnections;
	OnlineSessionSettings.bAllowJoinInProgress   = true;
	OnlineSessionSettings.bAllowJoinViaPresence  = true;
	OnlineSessionSettings.bShouldAdvertise       = true;
	OnlineSessionSettings.bUsesPresence          = true;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;

	// get local player
	const auto LocalPlayer = InPlayerController.GetLocalPlayer();
	check(LocalPlayer != nullptr);

	// get index of player controller
	const auto PlayerIndex = LocalPlayer->GetLocalPlayerIndex();

	// set callback
	OnCreateSessionCompleteDelegateHandle =
	    OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
	        OnCreateSessionCompleteDelegate);

	UE_LOG(LogOnlineSystem, Log, TEXT("Try to create session"));

	// change state to running
	IsRunning = true;

	// start creating session
	const bool bCreateSessionSuccessfullyStarted =
	    OnlineSessionInterface->CreateSession(PlayerIndex, NAME_GameSession,
	                                          OnlineSessionSettings);

	// if failed to start creating session
	if (!bCreateSessionSuccessfullyStarted) {
		// log
		UE_LOG(LogOnlineSystem, Error, TEXT("Failed to start creating session"));

		// unbind callback
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
		    OnCreateSessionCompleteDelegateHandle);
		OnCreateSessionCompleteDelegateHandle.Reset();

		// finish
		Finish(ECreateSessionResult::Failure);
		return;
	}
}

void FCreateSessionLatentAction::UpdateOperation(FLatentResponse& Response) {
	Response.FinishAndTriggerIf(IsRunning == false, ExecutionFunction, OutputLink,
	                            CallbackTarget);
}

void FCreateSessionLatentAction::Finish(const ECreateSessionResult& InResult) {
	Result    = InResult;
	IsRunning = false;
}

void FCreateSessionLatentAction::OnCreateSessionComplete(FName SessionName,
                                                         bool  bWasSuccessful) {
	// get online session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// unbind callback
	OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
	    OnCreateSessionCompleteDelegateHandle);
	OnCreateSessionCompleteDelegateHandle.Reset();

	// if failure
	if (!bWasSuccessful) {
		Finish(ECreateSessionResult::Failure);
		return;
	}

	// set SessionId
	const auto GameSession =
	    OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	check(GameSession != nullptr);
	SessionId = GameSession->GetSessionIdStr();

	// finish
	Finish(ECreateSessionResult::Success);
}
