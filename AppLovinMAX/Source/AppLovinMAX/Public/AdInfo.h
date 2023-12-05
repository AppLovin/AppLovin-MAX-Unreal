// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdInfo.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdInfo
{
    GENERATED_BODY()

    FString ToString() const;

    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString AdUnitIdentifier;

    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString NetworkName;

    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString CreativeIdentifier;

    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString Placement;

    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    double Revenue = 0;
};
