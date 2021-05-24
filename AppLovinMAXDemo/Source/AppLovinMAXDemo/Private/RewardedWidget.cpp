// Copyright AppLovin Corporation. All Rights Reserved.


#include "RewardedWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

void URewardedWidget::LoadRewardedAdButtonClicked()
{
    DEMO_LOG("Loading rewarded ad...");
    UAppLovinMAX::LoadRewardedAd(AdUnitIdentifier::Rewarded);
}

void URewardedWidget::ShowRewardedAdButtonClicked()
{
    if ( UAppLovinMAX::IsRewardedAdReady(AdUnitIdentifier::Rewarded) )
    {
        DEMO_LOG("Showing rewarded ad...");
        UAppLovinMAX::ShowRewardedAd(AdUnitIdentifier::Rewarded);
    }
    else
    {
        DEMO_LOG("Rewarded ad not ready");
    }
}
