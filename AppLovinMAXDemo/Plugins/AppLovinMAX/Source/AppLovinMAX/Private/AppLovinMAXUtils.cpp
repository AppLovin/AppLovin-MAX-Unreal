// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAXUtils.h"

const FString _DictKeyValueSeparator = FString::Printf(TEXT("%c"), (char)28);
const FString _DictKeyValuePairSeparator = FString::Printf(TEXT("%c"), (char)29);

FString AppLovinMAXUtils::ParseMapIntoString(const TMap<FString, FString> &Map)
{
    FString Serialization;
    for (auto &Entry : Map)
    {
        Serialization += Entry.Key;
        Serialization += _DictKeyValueSeparator;
        Serialization += Entry.Value;
        Serialization += _DictKeyValuePairSeparator;
    }
    return Serialization;
}

TMap<FString, FString> AppLovinMAXUtils::ParseStringIntoMap(const FString &String)
{
    TMap<FString, FString> Result;

    if (String.IsEmpty()) return Result;

    TArray<FString> Components;
    String.ParseIntoArray(Components, *_DictKeyValuePairSeparator, true);

    for (auto &Component : Components)
    {
        FString Key, Value;
        if (Component.Split(_DictKeyValueSeparator, &Key, &Value) && !Result.Contains(Key))
        {
            Result.Add(Key, Value);
        }
    }

    return Result;
}

FString AppLovinMAXUtils::ParseColor(const FColor &Color)
{
    FString Hex = Color.ToHex(); // RRGGBBAA
    return "#" + Hex.Right(2) + Hex.Left(6);
}
