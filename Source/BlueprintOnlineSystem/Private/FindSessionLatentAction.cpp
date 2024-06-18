// Fill out your copyright notice in the Description page of Project Settings.

#include "FindSessionLatentAction.h"

#include "LogOnlineSystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

FFindSessionLatentAction::FFindSessionLatentAction(
    const FLatentActionInfo& InLatentInfo,
    const APlayerController* InPlayerController, const FString& InSessionId,
    FBlueprintSessionSearchResult& OutBlueprintSessionSearchResult,
    EFindSessionResult&            OutResult)
    : OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateRaw(
          this, &FFindSessionLatentAction::OnFindSessionsComplete)),
      ExecutionFunction(InLatentInfo.ExecutionFunction),
      OutputLink(InLatentInfo.Linkage),
      CallbackTarget(InLatentInfo.CallbackTarget),
      BlueprintSessionSearchResult(OutBlueprintSessionSearchResult),
      Result(OutResult), SearchingSessionId(InSessionId) {
	// get session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// make search settings
	OnlineSessionSearch->bIsLanQuery =
	    OnlineSubsystem->GetSubsystemName() == "NULL" ? true : false;
	OnlineSessionSearch->MaxSearchResults = 1;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true,
	                                       EOnlineComparisonOp::Equals);

	// get local player
	const auto LocalPlayer = InPlayerController->GetLocalPlayer();
	check(LocalPlayer != nullptr);

	// get index of player controller
	const auto PlayerIndex = LocalPlayer->GetLocalPlayerIndex();

	// set callback
	OnFindSessionsCompleteDelegateHandle =
	    OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
	        OnFindSessionsCompleteDelegate);

	// log
	UE_LOG(LogOnlineSystem, Log, TEXT("searching session: %s"), *InSessionId);

	// start searching session
	const bool bFindSessionsSuccessfullyStarted =
	    OnlineSessionInterface->FindSessions(PlayerIndex, OnlineSessionSearch);

	// if failed to start searching session
	if (!bFindSessionsSuccessfullyStarted) {
		// log
		UE_LOG(LogOnlineSystem, Error,
		       TEXT("Failed to start searching session: %s"), *InSessionId);

		// unbind callback
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
		    OnFindSessionsCompleteDelegateHandle);
		OnFindSessionsCompleteDelegateHandle.Reset();

		FinishAsFailure();
		return;
	}

	IsRunning = true;
}

void FFindSessionLatentAction::UpdateOperation(FLatentResponse& Response) {
	Response.FinishAndTriggerIf(IsRunning == false, ExecutionFunction, OutputLink,
	                            CallbackTarget);
}

void FFindSessionLatentAction::FinishAsSuccess(
    const FOnlineSessionSearchResult& OnlineSessionSearchResult) {
	BlueprintSessionSearchResult = {OnlineSessionSearchResult};
	Result                       = EFindSessionResult::Success;
	IsRunning                    = false;
}

void FFindSessionLatentAction::FinishAsFailure() {
	Result    = EFindSessionResult::Failure;
	IsRunning = false;
}

void FFindSessionLatentAction::OnFindSessionsComplete(bool bWasSuccessful) {
	// get online session interface
	const auto OnlineSubsystem = IOnlineSubsystem::Get();
	check(OnlineSubsystem != nullptr);

	const auto OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	check(OnlineSessionInterface != nullptr);

	// unbind callback
	OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
	    OnFindSessionsCompleteDelegateHandle);
	OnFindSessionsCompleteDelegateHandle.Reset();

	// if not successful
	if (!bWasSuccessful) {
		FinishAsFailure();
		return;
	}

	// get search results
	const auto SearchResults = OnlineSessionSearch->SearchResults;

	// log
	UE_LOG(LogOnlineSystem, Verbose, TEXT("%d sessions found"),
	       SearchResults.Num());

	// search session with session id == SearchingSessionId
	for (const auto& SearchResult : SearchResults) {
		// this SearchResult's SessionId
		const auto& SessionId = SearchResult.GetSessionIdStr();

		// log
		UE_LOG(LogOnlineSystem, Verbose,
		       TEXT("Scanning a session with SessionId: %s"), *SessionId);

		// if found
		if (SearchingSessionId == SessionId) {
			// finish
			FinishAsSuccess(SearchResult);
			return;
		}
	}

	// if not found
	FinishAsFailure();
}
