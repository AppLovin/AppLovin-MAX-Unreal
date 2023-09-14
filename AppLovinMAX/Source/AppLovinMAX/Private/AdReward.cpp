// Copyright AppLovin Corporation. All Rights Reserved.

#include "AdReward.h"

FString FAdReward::ToString() const
{
    return FString::Printf(TEXT("Reward: %d %s"), Amount, *Label);
}

bool FAdReward::IsValid() const
{
    return !Label.IsEmpty() && Amount > 0;
}
