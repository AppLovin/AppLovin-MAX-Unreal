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
    [GetIOSPlugin() initialize:GetNSString(PluginVersion) sdkKey:GetNSString(SdkKey)];
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

void UAppLovinMAX::ShowMediationDebugger()
{
#if PLATFORM_IOS
    [GetIOSPlugin() showMediationDebugger];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowMediationDebugger();
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
#if PLATFORM_IOS  // t
    return [GetIOSPlugin() isDoNotSell];
#elif PLATFORM_ANDROID  // t
    return GetAndroidPlugin()->IsDoNotSell();
#else
    return false;
#endif
}

// MARK: General (T)

bool UAppLovinMAX::IsTablet()
{
#if PLATFORM_IOS // nt
    return [GetIOSPlugin() isTablet];
#elif PLATFORM_ANDROID // nt
    return GetAndroidPlugin()->IsTablet();
#else
    return false;
#endif
}

void UAppLovinMAX::SetUserId(const FString &UserId)
{
#if PLATFORM_IOS
    [GetIOSPlugin() setUserId:GetNSString(UserId)];
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
    [GetIOSPlugin() trackEvent:GetNSString(Name) parameters:GetNSDictionary(Parameters)];
#elif PLATFORM_ANDROID
    FString SerializedParameters = AppLovinMAXUtils::ParseMapIntoString(Parameters);
    GetAndroidPlugin()->TrackEvent(Name, SerializedParameters);
#endif
}

// MARK: Banners

// T
void UAppLovinMAX::CreateBanner(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "create banner");
    const FString BannerPositionString = GetAdViewPositionString(BannerPosition);
#if PLATFORM_IOS
    [GetIOSPlugin() createBannerWithAdUnitIdentifier:GetNSString(AdUnitIdentifier) atPosition:GetNSString(BannerPositionString)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->CreateBanner(AdUnitIdentifier, BannerPositionString);
#endif
}

// t
void UAppLovinMAX::SetBannerBackgroundColor(const FString &AdUnitIdentifier, const FColor &Color)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set banner background color");
    FString HexColorCode = AppLovinMAXUtils::ParseColor(Color);
#if PLATFORM_IOS
    [GetIOSPlugin() setBannerBackgroundColorForAdUnitIdentifier:GetNSString(AdUnitIdentifier) hexColorCode:GetNSString(HexColorCode)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerBackgroundColor(AdUnitIdentifier, HexColorCode);
#endif
}

void UAppLovinMAX::SetBannerPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set banner placement");
#if PLATFORM_IOS
    [GetIOSPlugin() setBannerPlacement:GetNSString(Placement) forAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerPlacement(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetBannerExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set banner extra parameter");
#if PLATFORM_IOS
    [GetIOSPlugin() setBannerExtraParameterForAdUnitIdentifier:GetNSString(AdUnitIdentifier) key:GetNSString(Key) value:GetNSString(Value)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetBannerExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

void UAppLovinMAX::UpdateBannerPosition(const FString &AdUnitIdentifier, EAdViewPosition BannerPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "update banner position");
    const FString BannerPositionString = GetAdViewPositionString(BannerPosition);
#if PLATFORM_IOS // T
    [GetIOSPlugin() updateBannerPosition:GetNSString(BannerPositionString) forAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->UpdateBannerPosition(AdUnitIdentifier, BannerPositionString);
#endif
}

// T
void UAppLovinMAX::ShowBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "show banner");
#if PLATFORM_IOS
    [GetIOSPlugin() showBannerWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowBanner(AdUnitIdentifier);
#endif
}

// T
void UAppLovinMAX::HideBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "hide banner");
#if PLATFORM_IOS
    [GetIOSPlugin() hideBannerWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->HideBanner(AdUnitIdentifier);
#endif
}

// nt
void UAppLovinMAX::DestroyBanner(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "destroy banner");
#if PLATFORM_IOS
    [GetIOSPlugin() destroyBannerWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->DestroyBanner(AdUnitIdentifier);
#endif
}

// MARK: MRECs

// T
void UAppLovinMAX::CreateMRec(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "create MREC");
    const FString MRecPositionString = GetAdViewPositionString(MRecPosition);
#if PLATFORM_IOS
    [GetIOSPlugin() createMRecWithAdUnitIdentifier:GetNSString(AdUnitIdentifier) atPosition:GetNSString(MRecPositionString)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->CreateMRec(AdUnitIdentifier, MRecPositionString);
#endif
}

// nt
void UAppLovinMAX::SetMRecPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set MREC placement");
#if PLATFORM_IOS
    [GetIOSPlugin() setMRecPlacement:GetNSString(Placement) forAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetMRecPlacement(AdUnitIdentifier, Placement);
#endif
}

// nt
void UAppLovinMAX::SetMRecExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set MREC extra parameter");
#if PLATFORM_IOS
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetMRecExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

// t
void UAppLovinMAX::UpdateMRecPosition(const FString &AdUnitIdentifier, EAdViewPosition MRecPosition)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "update MREC position");
    const FString MRecPositionString = GetAdViewPositionString(MRecPosition);
