// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdReward.generated.h"

USTRUCT()
struct APPLOVINMAX_API FAdReward
{
    GENERATED_BODY()

    FString ToString() const;
    bool IsValid() const;

    UPROPERTY()
    FString Label;

    UPROPERTY()
    int Amount;
};
