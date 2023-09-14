// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SdkConfiguration.generated.h"

// MARK: - Enums

UENUM(BlueprintType)
enum class EAppTrackingStatus : uint8 // iOS only
{
    Unavailable,
    NotDetermined,
    Restricted,
    Denied,
    Authorized,
};

// MARK: - SdkConfiguration
USTRUCT(BlueprintType)
struct APPLOVINMAX_API FSdkConfiguration
{
    GENERATED_BODY()

    /** Get the country code for this user. */
    UPROPERTY(BlueprintReadOnly)
    FString CountryCode;

    /** True if the user has provided consent for information sharing with AppLovin and other providers. */
    UPROPERTY(BlueprintReadOnly)
    bool HasUserConsent = false;

    /** True if the user is age restricted. */
    UPROPERTY(BlueprintReadOnly)
    bool IsAgeRestrictedUser = false;

    /** True if the user has opted out of the sale of their personal information. */
    UPROPERTY(BlueprintReadOnly)
    bool IsDoNotSell = false;

    /** True if the device is a tablet. */
    UPROPERTY(BlueprintReadOnly)
    bool IsTablet = false;

    /** iOS only. App tracking status values primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    UPROPERTY(BlueprintReadOnly)
    EAppTrackingStatus AppTrackingStatus = EAppTrackingStatus::NotDetermined;
};