#if PLATFORM_IOS
    [GetIOSPlugin() updateMRecPosition:GetNSString(MRecPositionString) forAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->UpdateMRecPosition(AdUnitIdentifier, MRecPositionString);
#endif
}

// T
void UAppLovinMAX::ShowMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "show MREC");
#if PLATFORM_IOS
    [GetIOSPlugin() showMRecWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowMRec(AdUnitIdentifier);
#endif
}

// T
void UAppLovinMAX::HideMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "hide MREC");
#if PLATFORM_IOS
    [GetIOSPlugin() hideMRecWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->HideMRec(AdUnitIdentifier);
#endif
}

// nt
void UAppLovinMAX::DestroyMRec(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "destroy MREC");
#if PLATFORM_IOS
    [GetIOSPlugin() destroyMRecWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->DestroyMRec(AdUnitIdentifier);
#endif
}

// MARK: Interstitials (T)

void UAppLovinMAX::LoadInterstitial(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "load interstitial");
#if PLATFORM_IOS
    [GetIOSPlugin() loadInterstitialWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->LoadInterstitial(AdUnitIdentifier);
#endif
}

bool UAppLovinMAX::IsInterstitialReady(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "check interstitial loaded");
#if PLATFORM_IOS
    return [GetIOSPlugin() isInterstitialReadyWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
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
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "show interstitial");
#if PLATFORM_IOS
    [GetIOSPlugin() showInterstitialWithAdUnitIdentifier:GetNSString(AdUnitIdentifier) placement:GetNSString(Placement)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowInterstitial(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetInterstitialExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set interstitial extra parameter");
#if PLATFORM_IOS
    [GetIOSPlugin() setInterstitialExtraParameterForAdUnitIdentifier:GetNSString(AdUnitIdentifier) key:GetNSString(Key) value:GetNSString(Value)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetInterstitialExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

// MARK: Rewarded (T)

void UAppLovinMAX::LoadRewardedAd(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "load rewarded ad");
#if PLATFORM_IOS
    [GetIOSPlugin() loadRewardedAdWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->LoadRewardedAd(AdUnitIdentifier);
#endif
}

bool UAppLovinMAX::IsRewardedAdReady(const FString &AdUnitIdentifier)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "check rewarded ad loaded");
#if PLATFORM_IOS
    return [GetIOSPlugin() isRewardedAdReadyWithAdUnitIdentifier:GetNSString(AdUnitIdentifier)];
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
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "show rewarded ad");
#if PLATFORM_IOS
    [GetIOSPlugin() showRewardedAdWithAdUnitIdentifier:GetNSString(AdUnitIdentifier) placement:GetNSString(Placement)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->ShowRewardedAd(AdUnitIdentifier, Placement);
#endif
}

void UAppLovinMAX::SetRewardedAdExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    UAppLovinMAX::ValidateAdUnitIdentifier(AdUnitIdentifier, "set rewarded ad extra parameter");
#if PLATFORM_IOS
    [GetIOSPlugin() setRewardedAdExtraParameterForAdUnitIdentifier:GetNSString(AdUnitIdentifier) key:GetNSString(Key) value:GetNSString(Value)];
#elif PLATFORM_ANDROID
    GetAndroidPlugin()->SetRewardedAdExtraParameter(AdUnitIdentifier, Key, Value);
#endif
}

