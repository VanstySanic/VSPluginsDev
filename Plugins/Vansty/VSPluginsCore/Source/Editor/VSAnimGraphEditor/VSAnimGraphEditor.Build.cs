using UnrealBuildTool;

public class VSAnimGraphEditor : ModuleRules
{
    public VSAnimGraphEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayTags",
                "AnimGraph",
                "AnimationBlueprintLibrary",
                "AnimGraphRuntime",
                "UnrealEd",
                "AnimGraph",
                "BlueprintGraph",
                "ToolMenus",
                "KismetCompiler",
                
                "VSAnimGraph",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}