// Copyright AppLovin Corporation. All Rights Reserved.

#include "Reward.h"

FString FReward::ToString() const
{
    return FString::Printf(TEXT("Reward: %d %s"), Amount, *Label);
}

bool FReward::IsValid() const
{
    return !Label.IsEmpty() && Amount > 0;
}
