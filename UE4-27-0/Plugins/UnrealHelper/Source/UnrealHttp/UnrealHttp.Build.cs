// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

using UnrealBuildTool;

public class UnrealHttp : ModuleRules
{
	public UnrealHttp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"CoreUObject",
				"Engine",
				"HTTP"
			}
			);
	}
}
