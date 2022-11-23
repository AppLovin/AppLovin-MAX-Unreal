// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// MARK: - Enums

UENUM()
enum class EAppTrackingStatus : uint8 // Only for iOS
{
    Unavailable,
    NotDetermined,
    Restricted,
    Denied,
    Authorized
};

// MARK: - SdkConfiguration

struct APPLOVINMAX_API FSdkConfiguration
{
    FSdkConfiguration(const TMap<FString, FString> &EventBody);

    /** Get the country code for this user. */
    FString CountryCode;

    /** True if the user has provided consent for information sharing with AppLovin and other providers. */
    bool HasUserConsent;

    /** True if the user is age restricted. */
    bool IsAgeRestrictedUser;

    /** True if the user has opted out of the sale of their personal information. */
    bool IsDoNotSell;

    /** True if the device is a tablet. */
    bool IsTablet;

#if PLATFORM_IOS
    /** App tracking status values. Primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    EAppTrackingStatus AppTrackingStatus;
#endif
};
