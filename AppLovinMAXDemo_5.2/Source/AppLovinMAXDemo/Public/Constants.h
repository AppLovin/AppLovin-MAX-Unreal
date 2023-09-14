// Copyright AppLovin Corporation. All Rights Reserved.


#pragma once

const FString SdkKey = "hBDh6tzZrp-fWye63N4nhbgw8umnTzD99QsGIpq8bpo7lRDppHZVuEQ17Bpa80lIRaTlONt-Af6v5JiubGOUVp";

namespace AdUnitIdentifier
{
#if PLATFORM_IOS
    const FString Banner = "35858295296a8c80";
    const FString MRec = "f865a23962fa00e5";
    const FString Interstitial = "65d8d0195e50bda6";
    const FString Rewarded = "82076aefed4737ed";
#elif PLATFORM_ANDROID
    const FString Banner = "3c2e14fa5f22c4f6";
    const FString MRec = "6e882d2f921d6d7c";
    const FString Interstitial = "f5df6799ac75cd34";
    const FString Rewarded = "49cdb15b16042a49";
#else // Empty values for compiling in editor or other platforms
    const FString Banner = "";
    const FString MRec = "";
    const FString Interstitial = "";
    const FString Rewarded = "";
#endif
}
