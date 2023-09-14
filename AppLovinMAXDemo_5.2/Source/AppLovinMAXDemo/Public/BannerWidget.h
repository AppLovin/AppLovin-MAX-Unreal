// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BannerWidget.generated.h"

/**
 * 
 */
UCLASS()
class APPLOVINMAXDEMO_API UBannerWidget : public UUserWidget
{
    GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category="Banner")
    void LoadBannerButtonClicked();

    UFUNCTION(BlueprintCallable, Category="Banner")
    void ShowBannerButtonClicked();

    UFUNCTION(BlueprintCallable, Category="Banner")
    void HideBannerButtonClicked();

protected:
	virtual void NativeConstruct() override;
};
