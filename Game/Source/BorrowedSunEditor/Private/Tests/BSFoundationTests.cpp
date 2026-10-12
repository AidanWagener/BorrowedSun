#include "Core/BSStableId.h"
#include "Travel/BSSectorMapping.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSStableIdentityTest, "BorrowedSun.Smoke.Identity.CanonicalAndDistinct",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSStableIdentityTest::RunTest(const FString& Parameters)
{
    FBSStableId First;
    First.Namespace = TEXT("orison.market");
    First.Key = TEXT("stall-001");
    FBSStableId Same = First;
    FBSStableId Other = First;
    Other.Key = TEXT("stall-002");
    TestTrue(TEXT("Authored key is valid"), First.IsValid());
    TestEqual(TEXT("Canonical serialization"), First.ToString(), FString(TEXT("orison.market:stall-001")));
    TestTrue(TEXT("Independent copies retain identity"), First == Same);
    TestFalse(TEXT("Different placements remain distinct"), First == Other);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSInvalidIdentityTest, "BorrowedSun.Smoke.Identity.RejectsAmbiguity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSInvalidIdentityTest::RunTest(const FString& Parameters)
{
    FBSStableId Id;
    Id.Namespace = TEXT("orison");
    for (const FString& Invalid : { FString(), FString(TEXT("Market")), FString(TEXT("a:b")), FString(TEXT("a/b")), FString(TEXT("two words")) })
    {
        Id.Key = Invalid;
        TestFalse(TEXT("Invalid key rejected"), Id.IsValid());
        TestTrue(TEXT("Invalid key has no serialized identity"), Id.ToString().IsEmpty());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSTransferConservationTest, "BorrowedSun.Smoke.Travel.RigidMappingConservesMotion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSTransferConservationTest::RunTest(const FString& Parameters)
{
    const FTransform Source(FQuat::Identity, FVector(100.0, 200.0, 300.0));
    const FTransform Destination(FQuat(FVector::UpVector, UE_PI / 2.0), FVector(10000.0, 0.0, 1000.0));
    FBSMotionSample Input;
    Input.Position = FVector(110.0, 200.0, 300.0);
    Input.LinearVelocity = FVector(200.0, 0.0, 0.0);
    Input.AngularVelocity = FVector(10.0, 0.0, 0.0);
    FBSMotionSample Output;
    TestTrue(TEXT("Rigid mapping succeeds"), BorrowedSun::TryMapSectorMotion(Source, Destination, Input, Output));
    TestTrue(TEXT("Relative position rotates into destination"), Output.Position.Equals(FVector(10000.0, 10.0, 1000.0), 0.001));
    TestTrue(TEXT("Linear velocity rotates"), Output.LinearVelocity.Equals(FVector(0.0, 200.0, 0.0), 0.001));
    TestTrue(TEXT("Angular velocity rotates"), Output.AngularVelocity.Equals(FVector(0.0, 10.0, 0.0), 0.001));
    TestTrue(TEXT("Speed conserved"), FMath::IsNearlyEqual(Input.LinearVelocity.Size(), Output.LinearVelocity.Size(), 0.001));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSTransferRoundTripTest, "BorrowedSun.Smoke.Travel.RoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSTransferRoundTripTest::RunTest(const FString& Parameters)
{
    const FTransform Source(FQuat(FVector::UpVector, 0.4), FVector(150.0, -900.0, 2.0));
    const FTransform Destination(FQuat(FVector::RightVector, -0.7), FVector(300000.0, 80.0, 90000.0));
    FBSMotionSample Original;
    Original.Position = FVector(187.0, -834.0, 11.0);
    Original.Orientation = FQuat(FVector::ForwardVector, 0.2);
    Original.LinearVelocity = FVector(-30.0, 44.0, 17.0);
    Original.AngularVelocity = FVector(0.1, -0.4, 0.9);
    FBSMotionSample Away;
    FBSMotionSample Back;
    TestTrue(TEXT("Outbound transfer"), BorrowedSun::TryMapSectorMotion(Source, Destination, Original, Away));
    TestTrue(TEXT("Inbound transfer"), BorrowedSun::TryMapSectorMotion(Destination, Source, Away, Back));
    TestTrue(TEXT("Position restored"), Original.Position.Equals(Back.Position, 0.001));
    TestTrue(TEXT("Orientation restored"), Original.Orientation.Equals(Back.Orientation, 0.0001));
    TestTrue(TEXT("Velocity restored"), Original.LinearVelocity.Equals(Back.LinearVelocity, 0.001));
    TestTrue(TEXT("Angular velocity restored"), Original.AngularVelocity.Equals(Back.AngularVelocity, 0.001));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSTransferScaleRejectionTest, "BorrowedSun.Smoke.Travel.RejectsScaleWithoutMutation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSTransferScaleRejectionTest::RunTest(const FString& Parameters)
{
    FTransform InvalidAnchor = FTransform::Identity;
    InvalidAnchor.SetScale3D(FVector(2.0));
    FBSMotionSample Output;
    Output.Position = FVector(42.0, 7.0, 9.0);
    const FVector Before = Output.Position;
    TestFalse(TEXT("Scaled anchor rejected"), BorrowedSun::TryMapSectorMotion(FTransform::Identity, InvalidAnchor, FBSMotionSample(), Output));
    TestTrue(TEXT("Failure preserves output"), Before.Equals(Output.Position));
    return true;
}

#endif
