// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BlueprintSessionSearchResult.h"
#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"

// blueprint version of EOnJoinSessionCompleteResult::Type
UENUM(BlueprintType)
enum class EJoinSessionResult : uint8 {
	/* The join worked as expected */
	Success,
	/* There are no open slots to join */
	SessionIsFull,
	/* The session couldn't be found on the
	   service */
	SessionDoesNotExist,
	/* There was an error getting the
	   session server's address */
	CouldNotRetrieveAddress,
	/* The user attempting to join is
	   already a member of the session */
	AlreadyInSession,
	/* There was an error in GetResolvedConnectString */
	ErrorInGetResolvedConnectString,
	/* An error not covered above occurred */
	UnknownError
};

/**
 * Internal class for OnlineSystem::JoinSession
 */
class FJoinSessionLatentAction: public FPendingLatentAction {
public:
	FJoinSessionLatentAction(
	    const FLatentActionInfo&             InLatentInfo,
	    const APlayerController&             InPlayerController,
	    const FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
	    FString& OutConnectInfo, EJoinSessionResult& OutResult);

public:
	// this function is called every frame to check if it has finished.
	virtual void UpdateOperation(FLatentResponse& Response) override;

	/* internal functions */
private:
	// set result and finish latent action
	void Finish(const EJoinSessionResult& InResult);

	// callback function
	void OnJoinSessionComplete(
	    FName                              SessionName,
	    EOnJoinSessionCompleteResult::Type OnJoinSessionCompleteResult);

	/* internal callback delegates */
private:
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle                OnJoinSessionCompleteDelegateHandle;

	/* internal fields */
private:
	bool IsRunning = false;

	FName                      ExecutionFunction;
	int32                      OutputLink;
	FWeakObjectPtr             CallbackTarget;
	IOnlineSessionPtr          OnlineSessionInterface;
	FOnlineSessionSearchResult OnlineSessionSearchResult;
	FString&                    ConnectInfo;
	EJoinSessionResult&        Result;
};
