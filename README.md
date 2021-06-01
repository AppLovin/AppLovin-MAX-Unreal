# Integration

This page shows you how to download, import and configure the AppLovin MAX Unreal Plugin.

## Download the Latest Unreal Plugin

Link to extension

## Import the Plugin

Take the following steps to import the plugin into your project:

1. Go to the directory in your project containing the file `{PROJECTNAME}.uproject`
2. Create a `Plugins` directory if it does not exist.
3. Move the `AppLovinMAX` directory to the `Plugins` directory in your project.
4. Add `AppLovinMAX` as a dependency to the `PublicDependencyModuleNames` array inside of your app's `{PROJECTNAME}.Build.cs` file:

```c#
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AppLovinMAX" });
```

## Requirements

- Unreal Engine 4.x
- For iOS builds:
    - Your build will fail if you do not build with Xcode 12 or higher. [The App Store now requires apps to be built with Xcode 12.](https://developer.apple.com/news/?id=ib31uj1j) AppLovin recommends that you update Xcode to version 12.5 or higher in order to stay ahead of anticipated minimum requirements.

## Initialize the SDK

Attach a function delegate to the `OnSdkInitializedDelegate` and initialize the SDK as soon as your app launches, as in the code sample below.

You may want to tag users with your own internal user ID so that you can then receive the ID in an impression-level user revenue API or in [S2S-rewarded](https://dash.applovin.com/documentation/mediation/s2s-rewarded-callback-api) postbacks. If so, set the user ID before you initialize the SDK.

```cpp
#include "AppLovinMAX.h"

UAppLovinMAX::OnSdkInitializedDelegate.AddLambda([](const FSdkConfiguration &SdkConfiguration)
{
    // AppLovin SDK is initialized, start loading ads
});

UAppLovinMAX::SetUserId("USER_ID");
UAppLovinMAX::Initialize("YOUR_SDK_KEY");
```

## iOS 14 Support

In iOS 14, Apple introduced global privacy changes that you need to comply with. This section explains how to comply with these changes and thereby avoid a material drop in revenue.

### SKAdNetwork

Update your app’s `Info.plist` in Unreal's Project Settings with network-specific identifiers. See the [SKAdNetwork documentation](#skadnetwork-1) for instructions.

### Consent Flow

Implement the [MAX Consent Flow](#consent-flow) to gain consent from your users on behalf of monetization partners. This prevents material ad revenue loss.

# Consent Flow

In iOS 14, Apple introduced the [App Tracking Transparency](https://developer.apple.com/documentation/apptrackingtransparency) framework. If your app wants to collect data that tracks a user or device across apps or sites, your app must use this framework to ask the user to allow such tracking. This page explains how you can use MAX to integrate this authorization process into your app.

MAX provides a built-in consent flow in **iOS SDK 10.0.0** and later. This flow communicates consent values to your monetization partners, which helps you monetize effectively on iOS 14.

MAX consent flow is a standalone solution for iOS 14.5+. To ask for consent from users on iOS 14.5+ you can use the MAX consent flow in place of any other consent flow you may have previously implemented. MAX consent flow is disabled by default, however. To enable it, follow the instructions below.

## Overview

[Same as documentation here](https://dash.applovin.com/documentation/mediation/ios/getting-started/consent-flow#overview)

## Enabling MAX Consent Flow

To enable MAX's built-in consent flow by using the app’s `Info.plist`, modify the `<iosPlistUpdates>` tag in `AppLovin_IOS_UPL.xml` as follows:

1. Add the `NSUserTrackingUsageDescription` value. This string is how you inform your user why the app is requesting permission to use data that tracks the user or the device. AppLovin recommends that you use the string: **This only uses device info for less annoying, more relevant ads.**
2. Create a new key named `AppLovinConsentFlowInfo`.
3. Add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowEnabled` and the value `true`.
4. Add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowPrivacyPolicy` and the value of your Privacy Policy URL.
5. Optionally, add a new key-value pair inside `AppLovinConsentFlowInfo` with the key `AppLovinConsentFlowTermsOfService` and the value of your Terms of Service URL.

The final result should look similar to this:

```xml
<iosPListUpdates>
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
</iosPListUpdates>
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

The following code shows you how to bind to delegates and load the first interstitial.

```cpp
// UMyWidget.cpp (UMyWidget inherits from UObject to access TimerManager)

#include "UMyWidget.h"
#include "AppLovinMAX.h"

// For retry timer logic
#include "Async/Async.h"
#include "Engine/World.h"
#include "TimerManager.h"

const FString InterstitialAdUnitId = TEXT("YOUR_AD_UNIT_ID");
int RetryAttempt = 0;
FTimerHandle LoadTimerHandle;

void UMyWidget::InitializeInterstitialAds()
{
    // Bind member functions to delegates
    UAppLovinMAX::OnInterstitialLoadedDelegate.AddUObject(this, &UMyWidget::OnInterstitialLoaded);
    UAppLovinMAX::OnInterstitialLoadFailedDelegate.AddUObject(this, &UMyWidget::OnInterstitialLoadFailed);
    UAppLovinMAX::OnInterstitialAdFailedToDisplayDelegate.AddUObject(this, &UMyWidget::OnInterstitialAdFailedToDisplay);
    UAppLovinMAX::OnInterstitialHiddenDelegate.AddUObject(this, &UMyWidget::OnInterstitialHidden);
    
    // Load first interstitial
    LoadInterstitial();
}

void UMyWidget::LoadInterstitial()
{
    UAppLovinMAX::LoadInterstitial(InterstitialAdUnitId);
}

void UMyWidget::OnInterstitialLoaded(const FAdInfo &AdInfo)
{
    // Interstitial ad is ready to be shown. UAppLovinMAX::IsInterstitialReady(InterstitialAdUnitId) will now return 'true'

    // Reset retry attempt
    RetryAttempt = 0;
}

void UMyWidget::OnInterstitialLoadFailed(const FAdInfo &AdInfo, const FAdError &AdError)
{
    // Interstitial ad failed to load 
    // AppLovin recommends that you retry with exponentially higher delays, up to a maximum delay (in this case 64 seconds)
    
    RetryAttempt++;
    AsyncTask(ENamedThreads::GameThread, [this]() {           
        float RetryDelay = FMath::Pow(2.0f, FMath::Min(6, RetryAttempt));
        GetWorld()->GetTimerManager().SetTimer(LoadTimerHandle, this, &UMyWidget::LoadInterstitial, RetryDelay, false);
    });
}

void UMyWidget::OnInterstitialAdFailedToDisplay(const FAdInfo &AdInfo, const FAdError &AdError)
{
    // Interstitial ad failed to display. AppLovin recommends that you load the next ad.
    LoadInterstitial();
}

void UMyWidget::OnInterstitialHidden(const FAdInfo &AdInfo)
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

The following code shows you how to bind to delegates and load the first rewarded ad:

```cpp
// UMyWidget.cpp (UMyWidget inherits from UObject to access TimerManager)

#include "UMyWidget.h"
#include "AppLovinMAX.h"

// For retry timer logic
#include "Async/Async.h"
#include "Engine/World.h"
#include "TimerManager.h"

const FString RewardedAdUnitId = TEXT("YOUR_AD_UNIT_ID");
int RetryAttempt = 0;
FTimerHandle LoadTimerHandle;

void UMyWidget::InitializeRewardedAds()
{   
	// Bind member functions to delegates
    UAppLovinMAX::OnRewardedAdLoadedDelegate.AddUObject(this, &UMyWidget::OnRewardedAdLoaded);
    UAppLovinMAX::OnRewardedAdLoadFailedDelegate.AddUObject(this, &UMyWidget::OnRewardedAdLoadFailed);
    UAppLovinMAX::OnRewardedAdFailedToDisplayDelegate.AddUObject(this, &UMyWidget::OnRewardedAdFailedToDisplay);
    UAppLovinMAX::OnRewardedAdHiddenDelegate.AddUObject(this, &UMyWidget::OnRewardedAdHidden);
    UAppLovinMAX::FOnRewardedAdReceivedRewardDelegate.AddUObject(this, &UMyWidget::OnRewardedAdReceivedReward);
    
    // Load first rewarded ad
    LoadRewardedAd();
}

void UMyWidget::LoadRewardedAd()
{
    UAppLovinMAX::LoadRewardedAd(RewardedAdUnitId);
}

void UMyWidget::OnRewardedAdLoaded(const FAdInfo &AdInfo)
{
    // Rewarded ad is ready to be shown. UAppLovinMAX::IsRewardedAdReady(RewardedAdUnitId) will now return 'true'

    // Reset retry attempt
    RetryAttempt = 0;
}

void UMyWidget::OnRewardedAdLoadFailed(const FAdInfo &AdInfo, const FAdError &AdError)
{
    // Rewarded ad failed to load 
    // AppLovin recommends that you retry with exponentially higher delays, up to a maximum delay (in this case 64 seconds)
    
    RetryAttempt++;
    
    AsyncTask(ENamedThreads::GameThread, [this]() {           
        float RetryDelay = FMath::Pow(2.0f, FMath::Min(6, RetryAttempt));
        GetWorld()->GetTimerManager().SetTimer(LoadTimerHandle, this, &UMyWidget::LoadRewardedAd, RetryDelay, false);
    });
}

void UMyWidget::OnRewardedAdFailedToDisplay(const FAdInfo &AdInfo, const FAdError &AdError)
{
    // Rewarded ad failed to display. AppLovin recommends that you load the next ad.
    LoadRewardedAd();
}

void UMyWidget::OnRewardedAdHidden(const FAdInfo &AdInfo)
{
    // Rewarded ad is hidden. Pre-load the next ad.
    LoadRewardedAd();
}

void UMyWidget::OnRewardedAdReceivedReward(const FAdInfo &AdInfo, const FAdReward &Reward)
{
    // Rewarded ad was displayed and user should receive the reward
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

## Creating a Banner

To load a banner, call `CreateBanner()`, passing it your Ad Unit ID and desired banner position:

```cpp
const FString BannerAdUnitId = TEXT("YOUR_AD_UNIT_ID");

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

Set your banner background color to any `FColor`, for example `FColor::Black` or `FColor::FromHex("#FFF200")` (yellow).

## Showing a Banner

To show a banner, call `ShowBanner()`:

```cpp
UAppLovinMAX::ShowBanner(BannerAdUnitId);
```

## Hiding a Banner

To hide a banner, call `HideBanner()`:

```cpp
UAppLovinMAX::HideBanner(BannerAdUnitId);
```

# MRECs

## Loading an MREC

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

## Showing an MREC

To show an MREC ad, call `ShowMRec()`:

```cpp
UAppLovinMAX::ShowMRec(MRecAdUnitId);
```

## Hiding an MREC

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

# Error Codes

Your ad display delegate or callback interface will receive a call if an ad failed to load or failed to display. This call will be accompanied by an error code. This page describes the error codes you may see.

### Error Object

The error that you receive in your callback is a first-class error object , `AdError`. This object has three properties:

1. `Code` — the error code, which you can find in the table below.
2. `Message` — a human-readable message that describes the error.
3. `AdLoadFailureInfo` — a description string that, when present, contains error codes and reasons why each mediated network failed to load an ad.

## MAX Error Codes

[Same as documentation here](https://dash.applovin.com/documentation/mediation/react-native/getting-started/errorcodes#error-codes)

# SKAdNetwork

Apple provides SKAdNetwork as a privacy-safe way for ad networks like AppLovin to track installs. Until iOS 14, ad networks could attribute campaigns based on a user’s IDFA. But starting with iOS 14, ad networks are less able to use IDFAs to attribute which users have installed based on which ads they have interacted with.

## Why Should I Implement SKAdNetwork?

You can no longer rely on IDFA to track ad campaign performance. If you support SKAdNetwork, this increases the number of eligible campaigns that may serve on your app. This increases demand and therefore eCPMs.

## How Do I Implement SKAdNetwork?

To use SKAdNetwork, determine the set of networks that have permission to show ads within your app, and check their checkboxes in the Info.plist Generator below. Then add each network’s `SKAdNetworkIdentifier` list, as shown in concatenated form in the code segment generated by the **Info.plist Generator**, to your app’s `Info.plist` file (e.g., by including it under `iosPListUpdates` in `AppLovinMAX_IOS_UPL.xml`). For more information, see [“SKAdNetwork: Configuring the Participating Apps”](https://developer.apple.com/documentation/storekit/skadnetwork/configuring_the_participating_apps) in Apple’s StoreKit Developer documentation.

If you only need SKAdNetwork identifiers for the AppLovin network (not for any of the other ad networks that AppLovin mediates), you can also retrieve that list in the JSON or XML structure that is returned if you request one of the following URLs:

- https://dash.applovin.com/docs/v1/skadnetwork_ids.json (JSON)
- https://dash.applovin.com/docs/v1/skadnetwork_ids.xml (XML)

If you also need SKAdNetwork identifiers for other mediated ad networks that you integrated with MAX, check those networks in the **Info.plist Generator** below. This will update the `Info.plist` code segment in the Generator by adding each of those networks’ `SKAdNetworkIdentifier` lists. (If you do not check any networks, only the `SKAdNetworkIdentifier` list from the AppLovin network will appear in the code segment.)

# Advanced Settings

This page explains some advanced features of the AppLovin SDK. You will learn how to set the placement name for various ad unit types, how to request ads with muted audio, how to enable verbose logging, and how to retrieve the creative ID and network name of displayed ads.

## Ad Placements

You can set a placement name for each ad unit (for example, "Rewarded VideoLevels", "INTER_levelEnd", or "RewardedVideoCoinStore"). This can help you aggregate statistics for different placement categories.

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

## Creative ID and Network Name

You can retrieve the creative ID and the network name of displayed ads of various mediated networks. Refer to the [Creative Debugger](https://dash.applovin.com/documentation/mediation/unreal/testing-networks/creative-debugger) documentation for more information.

---

# MAX Demo App

MAX Demo App is a sample app that demonstrates how you can integrate MAX into your mobile apps.

You can integrate those network SDKs that you desire. Then you can monitor the callbacks that MAX makes to your app that notify you of events for each ad format. This way you can ensure that you have completed the integration successfully.

AppLovin recommends that you use [Test Mode](https://dash.applovin.com/documentation/mediation/unreal/testing-networks/test-mode) with this **Demo App** as you load and show ads during the testing process.You can find the open-source MAX demo apps in the AppLovin [Unreal Engine repository](https://github.com/AppLovin/AppLovin-MAX-Unreal).

Watch the repository in order to receive the latest notifications on **MAX Demo App** updates and SDK releases.

# Creative Debugger

With the AppLovin MAX SDK, you can expose comprehensive ad information in your apps. You can identify problematic ads when you incorporate the creative debugger into your test environments, QA processes, and support ticket systems.

The creative debugger displays a button over fullscreen ads when you complete a special gesture (described in the video below). If you then press the button that appears, you will see an alert that contains information about the current ad (such as its mediated network and creative ID), and you will be able to report creative issues for that ad.

To disable the creative debugger in code, pass `false` to the `SetCreativeDebuggerEnabled()` method of the MAX SDK:

```cpp
UAppLovinMAX::SetCreativeDebuggerEnabled(false);
```

## Creative ID

You can also retrieve the creative ID of displayed ads of various mediated networks. You can do this either by using the creative debugger, or by using one of the ad lifecycle callbacks in the MAX SDK. The following code shows you how to do this with the MAX SDK in the "ad displayed" callback of an interstitial ad:

```cpp
UAppLovinMAX::OnInterstitialDisplayedDelegate.AddLambda([](const FAdInfo &AdInfo)
{
    FString CreativeId = AdInfo.CreativeIdentifier;
});                                            
```

## Network Name

You can also retrieve the name of the ad network that is responsible for an ad. The following code shows you how to do this with the MAX SDK in the "ad displayed" callback of an interstitial ad:

```cpp
UAppLovinMAX::OnInterstitialDisplayedDelegate.AddLambda([](const FAdInfo &AdInfo)
{
    FString NetworkName = AdInfo.NetworkName;
});       
```

# Mediation Debugger

## Displaying the Mediation Debugger

To display the Mediation Debugger, make the following call:

```cpp
UAppLovinMAX::ShowMediationDebugger();
```

