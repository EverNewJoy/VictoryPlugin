using UnrealBuildTool;

public class UnrealMediaOutputSocket : ModuleRules
{
	public UnrealMediaOutputSocket(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"MediaIOCore",
				"RenderCore",
				"RHI",
				"Networking",
				"Sockets"
			}
			);
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
			);
	}
}
