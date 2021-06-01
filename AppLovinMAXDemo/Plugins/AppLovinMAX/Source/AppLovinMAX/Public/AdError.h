// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct APPLOVINMAX_API FAdError
{
    /** The error code for the error. */
    const int Code;

    /** The error message for the error. */
    const FString Message;

    /** A description string containing error codes and reasons for why each mediated network failed to load an ad. */
    const FString AdLoadFailureInfo;
};
