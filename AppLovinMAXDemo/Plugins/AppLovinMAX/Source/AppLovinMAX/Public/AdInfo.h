// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdInfo.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdInfo
{
    GENERATED_BODY()

    FString ToString() const;

    UPROPERTY(BlueprintReadOnly)
    FString AdUnitIdentifier;

    UPROPERTY(BlueprintReadOnly)
    FString NetworkName;

    UPROPERTY(BlueprintReadOnly)
    FString CreativeIdentifier;

    UPROPERTY(BlueprintReadOnly)
    FString Placement;

    UPROPERTY(BlueprintReadOnly)
    float Revenue; // NOTE: doubles are not supported by UE4 blueprints, so cropping to float
};
