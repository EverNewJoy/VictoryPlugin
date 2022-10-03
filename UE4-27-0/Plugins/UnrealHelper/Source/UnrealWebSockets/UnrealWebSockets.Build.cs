// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

using UnrealBuildTool;

public class UnrealWebSockets : ModuleRules
{
	public UnrealWebSockets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"WebSockets",
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"HTTP",
				"Json",
				"JsonUtilities",
				"WebSockets",
			}
			);
	}
}
