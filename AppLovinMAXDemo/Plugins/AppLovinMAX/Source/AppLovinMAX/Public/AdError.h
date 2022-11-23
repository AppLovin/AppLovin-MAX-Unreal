// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct APPLOVINMAX_API FAdError
{
    /** The error code for the error. */
    const int Code;

    /** The error message for the error. */
    const FString Message;

    /** The underlying waterfall of ad responses. */
    const FString Waterfall;
};
