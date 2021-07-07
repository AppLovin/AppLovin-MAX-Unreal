// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct APPLOVINMAX_API FAdReward
{
    FString ToString() const;
    bool IsValid() const;

    FString Label;
    int Amount;
};
