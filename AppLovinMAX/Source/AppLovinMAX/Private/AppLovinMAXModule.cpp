// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAXModule.h"

#define LOCTEXT_NAMESPACE "FAppLovinMAXModule"

void FAppLovinMAXModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FAppLovinMAXModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAppLovinMAXModule, AppLovinMAX)
