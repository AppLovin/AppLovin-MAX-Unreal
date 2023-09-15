// Copyright AppLovin Corporation. All Rights Reserved.


#include "InterstitialWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

void UInterstitialWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UAppLovinMAX::OnInterstitialAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial loaded: %s", *AdInfo.ToString());
    });

    UAppLovinMAX::OnInterstitialAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Interstitial failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnInterstitialAdDisplayedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial displayed");
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

    UAppLovinMAX::OnInterstitialAdRevenuePaidDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial revenue paid");
    });
}

void UInterstitialWidget::LoadInterstitialButtonClicked()
{
    DEMO_LOG("Loading interstitial...");
    UAppLovinMAX::LoadInterstitial(AdUnitIdentifier::Interstitial);
}

void UInterstitialWidget::ShowInterstitialButtonClicked()
{
    if ( UAppLovinMAX::IsInterstitialReady(AdUnitIdentifier::Interstitial) )
    {
        DEMO_LOG("Showing interstitial...");
        UAppLovinMAX::ShowInterstitial(AdUnitIdentifier::Interstitial);
    }
    else
    {
        DEMO_LOG("Interstitial not ready");
    }
}
