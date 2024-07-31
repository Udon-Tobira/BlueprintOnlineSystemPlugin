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
    FString& OutConnectInfo, EJoinSessionResult& OutResult)
    : OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateRaw(
          this, &FJoinSessionLatentAction::OnJoinSessionComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget),
      OnlineSessionSearchResult(
          BlueprintSessionSearchResult.OnlineSessionSearchResult),
      ConnectInfo(OutConnectInfo), Result(OutResult) {
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

	// get connect string
	const bool bSuccessfullyGetConnectInfo =
	    OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession,
	                                                     ConnectInfo);

	// if failed to get connect info
	if (!bSuccessfullyGetConnectInfo) {
		// finish
		Finish(EJoinSessionResult::ErrorInGetResolvedConnectString);
		return;
	}

	// log connect string
	UE_LOG(LogOnlineSystem, Log, TEXT("JoinSession success. Connect Info: %s"),
	       *ConnectInfo);

	// finish
	Finish(EJoinSessionResult::Success);
}
