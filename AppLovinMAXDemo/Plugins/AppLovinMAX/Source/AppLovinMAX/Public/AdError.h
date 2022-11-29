// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdError.generated.h"

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FAdError
{
    GENERATED_BODY()

    /** The error code for the error. */
    UPROPERTY(BlueprintReadOnly)
    int Code;

    /** The error message for the error. */
    UPROPERTY(BlueprintReadOnly)
    FString Message;

    /** The underlying waterfall of ad responses. */
    UPROPERTY(BlueprintReadOnly)
    FString Waterfall;
};
