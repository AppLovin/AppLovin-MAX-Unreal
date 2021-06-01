// Copyright AppLovin Corporation. All Rights Reserved.

#include "SdkConfiguration.h"
#include "AppLovinMAXUtils.h"

FSdkConfiguration::FSdkConfiguration(const TMap<FString, FString> &EventBody)
{
    CountryCode = EventBody.FindRef(TEXT("countryCode"));

    FString ConsentDialogStateStr = EventBody.FindRef(TEXT("consentDialogState"));
    if (ConsentDialogStateStr == TEXT("1"))
    {
        ConsentDialogState = EConsentDialogState::Applies;
    }
    else if (ConsentDialogStateStr == TEXT("2"))
    {
        ConsentDialogState = EConsentDialogState::DoesNotApply;
    }
    else
    {
        ConsentDialogState = EConsentDialogState::Unknown;
    }

#if PLATFORM_IOS
    FString AppTrackingStatusStr = EventBody.FindRef(TEXT("appTrackingStatus"));
    if (AppTrackingStatusStr == TEXT("-1"))
    {
        AppTrackingStatus = EAppTrackingStatus::Unavailable;
    }
    else if (AppTrackingStatusStr == TEXT("0"))
    {
        AppTrackingStatus = EAppTrackingStatus::NotDetermined;
    }
    else if (AppTrackingStatusStr == TEXT("1"))
    {
        AppTrackingStatus = EAppTrackingStatus::Restricted;
    }
    else if (AppTrackingStatusStr == TEXT("2"))
    {
        AppTrackingStatus = EAppTrackingStatus::Denied;
    }
    else  // "3" is authorized
    {
        AppTrackingStatus = EAppTrackingStatus::Authorized;
    }
#endif
}
