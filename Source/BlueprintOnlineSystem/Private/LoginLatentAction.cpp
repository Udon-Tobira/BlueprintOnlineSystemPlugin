// Fill out your copyright notice in the Description page of Project Settings.

#include "LoginLatentAction.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "LogOnlineSystem.h"
#include "Misc/AssertionMacros.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

FLoginLatentAction::FLoginLatentAction(
    const FLatentActionInfo& InLatentInfo,
    const APlayerController& InPlayerController, const FString& InAuthType,
    ELoginResult& OutResult)
    : OnLoginCompleteDelegate(FOnLoginCompleteDelegate::CreateRaw(
          this, &FLoginLatentAction::OnLoginComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget), Result(OutResult) {
	// get World
	const auto World = InPlayerController.GetWorld();
	check(World != nullptr);

	// get online identity interface
	const auto OnlineSubsystem = Online::GetSubsystem(World);
	check(OnlineSubsystem != nullptr);

	OnlineIdentityInterface = OnlineSubsystem->GetIdentityInterface();
	check(OnlineIdentityInterface != nullptr);

	// get local player
	const auto LocalPlayer = InPlayerController.GetLocalPlayer();
	check(LocalPlayer != nullptr);

	// get index of player controller
	const auto PlayerIndex = LocalPlayer->GetLocalPlayerIndex();

	// get login status
	const auto LoginStatus = OnlineIdentityInterface->GetLoginStatus(PlayerIndex);

	// if already logged in, return
	if (ELoginStatus::LoggedIn == LoginStatus) {
		UE_LOG(LogOnlineSystem, Warning, TEXT("Already logged in"));
		Finish(ELoginResult::Success);
		return;
	}

	// set callback
	OnLoginCompleteDelegateHandle =
	    OnlineIdentityInterface->AddOnLoginCompleteDelegate_Handle(
	        PlayerIndex, OnLoginCompleteDelegate);

	// make credentials
	FOnlineAccountCredentials AccountCredentials;
	AccountCredentials.Type = InAuthType;

	// change state to running
	IsRunning = true;

	// start login
	const bool bLoginSuccessfullyStarted =
	    OnlineIdentityInterface->Login(PlayerIndex, AccountCredentials);

	// if failed to start login
	if (!bLoginSuccessfullyStarted) {
		// log
		UE_LOG(LogOnlineSystem, Error, TEXT("Failed to start login"));

		// unbind callback
		OnlineIdentityInterface->ClearOnLoginCompleteDelegate_Handle(
		    PlayerIndex, OnLoginCompleteDelegateHandle);
		OnLoginCompleteDelegateHandle.Reset();

		// finish
		Finish(ELoginResult::Failure);
		return;
	}
}

void FLoginLatentAction::UpdateOperation(FLatentResponse& Response) {
	Response.FinishAndTriggerIf(IsRunning == false, ExecutionFunction, OutputLink,
	                            CallbackTarget);
}

void FLoginLatentAction::Finish(const ELoginResult& InResult) {
	Result    = InResult;
	IsRunning = false;
}

void FLoginLatentAction::OnLoginComplete(int32               LocalUserNum,
                                         bool                bWasSuccessful,
                                         const FUniqueNetId& UserId,
                                         const FString&      Error) {
	// unbind callback
	OnlineIdentityInterface->ClearOnLoginCompleteDelegate_Handle(
	    LocalUserNum, OnLoginCompleteDelegateHandle);
	OnLoginCompleteDelegateHandle.Reset();

	// finish
	Finish(bWasSuccessful ? ELoginResult::Success : ELoginResult::Failure);
}
