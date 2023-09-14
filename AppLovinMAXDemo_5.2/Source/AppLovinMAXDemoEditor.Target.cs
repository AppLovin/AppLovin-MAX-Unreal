// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AppLovinMAXDemoEditorTarget : TargetRules
{
	public AppLovinMAXDemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
	
#if UE_5_3_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
#elif UE_5_2_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
#else
		DefaultBuildSettings = BuildSettingsVersion.V2;
#endif

		ExtraModuleNames.AddRange( new string[] { "AppLovinMAXDemo" } );

		// Add compiler arguments to support building on macOS with Xcode 13.3+
		// if ( Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac )
		// {
			// bOverrideBuildEnvironment = true;
			// AdditionalCompilerArguments = "-Wno-unknown-warning-option -Wno-unused-but-set-variable -Wno-deprecated-builtins -Wno-bitwise-instead-of-logical -Wno-single-bit-bitfield-constant-conversion";
		// }		
	}
}
