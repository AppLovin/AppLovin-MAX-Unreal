// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAX.h"
#include "AppLovinMAXLogger.h"
#include "AppLovinMAXUtils.h"

#if PLATFORM_IOS
#include "IOS/IOSAppDelegate.h"

THIRD_PARTY_INCLUDES_START
#include <MAX_Unreal_Plugin/MAX_Unreal_Plugin.h>
THIRD_PARTY_INCLUDES_END

#elif PLATFORM_ANDROID
#include "Android/AndroidJavaAppLovinMAX.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

// MARK: Initialization (T)

void UAppLovinMAX::Initialize()
{
    // Pass empty string as SDK key
    UAppLovinMAX::Initialize(FString());
}

void UAppLovinMAX::Initialize(const FString &SdkKey)
{
    FString PluginVersion = FEngineVersion::Current().ToString(EVersionComponent::Patch);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() initialize:PluginVersion.GetNSString() sdkKey:SdkKey.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->Initialize(PluginVersion, SdkKey);
#endif
}

bool UAppLovinMAX::IsInitialized()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isInitialized];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsInitialized();
#else
    return false;
#endif
}

// MARK: Privacy (T)

void UAppLovinMAX::SetHasUserConsent(bool bHasUserConsent)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setHasUserConsent:bHasUserConsent];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetHasUserConsent(bHasUserConsent);
#endif
}

bool UAppLovinMAX::HasUserConsent()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() hasUserConsent];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->HasUserConsent();
#else
    return false;
#endif
}

void UAppLovinMAX::SetIsAgeRestrictedUser(bool bIsAgeRestrictedUser)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setIsAgeRestrictedUser:bIsAgeRestrictedUser];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetIsAgeRestrictedUser(bIsAgeRestrictedUser);
#endif
}

bool UAppLovinMAX::IsAgeRestrictedUser()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isAgeRestrictedUser];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsAgeRestrictedUser();
#else
    return false;
#endif
}

void UAppLovinMAX::SetDoNotSell(bool bDoNotSell)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setDoNotSell:bDoNotSell];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetDoNotSell(bDoNotSell);
#endif
}

bool UAppLovinMAX::IsDoNotSell()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isDoNotSell];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsDoNotSell();
#else
    return false;
#endif
}

// MARK: General (T)

bool UAppLovinMAX::IsTablet()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isTablet];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsTablet();
#else
    return false;
#endif
}

void UAppLovinMAX::ShowMediationDebugger()
{
#if PLATFORM_IOS
    [GetIOSPlugin() showMediationDebugger];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowMediationDebugger();
#endif
}

void UAppLovinMAX::SetUserId(const FString &UserId)
{
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setUserId:UserId.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetUserId(UserId);
#endif
}

void UAppLovinMAX::SetMuted(bool bMuted)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setMuted:bMuted];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetMuted(bMuted);
#endif
}

bool UAppLovinMAX::IsMuted()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isMuted];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsMuted();
#else
    return false;
#endif
}

void UAppLovinMAX::SetVerboseLoggingEnabled(bool bEnabled)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setVerboseLoggingEnabled:bEnabled];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetVerboseLoggingEnabled(bEnabled);
#endif
}

bool UAppLovinMAX::IsVerboseLoggingEnabled()
{
#if PLATFORM_IOS
    return [GetIOSPlugin() isVerboseLoggingEnabled];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsVerboseLoggingEnabled();
#else
    return false;
#endif
}

void UAppLovinMAX::SetCreativeDebuggerEnabled(bool bEnabled)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setCreativeDebuggerEnabled:bEnabled];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetCreativeDebuggerEnabled(bEnabled);
#endif
}

void UAppLovinMAX::SetTestDeviceAdvertisingIdentifiers(const TArray<FString> &AdvertisingIdentifiers)
{
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setTestDeviceAdvertisingIds:GetNSArray(AdvertisingIdentifiers)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetTestDeviceAdvertisingIdentifiers(AdvertisingIdentifiers);
#endif
}

// MARK: Event Tracking (T)

void UAppLovinMAX::TrackEvent(const FString &Name)
{
    TMap<FString, FString> EmptyParameters;
    UAppLovinMAX::TrackEvent(Name, EmptyParameters);
}

void UAppLovinMAX::TrackEvent(const FString &Name, const TMap<FString, FString> &Parameters)
{
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() trackEvent:Name.GetNSString() parameters:GetNSDictionary(Parameters)];
#elif PLATFORM_ANDROID
    FString SerializedParameters = AppLovinMAXUtils::ParseMapIntoString(Parameters);
    GetAndroidPlugin()->TrackEvent(Name, SerializedParameters);
