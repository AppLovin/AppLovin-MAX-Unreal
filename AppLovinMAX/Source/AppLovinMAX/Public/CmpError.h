// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CmpError.generated.h"

// TODO: Create enum for Code
// NOTE: Unspecified will be passed as 0, but must remember to set -1 to 0 in the native iOS and Android code before sending event!

USTRUCT(BlueprintType)
struct APPLOVINMAX_API FCmpError
{
    GENERATED_BODY()

    /** The error code for the error. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    int Code = 0;

    /** The error message for the error. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString Message;

    /** The error code returned by the CMP. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    int CmpCode = -1;

    /** The error message returned by the CMP. */
    UPROPERTY(BlueprintReadOnly, Category = "AppLovinMAX")
    FString CmpMessage;
};
