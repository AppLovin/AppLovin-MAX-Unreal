// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct APPLOVINMAX_API FAdInfo
{
    FAdInfo(const TMap<FString, FString> &EventBody);
    FString ToString() const;

    FString AdUnitIdentifier;
    FString NetworkName;
    FString CreativeIdentifier;
    FString Placement;
    double Revenue;
};
