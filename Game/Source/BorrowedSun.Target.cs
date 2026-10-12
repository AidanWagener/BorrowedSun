using UnrealBuildTool;

public class BorrowedSunTarget : TargetRules
{
    public BorrowedSunTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("BorrowedSun");
    }
}