// MARK: - Protected Methods

// T
FString UAppLovinMAX::GetAdViewPositionString(EAdViewPosition Position)
{
    if (Position == EAdViewPosition::TopLeft)
    {
        return "top_left";
    }
    else if (Position == EAdViewPosition::TopCenter)
    {
        return "top_center";
    }
    else if (Position == EAdViewPosition::TopRight)
    {
        return "top_right";
    }
    else if (Position == EAdViewPosition::Centered)
    {
        return "centered";
    }
    else if (Position == EAdViewPosition::CenterLeft)
    {
        return "center_left";
    }
    else if (Position == EAdViewPosition::CenterRight)
    {
        return "center_right";
    }
    else if (Position == EAdViewPosition::BottomLeft)
    {
        return "bottom_left";
    }
    else if (Position == EAdViewPosition::BottomCenter)
    {
        return "bottom_center";
    }
    else  // Position == EAdViewPosition::BottomRight
    {
        return "bottom_right";
    }
}

// T
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
UAppLovinMAX::FOnMRecAdLoadedDelegate UAppLovinMAX::OnMRecAdLoadedDelegate;
UAppLovinMAX::FOnMRecAdLoadFailedDelegate UAppLovinMAX::OnMRecAdLoadFailedDelegate;
UAppLovinMAX::FOnMRecAdClickedDelegate UAppLovinMAX::OnMRecAdClickedDelegate;
UAppLovinMAX::FOnMRecAdExpandedDelegate UAppLovinMAX::OnMRecAdExpandedDelegate;
UAppLovinMAX::FOnMRecAdCollapsedDelegate UAppLovinMAX::OnMRecAdCollapsedDelegate;
UAppLovinMAX::FOnInterstitialLoadedDelegate UAppLovinMAX::OnInterstitialLoadedDelegate;
UAppLovinMAX::FOnInterstitialLoadFailedDelegate UAppLovinMAX::OnInterstitialLoadFailedDelegate;
UAppLovinMAX::FOnInterstitialHiddenDelegate UAppLovinMAX::OnInterstitialHiddenDelegate;
UAppLovinMAX::FOnInterstitialDisplayedDelegate UAppLovinMAX::OnInterstitialDisplayedDelegate;
UAppLovinMAX::FOnInterstitialAdFailedToDisplayDelegate UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate;
UAppLovinMAX::FOnInterstitialClickedDelegate UAppLovinMAX::OnInterstitialClickedDelegate;
UAppLovinMAX::FOnRewardedAdLoadedDelegate UAppLovinMAX::OnRewardedAdLoadedDelegate;
UAppLovinMAX::FOnRewardedAdLoadFailedDelegate UAppLovinMAX::OnRewardedAdLoadFailedDelegate;
UAppLovinMAX::FOnRewardedAdDisplayedDelegate UAppLovinMAX::OnRewardedAdDisplayedDelegate;
UAppLovinMAX::FOnRewardedAdHiddenDelegate UAppLovinMAX::OnRewardedAdHiddenDelegate;
UAppLovinMAX::FOnRewardedAdClickedDelegate UAppLovinMAX::OnRewardedAdClickedDelegate;
UAppLovinMAX::FOnRewardedAdFailedToDisplayDelegate UAppLovinMAX::OnRewardedAdFailedToDisplayDelegate;
UAppLovinMAX::FOnRewardedAdReceivedRewardDelegate UAppLovinMAX::OnRewardedAdReceivedRewardDelegate;

