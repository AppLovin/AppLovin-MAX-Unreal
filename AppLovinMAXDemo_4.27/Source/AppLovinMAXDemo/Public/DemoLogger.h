// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define DEMO_LOG(message, ...)   if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, FString::Printf(TEXT(message), ##__VA_ARGS__)); }
