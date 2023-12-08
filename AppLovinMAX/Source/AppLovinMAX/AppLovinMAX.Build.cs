// Copyright AppLovin Corporation. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AppLovinMAX : ModuleRules
{
	public AppLovinMAX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.NoSharedPCHs;

		PublicIncludePaths.AddRange( new string[] {} );
		PrivateIncludePaths.AddRange( new string[] { "AppLovinMAX/Private" } );
		PrivateIncludePathModuleNames.AddRange( new string[] { "Settings" } );
		PublicDependencyModuleNames.AddRange( new string[] { "Core", "Json", "JsonUtilities" } );
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Projects"
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange( new string[] { } );
		
		if ( Target.Platform == UnrealTargetPlatform.IOS )
		{
			string AppLovinIOSPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "IOS" );
			string AppLovinSDKPath = Path.Combine( AppLovinIOSPath, "AppLovin", "AppLovinSDK.xcframework" );
			string AppLovinResourcesPath = Path.Combine( AppLovinIOSPath, "AppLovin", "AppLovinSDKResources.bundle" );
			string AppLovinPluginPath = Path.Combine( AppLovinIOSPath, "AppLovin", "MAX_Unreal_Plugin.framework" );
			string AppLovinPodsPath = Path.Combine( AppLovinIOSPath, "Pods" );

			if ( Directory.Exists( AppLovinSDKPath ) && Directory.Exists( AppLovinPluginPath ) )
			{
				System.Console.WriteLine( "AppLovin IOS Plugin found" );

				string PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
				AdditionalPropertiesForReceipt.Add( "IOSPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_IOS.xml" ) );

				bEnableObjCAutomaticReferenceCounting = true;

				// Add support for linking with Swift frameworks
				PrivateDependencyModuleNames.Add( "Swift" );
	
				// Add the AppLovin SDK framework
				PublicAdditionalFrameworks.Add(
					new Framework(
						"AppLovinSDK",
						AppLovinSDKPath,
						AppLovinResourcesPath
					)
				);

				// Add the AppLovin Unreal iOS plugin
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MAX_Unreal_Plugin",
						AppLovinPluginPath
					)
				);

				// NOTE: For integrating adapters, please refer to our documentation:
				// https://dash.applovin.com/documentation/mediation/unreal/mediation-adapters/ios
				
				// #1: [Pods] Add system frameworks needed by third-party SDKs
				PublicFrameworks.AddRange(
				        new string[]
				        {
				                "AVFoundation",
				                "AdSupport",
				                "AudioToolbox",
				                "CFNetwork",
				                "CoreGraphics",
				                "CoreMedia",
				                "CoreMotion",
				                "CoreTelephony",
				                "CoreVideo",
				                "MediaPlayer",
				                "MessageUI",
				                "MobileCoreServices",
				                "QuartzCore",
				                "SafariServices",
				                "Security",
				                "StoreKit",
				                "SystemConfiguration",
				                "UIKit",
				                "WebKit"
				        }
				);

				// #2: [Pods] Copy build rules for weak frameworks
				PublicWeakFrameworks.AddRange(
				        new string[]
				        {
				                "AdSupport",
				                "AppTrackingTransparency",
				                "JavaScriptCore",
				                "SafariServices",
				                "WebKit"
				        }
				);

				// #3: [Pods] Add additional libraries needed by third-party SDKs
				AddEngineThirdPartyPrivateStaticDependencies( Target, "zlib" );

				// #4: [Pods] Add build rules for adapters and additional framework dependencies here

				PublicDefinitions.Add( "WITH_APPLOVIN=1" );
			}
			else
			{
				System.Console.WriteLine( "AppLovin IOS Plugin not found" );
				PublicDefinitions.Add( "WITH_APPLOVIN=0" );
			}

		}
		else if ( Target.Platform == UnrealTargetPlatform.Android )
		{
			string AppLovinAndroidPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "Android", "repository", "com", "applovin", "applovin-max-unreal-plugin", "release" );
			string AppLovinPluginPath = Path.Combine( AppLovinAndroidPath, "applovin-max-unreal-plugin-release.aar" );
			if ( File.Exists( AppLovinPluginPath ) )
			{
				System.Console.WriteLine( "AppLovin Android Plugin found" );

				PrivateIncludePaths.Add( "AppLovinMAX/Private/Android" );
				
				// Includes Android JNI support 
				PrivateDependencyModuleNames.Add( "Launch" );

				string PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
				AdditionalPropertiesForReceipt.Add( "AndroidPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_Android.xml" ) );

				PublicDefinitions.Add( "WITH_APPLOVIN=1" );
			}
			else
			{
				System.Console.WriteLine( "AppLovin Android Plugin not found" );
				PublicDefinitions.Add( "WITH_APPLOVIN=0" );
			}
		}
		else
		{
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
		}
	}
}
