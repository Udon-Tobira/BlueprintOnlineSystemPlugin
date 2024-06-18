// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"

UENUM(BlueprintType)
enum class EDestroySessionResult : uint8 {
	// successfully log in
	Success,
	// failed to log in
	Failure
};

/**
 * Internal class for OnlineSystem::DestroySession
 */
class FDestroySessionLatentAction: public FPendingLatentAction {
public:
	FDestroySessionLatentAction(const FLatentActionInfo& InLatentInfo,
	                            EDestroySessionResult&   OutResult);

public:
	// this function is called every frame to check if it has finished.
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/* internal functions */
private:
	// set result and finish latent action
	void Finish(const EDestroySessionResult& InResult);

	// callback function
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/* internal callback delegates */
private:
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle                   OnDestroySessionCompleteDelegateHandle;

	/* internal fields */
private:
	bool IsRunning = false;

	FName                  ExecutionFunction;
	int32                  OutputLink;
	FWeakObjectPtr         CallbackTarget;
	EDestroySessionResult& Result;
};
