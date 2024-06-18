// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BlueprintSessionSearchResult.h"
#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

UENUM(BlueprintType)
enum class EFindSessionResult : uint8 {
	// successfully log in
	Success,
	// failed to log in
	Failure
};

/**
 * Internal class for OnlineSystem::CreateSession
 */
class FFindSessionLatentAction: public FPendingLatentAction {
public:
	FFindSessionLatentAction(
	    const FLatentActionInfo& InLatentInfo,
	    const APlayerController* InPlayerController, const FString& InSessionId,
	    FBlueprintSessionSearchResult& OutBlueprintSessionSearchResult,
	    EFindSessionResult&            OutResult);

public:
	// this function is called every frame to check if it has finished.
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/* internal functions */
private:
	// set result and finish latent action as success
	void FinishAsSuccess(
	    const FOnlineSessionSearchResult& OnlineSessionSearchResult);
	void FinishAsFailure();

	// callback function
	void OnFindSessionsComplete(bool bWasSuccessful);

	/* internal callback delegates */
private:
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle                 OnFindSessionsCompleteDelegateHandle;

	/* internal fields */
private:
	bool IsRunning = false;

	// only for FPendingLatentAction
	FName          ExecutionFunction;
	int32          OutputLink;
	FWeakObjectPtr CallbackTarget;

	// output pins
	FBlueprintSessionSearchResult& BlueprintSessionSearchResult;
	EFindSessionResult&            Result;

	// private field
	const FString                    SearchingSessionId;
	TSharedRef<FOnlineSessionSearch> OnlineSessionSearch =
	    MakeShared<FOnlineSessionSearch>();
};
