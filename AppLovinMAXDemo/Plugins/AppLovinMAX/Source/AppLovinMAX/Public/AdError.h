// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdError.generated.h"

USTRUCT()
struct APPLOVINMAX_API FAdError
{
    GENERATED_BODY()

    /** The error code for the error. */
    UPROPERTY()
    int Code;

    /** The error message for the error. */
    UPROPERTY()
    FString Message;

    /** The underlying waterfall of ad responses. */
    UPROPERTY()
    FString Waterfall;
};
