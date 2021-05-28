// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// MARK: - Enums

UENUM()
enum class EConsentDialogState : uint8
{
    Unknown,
    Applies,
    DoesNotApply
};

UENUM()
enum class EAppTrackingStatus : uint8  // Only for iOS
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

    /** Get the consent dialog state for this user. If no such determination could be made, `EConsentDialogState::Unknown` will be returned. */
    EConsentDialogState ConsentDialogState;

    /** Get the country code for this user. */
    FString CountryCode;

#if PLATFORM_IOS
    /** App tracking status values. Primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    EAppTrackingStatus AppTrackingStatus;
#endif
};
