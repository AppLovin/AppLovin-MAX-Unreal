// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "AdError.h"
#include "AdInfo.h"
#include "AdReward.h"
#include "CmpError.h"
#include "SdkConfiguration.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AppLovinMAX.generated.h"

// Forward declarations for plugin classes
#if PLATFORM_IOS
@class MAUnrealPlugin;
#elif PLATFORM_ANDROID
class FJavaAndroidMaxUnrealPlugin;
#endif

// MARK: - Enums

UENUM()
enum class EAdViewPosition : uint8
{
    TopLeft,
    TopCenter,
    TopRight,
    Centered,
    CenterLeft,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

UCLASS()
class APPLOVINMAX_API UAppLovinMAX : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // MARK: - Initialization

    /**
     * Initialize the default instance of AppLovin SDK.
     * Please make sure that application's Android manifest or Info.plist includes the AppLovin SDK key.
     */
    static void Initialize();

    /**
     * Initialize the default instance of AppLovin SDK.
     * @param SdkKey - AppLovin SDK key
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void Initialize(const FString &SdkKey);

    /**
     * Check if the SDK has been initialized.
     * @param SdkKey - AppLovin SDK key
     * @return True if SDK has been initialized
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsInitialized();

    // MARK: - Privacy

    /**
     * Set whether or not user has provided consent for information sharing with AppLovin and other providers.
     * @param bHasUserConsent - True if the user has provided consent for information sharing with AppLovin. False by default.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetHasUserConsent(bool bHasUserConsent);

    /**
     * Check if user has provided consent for information sharing with AppLovin and other providers.
     * @return True if the user has provided consent for information sharing with AppLovin and other providers
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool HasUserConsent();

    /**
     * Set whether or not user has opted out of the sale of their personal information.
     * @param bDoNotSell - True if the user has opted out of the sale of their personal information
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetDoNotSell(bool bDoNotSell);

    /**
     * Check if the user has opted out of the sale of their personal information.
     * @return True if the user has opted out of the sale of their personal information
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsDoNotSell();

    // MARK: - Terms and Privacy Policy Flow

    /**
     * Enables the MAX Terms and Privacy Policy Flow. You must also set a privacy policy URL and (optionally) terms of service URL.
     *
     * For iOS, you must also provide a @c NSUserTrackingUsageDescription string in your Info.plist.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetTermsAndPrivacyPolicyFlowEnabled(bool bEnabled);

    /**
     * The URL of your company’s privacy policy. This is required in order to enable the Terms Flow.
     *
     * For iOS, this defaults to the value that you entered into your Info.plist via @c AppLovinConsentFlowInfo => @c AppLovinConsentFlowPrivacyPolicy.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetPrivacyPolicyUrl(const FString &Url);

    /**
     * The URL for your company’s terms of service. This is optional; you can enable the Terms Flow with or without it.
     *
     * For iOS, this defaults to the value that you entered into your Info.plist via @c AppLovinConsentFlowInfo => @c AppLovinConsentFlowTermsOfService.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetTermsOfServiceUrl(const FString &Url);

    /**
     * Set debug user geography. You may use this to test CMP flow by setting this to @c GDPR
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetConsentFlowDebugUserGeography(EConsentFlowUserGeography UserGeography);

    /**
     * Shows the CMP flow to an existing user.
     * Note that the user's current consent will be reset before the CMP alert is shown
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowCmpForExistingUser();

    /**
     * Returns whether or not a supported CMP is integrated.
     * @return True if a supported CMP is integrated.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool HasSupportedCmp();

    // MARK: - General

    /**
     * Returns whether or not the device is a tablet.
     * @return True if the device is a tablet
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsTablet();

    /**
     * Present the mediation debugger UI.
     * This debugger tool provides the status of your integration for each third-party ad network.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowMediationDebugger();

    /**
     * Set an identifier for the current user. This identifier will be tied to SDK events and our optional S2S postbacks.
     *
     * If you're using reward validation, you can optionally set an identifier to be included with currency validation postbacks.
     * For example, a username or email. We'll include this in the postback when we ping your currency endpoint from our server.
     * @param UserId - The user identifier to be set
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetUserId(const FString &UserId);

    /**
     * Set whether or not to begin video ads in a muted state or not.
     * Please call this method after the SDK has initialized.
     * @param bMuted - True if video ads should begin in muted state
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetMuted(bool bMuted);

    /**
     * Check if video ads begin in a muted state or not.
     * @return True if video ads begin in muted state. Defaults to false. Also return falses if the SDK is not initialized.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsMuted();

    /**
     * Toggle verbose logging of AppLovin SDK. If enabled, AppLovin messages will appear in standard application log accessible via console. Log messages in Unreal are tagged with the "LogAppLovinMAX" category, and all other log messages will have the
     * "AppLovinSdk" tag.
     * @param bEnabled - True if verbose logging should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetVerboseLoggingEnabled(bool bEnabled);

    /**
     * Check whether or not verbose logging is enabled.
     * @returns True if verbose logging is enabled
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsVerboseLoggingEnabled();

    /**
     * Set whether or not the creative debugger will be displayed on fullscreen ads after flipping the device screen down twice. Defaults to true.
     * @param bEnabled - True if the creative debugger should be enabled
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetCreativeDebuggerEnabled(bool bEnabled);

    /**
     * Enable devices to receive test ads, by passing in the advertising identifier (IDFA or IDFV) of each test device.
     * Refer to AppLovin logs for the IDFA or IDFV of your current device.
     * @param AdvertisingIdentifiers - A string array of advertising identifiers from devices that will receive test ads.
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetTestDeviceAdvertisingIdentifiers(const TArray<FString> &AdvertisingIdentifiers);

    // MARK: - Event Tracking

    /**
     * Track an event using AppLovin.
     * @param Name - An event from the list of pre-defined events may be found in MaxEvents.cs as part of the AppLovin SDK framework
     */
    static void TrackEvent(const FString &Name);

