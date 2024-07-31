// Fill out your copyright notice in the Description page of Project Settings.

#include "DestroySessionLatentAction.h"

#include "LogOnlineSystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

FDestroySessionLatentAction::FDestroySessionLatentAction(
    const UObject& WorldContextObject, const FLatentActionInfo& InLatentInfo,
    EDestroySessionResult& OutResult)
    : OnDestroySessionCompleteDelegate(
          FOnDestroySessionCompleteDelegate::CreateRaw(
              this, &FDestroySessionLatentAction::OnDestroySessionComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget), Result(OutResult) {
	// get World
	const auto World = WorldContextObject.GetWorld();
	check(World != nullptr);

	// get session interface
	const auto OnlineSubsystem = Online::GetSubsystem(World);
	check(OnlineSubsystem != nullptr);

	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// set callback
	OnDestroySessionCompleteDelegateHandle =
	    OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
	        OnDestroySessionCompleteDelegate);

	// log
	UE_LOG(LogOnlineSystem, Log, TEXT("Try to destroy session"));

	// change state to running
	IsRunning = true;

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
	// unbind callback
	OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
	    OnDestroySessionCompleteDelegateHandle);
	OnDestroySessionCompleteDelegateHandle.Reset();

	// finish
	Finish(bWasSuccessful ? EDestroySessionResult::Success
	                      : EDestroySessionResult::Failure);
}
