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
    });

    UAppLovinMAX::SetVerboseLoggingEnabled(true);
    UAppLovinMAX::Initialize(SdkKey);
}