void ForwardEvent(const FString &Name, const TMap<FString, FString> &Body)
{
    if (Name == "OnSdkInitializedEvent")
    {
        FSdkConfiguration SdkConfiguration(Body);
        UAppLovinMAX::OnSdkInitializedDelegate.Broadcast(SdkConfiguration);
    }
    else  // Ad Events
    {
        FAdInfo AdInfo(Body);
        int ErrorCode = FCString::Atoi(*Body.FindRef("errorCode"));
        if (Name == "OnBannerAdLoadedEvent")
        {
            UAppLovinMAX::OnBannerAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnBannerAdLoadFailedEvent")
        {
            UAppLovinMAX::OnBannerAdLoadFailedDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnBannerAdClickedEvent")
        {
            UAppLovinMAX::OnBannerAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnBannerAdExpandedEvent")
        {
            UAppLovinMAX::OnBannerAdExpandedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnBannerAdCollapsedEvent")
        {
            UAppLovinMAX::OnBannerAdCollapsedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnMRecAdLoadedEvent")
        {
            UAppLovinMAX::OnMRecAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnMRecAdLoadFailedEvent")
        {
            UAppLovinMAX::OnMRecAdLoadFailedDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnMRecAdClickedEvent")
        {
            UAppLovinMAX::OnMRecAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnMRecAdExpandedEvent")
        {
            UAppLovinMAX::OnMRecAdExpandedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnMRecAdCollapsedEvent")
        {
            UAppLovinMAX::OnMRecAdCollapsedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnInterstitialLoadedEvent")
        {
            UAppLovinMAX::OnInterstitialLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnInterstitialLoadFailedEvent")
        {
            UAppLovinMAX::OnInterstitialLoadFailedDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnInterstitialHiddenEvent")
        {
            UAppLovinMAX::OnInterstitialHiddenDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnInterstitialDisplayedEvent")
        {
            UAppLovinMAX::OnInterstitialDisplayedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnInterstitialAdFailedToDisplayEvent")
        {
            UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnInterstitialClickedEvent")
        {
            UAppLovinMAX::OnInterstitialClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnRewardedAdLoadedEvent")
        {
            UAppLovinMAX::OnRewardedAdLoadedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnRewardedAdLoadFailedEvent")
        {
            UAppLovinMAX::OnRewardedAdLoadFailedDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnRewardedAdDisplayedEvent")
        {
            UAppLovinMAX::OnRewardedAdDisplayedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnRewardedAdHiddenEvent")
        {
            UAppLovinMAX::OnRewardedAdHiddenDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnRewardedAdClickedEvent")
        {
            UAppLovinMAX::OnRewardedAdClickedDelegate.Broadcast(AdInfo);
        }
        else if (Name == "OnRewardedAdFailedToDisplayEvent")
        {
            UAppLovinMAX::OnRewardedAdFailedToDisplayDelegate.Broadcast(AdInfo, ErrorCode);
        }
        else if (Name == "OnRewardedAdReceivedRewardEvent")
        {
            FReward Reward{Body.FindRef("rewardLabel"), FCString::Atoi(*Body.FindRef("rewardAmount"))};
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

NSString *UAppLovinMAX::GetNSString(const FString &String)
{
    return [NSString stringWithUTF8String:TCHAR_TO_ANSI(*String)];
}

NSArray<NSString *> *UAppLovinMAX::GetNSArray(const TArray<FString> &Array)
{
    NSMutableArray<NSString *> *NewArray = [[NSMutableArray arrayWithCapacity:Array.Num()] autorelease];
    for (auto &Element : Array)
    {
        [NewArray addObject:GetNSString(Element)];
    }
    return [NewArray copy];
}

NSDictionary<NSString *, NSString *> *UAppLovinMAX::GetNSDictionary(const TMap<FString, FString> &Map)
{
    NSMutableDictionary<NSString *, NSString *> *NewDictionary = [[NSMutableDictionary dictionaryWithCapacity:Map.Num()] autorelease];
    for (auto &Entry : Map)
    {
        NewDictionary[GetNSString(Entry.Key)] = GetNSString(*Entry.Value);
    }
    [NewDictionary autorelease];
    return [NewDictionary copy];
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
