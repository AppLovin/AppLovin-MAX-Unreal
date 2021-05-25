# Integration

This page shows you how to download, import and configure the AppLovin MAX Unreal Plugin.

## Download the Latest Unreal Plugin

Link to extension

## Import the Plugin

Add the `AppLovinMAX` plugin to the `Plugins` directory in the root directory of your UE4 project.

## Requirements

- Unreal Engine 4.x
- For Android: ==TODO==
- For iOS builds:
    - Your build will fail if you do not build with Xcode 12 or higher. [The App Store now requires apps to be built with Xcode 12.](https://developer.apple.com/news/?id=ib31uj1j) AppLovin recommends that you update Xcode to version 12.5 or higher in order to stay ahead of anticipated minimum requirements.
    - The AppLovin MAX plugin requires CocoaPods. Install CocoaPods by following the instructions at [the CocoaPods Getting Started guide](https://guides.cocoapods.org/using/getting-started.html#getting-started).

## Initialize the SDK

Attach a function delegate to the `OnSdkInitializedDelegate` and initialize the SDK as soon as your app launches, as in the following code sample.

You may want to tag users with your own internal user ID so that you can then receive the ID in an impression-level user revenue API or in [S2S-rewarded](https://dash.applovin.com/documentation/mediation/s2s-rewarded-callback-api) postbacks. If so, set the user ID before you initialize the SDK.

```cpp
#include "AppLovinMAX.h"

UAppLovinMAX::OnSdkInitializedDelegate.AddLambda([](const FSdkConfiguration& SdkConfiguration)
{
    // AppLovin SDK is initialized, start loading ads
});

UAppLovinMAX::SetUserId("USER_ID")
UAppLovinMAX::Initialize("YOUR_SDK_KEY");
```

## iOS 14 Support

In iOS 14, Apple introduced global privacy changes that you need to comply with. This section explains how to comply with these changes and thereby avoid a material drop in revenue.

### SKAdNetwork

Update your app’s `Info.plist` in Unreal's Project Settings with network-specific identifiers. See the [SKAdNetwork documentation](#skadnetwork-2) for instructions.

### Consent Flow

Implement the [MAX Consent Flow](#consent-flow) to gain consent from your users on behalf of monetization partners. This prevents material ad revenue loss.

# Consent Flow

In iOS 14, Apple introduced the [App Tracking Transparency](https://developer.apple.com/documentation/apptrackingtransparency) framework. If your app wants to collect data that tracks a user or device across apps or sites, your app must use this framework to ask the user to allow such tracking. This page explains how you can use MAX to integrate this authorization process into your app.

MAX provides a built-in consent flow in **iOS SDK 10.0.0** and later. This flow communicates consent values to your monetization partners, which helps you monetize effectively on iOS 14.

MAX consent flow is a standalone solution for iOS 14.5+. To ask for consent from users on iOS 14.5+ you can use the MAX consent flow in place of any other consent flow you may have previously implemented. MAX consent flow is disabled by default, however. To enable it, follow the instructions below.

## Overview

[Same as documentation here](https://dash.applovin.com/documentation/mediation/ios/getting-started/consent-flow#overview)

## Enabling MAX Consent Flow

To enable MAX's built-in consent flow by using the app’s `Info.plist`, modify the `AppLovin_IOS_UPL.xml` file as follows:

1. Add the `NSUserTrackingUsageDescription` value. This string is how you inform your user why the app is requesting permission to use data that tracks the user or the device. AppLovin recommends that you use the string: **This only uses device info for less annoying, more relevant ads.**
2. Create a new key named `AppLovinConsentFlowInfo`.
3. Add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowEnabled` and the value `true`.
4. Add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowPrivacyPolicy` and the value of your Privacy Policy URL.
5. Optionally, add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowTermsOfService` and the value of your Terms of Service URL.

The final result should look similar to this:

```xml
<addElements tag="dict" once="true">
    <key>NSUserTrackingUsageDescription</key>
    <string>This only uses device info for less annoying, more relevant ads</string>
    <key>AppLovinConsentFlowInfo</key>
    <dict>
        <key>AppLovinConsentFlowEnabled</key>
        <true/>
        <key>AppLovinConsentFlowPrivacyPolicy</key>
        <string>https://your_company_name.com/privacy_policy</string>
        <key>AppLovinConsentFlowTermsOfService</key>
        <string>https://your_company_name.com/terms_of_service</string>
    </dict>
</addElements>
```

## Integration

The SDK presents the consent flow when you initialize the SDK. When the user completes the flow, the SDK calls your initialization completion callback.

AppLovin recommends that you initialize third-party SDKs (such as MMPs or analytics SDKs) in this callback. You should not initialize mediated network SDKs here, however, as MAX does this automatically.

If you set the user ID in your MMP integration, set it where you set the AppLovin user ID. The code below uses Adjust as an example. Please refer to Adjust’s documentation for more info on how to [initialize Adjust](https://github.com/adjust/unreal_sdk#integrate-the-sdk-into-your-app) and [set the user ID](https://github.com/adjust/unreal_sdk#session-callback-parameters).

```cpp
UAppLovinMAX::OnSdkInitializedDelegate.AddLambda([](const FSdkConfiguration& SdkConfiguration)
{
    // You can check app transparency tracking authorization in SdkConfiguration.AppTrackingStatus for iOS targets.
    // Initialize other third-party SDKs; do not initialize mediated advertising SDKs (MAX does that for you). Not following this step will result in noticeable integration issues.

    // Initialize the Adjust SDK inside the AppLovin SDK's initialization callback
    FAdjustConfig AdjustConfig;
    AdjustConfig.AppToken = "{YOUR_ADJUST_APP_TOKEN}";
    AdjustConfig.Environment = EAdjustEnvironment::Sandbox or EAdjustEnvironment::Production;
    
    UAdjust::Initialize(AdjustConfig);
});

UAppLovinMAX::SetUserId("USER_ID");
UAdjust::AddSessionCallbackParameter("YOUR_USER_ID_KEY", "USER_ID");

UAppLovinMAX::Initialize("SDK_KEY");
```

# Interstitials

## Loading an Interstitial Ad

The following code shows you how to attach listeners and load the first interstitial: ==TODO: Verify==

```cpp
const FString InterstitialAdUnitId = "YOUR_AD_UNIT_ID";
int RetryAttempt = 0;

static void InitializeInterstitialAds()
{
    // Attach function delegates
    UAppLovinMAX::OnInterstitialLoadedDelegate.Add(&OnInterstitialLoaded);
    UAppLovinMAX::OnInterstitialLoadFailedDelegate.Add(&OnInterstitialLoadFailed);
    UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate.Add(&OnInterstitialAdFailedToDisplay);
    UAppLovinMAX::OnInterstitialHiddenDelegate.Add(&OnInterstitialHidden);
    
    // Load first interstitial
    LoadInterstitial();
}

static void LoadInterstitial()
{
    UAppLovinMAX::LoadInterstitial(InterstitialAdUnitId);
}

static void OnInterstitialLoaded(const FAdInfo& AdInfo)
{
    // Interstitial ad is ready to be shown. UAppLovinMAX::IsInterstitialReady(InterstitialAdUnitId) will now return 'true'

    // Reset retry attempt
    RetryAttempt = 0;
}

static void OnInterstitialLoadFailed(const FAdInfo& AdInfo, const int ErrorCode)
{
    // Interstitial ad failed to load 
    // AppLovin recommends that you retry with exponentially higher delays, up to a maximum delay (in this case 64 seconds)
    
    RetryAttempt++;
    float RetryDelay = FMath::Pow(2.0f, FMath::Min(6, RetryAttempt));
            
    // TODO: Add timer logic 
}

static void OnInterstitialAdFailedToDisplay(const FAdInfo& AdInfo, const int ErrorCode)
{
    // Interstitial ad failed to display. AppLovin recommends that you load the next ad.
    LoadInterstitial();
}

static void OnInterstitialHidden(const FAdInfo& AdInfo)
{
    // Interstitial ad is hidden. Pre-load the next ad.
    LoadInterstitial();
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

The following code shows you how to attach listeners and load the first rewarded ad: ==TODO==

```cpp
const FString RewardedAdUnitId = "YOUR_AD_UNIT_ID";
int retryAttempt = 0;

void InitializeRewardedAds()
{
    // Attach function delegates
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

# MRECs

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
UAppLovinMAX::ShowMRec(MRecAdUnitId);
```

To hide an MREC ad, call `HideMRec()`:

```cpp
UAppLovinMAX::HideMRec(MRecAdUnitId);
```

# Privacy

The AppLovin SDK requires you to set a flag that indicates whether a user who is located in the European Union, European Economic Area, United Kingdom, and Switzerland (collectively, the “European Countries”) has provided opt-in consent for the collection and use of personal data for interest-based advertising. This framework facilitates GDPR compliance. It also enables you to comply with certain California privacy requirements and various children data restrictions under GDPR, COPPA, and App Store policies.

The following sections of this page describe the GDPR, children data, and CCPA requirements in detail.

## General Data Protection Regulation (“GDPR”)

AppLovin recommends that after you initialize the AppLovin SDK, you use the AppLovin SDK configuration object to determine if you should prompt a user in the European Countries for a consent dialog. If the user is located in the European Countries, AppLovin concludes that GDPR applies to that user and that you should prompt the user with a consent dialogue.

```cpp
UAppLovinMAX::OnSdkInitializedDelegate.AddLambda([](const FSdkConfiguration& SdkConfiguration)
{
    if (SdkConfiguration.ConsentDialogState == EConsentDialogState::Applies)
    {
        // Show user consent dialog
    }
    else if (SdkConfiguration.ConsentDialogState == EConsentDialogState::DoesNotApply)
    {
        // No need to show consent dialog, proceed with initialization
    }
    else
    {
        // Consent dialog state is unknown. Proceed with initialization, but check if the consent
        // dialog should be shown on the next application initialization
    }
});
```


If the user consents, set the user consent flag to `true` by calling `SetHasUserConsent(true)` and start requesting ads. Once you set the consent value to `true`, AppLovin will respect that value for the lifetime of your application or until the user revokes consent.

```cpp
UAppLovinMAX::SetHasUserConsent(true);
```

If the user does not consent, set the user consent flag to `false` by calling `SetHasUserConsent(false)` and start requesting ads. Once you set the consent value to `false`, AppLovin will respect that value for the lifetime of your application or until the user consents.

```cpp
UAppLovinMAX::SetHasUserConsent(false);
```

You do not have to set this flag for users who are outside of the European Countries. If you do set the flag for such users, this will not impact how ads are served to them.

The AppLovin SDK writes your app’s privacy states (“Age Restricted User”, “Has User Consent”, and “Do Not Sell”) to the log when you initialize the SDK.

The “Has User Consent” value that you can see in the logs will be either “true” or “false” if you set it correctly. Otherwise, it retains the default value of “No value set”. Note that once you set the consent value, AppLovin will treat it accordingly for the lifetime of your application or until you change the consent value.

You can also refer to the Mediation Debugger to see if you have set user consent correctly.

AppLovin MAX helps you obtain consent values on behalf of supported mediation partners. MAX shares these consent values via adapters. You must be on GDPR-supported network SDKs. Please consult with your network partner to determine correct SDK versions.

**AppLovin MAX does not help you obtain consent on behalf of Facebook, Inc.** For those networks on whose behalf AppLovin MAX does not help you to obtain consent values, you must work directly with the network to understand your obligations for GDPR compliance.

## Children Data

iOS Apps in the “Kids” category or iOS Apps intended for children may not use the AppLovin SDK per App Store Review Guidelines [1.3](https://developer.apple.com/app-store/review/guidelines/#kids-category) and [5.1.4](https://developer.apple.com/app-store/review/guidelines/#kids).

To ensure COPPA, GDPR, and Google Play policy compliance, you should indicate when a user is a child. If you know that the user is in an age-restricted category (i.e., under the age of 16), set the age-restricted user flag to `true`.

```cpp
UAppLovinMAX::SetIsAgeRestrictedUser(true);
```

If you know that the user is not in an age-restricted category (i.e., age 16 or older), set the age-restricted user flag to `false`.

```cpp
UAppLovinMAX::SetIsAgeRestrictedUser(false);
```

## California Consumer Privacy Act (“CCPA”)

You may choose to display a “Do Not Sell My Personal Information” link. If so, you may set a flag that indicates whether a user who is located in California, USA, has opted not to allow their personal data to be sold.

If such a user does not opt out of the sale of their personal information, set the do-not-sell flag to `false`.

```cpp
UAppLovinMAX::SetDoNotSell(false);
```

If such a user opts out of the sale of their personal information, set the do-not-sell flag to `true`.

```cpp
UAppLovinMAX::SetDoNotSell(true);
```

You do not need to set this flag for users who are outside California (or the USA). If you do set this flag for such users, this will not impact how ads are served to them.

AppLovin MAX helps you to obtain CCPA opt-out values on behalf of supported mediation partners. For those networks on whose behalf MAX does not help you to obtain opt-out values, you must work directly with the network to understand your obligations for CCPA compliance. Please consult with your network partner on the correct SDK versions.

## Facebook Data Processing Options for Users in California

To learn how to implement Facebook’s Limited Data Use flag, read the [Facebook developer documentation](https://developers.facebook.com/docs/marketing-apis/data-processing-options).

# SKAdNetwork

Apple provides SKAdNetwork as a privacy-safe way for ad networks like AppLovin to track installs. Until iOS 14, ad networks could attribute campaigns based on a user’s IDFA. But starting with iOS 14, ad networks are less able to use IDFAs to attribute which users have installed based on which ads they have interacted with.

## Why Should I Implement SKAdNetwork?

You can no longer rely on IDFA to track ad campaign performance. If you support SKAdNetwork, this increases the number of eligible campaigns that may serve on your app. This increases demand and therefore eCPMs.

## How Do I Implement SKAdNetwork?

Same as [iOS SKAdNetwork documentation section](https://dash.applovin.com/documentation/mediation/ios/getting-started/skadnetwork-info#how-do-i-implement-skadnetwork?).

# Advanced Settings

This page explains some advanced features of the AppLovin SDK. You will learn how to set the placement name for various ad unit types, how to request ads with muted audio, how to enable verbose logging, and how to retrieve the creative ID and network name of displayed ads.

## Ad Placements

You can set a placement name for each ad unit (for example, “Rewarded VideoLevels”, “INTER_levelEnd”, or “RewardedVideoCoinStore”). This can help you aggregate statistics for different placement categories.

Below are code snippets that show you how to set the placement name for various ad formats. Note that for banners and MRECs you must set the placement name before you load the banner or MREC.

### Banners

```cpp
UAppLovinMAX::CreateBanner(BannerAdUnitId, EAdViewPosition::BottomCenter);
UAppLovinMAX::SetBannerPlacement(BannerAdUnitId, MY_BANNER_PLACEMENT);
```

### MRECs

```cpp
UAppLovinMAX::CreateMRec(BannerAdUnitId, EAdViewPosition::Centered);
UAppLovinMAX::SetMRecPlacement(MRecAdUnitId, MY_MREC_PLACEMENT);
```

### Interstitials

```cpp
UAppLovinMAX::ShowInterstitial(InterstitialAdUnitId, MY_INTER_PLACEMENT);
```

### Rewarded Ads

```cpp
UAppLovinMAX::ShowRewardedAd(RewardedAdUnitId, MY_REWARDED_PLACEMENT);
```

## Mute Audio

You can mute audio for certain mediated SDK networks when you launch your app. The networks that support this functionality via the AppLovin SDK are AdMob, AppLovin, Mintegral, Snap, Tencent, and Vungle. For other networks, consult your network’s account team to learn whether this functionality is available and how to access it. The following code snippet shows how you mute audio on the supported networks:

```cpp
UAppLovinMAX::SetMuted(true);  // to mute
UAppLovinMAX::SetMuted(false); // to unmute
```

## Enable Verbose Logging

The following code snippet shows how you enable verbose logging:

```cpp
UAppLovinMAX::SetVerboseLoggingEnabled(true);
```

To verify that you enabled verbose logs successfully you can check for the line that reads **Verbose Logging On: true** in the initialization section of the AppLovin SDK logs:

```
AppLovin SDK
Version: 9.13.4
⋮
Verbose Logging On: true
⋮
```

AppLovin SDK tags its logs with the tag **/AppLovinSdk: [AppLovinSdk]**.