    /**
     * Track an event using AppLovin.
     * @param Name - An event from the list of pre-defined events may be found in MaxEvents.cs us part of the AppLovin SDK framework
     * @param Parameters - A dictionary containing key-value pairs further describing this event
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void TrackEvent(const FString &Name, const TMap<FString, FString> &Parameters);

    // MARK: - Banners

    /**
     * Create a new banner.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to create
     * @param BannerPosition - The banner position
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void CreateBanner(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition);

    /**
     * Set non-transparent background color for banners to be fully functional.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to set background color for
     * @param Color - A background color to set for the ad
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetBannerBackgroundColor(const FString &AdUnitIdentifier, const FColor &Color);

    /**
     * Set the banner placement for an ad unit identifier to tie the future ad events to.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to set the placement for
     * @param Placement - The placement to tie ad events to
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetBannerPlacement(const FString &AdUnitIdentifier, const FString &Placement);

    /**
     * Set an extra parameter for the banner ad.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to set the extra parameter for
     * @param Key - The key for the extra parameter
     * @param Value - The value for the extra parameter
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetBannerExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

    /**
     * Updates the position of the banner to the new position provided.
     * @param AdUnitIdentifier - The ad unit identifier of the banner for which to update the position
     * @param BannerPosition - A new position for the banner
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void UpdateBannerPosition(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition);

    /**
     * Show banner at a position determined by the 'CreateBanner' call.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to show
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowBanner(const FString &AdUnitIdentifier);

    /**
     * Hide banner.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to hide
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void HideBanner(const FString &AdUnitIdentifier);

    /**
     * Remove banner from the ad view and destroy it.
     * @param AdUnitIdentifier - The ad unit identifier of the banner to destroy
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void DestroyBanner(const FString &AdUnitIdentifier);

    // MARK: - MRECs

    /**
     * Create a new MREC.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to create
     * @param MRecPosition - MREC position
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void CreateMRec(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition);

    /**
     * Set the MREC placement for an ad unit identifier to tie the future ad events to.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to set the placement for
     * @param Placement - The placement to tie ad events to
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetMRecPlacement(const FString &AdUnitIdentifier, const FString &Placement);

    /**
     * Set an extra parameter for the MREC ad.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to set the extra parameter for
     * @param Key - The key for the extra parameter
     * @param Value - The value for the extra parameter
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetMRecExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

    /**
     * Updates the position of the MREC to the new position provided.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC for which to update the position
     * @param MRecPosition - A new position for the MREC
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void UpdateMRecPosition(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition);

    /**
     * Show MREC at a position determined by the 'CreateMRec' call.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to show
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowMRec(const FString &AdUnitIdentifier);

    /**
     * Hide MREC.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to hide
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void HideMRec(const FString &AdUnitIdentifier);

    /**
     * Remove MREC from the ad view and destroy it.
     * @param AdUnitIdentifier - The ad unit identifier of the MREC to destroy
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void DestroyMRec(const FString &AdUnitIdentifier);

    // MARK: - Interstitials

    /**
     * Start loading an interstitial.
     * @param AdUnitIdentifier - The ad unit identifier of the interstitial to load
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void LoadInterstitial(const FString &AdUnitIdentifier);

    /**
     * Check if the interstitial is loaded and ready to be displayed.
     * @param AdUnitIdentifier - The ad unit identifier of the interstitial to check if ready to be displayed
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsInterstitialReady(const FString &AdUnitIdentifier);

    /**
     * Present loaded interstitial. If the interstitial is not ready to be displayed nothing will happen.
     * @param AdUnitIdentifier - The ad unit identifier of the interstitial to show
     */
    static void ShowInterstitial(const FString &AdUnitIdentifier);

