using UnrealBuildTool;

public class BorrowedSunEditor : ModuleRules
{
    public BorrowedSunEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "BorrowedSun" });
    }
}
