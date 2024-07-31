// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"

UENUM(BlueprintType)
enum class ECreateSessionResult : uint8 {
	// successfully log in
	Success,
	// failed to log in
	Failure
};

/**
 * Internal class for OnlineSystem::CreateSession
 */
class FCreateSessionLatentAction: public FPendingLatentAction {
public:
	FCreateSessionLatentAction(const FLatentActionInfo& InLatentInfo,
	                           const APlayerController& InPlayerController,
	                           int32 MaxConnections, FString& OutSessionId,
	                           ECreateSessionResult& OutResult);

public:
	// this function is called every frame to check if it has finished.
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/* internal functions */
private:
	// set result and finish latent action
	void Finish(const ECreateSessionResult& InResult);

	// callback function
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/* internal callback delegates */
private:
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle                  OnCreateSessionCompleteDelegateHandle;

	/* internal fields */
private:
	bool IsRunning = false;

	FName                 ExecutionFunction;
	int32                 OutputLink;
	FWeakObjectPtr        CallbackTarget;
	IOnlineSessionPtr     OnlineSessionInterface;
	FString&              SessionId;
	ECreateSessionResult& Result;
};
