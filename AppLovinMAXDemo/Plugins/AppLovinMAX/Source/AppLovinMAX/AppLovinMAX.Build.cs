// Copyright AppLovin Corporation. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AppLovinMAX : ModuleRules
{
	private const string VersionString = "4.26.2";

	public AppLovinMAX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange( new string[] {} );
		PrivateIncludePaths.AddRange( new string[] { "AppLovinMAX/Private" } );
		PrivateIncludePathModuleNames.AddRange( new string[] { "Settings" } );
		PublicDependencyModuleNames.AddRange( new string[] { "Core" } );
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange( new string[] { } );
		
		if ( Target.Platform == UnrealTargetPlatform.IOS )
		{
			string AppLovinIOSPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "IOS" );
			string AppLovinSDKPath = Path.Combine( AppLovinIOSPath, "AppLovinSDK.embeddedframework.zip" );
			string AppLovinBundlePath = Path.Combine( "Resources", "AppLovinSDKResources.bundle" );
			string PluginPath = Path.Combine( AppLovinIOSPath, "MAX_Unreal_Plugin.embeddedframework.zip" );
			if ( File.Exists( AppLovinSDKPath ) && File.Exists( PluginPath ) )
			{
				System.Console.WriteLine( "AppLovin IOS Plugin found" );
				
				// Add the AppLovin framework
				PublicAdditionalFrameworks.Add(
					new Framework(
						"AppLovinSDK",
						AppLovinSDKPath,
						AppLovinBundlePath
					)
				);

				// Add the AppLovin Unreal iOS plugin
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MAX_Unreal_Plugin",
						PluginPath
					)
				);

				PublicFrameworks.AddRange(
					new string[]
					{
						"AdSupport",
						"AppTrackingTransparency",
						"AudioToolbox",
						"AVFoundation",
						"CFNetwork",
						"CoreGraphics",
						"CoreMedia",
						"CoreTelephony",
						"Foundation",
						"SafariServices",
						"StoreKit",
						"SystemConfiguration",
						"UIKit",
						"WebKit"
					}
				);

				PublicDefinitions.Add( "WITH_APPLOVIN=1" );
				PublicDefinitions.Add( "UE4_APPLOVIN_VER=" + VersionString );

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
			string AppLovinAndroidPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "Android" );
			string AppLovinPluginPath = Path.Combine( AppLovinAndroidPath, "applovin-max-unreal-plugin.aar" );
			string AppLovinSDKPath = Path.Combine( AppLovinAndroidPath, "applovin-sdk-10.2.1.aar" );
			if ( File.Exists( AppLovinPluginPath ) )
			{
				System.Console.WriteLine( "AppLovin Android Plugin found" );

				PrivateIncludePaths.Add( "AppLovinMAX/Private/Android" );
				
				// Includes Android JNI support 
				PrivateDependencyModuleNames.Add( "Launch" );

				string PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
				AdditionalPropertiesForReceipt.Add( new ReceiptProperty( "AndroidPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_Android.xml" ) ) );

				PublicDefinitions.Add( "WITH_APPLOVIN=1" );
				PublicDefinitions.Add( "UE4_APPLOVIN_VER=" + VersionString );
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
