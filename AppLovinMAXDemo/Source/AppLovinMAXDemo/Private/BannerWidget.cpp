// Copyright AppLovin Corporation. All Rights Reserved.


#include "BannerWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

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
