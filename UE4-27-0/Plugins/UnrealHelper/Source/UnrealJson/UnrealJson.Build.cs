using System.IO;
using UnrealBuildTool;

public class UnrealJson : ModuleRules
{
	public UnrealJson(ReadOnlyTargetRules Target) : base(Target)
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
				"CoreUObject",
				"Engine",
				"HTTP",
				"Json",
				"JsonUtilities"
			}
			);

	}
}
