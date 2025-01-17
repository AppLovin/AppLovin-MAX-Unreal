// Copyright AppLovin Corporation. All Rights Reserved.


#include "MRecWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

void UMRecWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UAppLovinMAX::OnMRecAdLoadedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC loaded");
    });

    UAppLovinMAX::OnMRecAdLoadFailedDelegate.AddLambda([](const FAdInfo &AdInfo, const FAdError &AdError)
    {
        DEMO_LOG("MREC failed to load with error: %s", *AdError.Message);
    });

    UAppLovinMAX::OnMRecAdClickedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC clicked");
    });

    UAppLovinMAX::OnMRecAdExpandedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC expanded");
    });

    UAppLovinMAX::OnMRecAdCollapsedDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC collapsed");
    });

    UAppLovinMAX::OnMRecAdRevenuePaidDelegate.AddLambda([](const FAdInfo &AdInfo)
    {
        DEMO_LOG("MREC revenue paid: %f", AdInfo.Revenue);
    });
}

void UMRecWidget::LoadMRecButtonClicked()
{
    DEMO_LOG("Loading MREC...");
    UAppLovinMAX::CreateMRec(AdUnitIdentifier::MRec, EAdViewPosition::TopCenter);
}

void UMRecWidget::ShowMRecButtonClicked()
{
    DEMO_LOG("Showing MREC...");
    UAppLovinMAX::ShowMRec(AdUnitIdentifier::MRec);
}

void UMRecWidget::HideMRecButtonClicked()
{
    DEMO_LOG("Hiding MREC...");
    UAppLovinMAX::HideMRec(AdUnitIdentifier::MRec);
}
