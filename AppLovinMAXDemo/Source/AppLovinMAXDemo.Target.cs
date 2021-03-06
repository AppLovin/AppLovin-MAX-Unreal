// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AppLovinMAXDemoTarget : TargetRules
{
	public AppLovinMAXDemoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "AppLovinMAXDemo" } );

		// Add compiler argument to support building on macOS with Xcode 13.3+
		// bOverrideBuildEnvironment = true;
		// AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
	}
}
