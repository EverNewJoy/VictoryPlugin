// Some copyright should be here...

using UnrealBuildTool;

public class VictoryBPLibrary : ModuleRules
{
	public VictoryBPLibrary(ReadOnlyTargetRules Target) : base(Target)
	{ 
		//4.15 Include What You Use
		bEnforceIWYU = false;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"VictoryBPLibrary/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"VictoryBPLibrary/Private",
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject", 
				"Engine", 
				"InputCore",
				
				"RHI",
				"RenderCore",
				 
				"HTTP",
				"UMG", "Slate", "SlateCore",
				
				"ImageWrapper",
				
				"PhysX", 
				
				"HeadMountedDisplay",
  
                "AIModule"	
			}
			);
		
		//APEX EXCLUSIONS
		if (Target.Platform != UnrealTargetPlatform.Android && Target.Platform != UnrealTargetPlatform.HTML5 && Target.Platform != UnrealTargetPlatform.IOS)
		{
			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"APEX"
			}
			);
			
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"ApexDestruction"
			}
			);
		
		}
	
		 
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
