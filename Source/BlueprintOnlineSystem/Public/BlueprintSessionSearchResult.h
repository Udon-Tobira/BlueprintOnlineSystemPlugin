// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"

#include "BlueprintSessionSearchResult.generated.h"

USTRUCT(BlueprintType)
struct FBlueprintSessionSearchResult {
	GENERATED_BODY()

public:
	FOnlineSessionSearchResult OnlineSessionSearchResult;
};
