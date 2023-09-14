// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MRecWidget.generated.h"

/**
 * 
 */
UCLASS()
class APPLOVINMAXDEMO_API UMRecWidget : public UUserWidget
{
    GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category="MREC")
    void LoadMRecButtonClicked();

    UFUNCTION(BlueprintCallable, Category="MREC")
    void ShowMRecButtonClicked();

    UFUNCTION(BlueprintCallable, Category="MREC")
    void HideMRecButtonClicked();

protected:
	virtual void NativeConstruct() override;
};
