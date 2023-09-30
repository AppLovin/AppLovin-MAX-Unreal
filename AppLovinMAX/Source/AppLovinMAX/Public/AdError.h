// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdError.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdError
{
    GENERATED_BODY()

    /** The error code for the error. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    int Code = 0;

    /** The error message for the error. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString Message;

    /** The underlying waterfall of ad responses. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString Waterfall;
};
