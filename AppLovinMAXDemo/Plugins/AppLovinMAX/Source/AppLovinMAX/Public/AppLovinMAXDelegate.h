// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SdkConfiguration.h"
#include "AppLovinMAXDelegate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSdkInitializedDynamicDelegate, const FSdkConfiguration, SdkConfiguration);

/**
 * This class is used for broadcasting to dynamic delegates, i.e., via blueprints. For C++ projects, one should prefer binding to the delegates available on UAppLovinMAX.
 */
UCLASS(ClassGroup = (AppLovinMAX), DisplayName = "AppLovin MAX Delegate", meta = (BlueprintSpawnableComponent))
class APPLOVINMAX_API UAppLovinMAXDelegate : public UActorComponent
{
    GENERATED_BODY()

public:
    static void OnSdkInitialized(const FSdkConfiguration &SdkConfiguration);

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnSdkInitializedDynamicDelegate OnSdkInitializedDynamicDelegate;
};
