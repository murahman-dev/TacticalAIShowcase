using UnrealBuildTool;

public class TacticalAIShowcase : ModuleRules
{
	public TacticalAIShowcase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "GameplayTasks", "NavigationSystem", "GameplayTags" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });
	}
}
