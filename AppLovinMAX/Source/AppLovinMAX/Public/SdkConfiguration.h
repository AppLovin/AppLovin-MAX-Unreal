// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SdkConfiguration.generated.h"

// MARK: - Enums

UENUM()
enum class EConsentFlowUserGeography : uint8
{
    Unknown,
    GDPR,
    Other
};

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

    /**
     * Get the user's geography used to determine the type of consent flow shown to the user.
     * If no such determination could be made, EConsentFlowUserGeography::Unknown will be returned.
     */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    EConsentFlowUserGeography ConsentFlowUserGeography;

    /** Get the country code for this user. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString CountryCode;

    /** True if the user has provided consent for information sharing with AppLovin and other providers. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    bool HasUserConsent = false;

    /** True if the user is age restricted. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    bool IsAgeRestrictedUser = false;

    /** True if the user has opted out of the sale of their personal information. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    bool IsDoNotSell = false;

    /** True if the device is a tablet. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    bool IsTablet = false;

    /** iOS only. App tracking status values primarily used in conjunction with iOS 14's AppTrackingTransparency.framework. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    EAppTrackingStatus AppTrackingStatus = EAppTrackingStatus::NotDetermined;
};
