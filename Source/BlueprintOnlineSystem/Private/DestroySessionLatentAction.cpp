// Fill out your copyright notice in the Description page of Project Settings.

#include "DestroySessionLatentAction.h"

#include "LogOnlineSystem.h"
#include "OnlineSubsystem.h"

FDestroySessionLatentAction::FDestroySessionLatentAction(
    const FLatentActionInfo& InLatentInfo, EDestroySessionResult& OutResult)
    : OnDestroySessionCompleteDelegate(
          FOnDestroySessionCompleteDelegate::CreateRaw(
              this, &FDestroySessionLatentAction::OnDestroySessionComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget), Result(OutResult) {
	// get session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// set callback
	OnDestroySessionCompleteDelegateHandle =
	    OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
	        OnDestroySessionCompleteDelegate);

	// log
	UE_LOG(LogOnlineSystem, Log, TEXT("Try to destroy session"));

	// start destroying session
	const bool bDestroySessionSuccessfullyStarted =
	    OnlineSessionInterface->DestroySession(NAME_GameSession);

	// if failed to start destroying session
	if (!bDestroySessionSuccessfullyStarted) {
		// log
		UE_LOG(LogOnlineSystem, Error, TEXT("Failed to start destroying session"));

		// unbind callback
		OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
		    OnDestroySessionCompleteDelegateHandle);
		OnDestroySessionCompleteDelegateHandle.Reset();

		// finish
		Finish(EDestroySessionResult::Failure);
		return;
	}

	IsRunning = true;
}

void FDestroySessionLatentAction::UpdateOperation(FLatentResponse& Response) {
	Response.FinishAndTriggerIf(IsRunning == false, ExecutionFunction, OutputLink,
	                            CallbackTarget);
}

void FDestroySessionLatentAction::Finish(
    const EDestroySessionResult& InResult) {
	Result    = InResult;
	IsRunning = false;
}

void FDestroySessionLatentAction::OnDestroySessionComplete(
    FName SessionName, bool bWasSuccessful) {
	// get online session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// unbind callback
	OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
	    OnDestroySessionCompleteDelegateHandle);
	OnDestroySessionCompleteDelegateHandle.Reset();

	// finish
	Finish(bWasSuccessful ? EDestroySessionResult::Success
	                      : EDestroySessionResult::Failure);
}
