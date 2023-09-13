// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AppLovinMAXDemoEditorTarget : TargetRules
{
	public AppLovinMAXDemoEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "AppLovinMAXDemo" } );

		if ( Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac )
		{
			bOverrideBuildEnvironment = true;
			AdditionalCompilerArguments = "-Wno-unknown-warning-option -Wno-unused-but-set-variable -Wno-deprecated-builtins -Wno-bitwise-instead-of-logical -Wno-single-bit-bitfield-constant-conversion";
		}		
	}
}
