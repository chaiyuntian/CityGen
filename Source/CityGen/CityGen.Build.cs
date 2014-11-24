// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CityGen : ModuleRules
{
    public CityGen(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] { 
				"Engine",
                "UnrealEd",
                "InputCore",
                "Core",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "CoreUObject",
                "LevelEditor",
			}
		);

		//Private Paths
        PrivateIncludePaths.AddRange(new string[] {
			"CityGen/Private"
		});
    }
}
