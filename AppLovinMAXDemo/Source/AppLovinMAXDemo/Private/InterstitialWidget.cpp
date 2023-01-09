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
        DEMO_LOG("Interstitial ad loaded: %s", *AdInfo.ToString());
    });

    UAppLovinMAX::OnInterstitialAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Interstitial ad failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnInterstitialAdDisplayedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial ad displayed");
    });

    UAppLovinMAX::OnInterstitialAdDisplayFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Interstitial ad failed to display with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnInterstitialAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial ad clicked");
    });

    UAppLovinMAX::OnInterstitialAdHiddenDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial ad hidden");
    });

    UAppLovinMAX::OnInterstitialAdRevenuePaidDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Interstitial ad revenue paid");
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
