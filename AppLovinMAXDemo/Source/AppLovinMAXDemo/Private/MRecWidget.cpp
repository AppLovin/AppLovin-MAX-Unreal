// Copyright AppLovin Corporation. All Rights Reserved.


#include "MRecWidget.h"
#include "AppLovinMAX.h"
#include "Constants.h"
#include "DemoLogger.h"

void UMRecWidget::LoadMRecButtonClicked()
{
    DEMO_LOG("Loading MRec...");
    UAppLovinMAX::CreateMRec(AdUnitIdentifier::MRec, EAdViewPosition::TopCenter);
}

void UMRecWidget::ShowMRecButtonClicked()
{
    DEMO_LOG("Showing MRec...");
    UAppLovinMAX::ShowMRec(AdUnitIdentifier::MRec);
}

void UMRecWidget::HideMRecButtonClicked()
{
    DEMO_LOG("Hiding MRec...");
    UAppLovinMAX::HideMRec(AdUnitIdentifier::MRec);
}
