# AppLovin MAX Unreal Plugin

AppLovin MAX Unreal Plugin for Android and iOS.

## Documentation

Check out our integration docs [here](https://dash.applovin.com/documentation/mediation/unreal/getting-started/integration).

## Demo App

The `/AppLovinMAXDemo` directory contains the demo app. The Unreal plugin is contained in `/AppLovinMAXDemo/Plugins/AppLovinMAX`.

**Note: If you're building on macOS with Xcode 13.3+, you'll need to include the following lines in `AppLovinMAXDemo.Target.cs` and `AppLovinMAXDemoEditor.Target.cs`**

```cpp
    // Add compiler argument to support building on macOS with Xcode 13.3+
    bOverrideBuildEnvironment = true;
    AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
```

<img src="https://user-images.githubusercontent.com/17148467/160496309-e1ef6519-c4cf-4d71-a34f-a17e79fb8bae.png" width="350" />

## License

MIT
