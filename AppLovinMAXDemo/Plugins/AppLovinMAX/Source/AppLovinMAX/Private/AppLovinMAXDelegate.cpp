// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAXDelegate.h"

bool IsValidDelegate(UAppLovinMAXDelegate *Delegate)
{
    // Check if delegate is non-null and not pending kill
    if (!IsValid(Delegate)) return false;

    UWorld *World = Delegate->GetWorld();
    return IsValid(World) && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
}

void UAppLovinMAXDelegate::BroadcastSdkInitializedEvent(const FSdkConfiguration &SdkConfiguration)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        for (TObjectIterator<UAppLovinMAXDelegate> Itr; Itr; ++Itr)
        {
            if (IsValidDelegate(*Itr))
            {
                Itr->OnSdkInitializedDynamicDelegate.Broadcast(SdkConfiguration);
            }
        }
    });
}

void UAppLovinMAXDelegate::BroadcastAdEvent(const FString &Name, const FAdInfo &AdInfo)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        for (TObjectIterator<UAppLovinMAXDelegate> Itr; Itr; ++Itr)
        {
            if (IsValidDelegate(*Itr))
            {
                if (Name == TEXT("OnBannerAdLoadedEvent"))
                {
                    Itr->OnBannerAdLoadedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnBannerAdClickedEvent"))
                {
                    Itr->OnBannerAdClickedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnBannerAdExpandedEvent"))
                {
                    Itr->OnBannerAdExpandedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnBannerAdCollapsedEvent"))
                {
                    Itr->OnBannerAdCollapsedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnBannerAdRevenuePaidEvent"))
                {
                    Itr->OnBannerAdRevenuePaidDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnMRecAdLoadedEvent"))
                {
                    Itr->OnMRecAdLoadedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnMRecAdClickedEvent"))
                {
                    Itr->OnMRecAdClickedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnMRecAdExpandedEvent"))
                {
                    Itr->OnMRecAdExpandedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnMRecAdCollapsedEvent"))
                {
                    Itr->OnMRecAdCollapsedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnMRecAdRevenuePaidEvent"))
                {
                    Itr->OnMRecAdRevenuePaidDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnInterstitialLoadedEvent"))
                {
                    Itr->OnInterstitialLoadedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnInterstitialHiddenEvent"))
                {
                    Itr->OnInterstitialHiddenDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnInterstitialDisplayedEvent"))
                {
                    Itr->OnInterstitialDisplayedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnInterstitialClickedEvent"))
                {
                    Itr->OnInterstitialClickedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnInterstitialAdRevenuePaidEvent"))
                {
                    Itr->OnInterstitialAdRevenuePaidDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnRewardedAdLoadedEvent"))
                {
                    Itr->OnRewardedAdLoadedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnRewardedAdDisplayedEvent"))
                {
                    Itr->OnRewardedAdDisplayedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnRewardedAdHiddenEvent"))
                {
                    Itr->OnRewardedAdHiddenDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnRewardedAdClickedEvent"))
                {
                    Itr->OnRewardedAdClickedDynamicDelegate.Broadcast(AdInfo);
                }
                else if (Name == TEXT("OnRewardedAdRevenuePaidEvent"))
                {
                    Itr->OnRewardedAdRevenuePaidDynamicDelegate.Broadcast(AdInfo);
                }
            }
        }
    });
}

void UAppLovinMAXDelegate::BroadcastAdErrorEvent(const FString &Name, const FAdInfo &AdInfo, const FAdError &AdError)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        for (TObjectIterator<UAppLovinMAXDelegate> Itr; Itr; ++Itr)
        {
            if (IsValidDelegate(*Itr))
            {
                if (Name == TEXT("OnBannerAdLoadFailedEvent"))
                {
                    Itr->OnBannerAdLoadFailedDynamicDelegate.Broadcast(AdInfo, AdError);
                }
                else if (Name == TEXT("OnMRecAdLoadFailedEvent"))
                {
                    Itr->OnMRecAdLoadFailedDynamicDelegate.Broadcast(AdInfo, AdError);
                }
                else if (Name == TEXT("OnInterstitialLoadFailedEvent"))
                {
                    Itr->OnInterstitialLoadFailedDynamicDelegate.Broadcast(AdInfo, AdError);
                }
                else if (Name == TEXT("OnInterstitialAdFailedToDisplayEvent"))
                {
                    Itr->OnInterstitialAdFailedToDisplayDynamicDelegate.Broadcast(AdInfo, AdError);
                }
                else if (Name == TEXT("OnRewardedAdLoadFailedEvent"))
                {
                    Itr->OnRewardedAdLoadFailedDynamicDelegate.Broadcast(AdInfo, AdError);
                }
                else if (Name == TEXT("OnRewardedAdFailedToDisplayEvent"))
                {
                    Itr->OnRewardedAdFailedToDisplayDynamicDelegate.Broadcast(AdInfo, AdError);
                }
            }
        }
    });
}

void UAppLovinMAXDelegate::BroadcastRewardedAdReceivedRewardEvent(const FAdInfo &AdInfo, const FAdReward &Reward)
{
    AsyncTask(ENamedThreads::GameThread, [=]()
    {
        for (TObjectIterator<UAppLovinMAXDelegate> Itr; Itr; ++Itr)
        {
            Itr->OnRewardedAdReceivedRewardDynamicDelegate.Broadcast(AdInfo, Reward);
        }
    });
}

