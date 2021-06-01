// Copyright AppLovin Corporation. All Rights Reserved.

#include "AdInfo.h"
#include "AppLovinMAXUtils.h"

FAdInfo::FAdInfo(const TMap<FString, FString> &EventBody)
{
    AdUnitIdentifier = EventBody.FindRef(TEXT("adUnitId"));
    NetworkName = EventBody.FindRef(TEXT("networkName"));
    CreativeIdentifier = EventBody.FindRef(TEXT("creativeId"));
    Placement = EventBody.FindRef(TEXT("placement"));

    if (EventBody.Contains(TEXT("revenue")))
    {
        // NOTE: Defaults to 0 if revenue string cannot be converted to double
        Revenue = FCString::Atod(*EventBody.FindRef(TEXT("revenue")));
    }
    else
    {
        Revenue = -1;
    }
}

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
