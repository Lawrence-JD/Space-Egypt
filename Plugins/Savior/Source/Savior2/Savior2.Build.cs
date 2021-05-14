// Copyright 2018 (C) Bruno Xavier B. Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class Savior2 : ModuleRules {
    public Savior2(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/Savior2_Shared.h";
		bEnforceIWYU = true;
		//
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"UMG",
				"Core",
				"Json",
				"Slate",
				"Engine",
				"SlateCore",
				"CoreUObject",
				"MoviePlayer",
				"GameplayTags",
				"JsonUtilities"
			}///
		);//
		//
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
	}///
}///
//
}