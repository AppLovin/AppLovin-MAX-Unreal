// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdInfo.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdInfo
{
    GENERATED_BODY()

    FString ToString() const;

    UPROPERTY(BlueprintReadWrite)
    FString AdUnitIdentifier;

    UPROPERTY(BlueprintReadWrite)
    FString NetworkName;

    UPROPERTY(BlueprintReadWrite)
    FString CreativeIdentifier;

    UPROPERTY(BlueprintReadWrite)
    FString Placement;

    UPROPERTY(BlueprintReadWrite)
    float Revenue; // TODO: Double not supported in UE4, currently cropping to float
};
