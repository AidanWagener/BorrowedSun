using UnrealBuildTool;

public class BorrowedSun : ModuleRules
{
    public BorrowedSun(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "InputCore" });
        PrivateDependencyModuleNames.Add("PhysicsCore");
    }
}
