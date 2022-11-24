// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace AppLovinMAXUtils
{
    /** Returns serialized JSON for a TMap. */
    FString SerializeMap(const TMap<FString, FString> &Map);

    /** Returns the hexadecimal color code as #AARRGGBB for the given FColor. */
    FString ParseColor(const FColor &Color);
} // namespace AppLovinMAXUtils
