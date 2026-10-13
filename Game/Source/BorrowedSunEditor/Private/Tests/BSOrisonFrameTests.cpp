#include "World/BSOrisonFrame.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSOrisonFrameTest, "BorrowedSun.Smoke.World.OrisonCoordinateAuthority",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSOrisonFrameTest::RunTest(const FString& Parameters)
{
    // Independently authored anchors catch metres/centimetres, sign and vertical-datum mistakes.
    TestTrue(TEXT("Canonical workshop retains the shipped physical-prop anchor"),
        FBSOrisonFrame::ToRuntime(FVector(-4780,-1370,92)).Equals(FVector(-2570,-470,8), .0001));
    TestTrue(TEXT("City centre sea level is east and north of the workshop, below the quay"),
        FBSOrisonFrame::ToRuntime(FVector::ZeroVector).Equals(FVector(475430,136530,-9192), .0001));
    TestTrue(TEXT("Southwest grid corner preserves east/north axes"),
        FBSOrisonFrame::ToRuntime(FVector(-13500,-12000,0)).Equals(FVector(-874570,-1063470,-9192), .0001));
    const FVector Rim(-11971,767,1447);
    TestTrue(TEXT("Far regional coordinates round-trip without float precision loss"),
        FBSOrisonFrame::ToCity(FBSOrisonFrame::ToRuntime(Rim)).Equals(Rim, .000001));
    return true;
}
#endif