    /**
     * Present loaded interstitial for a given placement to tie ad events to. If the interstitial is not ready to be displayed nothing will happen.
     * @param AdUnitIdentifier - The ad unit identifier of the interstitial to show
     * @param Placement - The placement to tie the showing ad events to
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowInterstitial(const FString &AdUnitIdentifier, const FString &Placement);

    /**
     * Set an extra parameter for the interstitial.
     * @param AdUnitIdentifier - The ad unit identifier of the interstitial to set the extra parameter for
     * @param Key - The key for the extra parameter
     * @param Value - The value for the extra parameter
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetInterstitialExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

    // MARK: - Rewarded

    /**
     * Start loading a rewarded ad.
     * @param AdUnitIdentifier - The ad unit identifier of the rewarded ad to load
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void LoadRewardedAd(const FString &AdUnitIdentifier);

    /**
     * Check if the rewarded ad is loaded and ready to be displayed.
     * @param AdUnitIdentifier - The ad unit identifier of the rewarded to check if ready to be displayed
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static bool IsRewardedAdReady(const FString &AdUnitIdentifier);

    /**
     * Present loaded rewarded ad. If the rewarded ad is not ready to be displayed nothing will happen.
     * @param AdUnitIdentifier - The ad unit identifier of the rewarded ad to show
     */
    static void ShowRewardedAd(const FString &AdUnitIdentifier);

    /**
     * Present loaded rewarded ad for a given placement to tie ad events to. If the rewarded ad is not ready to be displayed nothing will happen.
     * @param AdUnitIdentifier - The ad unit identifier of the rewarded ad to show
     * @param Placement - The placement to tie the showing ad events to
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void ShowRewardedAd(const FString &AdUnitIdentifier, const FString &Placement);

    /**
     * Set an extra parameter for the rewarded ad.
     * @param AdUnitIdentifier - The ad unit identifier of the rewarded ad to set the extra parameter for
     * @param Key - The key for the extra parameter
     * @param Value - The value for the extra parameter
     */
    UFUNCTION(BlueprintCallable, Category = "AppLovinMAX")
    static void SetRewardedAdExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

