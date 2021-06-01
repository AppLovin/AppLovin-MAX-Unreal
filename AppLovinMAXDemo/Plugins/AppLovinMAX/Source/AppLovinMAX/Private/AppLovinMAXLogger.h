// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAppLovinMAX, Log, All);

DEFINE_LOG_CATEGORY(LogAppLovinMAX);

#define MAX_D(message, ...)           UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__)
#define MAX_W(message, ...)           UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__)
#define MAX_E(message, ...)           UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__)

#define MAX_USER_DEBUG(message, ...)      { if (UAppLovinMAX::IsVerboseLoggingEnabled()) { UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__) }}
#define MAX_USER_WARN(message, ...)    { if (UAppLovinMAX::IsVerboseLoggingEnabled()) { UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__) }}
#define MAX_USER_ERROR(message, ...)      { if (UAppLovinMAX::IsVerboseLoggingEnabled()) { UE_LOG(LogAppLovinMAX, Log, TEXT(message), ##__VA_ARGS__) }}
