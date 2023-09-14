// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardedWidget.generated.h"

/**
 * 
 */
UCLASS()
class APPLOVINMAXDEMO_API URewardedWidget : public UUserWidget
{
    GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category="Rewarded")
    void LoadRewardedAdButtonClicked();

    UFUNCTION(BlueprintCallable, Category="Rewarded")
    void ShowRewardedAdButtonClicked();

protected:
    virtual void NativeConstruct() override;
};
