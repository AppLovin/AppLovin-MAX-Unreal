// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAXDelegate.h"

bool IsValidDelegate(UAppLovinMAXDelegate *Delegate)
{
    // Check if delegate is non-null and not pending kill
    if (!IsValid(Delegate)) return false;

    UWorld *World = Delegate->GetWorld();
    return IsValid(World) && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
}

void UAppLovinMAXDelegate::OnSdkInitialized(const FSdkConfiguration &SdkConfiguration)
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
