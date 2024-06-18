// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"

UENUM(BlueprintType)
enum class ELoginResult : uint8 {
	// successfully log in
	Success,
	// failed to log in
	Failure
};

/**
 * Internal class for OnlineSystem::Login
 */
class FLoginLatentAction: public FPendingLatentAction {
public:
	FLoginLatentAction(const FLatentActionInfo& InLatentInfo,
	                   const APlayerController* InPlayerController,
	                   const FString& InAuthType, ELoginResult& OutResult);

public:
	// this function is called every frame to check if it has finished.
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/* internal functions */
private:
	// set result and finish latent action
	void Finish(const ELoginResult& InResult);

	// callback function
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful,
	                     const FUniqueNetId& UserId, const FString& Error);

	/* internal callback delegates */
private:
	FOnLoginCompleteDelegate OnLoginCompleteDelegate;
	FDelegateHandle          OnLoginCompleteDelegateHandle;

	/* internal fields */
private:
	bool IsRunning = false;

	FName          ExecutionFunction;
	int32          OutputLink;
	FWeakObjectPtr CallbackTarget;
	ELoginResult&  Result;
};
