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
		PrivatePCHHeaderFile = "Public/AppLovinMAX.h";

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
		var AppLovinSDKPath = Path.Combine( AppLovinIOSPath, "AppLovin", "AppLovinSDK.xcframework" );
		var AppLovinResourcesPath = Path.Combine( AppLovinIOSPath, "AppLovin", "AppLovinSDKResources.bundle" );
		var AppLovinPluginPath = Path.Combine( AppLovinIOSPath, "AppLovin", "MAX_Unreal_Plugin.framework" );
		var AppLovinPodsPath = Path.Combine( AppLovinIOSPath, "Pods" );

		if ( !Directory.Exists( AppLovinSDKPath ) || !Directory.Exists( AppLovinPluginPath ) )
		{
			System.Console.WriteLine( "AppLovin IOS Plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
		}

		System.Console.WriteLine( "AppLovin IOS Plugin found" );

		var PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
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

		var PluginFrameworks = new HashSet<string> {
			"AdSupport",
			"AudioToolbox",
			"AVFoundation",
			"CFNetwork",
			"CoreGraphics",
			"CoreMedia",
			"CoreMotion",
			"CoreTelephony",
			"MessageUI",
			"SafariServices",
			"StoreKit",
			"SystemConfiguration",
			"UIKit",
			"WebKit"
		};

		var PluginWeakFrameworks = new HashSet<string> { "AppTrackingTransparency" };
		var PluginSystemLibraries = new HashSet<string>();

		// Parse installed Pods configuration
		if ( Directory.Exists( AppLovinPodsPath ) )
		{
			XDocument PodConfig = XDocument.Load( Path.Combine( AppLovinPodsPath, "config.xml" ) );

			var TagsToPublicSet = new[] {
		        ("PublicFrameworks", PluginFrameworks),
		        ("PublicWeakFrameworks", PluginWeakFrameworks),
		        ("PublicSystemLibraries", PluginSystemLibraries)
		    };

		    foreach ( var (Tag, PluginSet) in TagsToPublicSet )
		    {
		        foreach ( var Item in PodConfig.Descendants( Tag ).Elements( "Item" ) )
		        {
		        	PluginSet.Add( Item.Value );
		        }
		    }

			// Process the additional libraries
			foreach ( var Item in PodConfig.Descendants( "PublicAdditionalLibraries" ).Elements( "Item" ) )
			{
				var LibraryPath = Path.Combine( AppLovinPodsPath, Item.Value );

				System.Console.WriteLine( "Adding CocoaPod library: " + LibraryPath );

				PublicAdditionalLibraries.Add( LibraryPath );
			}

			// Process the additional frameworks
			foreach ( var Item in PodConfig.Descendants( "PublicAdditionalFrameworks" ).Elements( "Item" ) )
			{
                var Name = Item.Element( "Name" ).Value;
				var FrameworkPath = Path.Combine( AppLovinPodsPath, Item.Element( "Path" ).Value );
				var Resources = Path.Combine( AppLovinPodsPath, Item.Element( "Resources" ).Value );

				System.Console.WriteLine( "Adding CocoaPod framework: " + Name + " (" + FrameworkPath + ")" );

				var AdditionalFramework = Resources != "None"
		            ? new Framework( Name, FrameworkPath, Resources )
		            : new Framework( Name, FrameworkPath );

				PublicAdditionalFrameworks.Add( AdditionalFramework );
			}
		}

		PublicFrameworks.AddRange( PluginFrameworks.ToArray() );
		PublicWeakFrameworks.AddRange( PluginWeakFrameworks.ToArray() );
		PublicSystemLibraries.AddRange( PluginSystemLibraries.ToArray() );

		AddEngineThirdPartyPrivateStaticDependencies( Target, "zlib" );

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