#endif
}

// MARK: Banners

void UAppLovinMAX::CreateBanner(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("create banner"));
    const FString BannerPositionString = GetAdViewPositionString(BannerPosition);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() createBannerWithAdUnitIdentifier:AdUnitIdentifier.GetNSString() atPosition:BannerPositionString.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->CreateBanner(AdUnitIdentifier, BannerPositionString);
#endif
}

void UAppLovinMAX::SetBannerBackgroundColor(const FString &AdUnitIdentifier, const FColor &Color)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set banner background color"));
    FString HexColorCode = AppLovinMAXUtils::ParseColor(Color);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setBannerBackgroundColorForAdUnitIdentifier:AdUnitIdentifier.GetNSString() hexColorCode:HexColorCode.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerBackgroundColor(AdUnitIdentifier, HexColorCode);
#endif
}

void UAppLovinMAX::SetBannerPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set banner placement"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setBannerPlacement:Placement.GetNSString() forAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerPlacement(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetBannerExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set banner extra parameter"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setBannerExtraParameterForAdUnitIdentifier:AdUnitIdentifier.GetNSString() key:Key.GetNSString() value:Value.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

void UAppLovinMAX::UpdateBannerPosition(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("update banner position"));
    const FString BannerPositionString = GetAdViewPositionString(BannerPosition);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() updateBannerPosition:BannerPositionString.GetNSString() forAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->UpdateBannerPosition(AdUnitIdentifier, BannerPositionString);
#endif
}

void UAppLovinMAX::ShowBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("show banner"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() showBannerWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowBanner(AdUnitIdentifier);
#endif
}

void UAppLovinMAX::HideBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("hide banner"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() hideBannerWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->HideBanner(AdUnitIdentifier);
#endif
}

void UAppLovinMAX::DestroyBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("destroy banner"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() destroyBannerWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->DestroyBanner(AdUnitIdentifier);
#endif
}

// MARK: MRECs

void UAppLovinMAX::CreateMRec(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("create MREC"));
    const FString MRecPositionString = GetAdViewPositionString(MRecPosition);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() createMRecWithAdUnitIdentifier:AdUnitIdentifier.GetNSString() atPosition:MRecPositionString.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->CreateMRec(AdUnitIdentifier, MRecPositionString);
#endif
}

void UAppLovinMAX::SetMRecPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set MREC placement"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setMRecPlacement:Placement.GetNSString() forAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetMRecPlacement(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetMRecExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set MREC extra parameter"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setMRecExtraParameterForAdUnitIdentifier:AdUnitIdentifier.GetNSString() key:Key.GetNSString() value:Value.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetMRecExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

void UAppLovinMAX::UpdateMRecPosition(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("update MREC position"));
    const FString MRecPositionString = GetAdViewPositionString(MRecPosition);
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() updateMRecPosition:MRecPositionString.GetNSString() forAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->UpdateMRecPosition(AdUnitIdentifier, MRecPositionString);
#endif
}

void UAppLovinMAX::ShowMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("show MREC"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() showMRecWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowMRec(AdUnitIdentifier);
#endif
}

void UAppLovinMAX::HideMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("hide MREC"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() hideMRecWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->HideMRec(AdUnitIdentifier);
#endif
}

void UAppLovinMAX::DestroyMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("destroy MREC"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() destroyMRecWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->DestroyMRec(AdUnitIdentifier);
#endif
}

// MARK: Interstitials (T)

