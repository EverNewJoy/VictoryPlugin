// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VictoryEdEngine : ModuleRules
{
    public VictoryEdEngine(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"UnrealEd",
				"Slate",
                "SlateCore",
                "ImageWrapper"
			}
		);
		//Private Paths
        PrivateIncludePaths.AddRange(new string[] { 
			"VictoryEdEngine/Public",
			"VictoryEdEngine/Private"
		});
    }
}
