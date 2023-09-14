// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AppLovinMAXDemoEditorTarget : TargetRules
{
	public AppLovinMAXDemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "AppLovinMAXDemo" } );

		// Add compiler arguments to support building on macOS with Xcode 13.3+
		if ( Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac )
		{
			bOverrideBuildEnvironment = true;
			AdditionalCompilerArguments = "-Wno-unused-but-set-variable -Wno-bitwise-instead-of-logical";
		}
	}
}
