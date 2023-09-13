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

		if ( Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac )
		{
			// Add compiler argument to support building on macOS with Xcode 13.3+
			bOverrideBuildEnvironment = true;
			AdditionalCompilerArguments = "-Wno-unused-but-set-variable -Wno-deprecated-builtins -Wno-bitwise-instead-of-logical -Wno-single-bit-bitfield-constant-conversion";
		}		
	}
}
