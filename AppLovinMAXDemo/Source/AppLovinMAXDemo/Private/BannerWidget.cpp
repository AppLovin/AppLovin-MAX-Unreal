// Copyright AppLovin Corporation. All Rights Reserved.


#include "BannerWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

void UBannerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UAppLovinMAX::OnBannerAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner loaded");
    });

    UAppLovinMAX::OnBannerAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("Banner failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnBannerAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner clicked");
    });

    UAppLovinMAX::OnBannerAdExpandedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner expanded");
    });

    UAppLovinMAX::OnBannerAdCollapsedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner collapsed");
    });

    UAppLovinMAX::OnBannerAdRevenuePaidDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("Banner revenue paid");
    });
}

void UBannerWidget::LoadBannerButtonClicked()
{
    DEMO_LOG("Loading banner...");
    UAppLovinMAX::CreateBanner(AdUnitIdentifier::Banner, EAdViewPosition::BottomCenter);
    UAppLovinMAX::SetBannerBackgroundColor(AdUnitIdentifier::Banner, FColor::Black);
}

void UBannerWidget::ShowBannerButtonClicked()
{
    DEMO_LOG("Showing banner...");
    UAppLovinMAX::ShowBanner(AdUnitIdentifier::Banner);
}

void UBannerWidget::HideBannerButtonClicked()
{
    DEMO_LOG("Hiding banner...");
    UAppLovinMAX::HideBanner(AdUnitIdentifier::Banner);
}
