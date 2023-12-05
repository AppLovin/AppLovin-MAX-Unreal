// Copyright AppLovin Corporation. All Rights Reserved.

#include "AndroidJavaMaxUnrealPlugin.h"

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Misc/EngineVersion.h"

FJavaAndroidMaxUnrealPlugin::FJavaAndroidMaxUnrealPlugin()
    : FJavaClassObject(GetClassName(), "(Landroid/app/Activity;)V", FAndroidApplication::GetGameActivityThis()),
      InitializeMethod(GetClassMethod("initialize", "(Ljava/lang/String;Ljava/lang/String;Lcom/applovin/unreal/MaxUnrealPlugin$EventListener;)V")),
      IsInitializedMethod(GetClassMethod("isInitialized", "()Z")),
      SetHasUserConsentMethod(GetClassMethod("setHasUserConsent", "(Z)V")),
      HasUserConsentMethod(GetClassMethod("hasUserConsent", "()Z")),
      SetIsAgeRestrictedUserMethod(GetClassMethod("setIsAgeRestrictedUser", "(Z)V")),
      IsAgeRestrictedUserMethod(GetClassMethod("isAgeRestrictedUser", "()Z")),
      SetDoNotSellMethod(GetClassMethod("setDoNotSell", "(Z)V")),
      IsDoNotSellMethod(GetClassMethod("isDoNotSell", "()Z")),
      SetTermsAndPrivacyPolicyFlowEnabled(GetClassMethod("setTermsAndPrivacyPolicyFlowEnabled", "(Z)V")),
      SetPrivacyPolicyURL(GetClassMethod("setPrivacyPolicyURL", "(Ljava/lang/String;)V")),
      SetTermsOfServiceURL(GetClassMethod("setTermsOfServiceURL", "(Ljava/lang/String;)V")),
      IsTabletMethod(GetClassMethod("isTablet", "()Z")),
      ShowMediationDebuggerMethod(GetClassMethod("showMediationDebugger", "()V")),
      SetUserIdMethod(GetClassMethod("setUserId", "(Ljava/lang/String;)V")),
      SetMutedMethod(GetClassMethod("setMuted", "(Z)V")),
      IsMutedMethod(GetClassMethod("isMuted", "()Z")),
      SetVerboseLoggingEnabledMethod(GetClassMethod("setVerboseLoggingEnabled", "(Z)V")),
      IsVerboseLoggingEnabledMethod(GetClassMethod("isVerboseLoggingEnabled", "()Z")),
      SetCreativeDebuggerEnabledMethod(GetClassMethod("setCreativeDebuggerEnabled", "(Z)V")),
      SetTestDeviceAdvertisingIdentifiersMethod(GetClassMethod("setTestDeviceAdvertisingIds", "([Ljava/lang/String;)V")),
      TrackEventMethod(GetClassMethod("trackEvent", "(Ljava/lang/String;Ljava/lang/String;)V")),
      CreateBannerMethod(GetClassMethod("createBanner", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetBannerBackgroundColorMethod(GetClassMethod("setBannerBackgroundColor", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetBannerPlacementMethod(GetClassMethod("setBannerPlacement", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetBannerExtraParameterMethod(GetClassMethod("setBannerExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")),
      UpdateBannerPositionMethod(GetClassMethod("updateBannerPosition", "(Ljava/lang/String;Ljava/lang/String;)V")),
      ShowBannerMethod(GetClassMethod("showBanner", "(Ljava/lang/String;)V")),
      HideBannerMethod(GetClassMethod("hideBanner", "(Ljava/lang/String;)V")),
      DestroyBannerMethod(GetClassMethod("destroyBanner", "(Ljava/lang/String;)V")),
      CreateMRecMethod(GetClassMethod("createMRec", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetMRecPlacementMethod(GetClassMethod("setMRecPlacement", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetMRecExtraParameterMethod(GetClassMethod("setMRecExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")),
      UpdateMRecPositionMethod(GetClassMethod("updateMRecPosition", "(Ljava/lang/String;Ljava/lang/String;)V")),
      ShowMRecMethod(GetClassMethod("showMRec", "(Ljava/lang/String;)V")),
      HideMRecMethod(GetClassMethod("hideMRec", "(Ljava/lang/String;)V")),
      DestroyMRecMethod(GetClassMethod("destroyMRec", "(Ljava/lang/String;)V")),
      LoadInterstitialMethod(GetClassMethod("loadInterstitial", "(Ljava/lang/String;)V")),
      IsInterstitialReadyMethod(GetClassMethod("isInterstitialReady", "(Ljava/lang/String;)Z")),
      ShowInterstitialMethod(GetClassMethod("showInterstitial", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetInterstitialExtraParameterMethod(GetClassMethod("setInterstitialExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")),
      LoadRewardedAdMethod(GetClassMethod("loadRewardedAd", "(Ljava/lang/String;)V")),
      IsRewardedAdReadyMethod(GetClassMethod("isRewardedAdReady", "(Ljava/lang/String;)Z")),
      ShowRewardedAdMethod(GetClassMethod("showRewardedAd", "(Ljava/lang/String;Ljava/lang/String;)V")),
      SetRewardedAdExtraParameterMethod(GetClassMethod("setRewardedAdExtraParameter", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
{
}

FJavaAndroidMaxUnrealPlugin::~FJavaAndroidMaxUnrealPlugin() {}

// MARK: - Initialization

void FJavaAndroidMaxUnrealPlugin::Initialize(const FString &PluginVersion, const FString &SdkKey)
{
    JNIEnv *JEnv = FAndroidApplication::GetJavaEnv();

    // Create listener for Android plugin event handling
    jclass ListenerClass;
    ListenerClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/GameActivity$MaxUnrealPluginListener");

    jmethodID Constructor = JEnv->GetMethodID(ListenerClass, "<init>", "()V");
    auto LocalListener = NewScopedJavaObject(JEnv, JEnv->NewObject(ListenerClass, Constructor));

    CallMethod<void>(InitializeMethod, *GetJString(PluginVersion), *GetJString(SdkKey), *LocalListener);
}

bool FJavaAndroidMaxUnrealPlugin::IsInitialized()
{
    return CallMethod<bool>(IsInitializedMethod);
}

// MARK: - Privacy

void FJavaAndroidMaxUnrealPlugin::SetHasUserConsent(bool bHasUserConsent)
{
    CallMethod<void>(SetHasUserConsentMethod, bHasUserConsent);
}

bool FJavaAndroidMaxUnrealPlugin::HasUserConsent()
{
    return CallMethod<bool>(HasUserConsentMethod);
}

void FJavaAndroidMaxUnrealPlugin::SetIsAgeRestrictedUser(bool bIsAgeRestrictedUser)
{
    CallMethod<void>(SetIsAgeRestrictedUserMethod, bIsAgeRestrictedUser);
}

bool FJavaAndroidMaxUnrealPlugin::IsAgeRestrictedUser()
{
    return CallMethod<bool>(IsAgeRestrictedUserMethod);
}

void FJavaAndroidMaxUnrealPlugin::SetDoNotSell(bool bDoNotSell)
{
    CallMethod<void>(SetDoNotSellMethod, bDoNotSell);
}

bool FJavaAndroidMaxUnrealPlugin::IsDoNotSell()
{
    return CallMethod<bool>(IsDoNotSellMethod);
}

// MARK: - Terms and Privacy Policy Flow

void FJavaAndroidMaxUnrealPlugin::SetTermsAndPrivacyPolicyFlowEnabled(bool bEnabled)
{
    CallMethod<void>(SetTermsAndPrivacyPolicyFlowEnabled, bEnabled);
}

void FJavaAndroidMaxUnrealPlugin::SetPrivacyPolicyURL(const FString &URL)
{
    CallMethod<void>(SetPrivacyPolicyURL, *GetJString(URL));
}

void FJavaAndroidMaxUnrealPlugin::SetTermsOfServiceURL(const FString &URL)
{
    CallMethod<void>(SetTermsOfServiceURL, *GetJString(URL));
}

// MARK: - General

bool FJavaAndroidMaxUnrealPlugin::IsTablet()
{
    return CallMethod<bool>(IsTabletMethod);
}

void FJavaAndroidMaxUnrealPlugin::ShowMediationDebugger()
{
    CallMethod<void>(ShowMediationDebuggerMethod);
}

void FJavaAndroidMaxUnrealPlugin::SetUserId(const FString &UserId)
{
    CallMethod<void>(SetUserIdMethod, *GetJString(UserId));
}

void FJavaAndroidMaxUnrealPlugin::SetMuted(bool bMuted)
{
    CallMethod<void>(SetMutedMethod, bMuted);
}

bool FJavaAndroidMaxUnrealPlugin::IsMuted()
{
    return CallMethod<bool>(IsMutedMethod);
}

void FJavaAndroidMaxUnrealPlugin::SetVerboseLoggingEnabled(bool bEnabled)
{
    CallMethod<void>(SetVerboseLoggingEnabledMethod, bEnabled);
}

bool FJavaAndroidMaxUnrealPlugin::IsVerboseLoggingEnabled()
{
    return CallMethod<bool>(IsVerboseLoggingEnabledMethod);
}

void FJavaAndroidMaxUnrealPlugin::SetCreativeDebuggerEnabled(bool bEnabled)
{
    CallMethod<void>(SetCreativeDebuggerEnabledMethod, bEnabled);
}

void FJavaAndroidMaxUnrealPlugin::SetTestDeviceAdvertisingIdentifiers(const TArray<FString> &AdvertisingIdentifiers)
{
    JNIEnv *Env = FAndroidApplication::GetJavaEnv();

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

void FJavaAndroidMaxUnrealPlugin::TrackEvent(const FString &Name, const FString &Parameters)
{
    CallMethod<void>(TrackEventMethod, *GetJString(Name), *GetJString(Parameters));
}

// MARK: - Banners

void FJavaAndroidMaxUnrealPlugin::CreateBanner(const FString &AdUnitIdentifier, const FString &BannerPosition)
{
    return CallMethod<void>(CreateBannerMethod, *GetJString(AdUnitIdentifier), *GetJString(BannerPosition));
}

void FJavaAndroidMaxUnrealPlugin::SetBannerBackgroundColor(const FString &AdUnitIdentifier, const FString &HexColorCode)
{
    return CallMethod<void>(SetBannerBackgroundColorMethod, *GetJString(AdUnitIdentifier), *GetJString(HexColorCode));
}

void FJavaAndroidMaxUnrealPlugin::SetBannerPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    return CallMethod<void>(SetBannerPlacementMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidMaxUnrealPlugin::SetBannerExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    return CallMethod<void>(SetBannerExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

void FJavaAndroidMaxUnrealPlugin::UpdateBannerPosition(const FString &AdUnitIdentifier, const FString &BannerPosition)
{
    return CallMethod<void>(UpdateBannerPositionMethod, *GetJString(AdUnitIdentifier), *GetJString(BannerPosition));
}

void FJavaAndroidMaxUnrealPlugin::ShowBanner(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(ShowBannerMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::HideBanner(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(HideBannerMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::DestroyBanner(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(DestroyBannerMethod, *GetJString(AdUnitIdentifier));
}

// MARK: - MRECs

void FJavaAndroidMaxUnrealPlugin::CreateMRec(const FString &AdUnitIdentifier, const FString &MRecPosition)
{
    return CallMethod<void>(CreateMRecMethod, *GetJString(AdUnitIdentifier), *GetJString(MRecPosition));
}

void FJavaAndroidMaxUnrealPlugin::SetMRecPlacement(const FString &AdUnitIdentifier, const FString &Placement)
{
    return CallMethod<void>(SetMRecPlacementMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidMaxUnrealPlugin::SetMRecExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    return CallMethod<void>(SetMRecExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

void FJavaAndroidMaxUnrealPlugin::UpdateMRecPosition(const FString &AdUnitIdentifier, const FString &MRecPosition)
{
    return CallMethod<void>(UpdateMRecPositionMethod, *GetJString(AdUnitIdentifier), *GetJString(MRecPosition));
}

void FJavaAndroidMaxUnrealPlugin::ShowMRec(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(ShowMRecMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::HideMRec(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(HideMRecMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::DestroyMRec(const FString &AdUnitIdentifier)
{
    return CallMethod<void>(DestroyMRecMethod, *GetJString(AdUnitIdentifier));
}

// MARK: - Interstitials

void FJavaAndroidMaxUnrealPlugin::LoadInterstitial(const FString &AdUnitIdentifier)
{
    CallMethod<void>(LoadInterstitialMethod, *GetJString(AdUnitIdentifier));
}

bool FJavaAndroidMaxUnrealPlugin::IsInterstitialReady(const FString &AdUnitIdentifier)
{
    return CallMethod<bool>(IsInterstitialReadyMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::ShowInterstitial(const FString &AdUnitIdentifier, const FString &Placement)
{
    CallMethod<void>(ShowInterstitialMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidMaxUnrealPlugin::SetInterstitialExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    CallMethod<void>(SetInterstitialExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

// MARK: - Rewarded

void FJavaAndroidMaxUnrealPlugin::LoadRewardedAd(const FString &AdUnitIdentifier)
{
    CallMethod<void>(LoadRewardedAdMethod, *GetJString(AdUnitIdentifier));
}

bool FJavaAndroidMaxUnrealPlugin::IsRewardedAdReady(const FString &AdUnitIdentifier)
{
    return CallMethod<bool>(IsRewardedAdReadyMethod, *GetJString(AdUnitIdentifier));
}

void FJavaAndroidMaxUnrealPlugin::ShowRewardedAd(const FString &AdUnitIdentifier, const FString &Placement)
{
    CallMethod<void>(ShowRewardedAdMethod, *GetJString(AdUnitIdentifier), *GetJString(Placement));
}

void FJavaAndroidMaxUnrealPlugin::SetRewardedAdExtraParameter(const FString &AdUnitIdentifier, const FString &Key, const FString &Value)
{
    CallMethod<void>(SetRewardedAdExtraParameterMethod, *GetJString(AdUnitIdentifier), *GetJString(Key), *GetJString(Value));
}

// MARK: - Private

FName FJavaAndroidMaxUnrealPlugin::GetClassName()
{
    return FName("com/applovin/unreal/MaxUnrealPlugin");
}

#endif
