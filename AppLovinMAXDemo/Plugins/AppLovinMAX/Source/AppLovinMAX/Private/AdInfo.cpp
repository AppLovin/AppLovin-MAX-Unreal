// Copyright AppLovin Corporation. All Rights Reserved.

#include "AdInfo.h"
#include "AppLovinMAXUtils.h"

FString FAdInfo::ToString() const
{
    TArray<FStringFormatArg> Args;
    Args.Add(FStringFormatArg(AdUnitIdentifier));
    Args.Add(FStringFormatArg(NetworkName));
    Args.Add(FStringFormatArg(CreativeIdentifier));
    Args.Add(FStringFormatArg(Placement));
    Args.Add(FStringFormatArg(Revenue));

    return FString::Format(TEXT("[FAdInfo adUnitIdentifier: {0} networkName: {1} creativeIdentifier: {2} placement: {3} revenue: {4}]"), Args);
}
