// Copyright AppLovin Corporation. All Rights Reserved.


#include "InterstitialWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

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
