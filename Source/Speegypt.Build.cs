// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Speegypt : ModuleRules
{
	public Speegypt(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "GameplayTags", "Savior3" });
		
		MinFilesUsingPrecompiledHeaderOverride = 1;
	}
}
