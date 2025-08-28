using UnrealBuildTool;

public class VSGenericWidgetPresets : ModuleRules
{
    public VSGenericWidgetPresets(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputCore",
                "UMG",
                "CommonUI",
                
                "VSPluginsCore",
                "VSWidgets",
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