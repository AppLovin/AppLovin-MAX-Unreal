// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InterstitialWidget.generated.h"

/**
 *
 */
UCLASS()
class APPLOVINMAXDEMO_API UInterstitialWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Interstitial")
    void LoadInterstitialButtonClicked();

    UFUNCTION(BlueprintCallable, Category="Interstitial")
    void ShowInterstitialButtonClicked();
};
