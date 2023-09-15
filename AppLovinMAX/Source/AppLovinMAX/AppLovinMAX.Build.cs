// Copyright AppLovin Corporation. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AppLovinMAX : ModuleRules
{
	public AppLovinMAX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
			string AppLovinSDKPath = Path.Combine( AppLovinIOSPath, "AppLovinSDK.embeddedframework.zip" );
			string AppLovinPluginPath = Path.Combine( AppLovinIOSPath, "MAX_Unreal_Plugin.embeddedframework.zip" );
			if ( File.Exists( AppLovinSDKPath ) && File.Exists( AppLovinPluginPath ) )
			{
				System.Console.WriteLine( "AppLovin IOS Plugin found" );

				// Add support for linking with Swift frameworks
				string IOSSdkRoot = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcrun", "--sdk iphoneos --show-sdk-path");
				PublicSystemLibraryPaths.Add(IOSSdkRoot + "/usr/lib/swift");
				PublicSystemLibraryPaths.Add(IOSSdkRoot + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos");
				PublicSystemLibraryPaths.Add(IOSSdkRoot + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.0/iphoneos");
				PublicSystemLibraryPaths.Add(IOSSdkRoot + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.5/iphoneos");
				
				// Add the AppLovin SDK framework
				PublicAdditionalFrameworks.Add(
					new Framework(
						"AppLovinSDK",                          // Framework name
						AppLovinSDKPath,                        // Zipped framework path
						"Resources/AppLovinSDKResources.bundle" // Resources path in ZIP
					)
				);

				// Add the AppLovin Unreal iOS plugin
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MAX_Unreal_Plugin",
						AppLovinPluginPath
					)
				);

				// NOTE: For integrating adapters, run install_adapters.py and update the build rules
				// below based on the script instructions

				// #1: [Adapters] Add build rules for adapters and additional framework dependencies here

				// #2: [Adapters] Add system frameworks needed by third-party SDKs
				PublicFrameworks.AddRange(
					new string[]
					{
						"AVFoundation",
						"AdSupport",
						"AppTrackingTransparency",
						"AudioToolbox",
						"CFNetwork",
						"CoreGraphics",
						"CoreMedia",
						"CoreMotion",
						"CoreTelephony",
						"Foundation",
						"MessageUI",
						"SafariServices",
						"StoreKit",
						"SystemConfiguration",
						"UIKit",
						"WebKit"
					}
				);

				string PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
				AdditionalPropertiesForReceipt.Add( "IOSPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_IOS.xml" ) );

				PublicDefinitions.Add( "WITH_APPLOVIN=1" );

				// #3: [Adapters] Add additional libraries needed by third-party SDKs
				AddEngineThirdPartyPrivateStaticDependencies( Target, "zlib" );
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
