// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AppLovinMAXDemoGameModeBase.generated.h"

struct FSdkConfiguration;

/**
 * 
 */
UCLASS()
class APPLOVINMAXDEMO_API AAppLovinMAXDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAppLovinMAXDemoGameModeBase(const FObjectInitializer& ObjectInitializer);
	void AttachCallbacks();
};
