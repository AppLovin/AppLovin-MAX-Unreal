# AppLovin MAX Unreal Plugin

AppLovin MAX Unreal Plugin for Android and iOS.

## Documentation

Please check out our [documentation](https://dash.applovin.com/documentation/mediation/unreal/getting-started/integration) to get started on integrating and enabling mediated networks using our guides.

## Demo App

The `/AppLovinMAXDemo` directory contains the demo app. The Unreal plugin is contained in `/AppLovinMAXDemo/Plugins/AppLovinMAX`.

### Getting Started

To get started with the demo app, follow the instructions below:

1. Clone this repository and open the AppLovinMAXDemo project in Unreal Engine.

    *If you're building on macOS with Xcode 13.3+, you'll need to first include the following lines in `AppLovinMAXDemo.Target.cs` and `AppLovinMAXDemoEditor.Target.cs` to compile the project:*

    ```cpp
        // Add compiler argument to support building on macOS with Xcode 13.3+
        bOverrideBuildEnvironment = true;
        AdditionalCompilerArguments = "-Wno-unused-but-set-variable -Wno-deprecated-builtins -Wno-bitwise-instead-of-logical -Wno-single-bit-bitfield-constant-conversion";
    ```

2. Under **Edit > Project Settings > Platforms**, modify the settings for your desired mobile platform(s):
    - **Android**:
        1. Under APK Packaging, click **Configure Now** if the project is not configured for the Android platform.
        2. Update the **Android Package Name** with the unique identifier associated with the app you will create or have already created in the MAX dashboard.
    - **iOS**:
        1. Under Bundle Information, update the **Bundle Identifier** with the unique identifier associated with the app you will create or have already created in the MAX dashboard.
        2. Under Build, enable **Automatic Signing** and enter your **IOS Team ID**.

3. Open the `Constants.h` file under `AppLovinMAXDemo/Source/AppLovinMAXDemo/Public/`.

4. Update the `SdkKey` value with the AppLovin SDK key associated with your account.

5. Update the MAX ad unit IDs for the ad formats you would like to test. Each ad format will correspond to a unique MAX ad unit ID you created in the AppLovin dashboard for the bundle ID used before. The demo app includes examples for both C++ and blueprint integrations.
    - **C++**: Update the values in `Constants.h`.
    - **Blueprints**: Open the widget blueprint for the corresponding ad format in the UE Editor (e.g., `InterstitialMenu`) and set the `AdUnitIdentifier` in the event graph. When the app is running, select the "Use Blueprint" checkbox in the ad format submenu to use the blueprint logic.

6. Deploy the demo app to a platform using the **Launch** button in UE4 or the **Platforms** button in UE5. Alternatively, package the project for a specific platform under **File > Package Project...**

<img src="https://user-images.githubusercontent.com/17148467/160496309-e1ef6519-c4cf-4d71-a34f-a17e79fb8bae.png" width="350" />

## License

MIT
