// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdReward.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdReward
{
    GENERATED_BODY()

    FString ToString() const;
    bool IsValid() const;

    UPROPERTY(BlueprintReadOnly)
    FString Label;

    UPROPERTY(BlueprintReadOnly)
    int Amount = 0;
};