void UAppLovinMAX::LoadInterstitial(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("load interstitial"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() loadInterstitialWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->LoadInterstitial(AdUnitIdentifier);
#endif
}

bool UAppLovinMAX::IsInterstitialReady(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("check interstitial loaded"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    return [GetIOSPlugin() isInterstitialReadyWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsInterstitialReady(AdUnitIdentifier);
#else
    return false;
#endif
}

void UAppLovinMAX::ShowInterstitial(const FString &AdUnitIdentifier)
{
    // Pass empty string as placement
    UAppLovinMAX::ShowInterstitial(AdUnitIdentifier, FString());
}

void UAppLovinMAX::ShowInterstitial(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("show interstitial"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() showInterstitialWithAdUnitIdentifier:AdUnitIdentifier.GetNSString() placement:Placement.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowInterstitial(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetInterstitialExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set interstitial extra parameter"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setInterstitialExtraParameterForAdUnitIdentifier:AdUnitIdentifier.GetNSString() key:Key.GetNSString() value:Value.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetInterstitialExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

// MARK: Rewarded (T)

void UAppLovinMAX::LoadRewardedAd(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("load rewarded ad"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() loadRewardedAdWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->LoadRewardedAd(AdUnitIdentifier);
#endif
}

bool UAppLovinMAX::IsRewardedAdReady(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("check rewarded ad loaded"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    return [GetIOSPlugin() isRewardedAdReadyWithAdUnitIdentifier:AdUnitIdentifier.GetNSString()];
#elif PLATFORM_ANDROID
    return GetAndroidPlugin()->IsRewardedAdReady(AdUnitIdentifier);
#else
    return false;
#endif
}

void UAppLovinMAX::ShowRewardedAd(const FString &AdUnitIdentifier)
{
    // Pass empty string as placement
    UAppLovinMAX::ShowRewardedAd(AdUnitIdentifier, FString());
}

void UAppLovinMAX::ShowRewardedAd(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("show rewarded ad"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() showRewardedAdWithAdUnitIdentifier:AdUnitIdentifier.GetNSString() placement:Placement.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowRewardedAd(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetRewardedAdExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, TEXT("set rewarded ad extra parameter"));
#if PLATFORM_IOS
    SCOPED_AUTORELEASE_POOL;
    [GetIOSPlugin() setRewardedAdExtraParameterForAdUnitIdentifier:AdUnitIdentifier.GetNSString() key:Key.GetNSString() value:Value.GetNSString()];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetRewardedAdExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

// MARK: - Protected Methods

FString UAppLovinMAX::GetAdViewPositionString(EAdViewPosition Position)
{
    if (Position == EAdViewPosition::TopLeft)
    {
        return TEXT("top_left");
    }
    else if (Position == EAdViewPosition::TopCenter)
    {
        return TEXT("top_center");
    }
    else if (Position == EAdViewPosition::TopRight)
    {
        return TEXT("top_right");
    }
    else if (Position == EAdViewPosition::Centered)
    {
        return TEXT("centered");
    }
    else if (Position == EAdViewPosition::CenterLeft)
    {
        return TEXT("center_left");
    }
    else if (Position == EAdViewPosition::CenterRight)
    {
        return TEXT("center_right");
    }
    else if (Position == EAdViewPosition::BottomLeft)
    {
        return TEXT("bottom_left");
    }
    else if (Position == EAdViewPosition::BottomCenter)
    {
        return TEXT("bottom_center");
    }
    else  // Position == EAdViewPosition::BottomRight
    {
        return TEXT("bottom_right");
    }
}

void UAppLovinMAX::ValidateAdUnitIdentifier(const FString &AdUnitIdentifier, const FString &DebugPurpose)
{
    if (AdUnitIdentifier.IsEmpty())
    {
        MAX_USER_ERROR("No MAX Ads Ad Unit ID specified for: %s", *DebugPurpose);
    }
}

// MARK: Event Forwarding

// Static Delegate Initialization
UAppLovinMAX::FOnSdkInitializedDelegate UAppLovinMAX::OnSdkInitializedDelegate;
UAppLovinMAX::FOnBannerAdLoadedDelegate UAppLovinMAX::OnBannerAdLoadedDelegate;
UAppLovinMAX::FOnBannerAdLoadFailedDelegate UAppLovinMAX::OnBannerAdLoadFailedDelegate;
UAppLovinMAX::FOnBannerAdClickedDelegate UAppLovinMAX::OnBannerAdClickedDelegate;
UAppLovinMAX::FOnBannerAdExpandedDelegate UAppLovinMAX::OnBannerAdExpandedDelegate;
UAppLovinMAX::FOnBannerAdCollapsedDelegate UAppLovinMAX::OnBannerAdCollapsedDelegate;
UAppLovinMAX::FOnBannerAdRevenuePaidDelegate UAppLovinMAX::OnBannerAdRevenuePaidDelegate;
UAppLovinMAX::FOnMRecAdLoadedDelegate UAppLovinMAX::OnMRecAdLoadedDelegate;
UAppLovinMAX::FOnMRecAdLoadFailedDelegate UAppLovinMAX::OnMRecAdLoadFailedDelegate;
UAppLovinMAX::FOnMRecAdClickedDelegate UAppLovinMAX::OnMRecAdClickedDelegate;
UAppLovinMAX::FOnMRecAdExpandedDelegate UAppLovinMAX::OnMRecAdExpandedDelegate;
UAppLovinMAX::FOnMRecAdCollapsedDelegate UAppLovinMAX::OnMRecAdCollapsedDelegate;
UAppLovinMAX::FOnMRecAdRevenuePaidDelegate UAppLovinMAX::OnMRecAdRevenuePaidDelegate;
UAppLovinMAX::FOnInterstitialLoadedDelegate UAppLovinMAX::OnInterstitialLoadedDelegate;
UAppLovinMAX::FOnInterstitialLoadFailedDelegate UAppLovinMAX::OnInterstitialLoadFailedDelegate;
UAppLovinMAX::FOnInterstitialHiddenDelegate UAppLovinMAX::OnInterstitialHiddenDelegate;
UAppLovinMAX::FOnInterstitialDisplayedDelegate UAppLovinMAX::OnInterstitialDisplayedDelegate;
UAppLovinMAX::FOnInterstitialAdFailedToDisplayDelegate UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate;
UAppLovinMAX::FOnInterstitialClickedDelegate UAppLovinMAX::OnInterstitialClickedDelegate;
UAppLovinMAX::FOnInterstitialAdRevenuePaidDelegate UAppLovinMAX::OnInterstitialAdRevenuePaidDelegate;
UAppLovinMAX::FOnRewardedAdLoadedDelegate UAppLovinMAX::OnRewardedAdLoadedDelegate;
UAppLovinMAX::FOnRewardedAdLoadFailedDelegate UAppLovinMAX::OnRewardedAdLoadFailedDelegate;
UAppLovinMAX::FOnRewardedAdDisplayedDelegate UAppLovinMAX::OnRewardedAdDisplayedDelegate;
UAppLovinMAX::FOnRewardedAdHiddenDelegate UAppLovinMAX::OnRewardedAdHiddenDelegate;
UAppLovinMAX::FOnRewardedAdClickedDelegate UAppLovinMAX::OnRewardedAdClickedDelegate;
UAppLovinMAX::FOnRewardedAdRevenuePaidDelegate UAppLovinMAX::OnRewardedAdRevenuePaidDelegate;
UAppLovinMAX::FOnRewardedAdFailedToDisplayDelegate UAppLovinMAX::OnRewardedAdFailedToDisplayDelegate;
UAppLovinMAX::FOnRewardedAdReceivedRewardDelegate UAppLovinMAX::OnRewardedAdReceivedRewardDelegate;

void ForwardEvent(const FString &Name, const TMap<FString, FString> &Body)
{
    if (Name == TEXT("OnSdkInitializedEvent"))
    {
        FSdkConfiguration SdkConfiguration(Body);
        UAppLovinMAX::OnSdkInitializedDelegate.Broadcast(SdkConfiguration);
    }
    else  // Ad Events
    {
        FAdInfo AdInfo(Body);

        FAdError AdError{FCString::Atoi(*Body.FindRef(TEXT("errorCode"))), Body.FindRef(TEXT("errorMessage")), Body.FindRef(TEXT("errorAdLoadFailureInfo"))};

        if (Name == TEXT("OnBannerAdLoadedEvent"))
        {
            UAppLovinMAX::OnBannerAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnBannerAdLoadFailedEvent"))
        {
            UAppLovinMAX::OnBannerAdLoadFailedDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnBannerAdClickedEvent"))
        {
            UAppLovinMAX::OnBannerAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnBannerAdExpandedEvent"))
        {
            UAppLovinMAX::OnBannerAdExpandedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnBannerAdCollapsedEvent"))
        {
            UAppLovinMAX::OnBannerAdCollapsedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnBannerAdRevenuePaidEvent"))
        {
            UAppLovinMAX::OnBannerAdRevenuePaidDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnMRecAdLoadedEvent"))
        {
            UAppLovinMAX::OnMRecAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnMRecAdLoadFailedEvent"))
        {
            UAppLovinMAX::OnMRecAdLoadFailedDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnMRecAdClickedEvent"))
        {
            UAppLovinMAX::OnMRecAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnMRecAdExpandedEvent"))
        {
            UAppLovinMAX::OnMRecAdExpandedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnMRecAdCollapsedEvent"))
        {
            UAppLovinMAX::OnMRecAdCollapsedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnMRecAdRevenuePaidEvent"))
        {
            UAppLovinMAX::OnMRecAdRevenuePaidDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnInterstitialLoadedEvent"))
        {
            UAppLovinMAX::OnInterstitialLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnInterstitialLoadFailedEvent"))
        {
            UAppLovinMAX::OnInterstitialLoadFailedDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnInterstitialHiddenEvent"))
        {
            UAppLovinMAX::OnInterstitialHiddenDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnInterstitialDisplayedEvent"))
        {
            UAppLovinMAX::OnInterstitialDisplayedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnInterstitialAdFailedToDisplayEvent"))
        {
            UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnInterstitialClickedEvent"))
        {
            UAppLovinMAX::OnInterstitialClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnInterstitialAdRevenuePaidEvent"))
        {
            UAppLovinMAX::OnInterstitialAdRevenuePaidDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdLoadedEvent"))
        {
            UAppLovinMAX::OnRewardedAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdLoadFailedEvent"))
        {
            UAppLovinMAX::OnRewardedAdLoadFailedDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnRewardedAdDisplayedEvent"))
        {
            UAppLovinMAX::OnRewardedAdDisplayedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdHiddenEvent"))
        {
            UAppLovinMAX::OnRewardedAdHiddenDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdClickedEvent"))
        {
            UAppLovinMAX::OnRewardedAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdRevenuePaidEvent"))
        {
            UAppLovinMAX::OnRewardedAdRevenuePaidDelegate.Broadcast(AdInfo);
        }
        else if (Name == TEXT("OnRewardedAdFailedToDisplayEvent"))
        {
            UAppLovinMAX::OnRewardedAdFailedToDisplayDelegate.Broadcast(AdInfo, AdError);
        }
        else if (Name == TEXT("OnRewardedAdReceivedRewardEvent"))
        {
            FAdReward Reward{Body.FindRef(TEXT("rewardLabel")), FCString::Atoi(*Body.FindRef(TEXT("rewardAmount")))};
            UAppLovinMAX::OnRewardedAdReceivedRewardDelegate.Broadcast(AdInfo, Reward);
        }
        else
        {
            MAX_USER_WARN("Unknown MAX Ads event fired: %s", *Name);
        }
    }
}

// MARK: IOS (T)

#if PLATFORM_IOS

NSArray<NSString *> *UAppLovinMAX::GetNSArray(const TArray<FString> &Array)
{
    NSMutableArray<NSString *> *NewArray = [NSMutableArray arrayWithCapacity:Array.Num()];
    for (const FString &Element : Array)
    {
        [NewArray addObject:Element.GetNSString()];
    }
    return NewArray;
}

NSDictionary<NSString *, NSString *> *UAppLovinMAX::GetNSDictionary(const TMap<FString, FString> &Map)
{
    NSMutableDictionary<NSString *, NSString *> *NewDictionary = [NSMutableDictionary dictionaryWithCapacity:Map.Num()];
    for (const TPair<FString, FString> &Entry : Map)
    {
        NewDictionary[Entry.Key.GetNSString()] = Entry.Value.GetNSString();
    }
    return NewDictionary;
}

extern "C" void ForwardIOSEvent(NSString *Name, NSString *Params)
{
    const char *NameCString = [Name UTF8String];
    const char *ParamsCString = [Params UTF8String];
    TMap<FString, FString> ParamsMap = AppLovinMAXUtils::ParseStringIntoMap((FString(ParamsCString)));
    ForwardEvent((FString(NameCString)), ParamsMap);
}

MAUnrealPlugin *UAppLovinMAX::GetIOSPlugin()
{
    static MAUnrealPlugin *PluginInstance = nil;
    static dispatch_once_t OnceToken;
    dispatch_once(&OnceToken, ^{
      UIView *MainView = (UIView *)[IOSAppDelegate GetDelegate].IOSView;
      PluginInstance = [[MAUnrealPlugin alloc] initWithView:MainView eventCallback:&ForwardIOSEvent];
    });
    return PluginInstance;
}

#endif

// MARK: Android (T)

#if PLATFORM_ANDROID

// Implementation for method defined in AppLovinMAX_UPL_Android.xml
extern "C" JNIEXPORT void JNICALL Java_com_epicgames_ue4_GameActivity_00024AppLovinMAXListener_forwardEvent(JNIEnv *env, jobject thiz, jstring name, jstring params)
{
    FString Name = FJavaHelper::FStringFromParam(env, name);
    FString Params = FJavaHelper::FStringFromParam(env, params);
    TMap<FString, FString> ParamsMap = AppLovinMAXUtils::ParseStringIntoMap(Params);
    ForwardEvent(Name, ParamsMap);
}

TSharedPtr<FJavaAndroidAppLovinMAX> UAppLovinMAX::GetAndroidPlugin()
{
    static TSharedPtr<FJavaAndroidAppLovinMAX, ESPMode::ThreadSafe> instance = MakeShared<FJavaAndroidAppLovinMAX, ESPMode::ThreadSafe>();
    return instance;
}

#endif
