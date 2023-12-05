// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJava.h"

// Wrapper for com/applovin/unreal/MaxUnrealPlugin.java.
class FJavaAndroidMaxUnrealPlugin : public FJavaClassObject
{
public:
    FJavaAndroidMaxUnrealPlugin();
    virtual ~FJavaAndroidMaxUnrealPlugin();

    // MARK: Initialization
    void Initialize(const FString &PluginVersion, const FString &SdkKey);
    bool IsInitialized();

    // MARK: Privacy
    void SetHasUserConsent(bool bHasUserConsent);
    bool HasUserConsent();
    void SetIsAgeRestrictedUser(bool bIsAgeRestrictedUser);
    bool IsAgeRestrictedUser();
    void SetDoNotSell(bool bDoNotSell);
    bool IsDoNotSell();

    // MARK: Terms and Privacy Policy Flow
    void SetTermsAndPrivacyPolicyFlowEnabled(bool bEnabled);
    void SetPrivacyPolicyURL(const FString &URL);
    void SetTermsOfServiceURL(const FString &URL);

    // MARK: General
    bool IsTablet();
    void ShowMediationDebugger();
    void SetUserId(const FString &UserId);
    void SetMuted(bool bMuted);
    bool IsMuted();
    void SetVerboseLoggingEnabled(bool bEnabled);
    bool IsVerboseLoggingEnabled();
    void SetCreativeDebuggerEnabled(bool bEnabled);
    void SetTestDeviceAdvertisingIdentifiers(const TArray<FString> &AdvertisingIdentifiers);

    // MARK: Event Tracking
    void TrackEvent(const FString &Name, const FString &Parameters);

    // MARK: Banners
    void CreateBanner(const FString &AdUnitIdentifier, const FString &BannerPosition);
    void SetBannerBackgroundColor(const FString &AdUnitIdentifier, const FString &HexColorCode);
    void SetBannerPlacement(const FString &AdUnitIdentifier, const FString &Placement);
    void SetBannerExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);
    void UpdateBannerPosition(const FString &AdUnitIdentifier, const FString &BannerPosition);
    void ShowBanner(const FString &AdUnitIdentifier);
    void HideBanner(const FString &AdUnitIdentifier);
    void DestroyBanner(const FString &AdUnitIdentifier);

    // MARK: MRECs
    void CreateMRec(const FString &AdUnitIdentifier, const FString &MRecPosition);
    void SetMRecPlacement(const FString &AdUnitIdentifier, const FString &Placement);
    void SetMRecExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);
    void UpdateMRecPosition(const FString &AdUnitIdentifier, const FString &MRecPosition);
    void ShowMRec(const FString &AdUnitIdentifier);
    void HideMRec(const FString &AdUnitIdentifier);
    void DestroyMRec(const FString &AdUnitIdentifier);

    // MARK: Interstitials
    void LoadInterstitial(const FString &AdUnitIdentifier);
    bool IsInterstitialReady(const FString &AdUnitIdentifier);
    void ShowInterstitial(const FString &AdUnitIdentifier, const FString &Placement);
    void SetInterstitialExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

    // MARK: Rewarded
    void LoadRewardedAd(const FString &AdUnitIdentifier);
    bool IsRewardedAdReady(const FString &AdUnitIdentifier);
    void ShowRewardedAd(const FString &AdUnitIdentifier, const FString &Placement);
    void SetRewardedAdExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value);

private:
    static FName GetClassName();

    FJavaClassMethod InitializeMethod;
    FJavaClassMethod IsInitializedMethod;

    FJavaClassMethod SetHasUserConsentMethod;
    FJavaClassMethod HasUserConsentMethod;
    FJavaClassMethod SetIsAgeRestrictedUserMethod;
    FJavaClassMethod IsAgeRestrictedUserMethod;
    FJavaClassMethod SetDoNotSellMethod;
    FJavaClassMethod IsDoNotSellMethod;

    
    FJavaClassMethod SetTermsAndPrivacyPolicyFlowEnabled;
    FJavaClassMethod SetPrivacyPolicyURL;
    FJavaClassMethod SetTermsOfServiceURL;

    FJavaClassMethod IsTabletMethod;
    FJavaClassMethod ShowMediationDebuggerMethod;
    FJavaClassMethod SetUserIdMethod;
    FJavaClassMethod SetMutedMethod;
    FJavaClassMethod IsMutedMethod;
    FJavaClassMethod SetVerboseLoggingEnabledMethod;
    FJavaClassMethod IsVerboseLoggingEnabledMethod;
    FJavaClassMethod SetCreativeDebuggerEnabledMethod;
    FJavaClassMethod SetTestDeviceAdvertisingIdentifiersMethod;

    FJavaClassMethod TrackEventMethod;

    FJavaClassMethod CreateBannerMethod;
    FJavaClassMethod SetBannerBackgroundColorMethod;
    FJavaClassMethod SetBannerPlacementMethod;
    FJavaClassMethod SetBannerExtraParameterMethod;
    FJavaClassMethod UpdateBannerPositionMethod;
    FJavaClassMethod ShowBannerMethod;
    FJavaClassMethod HideBannerMethod;
    FJavaClassMethod DestroyBannerMethod;

    FJavaClassMethod CreateMRecMethod;
    FJavaClassMethod SetMRecPlacementMethod;
    FJavaClassMethod SetMRecExtraParameterMethod;
    FJavaClassMethod UpdateMRecPositionMethod;
    FJavaClassMethod ShowMRecMethod;
    FJavaClassMethod HideMRecMethod;
    FJavaClassMethod DestroyMRecMethod;

    FJavaClassMethod LoadInterstitialMethod;
    FJavaClassMethod IsInterstitialReadyMethod;
    FJavaClassMethod ShowInterstitialMethod;
    FJavaClassMethod SetInterstitialExtraParameterMethod;

    FJavaClassMethod LoadRewardedAdMethod;
    FJavaClassMethod IsRewardedAdReadyMethod;
    FJavaClassMethod ShowRewardedAdMethod;
    FJavaClassMethod SetRewardedAdExtraParameterMethod;
};

#endif
