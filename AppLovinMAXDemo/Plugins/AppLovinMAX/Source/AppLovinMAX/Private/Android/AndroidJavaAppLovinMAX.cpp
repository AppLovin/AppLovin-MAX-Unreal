// Copyright AppLovin Corporation. All Rights Reserved.

#include "AndroidJavaAppLovinMAX.h"

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"


FJavaAndroidAppLovinMAX::FJavaAndroidAppLovinMAX()
    : FJavaClassObject(GetClassName(), "(Landroid/app/Activity;)V", FAndroidApplication::GetGameActivityThis())
    , InitializeMethod(GetClassMethod("initialize", "(Ljava/lang/String;Ljava/lang/String;Lcom/applovin/unreal/AppLovinMAX$EventListener;)V"))
    , IsInitializedMethod(GetClassMethod("isInitialized", "()Z"))
    , ShowMediationDebuggerMethod(GetClassMethod("showMediationDebugger", "()V"))
    , SetHasUserConsentMethod(GetClassMethod("setHasUserConsent", "(Z)V"))
    , HasUserConsentMethod(GetClassMethod("hasUserConsent", "()Z"))
    , SetIsAgeRestrictedUserMethod(GetClassMethod("setIsAgeRestrictedUser", "(Z)V"))
    , IsAgeRestrictedUserMethod(GetClassMethod("isAgeRestrictedUser", "()Z"))
    , SetDoNotSellMethod(GetClassMethod("setDoNotSell", "(Z)V"))
    , IsDoNotSellMethod(GetClassMethod("isDoNotSell", "()Z"))
    , IsTabletMethod(GetClassMethod("isTablet", "()Z"))
    , SetUserIdMethod(GetClassMethod("setUserId", "(Ljava/lang/String;)V"))
    , SetMutedMethod(GetClassMethod("setMuted", "(Z)V"))
    , IsMutedMethod(GetClassMethod("isMuted", "()Z"))
    , SetVerboseLoggingEnabledMethod(GetClassMethod("setVerboseLoggingEnabled", "(Z)V"))
    , IsVerboseLoggingEnabledMethod(GetClassMethod("isVerboseLoggingEnabled", "()Z"))
    , SetCreativeDebuggerEnabledMethod(GetClassMethod("setCreativeDebuggerEnabled", "(Z)V"))
    , SetTestDeviceAdvertisingIdentifiersMethod(GetClassMethod("setTestDeviceAdvertisingIds", "([Ljava/lang/String;)V"))
    , TrackEventMethod(GetClassMethod("trackEvent", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , CreateBannerMethod(GetClassMethod("createBanner", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetBannerBackgroundColorMethod(GetClassMethod("setBannerBackgroundColor", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetBannerPlacementMethod(GetClassMethod("setBannerPlacement", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetBannerExtraParameterMethod(GetClassMethod("setBannerExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    , UpdateBannerPositionMethod(GetClassMethod("updateBannerPosition", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , ShowBannerMethod(GetClassMethod("showBanner", "(Ljava/lang/String;)V"))
    , HideBannerMethod(GetClassMethod("hideBanner", "(Ljava/lang/String;)V"))
    , DestroyBannerMethod(GetClassMethod("destroyBanner", "(Ljava/lang/String;)V"))
    , CreateMRecMethod(GetClassMethod("createMRec", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetMRecPlacementMethod(GetClassMethod("setMRecPlacement", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetMRecExtraParameterMethod(GetClassMethod("setMRecExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    , UpdateMRecPositionMethod(GetClassMethod("updateMRecPosition", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , ShowMRecMethod(GetClassMethod("showMRec", "(Ljava/lang/String;)V"))
    , HideMRecMethod(GetClassMethod("hideMRec", "(Ljava/lang/String;)V"))
    , DestroyMRecMethod(GetClassMethod("destroyMRec", "(Ljava/lang/String;)V"))
    , LoadInterstitialMethod(GetClassMethod("loadInterstitial", "(Ljava/lang/String;)V"))
    , IsInterstitialReadyMethod(GetClassMethod("isInterstitialReady", "(Ljava/lang/String;)Z"))
    , ShowInterstitialMethod(GetClassMethod("showInterstitial", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetInterstitialExtraParameterMethod(GetClassMethod("setInterstitialExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    , LoadRewardedAdMethod(GetClassMethod("loadRewardedAd", "(Ljava/lang/String;)V"))
    , IsRewardedAdReadyMethod(GetClassMethod("isRewardedAdReady", "(Ljava/lang/String;)Z"))
    , ShowRewardedAdMethod(GetClassMethod("showRewardedAd", "(Ljava/lang/String;Ljava/lang/String;)V"))
    , SetRewardedAdExtraParameterMethod(GetClassMethod("setRewardedAdExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
{ }

FJavaAndroidAppLovinMAX::~FJavaAndroidAppLovinMAX() { }

// MARK: - Initialization

void FJavaAndroidAppLovinMAX::Initialize(const FString &PluginVersion, const FString &SdkKey)
{
    JNIEnv* JEnv = FAndroidApplication::GetJavaEnv();

    // Create listener for Android plugin event handling
    jclass ListenerClass = FAndroidApplication::FindJavaClass("com/epicgames/ue4/GameActivity$AppLovinMAXListener");
    jmethodID Constructor = JEnv->GetMethodID(ListenerClass, "<init>", "()V");
    auto LocalListener = NewScopedJavaObject(JEnv, JEnv->NewObject(ListenerClass, Constructor));

    CallMethod<void>(InitializeMethod, *GetJString(PluginVersion), *GetJString(SdkKey), *LocalListener);
}

bool FJavaAndroidAppLovinMAX::IsInitialized()
{
    return CallMethod<bool>(IsInitializedMethod);
}

void FJavaAndroidAppLovinMAX::ShowMediationDebugger()
{
    CallMethod<void>(ShowMediationDebuggerMethod);
}

// MARK: - Privacy

void FJavaAndroidAppLovinMAX::SetHasUserConsent( bool bHasUserConsent )
{
    CallMethod<void>(SetHasUserConsentMethod, bHasUserConsent);
}

bool FJavaAndroidAppLovinMAX::HasUserConsent()
{
    return CallMethod<bool>(HasUserConsentMethod);
}

void FJavaAndroidAppLovinMAX::SetIsAgeRestrictedUser( bool bIsAgeRestrictedUser )
{
    CallMethod<void>(SetIsAgeRestrictedUserMethod, bIsAgeRestrictedUser);
}

bool FJavaAndroidAppLovinMAX::IsAgeRestrictedUser()
{
    return CallMethod<bool>(IsAgeRestrictedUserMethod);
}

void FJavaAndroidAppLovinMAX::SetDoNotSell( bool bDoNotSell )
{
    CallMethod<void>(SetDoNotSellMethod, bDoNotSell);
}

bool FJavaAndroidAppLovinMAX::IsDoNotSell()
{
    return CallMethod<bool>(IsDoNotSellMethod);
}

// MARK: - General

bool FJavaAndroidAppLovinMAX::IsTablet()
{
    return CallMethod<bool>(IsTabletMethod);
}

void FJavaAndroidAppLovinMAX::SetUserId( const FString &UserId )
{
    CallMethod<void>(SetUserIdMethod, *GetJString(UserId));
}

void FJavaAndroidAppLovinMAX::SetMuted( bool bMuted )
{
    CallMethod<void>(SetMutedMethod, bMuted);
}

bool FJavaAndroidAppLovinMAX::IsMuted()
{
    return CallMethod<bool>(IsMutedMethod);
}

void FJavaAndroidAppLovinMAX::SetVerboseLoggingEnabled( bool bEnabled )
{
    CallMethod<void>(SetVerboseLoggingEnabledMethod, bEnabled);
}

bool FJavaAndroidAppLovinMAX::IsVerboseLoggingEnabled()
{
    return CallMethod<bool>(IsVerboseLoggingEnabledMethod);
}

void FJavaAndroidAppLovinMAX::SetCreativeDebuggerEnabled( bool bEnabled )
{
    CallMethod<void>(SetCreativeDebuggerEnabledMethod, bEnabled);
}

void FJavaAndroidAppLovinMAX::SetTestDeviceAdvertisingIdentifiers( const TArray<FString> &AdvertisingIdentifiers )
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    // Convert TArray into Java array
    auto AdvertisingIdentifiersArray = NewScopedJavaObject(Env, (jobjectArray)Env->NewObjectArray(AdvertisingIdentifiers.Num(), FJavaWrapper::JavaStringClass, nullptr));
    for (uint32 Index = 0; Index < AdvertisingIdentifiers.Num(); Index++)
    {
        auto AdvertisingIdentifier = GetJString(AdvertisingIdentifiers[Index]);
        Env->SetObjectArrayElement(*AdvertisingIdentifiersArray, Index, *AdvertisingIdentifier);
    }

    CallMethod<void>(SetTestDeviceAdvertisingIdentifiersMethod, *AdvertisingIdentifiersArray);
}

// MARK: - Event Tracking

void FJavaAndroidAppLovinMAX::TrackEvent( const FString &Name, const FString &Parameters )
{
    CallMethod<void>(TrackEventMethod, *GetJString(Name), *GetJString(Parameters));
}

// MARK: - Banners

void FJavaAndroidAppLovinMAX::CreateBanner( const FString &AdUnitIdentifier, const FString &BannerPosition )
{
    return CallMethod<void>(CreateBannerMethod, *GetJString(AdUnitIdentifier), *GetJString(BannerPosition));
}

void FJavaAndroidAppLovinMAX::SetBannerBackgroundColor( const FString &AdUnitIdentifier, const FString &HexColorCode)
{
    return CallMethod<void>(SetBannerBackgroundColorMethod, *GetJString(AdUnitIdentifier), *GetJString(HexColorCode));
}

void FJavaAndroidAppLovinMAX::SetBannerPlacement( const FString &AdUnitIdentifier, const FString &Placement )
{
    return CallMethod<void>(SetBannerPlacementMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidAppLovinMAX::SetBannerExtraParameter( const FString &AdUnitIdentifier, const FString &Key, const FString &Value )
{
    return CallMethod<void>(SetBannerExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

void FJavaAndroidAppLovinMAX::UpdateBannerPosition( const FString &AdUnitIdentifier, const FString &BannerPosition )
{
    return CallMethod<void>(UpdateBannerPositionMethod, *GetJString(AdUnitIdentifier), *GetJString(BannerPosition));
}

void FJavaAndroidAppLovinMAX::ShowBanner( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(ShowBannerMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::HideBanner( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(HideBannerMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::DestroyBanner( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(DestroyBannerMethod, *GetJString(AdUnitIdentifier));
}

// MARK: - MRECs

void FJavaAndroidAppLovinMAX::CreateMRec( const FString &AdUnitIdentifier, const FString &MRecPosition )
{
    return CallMethod<void>(CreateMRecMethod, *GetJString(AdUnitIdentifier), *GetJString(MRecPosition));
}

void FJavaAndroidAppLovinMAX::SetMRecPlacement( const FString &AdUnitIdentifier, const FString &Placement )
{
    return CallMethod<void>(SetMRecPlacementMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidAppLovinMAX::SetMRecExtraParameter( const FString &AdUnitIdentifier, const FString &Key, const FString &Value )
{
    return CallMethod<void>(SetMRecExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

void FJavaAndroidAppLovinMAX::UpdateMRecPosition( const FString &AdUnitIdentifier, const FString &MRecPosition )
{
    return CallMethod<void>(UpdateMRecPositionMethod, *GetJString(AdUnitIdentifier), *GetJString(MRecPosition));
}

void FJavaAndroidAppLovinMAX::ShowMRec( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(ShowMRecMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::HideMRec( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(HideMRecMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::DestroyMRec( const FString &AdUnitIdentifier )
{
    return CallMethod<void>(DestroyMRecMethod, *GetJString(AdUnitIdentifier));
}

// MARK: - Interstitials

void FJavaAndroidAppLovinMAX::LoadInterstitial( const FString &AdUnitIdentifier )
{
    CallMethod<void>(LoadInterstitialMethod, *GetJString(AdUnitIdentifier));
}

bool FJavaAndroidAppLovinMAX::IsInterstitialReady( const FString &AdUnitIdentifier )
{
    return CallMethod<bool>(IsInterstitialReadyMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::ShowInterstitial( const FString &AdUnitIdentifier, const FString &Placement)
{
    CallMethod<void>(ShowInterstitialMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidAppLovinMAX::SetInterstitialExtraParameter( const FString &AdUnitIdentifier, const FString &Key, const FString &Value )
{
    CallMethod<void>(SetInterstitialExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

// MARK: - Rewarded
   
void FJavaAndroidAppLovinMAX::LoadRewardedAd( const FString &AdUnitIdentifier )
{
    CallMethod<void>(LoadRewardedAdMethod, *GetJString(AdUnitIdentifier));
}

bool FJavaAndroidAppLovinMAX::IsRewardedAdReady( const FString &AdUnitIdentifier )
{
    return CallMethod<bool>(IsRewardedAdReadyMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidAppLovinMAX::ShowRewardedAd( const FString &AdUnitIdentifier, const FString &Placement )
{
    CallMethod<void>(ShowRewardedAdMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidAppLovinMAX::SetRewardedAdExtraParameter( const FString &AdUnitIdentifier, const FString &Key, const FString &Value )
{
    CallMethod<void>(SetRewardedAdExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

// MARK: - Private

FName FJavaAndroidAppLovinMAX::GetClassName()
{
    return FName("com/applovin/unreal/AppLovinMAX");
}

#endif
