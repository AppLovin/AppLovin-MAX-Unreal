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
    UPROPERTY(BlueprintReadWrite)
    FString CountryCode;

    /** True if the user has provided consent for information sharing with AppLovin and other providers. */
    UPROPERTY(BlueprintReadWrite)
    bool HasUserConsent;

    /** True if the user is age restricted. */
    UPROPERTY(BlueprintReadWrite)
    bool IsAgeRestrictedUser;

    /** True if the user has opted out of the sale of their personal information. */
    UPROPERTY(BlueprintReadWrite)
    bool IsDoNotSell;

    /** True if the device is a tablet. */
    UPROPERTY(BlueprintReadWrite)
    bool IsTablet;

    /** iOS only. App tracking status values primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    UPROPERTY(BlueprintReadWrite)
    EAppTrackingStatus AppTrackingStatus;
};
