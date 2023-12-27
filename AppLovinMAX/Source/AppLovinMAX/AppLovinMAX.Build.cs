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

		if ( Directory.Exists( AppLovinSDKPath ) && Directory.Exists( AppLovinPluginPath ) )
		{
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

			var SdkPublicFrameworks = new HashSet<string> {
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

			var SdkPublicWeakFrameworks = new HashSet<string> { "AppTrackingTransparency" };
			var SdkPublicSystemLibraries = new HashSet<string>();

			if ( Directory.Exists( AppLovinPodsPath ) )
			{
				XDocument PodConfig = XDocument.Load( Path.Combine( AppLovinPodsPath, "config.xml" ) );

				var PodPublicFrameworks = PodConfig.Descendants("PublicFrameworks").Descendants("item");
				foreach ( var Item in PodPublicFrameworks )
				{
					SdkPublicFrameworks.Add( Item.Value );
				}
				
				var PodPublicWeakFrameworks = PodConfig.Descendants("PublicWeakFrameworks").Descendants("item");
				foreach ( var Item in PodPublicWeakFrameworks )
				{
					SdkPublicWeakFrameworks.Add( Item.Value );
				}

				var PodPublicSystemLibraries = PodConfig.Descendants("PublicSystemLibraries").Descendants("item");
				foreach ( var Item in PodPublicSystemLibraries )
				{
					SdkPublicSystemLibraries.Add( Item.Value );
				}

		        var PodPublicAdditionalFrameworks = PodConfig.Descendants("PublicAdditionalFrameworks").Descendants("item");
				foreach ( var Item in PodPublicAdditionalFrameworks )
				{
                    var Name = Item.Element("Name")?.Value;
                    var PathComponents = Item.Element("PathComponents")?
						.Elements("item")
						.Select(Component => Component.Value)
						.ToList();
                    var Resources = Item.Element("Resources")?.Value;

					if ( Name != null && PathComponents != null )
					{
						var AdditionalFrameworkPath = Path.Combine( PathComponents.ToArray() );
						System.Console.WriteLine(AdditionalFrameworkPath);

						if ( Resources != null )
						{
							PublicAdditionalFrameworks.Add(
								new Framework( Name, AdditionalFrameworkPath, Resources )
							);
						}
						else
						{
							PublicAdditionalFrameworks.Add(
								new Framework( Name, AdditionalFrameworkPath )
							);
						}
					}
				}
			}

			PublicFrameworks.AddRange( SdkPublicFrameworks.ToArray() );
			PublicWeakFrameworks.AddRange( SdkPublicWeakFrameworks.ToArray() );
			PublicSystemLibraries.AddRange( SdkPublicSystemLibraries.ToArray() );

			AddEngineThirdPartyPrivateStaticDependencies( Target, "zlib" );

			PublicDefinitions.Add( "WITH_APPLOVIN=1" );
		}
		else
		{
			System.Console.WriteLine( "AppLovin IOS Plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
		}
	}

	private void InstallAndroid()
	{
		var AppLovinAndroidPath = Path.Combine( ModuleDirectory, "..", "ThirdParty", "Android", "repository", "com", "applovin", "applovin-max-unreal-plugin", "release" );
		var AppLovinPluginPath = Path.Combine( AppLovinAndroidPath, "applovin-max-unreal-plugin-release.aar" );
		if ( File.Exists( AppLovinPluginPath ) )
		{
			System.Console.WriteLine( "AppLovin Android Plugin found" );

			PrivateIncludePaths.Add( "AppLovinMAX/Private/Android" );
			
			// Includes Android JNI support 
			PrivateDependencyModuleNames.Add( "Launch" );

			var PluginPath = Utils.MakePathRelativeTo( ModuleDirectory, Target.RelativeEnginePath );
			AdditionalPropertiesForReceipt.Add( "AndroidPlugin", Path.Combine( PluginPath, "AppLovinMAX_UPL_Android.xml" ) );

			PublicDefinitions.Add( "WITH_APPLOVIN=1" );
		}
		else
		{
			System.Console.WriteLine( "AppLovin Android Plugin not found" );
			PublicDefinitions.Add( "WITH_APPLOVIN=0" );
		}
	}
}
