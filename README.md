# Integration

This page shows you how to download, import and configure the AppLovin MAX Unreal Plugin.

## Download the Latest Unreal Plugin

(Link to extension)

## Import the Plugin

Add the `AppLovinMAX` plugin to the `Plugins` directory in the root directory of your UE4 project.

## Requirements

- Unreal Engine 4 (==Add Version==)

## Initialize the SDK

Bind a function delegate to the `OnSdkInitializedDelegate` and initialize the SDK as soon as your app launches, as in the following code sample.

You may want to tag users with your own internal user ID so that you can then receive the ID in an impression-level user revenue API or in [S2S-rewarded](https://dash.applovin.com/documentation/mediation/s2s-rewarded-callback-api) postbacks. If so, set the user ID before you initialize the SDK.

```cpp
#include "AppLovinMAX.h"

UAppLovinMAX::OnSdkInitializedDelegate.AddLambda(
    [this](const FSdkConfiguration& SdkConfiguration)
    {
        // AppLovin SDK is initialized, start loading ads
    }
);

UAppLovinMAX::SetUserId("USER_ID")
UAppLovinMAX::Initialize("YOUR_SDK_KEY");
```

==Initializing with Blueprint==

## iOS 14 Support

In iOS 14, Apple introduced global privacy changes that you need to comply with. This section explains how to comply with these changes and thereby avoid a material drop in revenue.

### SKAdNetwork

==TODO==

### Consent Flow

Implement the [MAX Consent Flow ==TODO: link to heading==] to gain consent from your users on behalf of monetization partners. This prevents material ad revenue loss.

... (other stuff)

# Interstitials

## Loading an Interstitial Ad

The following code shows you how to attach listeners and load the first interstitial: 

```cpp
const FString InterstitialAdUnitId = "YOUR_AD_UNIT_ID";
int retryAttempt = 0;

void InitializeInterstitialAds()
{
    // Attach function delegates
    UAppLovinMAX::OnInterstitialLoadedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            // Interstitial ad is ready to be shown. UAppLovinMAX::IsInterstitialReady(InterstitialAdUnitId) will now return 'true'

            // Reset retry attempt
            retryAttempt = 0;
    	}
	);
    
    UAppLovinMAX::OnInterstitialLoadFailedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            // Interstitial ad failed to load 
        	// We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
            retryAttempt++;
            
            // TODO: Add timer logic            
    	}
	);
        
    UAppLovinMAX::OnInterstitialClickedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            ...
        }
    );
    
    // Load first interstitial
    loadInterstitial();
}

void loadInterstitial()
{
    UAppLovinMAX::LoadInterstitial(InterstitialAdUnitId);
}
```

## Showing an Interstitial Ad

To show an interstitial ad, call `ShowInterstitial()`

```cpp
if (UAppLovinMAX::IsInterstitialReady(InterstitialAdUnitId))
{
    UAppLovinMAX::ShowInterstitial(InterstitialAdUnitId);
}
```

# Rewarded Ads

## Loading a Rewarded Ad

The following code shows you how to attach listeners and load the first rewarded ad:

```cpp
const FString RewardedAdUnitId = "YOUR_AD_UNIT_ID";
int retryAttempt = 0;

void InitializeRewardedAds()
{
    // Attach function delegates
    UAppLovinMAX::OnInterstitialLoadedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            // Interstitial ad is ready to be shown. UAppLovinMAX::IsInterstitialReady(InterstitialAdUnitId) will now return 'true'

            // Reset retry attempt
            retryAttempt = 0;
    	}
	);
    
    UAppLovinMAX::OnInterstitialLoadFailedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            // Interstitial ad failed to load 
        	// We recommend retrying with exponentially higher delays up to a maximum delay (in this case 64 seconds)
            retryAttempt++;
            
            // TODO: Add timer logic            
    	}
	);
        
    UAppLovinMAX::OnInterstitialClickedDelegate.AddLambda(
        [this](const FAdInfo& AdInfo)
        {
            ...
        }
    );
    
    // Load first interstitial
    loadInterstitial();
}

void loadInterstitial()
{
    UAppLovinMAX::LoadInterstitial(InterstitialAdUnitId);
}
```

## Showing an Rewarded Ad

To show an rewarded ad, call `ShowRewardedAd()`

```cpp
if (UAppLovinMAX::IsRewardedAdReady(RewardedAdUnitId))
{
    UAppLovinMAX::ShowRewardedAd(RewardedAdUnitId);
}
```

## S2S Rewarded Callbacks

To learn how to receive callbacks to your currency server, see our [MAX S2S Rewarded Callback API](https://dash.applovin.com/documentation/mediation/s2s-rewarded-callback-api) guide and update the S2S Rewarded Callback URL in your MAX Ad Unit edit page.

# Banners

The following sections show you how to load and then show and hide a banner ad.

## Programmatic Method

### Creating a Banner

To load a banner, call `CreateBanner()`, passing it your Ad Unit ID and desired banner position:

```cpp
const FString BannerAdUnitId = "YOUR_AD_UNIT_ID";

void InitializeBannerAds()
{
        // Banners are automatically sized to 320×50 on phones and 728×90 on tablets
    // You may call the utility method AppLovinMAX.isTablet() to help with view sizing adjustments
    UAppLovinMAX::CreateBanner(BannerAdUnitId, EAdViewPosition::BottomCenter);
    
    // Set background or background color for banners to be fully functional
    UAppLovinMAX::SetBannerBackgroundColor(BannerAdUnitId, <YOUR_BANNER_BACKGROUND_COLOR>);
}
```

The above example creates a banner at the bottom-center of the display ( `BottomCenter`). The complete list of position options are `TopLeft`, `TopCenter`, `TopRight`, `Centered`, `CenterLeft`, `CenterRight`, `BottomLeft`, `BottomCenter`, and `BottomRight`.

Set your banner background color to any `FColor`, for example `FColor::Black`.

### Showing/Hiding a Banner

To show a banner, call `ShowBanner()`:

```cpp
UAppLovinMAX::ShowBanner(BannerAdUnitId);
```

To hide a banner, call `HideBanner()`:

```cpp
UAppLovinMAX::HideBanner(BannerAdUnitId);
```

## Blueprints

[Insert image]

# MRECs

## Programmatic Method

### Loading an MREC

To load an MREC, call `CreateMRec()`, passing in your Ad Unit ID and desired adview position:

```cpp
const FString MRecAdUnitId = "YOUR_AD_UNIT_ID";

void InitializeMRecAds()
{
    // MRECs are sized to 300x250 on phones and tablets
    // You may use the utility method `AppLovinMAX.isTablet()` to help with view sizing adjustments
    UAppLovinMAX::CreateMRec(MRecAdUnitId, EAdViewPosition::Center);
}
```

### Showing/Hiding an MRec

To show an MREC ad, call `ShowMRec()`:

```cpp
UAppLovinMAX::ShowMRec(BannerAdUnitId);
```

To hide an MREC ad, call `HideMRec()`:

```cpp
UAppLovinMAX::HideMRec(BannerAdUnitId);
```



## Blueprints

[Insert image]



# Advanced Settings

This page explains some advanced features of the AppLovin SDK. You will learn how to set the placement name for various ad unit types, how to request ads with muted audio, how to enable verbose logging, and how to retrieve the creative ID and network name of displayed ads.



## Enable Verbose Logging

### Programmatically

```cpp
AppLovinMAX.setVerboseLoggingEnabled(true);
```

### Blueprints

==TODO==

To verify that you enabled verbose logs successfully you can check for the line that reads **Verbose Logging On: true** in the initialization section of the AppLovin SDK logs:
AppLovin SDKVersion: 9.13.4⋮Verbose Logging On: true⋮
AppLovin SDK tags its logs with the tag **/AppLovinSdk: [AppLovinSdk]**.

To verify that you enabled verbose logs successfully you can check for the line that reads **Verbose Logging On: true** in the initialization section of the AppLovin SDK logs:

```
AppLovin SDKVersion: 9.13.4⋮Verbose Logging On: true⋮
```

==TODO== (two separate log locations)

AppLovin SDK tags its logs to Unreal with the tag **[LogAppLovinMAX]**

sdk device logs will show [AppLovinSdk]

# OTHER


```cpp
TArray<FString> TestDeviceIDs;
TestDeviceIDs.Add("TEST_DEVICE_ID1");
TestDeviceIDs.Add("TEST_DEVICE_ID2");

UAppLovinMAX::SetTestDeviceAdvertisingIdentifiers(TestDeviceIDs);
```

```cpp
    // Track Event Testing
    /* UAppLovinMAX::TrackEvent("Test Event"); */

    /* TMap<FString, FString> Params; */
    /* Params.Add("hello", "world"); */
    /* Params.Add("happy", "birthday"); */

    /* UAppLovinMAX::TrackEvent("Test Event 2", Params); */
```



