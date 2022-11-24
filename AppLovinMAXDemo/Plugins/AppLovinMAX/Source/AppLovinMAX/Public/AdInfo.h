// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdInfo.generated.h"

USTRUCT()
struct APPLOVINMAX_API FAdInfo
{
    GENERATED_BODY()

    FString ToString() const;

    UPROPERTY()
    FString AdUnitIdentifier;

    UPROPERTY()
    FString NetworkName;

    UPROPERTY()
    FString CreativeIdentifier;

    UPROPERTY()
    FString Placement;

    UPROPERTY()
    double Revenue;
};
