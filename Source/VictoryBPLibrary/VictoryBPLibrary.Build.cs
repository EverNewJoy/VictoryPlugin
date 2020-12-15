// Some copyright should be here...

using UnrealBuildTool;

public class VictoryBPLibrary : ModuleRules
{
	public VictoryBPLibrary(ReadOnlyTargetRules Target) : base(Target)
	{ 
		//Get rid of debug commandline length compile error
		//https://developercommunity.visualstudio.com/content/problem/668411/command-line-error-d8049-still-happening.html
		bLegacyPublicIncludePaths = false;
		
		PrivatePCHHeaderFile = "Private/VictoryBPLibraryPrivatePCH.h";
		
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
				"Core",
                "HTTP"
				
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
				"UMG", "Slate", "SlateCore",
				
				"ImageWrapper",

                "PhysicsCore", 
				"PhysX", 
				
				"HeadMountedDisplay",
  
                "AIModule",
				
				"NavigationSystem",
				
				"Vorbis",
				
				//FPlatformApplicationMisc
				"ApplicationCore"
			}
			);
		
		//APEX EXCLUSIONS
		if (Target.Platform != UnrealTargetPlatform.Android && Target.Platform != UnrealTargetPlatform.IOS)
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
