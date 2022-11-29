// Copyright AppLovin Corporation. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdError.h"
#include "AdInfo.h"
#include "AdReward.h"
#include "SdkConfiguration.h"
#include "AppLovinMAXDelegate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSdkInitializedDynamicDelegate, const FSdkConfiguration &, SdkConfiguration);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBannerAdLoadedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBannerAdLoadFailedDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBannerAdClickedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBannerAdExpandedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBannerAdCollapsedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBannerAdRevenuePaidDynamicDelegate, const FAdInfo &, AdInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMRecAdLoadedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMRecAdLoadFailedDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMRecAdClickedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMRecAdExpandedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMRecAdCollapsedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMRecAdRevenuePaidDynamicDelegate, const FAdInfo &, AdInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterstitialLoadedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInterstitialLoadFailedDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterstitialHiddenDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterstitialDisplayedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInterstitialAdFailedToDisplayDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterstitialClickedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInterstitialAdRevenuePaidDynamicDelegate, const FAdInfo &, AdInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardedAdLoadedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdLoadFailedDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardedAdDisplayedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardedAdHiddenDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardedAdClickedDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardedAdRevenuePaidDynamicDelegate, const FAdInfo &, AdInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdFailedToDisplayDynamicDelegate, const FAdInfo &, AdInfo, const FAdError &, AdError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardedAdReceivedRewardDynamicDelegate, const FAdInfo &, AdInfo, const FAdReward &, Reward);

/**
 * This class is used for binding and broadcasting to dynamic delegates, i.e., via blueprints. For C++ projects, one should prefer binding to the delegates available directly on UAppLovinMAX.
 */
UCLASS(ClassGroup = (AppLovinMAX), DisplayName = "AppLovin MAX Delegate", meta = (BlueprintSpawnableComponent))
class APPLOVINMAX_API UAppLovinMAXDelegate : public UActorComponent
{
    GENERATED_BODY()

public:
    // MARK: - Broadcast Methods
    
    static void BroadcastSdkInitializedEvent(const FSdkConfiguration &SdkConfiguration);
    static void BroadcastAdEvent(const FString &Name, const FAdInfo &AdInfo);
    static void BroadcastAdErrorEvent(const FString &Name, const FAdInfo &AdInfo, const FAdError &AdError);
    static void BroadcastRewardedAdReceivedRewardEvent(const FAdInfo &AdInfo, const FAdReward &Reward);

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnSdkInitializedDynamicDelegate OnSdkInitializedDynamicDelegate;

    // MARK: - Banners

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdLoadedDynamicDelegate OnBannerAdLoadedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdLoadFailedDynamicDelegate OnBannerAdLoadFailedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdClickedDynamicDelegate OnBannerAdClickedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdExpandedDynamicDelegate OnBannerAdExpandedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdCollapsedDynamicDelegate OnBannerAdCollapsedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnBannerAdRevenuePaidDynamicDelegate OnBannerAdRevenuePaidDynamicDelegate;

    // MARK: - MRECs

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdLoadedDynamicDelegate OnMRecAdLoadedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdLoadFailedDynamicDelegate OnMRecAdLoadFailedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdClickedDynamicDelegate OnMRecAdClickedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdExpandedDynamicDelegate OnMRecAdExpandedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdCollapsedDynamicDelegate OnMRecAdCollapsedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnMRecAdRevenuePaidDynamicDelegate OnMRecAdRevenuePaidDynamicDelegate;

    // MARK: - Interstitials

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialLoadedDynamicDelegate OnInterstitialLoadedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialLoadFailedDynamicDelegate OnInterstitialLoadFailedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialHiddenDynamicDelegate OnInterstitialHiddenDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialDisplayedDynamicDelegate OnInterstitialDisplayedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialAdFailedToDisplayDynamicDelegate OnInterstitialAdFailedToDisplayDynamicDelegate;
    
    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialClickedDynamicDelegate OnInterstitialClickedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnInterstitialAdRevenuePaidDynamicDelegate OnInterstitialAdRevenuePaidDynamicDelegate;

    // MARK: - Rewarded

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdLoadedDynamicDelegate OnRewardedAdLoadedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdLoadFailedDynamicDelegate OnRewardedAdLoadFailedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdDisplayedDynamicDelegate OnRewardedAdDisplayedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdHiddenDynamicDelegate OnRewardedAdHiddenDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdClickedDynamicDelegate OnRewardedAdClickedDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdRevenuePaidDynamicDelegate OnRewardedAdRevenuePaidDynamicDelegate;
    
    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdFailedToDisplayDynamicDelegate OnRewardedAdFailedToDisplayDynamicDelegate;

    UPROPERTY(BlueprintAssignable, Category = "AppLovinMAX")
    FOnRewardedAdReceivedRewardDynamicDelegate OnRewardedAdReceivedRewardDynamicDelegate;
};