    // MARK: - Delegates

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSdkInitializedDelegate, const FSdkConfiguration & /*SdkConfiguration*/);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCmpCompletedDelegate, const FCmpError & /*CmpError*/);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBannerAdLoadedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBannerAdLoadFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBannerAdClickedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBannerAdExpandedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBannerAdCollapsedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBannerAdRevenuePaidDelegate, const FAdInfo & /*AdInfo*/);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMRecAdLoadedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMRecAdLoadFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMRecAdClickedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMRecAdExpandedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMRecAdCollapsedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMRecAdRevenuePaidDelegate, const FAdInfo & /*AdInfo*/);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdLoadedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInterstitialAdLoadFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdDisplayedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInterstitialAdDisplayFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdHiddenDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdClickedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdRevenuePaidDelegate, const FAdInfo & /*AdInfo*/);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRewardedAdLoadedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdLoadFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRewardedAdDisplayedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdDisplayFailedDelegate, const FAdInfo & /*AdInfo*/, const FAdError & /*AdError*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRewardedAdHiddenDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRewardedAdClickedDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRewardedAdRevenuePaidDelegate, const FAdInfo & /*AdInfo*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdReceivedRewardDelegate, const FAdInfo & /*AdInfo*/, const FAdReward & /*Reward*/);

    static FOnSdkInitializedDelegate OnSdkInitializedDelegate;

    static FOnCmpCompletedDelegate OnCmpCompletedDelegate;

    static FOnBannerAdLoadedDelegate OnBannerAdLoadedDelegate;
    static FOnBannerAdLoadFailedDelegate OnBannerAdLoadFailedDelegate;
    static FOnBannerAdClickedDelegate OnBannerAdClickedDelegate;
    static FOnBannerAdExpandedDelegate OnBannerAdExpandedDelegate;
    static FOnBannerAdCollapsedDelegate OnBannerAdCollapsedDelegate;
    static FOnBannerAdRevenuePaidDelegate OnBannerAdRevenuePaidDelegate;

    static FOnMRecAdLoadedDelegate OnMRecAdLoadedDelegate;
    static FOnMRecAdLoadFailedDelegate OnMRecAdLoadFailedDelegate;
    static FOnMRecAdClickedDelegate OnMRecAdClickedDelegate;
    static FOnMRecAdExpandedDelegate OnMRecAdExpandedDelegate;
    static FOnMRecAdCollapsedDelegate OnMRecAdCollapsedDelegate;
    static FOnMRecAdRevenuePaidDelegate OnMRecAdRevenuePaidDelegate;

    static FOnInterstitialAdLoadedDelegate OnInterstitialAdLoadedDelegate;
    static FOnInterstitialAdLoadFailedDelegate OnInterstitialAdLoadFailedDelegate;
    static FOnInterstitialAdDisplayedDelegate OnInterstitialAdDisplayedDelegate;
    static FOnInterstitialAdDisplayFailedDelegate OnInterstitialAdDisplayFailedDelegate;
    static FOnInterstitialAdHiddenDelegate OnInterstitialAdHiddenDelegate;
    static FOnInterstitialAdClickedDelegate OnInterstitialAdClickedDelegate;
    static FOnInterstitialAdRevenuePaidDelegate OnInterstitialAdRevenuePaidDelegate;

    static FOnRewardedAdLoadedDelegate OnRewardedAdLoadedDelegate;
    static FOnRewardedAdLoadFailedDelegate OnRewardedAdLoadFailedDelegate;
    static FOnRewardedAdDisplayedDelegate OnRewardedAdDisplayedDelegate;
    static FOnRewardedAdDisplayFailedDelegate OnRewardedAdDisplayFailedDelegate;
    static FOnRewardedAdHiddenDelegate OnRewardedAdHiddenDelegate;
    static FOnRewardedAdClickedDelegate OnRewardedAdClickedDelegate;
    static FOnRewardedAdRevenuePaidDelegate OnRewardedAdRevenuePaidDelegate;
    static FOnRewardedAdReceivedRewardDelegate OnRewardedAdReceivedRewardDelegate;

protected:
    // MARK: - Utility Methods

    static FString GetAdViewPositionString(EAdViewPosition AdViewPosition);
    static FString GetUserGeographyString(EConsentFlowUserGeography UserGeography);
    static void ValidateAdUnitIdentifier(const FString &AdUnitIdentifier, const FString &DebugPurpose);

#if PLATFORM_IOS
    static NSString *GetNSString(const FString &String);
    static NSArray<NSString *> *GetNSArray(const TArray<FString> &Array);
    static NSDictionary<NSString *, NSString *> *GetNSDictionary(const TMap<FString, FString> &Map);
    static MAUnrealPlugin *GetIOSPlugin();
#elif PLATFORM_ANDROID
    static TSharedPtr<FJavaAndroidMaxUnrealPlugin> GetAndroidPlugin();
#endif
};
