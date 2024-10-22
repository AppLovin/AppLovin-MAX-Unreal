// Copyright AppLovin Corporation. All Rights Reserved.

using UnrealBuildTool;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;

public class AppLovinMAX : ModuleRules
{
	public AppLovinMAX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.NoSharedPCHs;
		PrivatePCHHeaderFile = "Private/AppLovinMAXPrivatePCH.h";

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
			InstallIOS();
		}
		else if ( Target.Platform == UnrealTargetPlatform.Android )
		{
			InstallAndroid();
		}
		else
		{
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
		}
	}

	private void InstallIOS()
	{
		var AppLovinIOSPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "IOS" );
        var AppLovinPluginPath = Path.Combine( AppLovinIOSPath, "AppLovin", "MAX_Unreal_Plugin.embeddedframework.zip" );

		if ( !File.Exists( AppLovinPluginPath ) )
		{
			System.Console.WriteLine( "AppLovin IOS plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
			return;
		}

		System.Console.WriteLine( "AppLovin IOS plugin found" );

		var PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
		AdditionalPropertiesForReceipt.Add( "IOSPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_IOS.xml" ) );

		bEnableObjCAutomaticReferenceCounting = true;

		// Add support for linking with Swift frameworks
		PrivateDependencyModuleNames.Add( "Swift" );

		// Add the AppLovin Unreal iOS plugin
		PublicAdditionalFrameworks.Add(
			new Framework(
				"MAX_Unreal_Plugin",
				AppLovinPluginPath
			)
		);

		// Link with AppLovin SDK if installed
		var AppLovinSDKPath = "/Users/ritam.sarmah/AppLovin/AppLovin-MAX-Unreal/AppLovinMAXDemo/Pods/AppLovinSDK/applovin-ios-sdk-13.0.0/AppLovinSDK.xcframework";
		if ( Directory.Exists( AppLovinSDKPath ) )
		{
			System.Console.WriteLine( "AppLovin SDK linked from CocoaPods" );
			PublicAdditionalFrameworks.Add(
				new Framework(
					"AppLovinSDK",
					AppLovinSDKPath,
					null,
					true
				)
			);
		}
		else
		{
			System.Console.WriteLine( "AppLovin SDK not linked with CocoaPods" );
		}

		PublicDefinitions.Add( "WITH_APPLOVIN=1" );
	}

	private void InstallAndroid()
	{
		var AppLovinAndroidPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "Android", "repository", "com", "applovin", "applovin-max-unreal-plugin", "release" );
		var AppLovinPluginPath = Path.Combine( AppLovinAndroidPath, "applovin-max-unreal-plugin-release.aar" );
		if ( !File.Exists( AppLovinPluginPath ) )
		{
			System.Console.WriteLine( "AppLovin Android Plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
			return;
		}

		System.Console.WriteLine( "AppLovin Android Plugin found" );

		PrivateIncludePaths.Add( "AppLovinMAX/Private/Android" );
		
		// Includes Android JNI support 
		PrivateDependencyModuleNames.Add( "Launch" );

		var PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
		AdditionalPropertiesForReceipt.Add( "AndroidPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_Android.xml" ) );

		PublicDefinitions.Add( "WITH_APPLOVIN=1" );
	}
}
