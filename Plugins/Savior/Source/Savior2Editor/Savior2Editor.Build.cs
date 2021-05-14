// Copyright 2018 (C) Bruno Xavier B. Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class Savior2Editor : ModuleRules {
    public Savior2Editor(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/Savior2Editor_Shared.h";
		bEnforceIWYU = true;
		//
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "Savior2",
                "CoreUObject"
            }///
        );//
        //
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Projects",
                "UnrealEd",
				"SlateCore",
                "AssetTools"
            }///
        );//
    }///
}///
//
}