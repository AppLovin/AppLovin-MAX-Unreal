// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SdkConfiguration.generated.h"

// MARK: - Enums

UENUM()
enum class EAppTrackingStatus : int8 // Only for iOS
{
    Unavailable = -1,
    NotDetermined = 0,
    Restricted = 1,
    Denied = 2,
    Authorized = 3
};

// MARK: - SdkConfiguration
USTRUCT()
struct APPLOVINMAX_API FSdkConfiguration
{
    GENERATED_BODY()

    /** Get the country code for this user. */
    UPROPERTY()
    FString CountryCode;

    /** True if the user has provided consent for information sharing with AppLovin and other providers. */
    UPROPERTY()
    bool HasUserConsent;

    /** True if the user is age restricted. */
    UPROPERTY()
    bool IsAgeRestrictedUser;

    /** True if the user has opted out of the sale of their personal information. */
    UPROPERTY()
    bool IsDoNotSell;

    /** True if the device is a tablet. */
    UPROPERTY()
    bool IsTablet;

    /** For iOS only. App tracking status values primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    UPROPERTY()
    EAppTrackingStatus AppTrackingStatus;
};
