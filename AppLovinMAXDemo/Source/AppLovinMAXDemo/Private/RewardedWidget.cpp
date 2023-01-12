// Copyright AppLovin Corporation. All Rights Reserved.


#include "RewardedWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"


void URewardedWidget::NativeConstruct()
{
	Super::NativeConstruct();

    UAppLovinMAX::OnRewardedAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad loaded: %s", *AdInfo.ToString());
    });

    UAppLovinMAX::OnRewardedAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Rewarded ad failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnRewardedAdDisplayedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad displayed");
    });

    UAppLovinMAX::OnRewardedAdDisplayFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Rewarded ad failed to display with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnRewardedAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad clicked");
    });

    UAppLovinMAX::OnRewardedAdHiddenDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad hidden");
    });

    UAppLovinMAX::OnRewardedAdRevenuePaidDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Rewarded ad revenue paid");
    });
}

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
