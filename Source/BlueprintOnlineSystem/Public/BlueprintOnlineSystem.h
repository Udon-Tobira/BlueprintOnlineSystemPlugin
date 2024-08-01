// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "CreateSessionLatentAction.h"
#include "DestroySessionLatentAction.h"
#include "FindSessionLatentAction.h"
#include "JoinSessionLatentAction.h"
#include "LoginLatentAction.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "BlueprintOnlineSystem.generated.h"

class FBlueprintOnlineSystemModule: public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

/**
 * Class to handle online subsystem.
 * This class is currently designed for EOS (Epic Online Services).
 */
UCLASS()
class UBlueprintOnlineSystem: public UGameInstanceSubsystem {
	GENERATED_BODY()

	/* Initializatinos */
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	/* Blueprint Functions */
public:
	// get online subsystem name
	UFUNCTION(BlueprintCallable)
	FName GetOnlineSubsystemName() const;

	// Login to current online subsystem
	UFUNCTION(BlueprintCallable, Category = "IdentityInterface",
	          meta = (AuthType          = "accountportal",
	                  ExpandEnumAsExecs = "LoginResult", Latent,
	                  LatentInfo        = "LatentActionInfo",
	                  WorldContext      = "WorldContextObject"))
	void Login(const UObject*           WorldContextObject,
	           FLatentActionInfo        LatentActionInfo,
	           const APlayerController* PlayerController, const FString& AuthType,
	           ELoginResult& LoginResult);

	// Create session on online subsystem and local
	UFUNCTION(BlueprintCallable, Category = "SessionInterface",
	          meta = (MaxConnections    = 32,
	                  ExpandEnumAsExecs = "CreateSessionResult", Latent,
	                  LatentInfo        = "LatentActionInfo",
	                  WorldContext      = "WorldContextObject"))
	void CreateSession(const UObject*           WorldContextObject,
	                   FLatentActionInfo        LatentActionInfo,
	                   const APlayerController* PlayerController,
	                   int32 MaxConnections, FString& SessionId,
	                   ECreateSessionResult& CreateSessionResult);

	// Destroy session on online subsystem and local
	UFUNCTION(BlueprintCallable, Category = "SessionInterface",
	          meta = (ExpandEnumAsExecs = "DestroySessionResult", Latent,
	                  LatentInfo        = "LatentActionInfo",
	                  WorldContext      = "WorldContextObject"))
	void DestroySession(const UObject*         WorldContextObject,
	                    FLatentActionInfo      LatentActionInfo,
	                    EDestroySessionResult& DestroySessionResult);

	// Find session on online subsystem
	UFUNCTION(BlueprintCallable, Category = "SessionInterface",
	          meta = (ExpandEnumAsExecs = "FindSessionResult", Latent,
	                  LatentInfo        = "LatentActionInfo",
	                  WorldContext      = "WorldContextObject"))
	void FindSession(const UObject*                 WorldContextObject,
	                 FLatentActionInfo              LatentActionInfo,
	                 const APlayerController*       PlayerController,
	                 const FString&                 SessionId,
	                 FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
	                 EFindSessionResult&            FindSessionResult);

	// Find session on online subsystem and local
	UFUNCTION(BlueprintCallable, Category = "SessionInterface",
	          meta = (ExpandEnumAsExecs = "JoinSessionResult", Latent,
	                  LatentInfo        = "LatentActionInfo",
	                  WorldContext      = "WorldContextObject"))
	void JoinSession(
	    const UObject* WorldContextObject, FLatentActionInfo LatentActionInfo,
	    const APlayerController*             PlayerController,
	    const FBlueprintSessionSearchResult& BlueprintSessionSearchResult,
	    FString& SessionConnectInfo, FString& BeaconConnectInfo,
	    EJoinSessionResult& JoinSessionResult);
};
