// Copyright AppLovin Corporation. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Xml.Linq;

public class AppLovinMAX : ModuleRules
{
	private const string AppLovinIOSVersion = "13.0.0";

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
		var AppLovinSDKPath = Path.Combine( AppLovinIOSPath, "AppLovin", "AppLovinSDK.xcframework" );
		var AppLovinSDKZipPath = Path.Combine( AppLovinIOSPath, "AppLovin", $"applovin-ios-sdk-{AppLovinIOSVersion}.zip" );
        var AppLovinPluginPath = Path.Combine( AppLovinIOSPath, "AppLovin", "MAX_Unreal_Plugin.embeddedframework.zip" );
		var AppLovinPodsPath = Path.Combine( AppLovinIOSPath, "Pods" );

		// Unzip the SDK if needed
		if ( !Directory.Exists( AppLovinSDKPath ) )
		{
			System.Console.WriteLine( $"Unzipping AppLovin IOS SDK: {AppLovinSDKZipPath}" );

			ProcessStartInfo startInfo = new ProcessStartInfo
			{
				FileName = "/bin/bash",
				Arguments = $"-c \"unzip {AppLovinSDKZipPath} -d {AppLovinSDKPath}\"",
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
				CreateNoWindow = true
			};

			try
			{
			    using (var process = Process.Start(startInfo))
			    {
			        process.WaitForExit();
			        if (process.ExitCode != 0)
			        {
			            string error = process.StandardError.ReadToEnd();
			            System.Console.WriteLine($"Failed to unzip AppLovin IOS SDK: {error}");
			        }
			        else
			        {
			            System.Console.WriteLine($"AppLovin IOS SDK unzipped to {AppLovinSDKPath}");
			        }
			    }
			}
			catch (System.Exception ex)
		    {
		        System.Console.WriteLine($"Failed to unzip AppLovin IOS SDK: {ex.Message}");
				return;
		    }
		}

		if ( !File.Exists( AppLovinPluginPath ) )
		{
			System.Console.WriteLine( "AppLovin IOS Plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
			return;
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
				null,
				true
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
				var Resources = Item.Element( "Resources" ).Value;
				var ResourcesPath = Resources != "None" ? Path.Combine( AppLovinPodsPath, Resources ) : null;

				System.Console.WriteLine( "Adding CocoaPod framework: " + Name + " (" + FrameworkPath + ")" );

				var AdditionalFramework = new Framework( Name, FrameworkPath, ResourcesPath, true );

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
