// Copyright AppLovin Corporation. All Rights Reserved.


#include "AppLovinMAXDemoGameModeBase.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

AAppLovinMAXDemoGameModeBase::AAppLovinMAXDemoGameModeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    UAppLovinMAX::OnSdkInitializedDelegate.AddLambda([this](const FSdkConfiguration& SdkConfiguration)
    {
        DEMO_LOG("AppLovin SDK Initialized");
        AttachCallbacks();
    });

    UAppLovinMAX::SetVerboseLoggingEnabled(true);
    UAppLovinMAX::Initialize(SdkKey);
}

void AAppLovinMAXDemoGameModeBase::AttachCallbacks()
{
    // Interstitials
    
    UAppLovinMAX::OnInterstitialAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial loaded");
        DEMO_LOG("%s", *AdInfo.ToString());
    });

    UAppLovinMAX::OnInterstitialAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Interstitial failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnInterstitialAdDisplayFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Interstitial failed to display with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnInterstitialAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial clicked");
    });

    UAppLovinMAX::OnInterstitialAdHiddenDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial hidden");
    });

    // Rewarded Ads

    UAppLovinMAX::OnRewardedAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad loaded");
    });

    UAppLovinMAX::OnRewardedAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Rewarded ad failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnRewardedAdDisplayFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Rewarded ad failed to display with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnRewardedAdHiddenDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad hidden");
    });

    UAppLovinMAX::OnRewardedAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad clicked");
    });

    UAppLovinMAX::OnRewardedAdReceivedRewardDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdReward &Reward)
    {
        DEMO_LOG("Rewarded ad received reward: %s", *Reward.ToString());
    });

    // Banner Ads
    
    UAppLovinMAX::OnBannerAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner loaded");
    });

    UAppLovinMAX::OnBannerAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner clicked");
    });

    UAppLovinMAX::OnBannerAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Banner ad failed to load with error: %s", *AdError.Message);
    });

    // MRec Ads

    UAppLovinMAX::OnMRecAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC loaded");
    });

    UAppLovinMAX::OnMRecAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC clicked");
    });

    UAppLovinMAX::OnMRecAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("MREC ad failed to load with error: %s", *AdError.Message);
    });
}
