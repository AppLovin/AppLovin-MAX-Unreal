// Copyright AppLovin Corporation. All Rights Reserved.


#pragma once

const FString SdkKey = "YOUR_SDK_KEY";

namespace AdUnitIdentifier
{
#if PLATFORM_IOS
    const FString Banner = "IOS_BANNER_AD_UNIT_ID";
    const FString MRec = "IOS_MREC_AD_UNIT_ID";
    const FString Interstitial = "IOS_INTERSTITIAL_AD_UNIT_ID";
    const FString Rewarded = "IOS_REWARDED_AD_UNIT_ID";
#elif PLATFORM_ANDROID
    const FString Banner = "ANDROID_BANNER_AD_UNIT_ID";
    const FString MRec = "ANDROID_MREC_AD_UNIT_ID";
    const FString Interstitial = "ANDROID_INTERSTITIAL_AD_UNIT_ID";
    const FString Rewarded = "ANDROID_REWARDED_AD_UNIT_ID";
#else // Empty values for compiling in editor or other platforms
    const FString Banner = "";
    const FString MRec = "";
    const FString Interstitial = "";
    const FString Rewarded = "";
#endif
}
