// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace AppLovinMAXUtils
{
    /** Returns serialized string for a TMap to forward to native iOS and Android plugins. */
    FString ParseMapIntoString(const TMap<FString, FString> &Map);

    /** Returns TMap for a serialized string forwarded by native iOS and Android plugins. */
    TMap<FString, FString> ParseStringIntoMap(const FString &String);

    /** Returns the hexadecimal color code as #AARRGGBB for the given FColor. */
    FString ParseColor(const FColor &Color);
}
