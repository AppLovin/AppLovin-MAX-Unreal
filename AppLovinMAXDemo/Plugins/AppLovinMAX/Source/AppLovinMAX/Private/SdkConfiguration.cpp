// Copyright AppLovin Corporation. All Rights Reserved.

#include "SdkConfiguration.h"
#include "AppLovinMAXUtils.h"

// nt
FSdkConfiguration::FSdkConfiguration(const TMap<FString, FString> &EventBody)
{
    CountryCode = EventBody.FindRef("countryCode");

    FString ConsentDialogStateStr = EventBody.FindRef("consentDialogState");
    if (ConsentDialogStateStr == "1")
    {
        ConsentDialogState = EConsentDialogState::Applies;
    }
    else if (ConsentDialogStateStr == "2")
    {
        ConsentDialogState = EConsentDialogState::DoesNotApply;
    }
    else
    {
        ConsentDialogState = EConsentDialogState::Unknown;
    }

#if PLATFORM_IOS
    FString AppTrackingStatusStr = EventBody.FindRef("appTrackingStatus");
    if (AppTrackingStatusStr == "-1")
    {
        AppTrackingStatus = EAppTrackingStatus::Unavailable;
    }
    else if (AppTrackingStatusStr == "0")
    {
        AppTrackingStatus = EAppTrackingStatus::NotDetermined;
    }
    else if (AppTrackingStatusStr == "1")
    {
        AppTrackingStatus = EAppTrackingStatus::Restricted;
    }
    else if (AppTrackingStatusStr == "2")
    {
        AppTrackingStatus = EAppTrackingStatus::Denied;
    }
    else  // "3" is authorized
    {
        AppTrackingStatus = EAppTrackingStatus::Authorized;
    }
#endif
}
