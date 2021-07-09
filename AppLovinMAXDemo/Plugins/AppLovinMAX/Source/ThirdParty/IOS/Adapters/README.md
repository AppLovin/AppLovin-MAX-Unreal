# Preparing Mediated Networks

This document includes instructions for integrating MAX mediation network adapters in your UE4 app.

## Requirements
- CocoaPods
- Python 3.5+

## Instructions

1. [Create a Podfile](https://dash.applovin.com/documentation/mediation/ios/mediation-adapters) for the adapters you want to integrate. This file should be in the same directory as the `install_adapters.py` script.
2. Run `install_adapters.py` to download the adapters.
3. Follow the instructions for ["Preparing Mediated Networks"](https://dash.applovin.com/documentation/mediation/manual-integration-ios"). Add any necessary changes to `Info.plist` in `AppLovinMAX_UPL_IOS.xml`
4. Download the third-party SDKs required by each installed adapter and create a corresponding `*.embeddedframework.zip` file in the directory `ThirdParty/IOS`.
5. Copy the iOS build rules below into `AppLovinMAX.Build.cs`. You will also need to add build rules for the third-party SDKs.

