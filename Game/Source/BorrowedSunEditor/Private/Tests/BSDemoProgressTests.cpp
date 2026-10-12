#include "Demo/BSDemoProgress.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    bool SameProgress(const FBSDemoProgress& A, const FBSDemoProgress& B)
    {
        return A.SchemaVersion == B.SchemaVersion && A.Stage == B.Stage && A.Choice == B.Choice
            && A.Revision == B.Revision && A.bSunseedOwned == B.bSunseedOwned;
    }

    FBSDemoProgress AtRelay()
    {
        FBSDemoProgress Result;
        FString Error;
        Result.TryAcquire(Error);
        Result.TryDepart(Error);
        Result.TryReachRelay(Error);
        return Result;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoOrderingTest, "BorrowedSun.Smoke.Demo.OrderingAndCustody",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSDemoOrderingTest::RunTest(const FString& Parameters)
{
    FBSDemoProgress Progress;
    FString Error;
    TestTrue(TEXT("Fresh journey is valid"), Progress.IsValid(Error));
    TestFalse(TEXT("Cannot depart without the Sunseed"), Progress.TryDepart(Error));
    TestEqual(TEXT("Rejected departure does not advance revision"), Progress.Revision, 0);
    TestTrue(TEXT("Acquisition succeeds once"), Progress.TryAcquire(Error));
    TestTrue(TEXT("Acquisition grants custody"), Progress.bSunseedOwned);
    TestFalse(TEXT("Duplicate acquisition rejected"), Progress.TryAcquire(Error));
    TestEqual(TEXT("Duplicate acquisition does not duplicate a state change"), Progress.Revision, 1);
    TestFalse(TEXT("Cannot skip flight to the relay"), Progress.TryReachRelay(Error));
    TestTrue(TEXT("Departure accepted"), Progress.TryDepart(Error));
    TestTrue(TEXT("Arrival accepted"), Progress.TryReachRelay(Error));
    TestFalse(TEXT("Cannot complete before authorizing a lease"), Progress.TryComplete(Error));
    TestTrue(TEXT("Clinic lease accepted"), Progress.TryChoose(EBSDemoChoice::Clinic, Error));
    TestTrue(TEXT("Lease retains physical custody during return"), Progress.bSunseedOwned);
    TestTrue(TEXT("Installing the Sunseed completes the mission"), Progress.TryComplete(Error));
    TestFalse(TEXT("Installation relinquishes custody"), Progress.bSunseedOwned);
    TestEqual(TEXT("Exactly five events were accepted"), Progress.Revision, 5);
    TestFalse(TEXT("Duplicate completion rejected"), Progress.TryComplete(Error));
    TestFalse(TEXT("Finished journey cannot reacquire cargo"), Progress.TryAcquire(Error));
    TestEqual(TEXT("Duplicate completion cannot grant another outcome"), Progress.Revision, 5);
    TestTrue(TEXT("Completed journey remains valid"), Progress.IsValid(Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoExclusiveChoiceTest, "BorrowedSun.Smoke.Demo.ExclusiveLeaseChoice",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSDemoExclusiveChoiceTest::RunTest(const FString& Parameters)
{
    for (const EBSDemoChoice Selected : { EBSDemoChoice::Clinic, EBSDemoChoice::Evacuation })
    {
        FBSDemoProgress Progress = AtRelay();
        FString Error;
        TestFalse(TEXT("None is not an authorization"), Progress.TryChoose(EBSDemoChoice::None, Error));
        TestEqual(TEXT("Invalid choice preserves relay revision"), Progress.Revision, 3);
        TestTrue(TEXT("Either real lease is available before choice"), Progress.TryChoose(Selected, Error));
        const FBSDemoProgress Committed = Progress;
        const EBSDemoChoice Other = Selected == EBSDemoChoice::Clinic ? EBSDemoChoice::Evacuation : EBSDemoChoice::Clinic;
        TestFalse(TEXT("The alternate service cannot also receive a lease"), Progress.TryChoose(Other, Error));
        TestFalse(TEXT("Repeated same choice is also rejected"), Progress.TryChoose(Selected, Error));
        TestTrue(TEXT("Both rejected choices preserve all progress fields"), SameProgress(Progress, Committed));
        TestTrue(TEXT("Either selected lease can complete"), Progress.TryComplete(Error));
        TestEqual(TEXT("Completion retains the single authorized service"), Progress.Choice, Selected);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoInvalidProgressTest, "BorrowedSun.Smoke.Demo.RejectsInvalidProgress",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSDemoInvalidProgressTest::RunTest(const FString& Parameters)
{
    TArray<FBSDemoProgress> InvalidStates;
    FBSDemoProgress Bad;
    Bad.SchemaVersion = 2; InvalidStates.Add(Bad);
    Bad = {}; Bad.SchemaVersion = 0; InvalidStates.Add(Bad);
    Bad = {}; Bad.Stage = static_cast<EBSDemoStage>(255); InvalidStates.Add(Bad);
    Bad = {}; Bad.Choice = static_cast<EBSDemoChoice>(255); InvalidStates.Add(Bad);
    Bad = {}; Bad.Revision = -1; InvalidStates.Add(Bad);
    Bad = {}; Bad.Revision = MAX_int32; InvalidStates.Add(Bad);
    Bad = {}; Bad.bSunseedOwned = true; InvalidStates.Add(Bad);
    Bad = AtRelay(); Bad.bSunseedOwned = false; InvalidStates.Add(Bad);
    Bad = AtRelay(); Bad.Choice = EBSDemoChoice::Clinic; InvalidStates.Add(Bad);
    Bad = {}; Bad.Stage = EBSDemoStage::Returning; Bad.Revision = 4; Bad.bSunseedOwned = true; InvalidStates.Add(Bad);
    Bad = {}; Bad.Stage = EBSDemoStage::Complete; Bad.Revision = 5; Bad.Choice = EBSDemoChoice::Clinic;
    Bad.bSunseedOwned = true; InvalidStates.Add(Bad);

    for (FBSDemoProgress Invalid : InvalidStates)
    {
        FString Error;
        const FBSDemoProgress Before = Invalid;
        TestFalse(TEXT("Invalid/version-unsupported state rejected"), Invalid.IsValid(Error));
        TestFalse(TEXT("Validation explains rejection"), Error.IsEmpty());
        TestFalse(TEXT("Invalid state cannot acquire"), Invalid.TryAcquire(Error));
        TestFalse(TEXT("Invalid state cannot depart"), Invalid.TryDepart(Error));
        TestFalse(TEXT("Invalid state cannot arrive"), Invalid.TryReachRelay(Error));
        TestFalse(TEXT("Invalid state cannot choose"), Invalid.TryChoose(EBSDemoChoice::Clinic, Error));
        TestFalse(TEXT("Invalid state cannot complete"), Invalid.TryComplete(Error));
        TestTrue(TEXT("Rejection preserves invalid input for diagnosis"), SameProgress(Before, Invalid));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoRejectedEventTest, "BorrowedSun.Smoke.Demo.RejectedEventsPreserveState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSDemoRejectedEventTest::RunTest(const FString& Parameters)
{
    FString Error;
    TArray<FBSDemoProgress> States;
    FBSDemoProgress Progress;
    States.Add(Progress);
    Progress.TryAcquire(Error); States.Add(Progress);
    Progress.TryDepart(Error); States.Add(Progress);
    Progress.TryReachRelay(Error); States.Add(Progress);
    Progress.TryChoose(EBSDemoChoice::Evacuation, Error); States.Add(Progress);
    Progress.TryComplete(Error); States.Add(Progress);

    for (const FBSDemoProgress& Before : States)
    {
        using FEvent = bool (FBSDemoProgress::*)(FString&);
        for (const FEvent Event : { &FBSDemoProgress::TryAcquire, &FBSDemoProgress::TryDepart,
            &FBSDemoProgress::TryReachRelay, &FBSDemoProgress::TryComplete })
        {
            FBSDemoProgress Candidate = Before;
            Error = TEXT("stale diagnostic");
            if ((Candidate.*Event)(Error))
            {
                TestEqual(TEXT("Accepted event advances exactly one revision"), Candidate.Revision, Before.Revision + 1);
                TestTrue(TEXT("Accepted event clears stale error"), Error.IsEmpty());
                TestTrue(TEXT("Accepted event preserves domain invariants"), Candidate.IsValid(Error));
            }
            else
            {
                TestTrue(TEXT("Out-of-order event leaves every field unchanged"), SameProgress(Candidate, Before));
                TestFalse(TEXT("Rejected event explains why"), Error.IsEmpty());
            }
        }
        FBSDemoProgress Candidate = Before;
        TestFalse(TEXT("Unknown choice always rejected"), Candidate.TryChoose(static_cast<EBSDemoChoice>(255), Error));
        TestTrue(TEXT("Unknown choice preserves every field"), SameProgress(Candidate, Before));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoSerializationTest, "BorrowedSun.Smoke.Demo.SerializationRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSDemoSerializationTest::RunTest(const FString& Parameters)
{
    // Memory serialization exercises Unreal's actual SaveGame format without touching player slots.
    for (const EBSDemoChoice Selected : { EBSDemoChoice::Clinic, EBSDemoChoice::Evacuation })
    {
        UBSDemoSave* Save = NewObject<UBSDemoSave>();
        Save->Progress = AtRelay();
        FString Error;
        TestTrue(TEXT("Lease chosen for serialized journey"), Save->Progress.TryChoose(Selected, Error));
        for (int32 Phase = 0; Phase < 2; ++Phase)
        {
            TArray<uint8> Bytes;
            if (!TestTrue(TEXT("Unreal serializes checkpoint to memory"), UGameplayStatics::SaveGameToMemory(Save, Bytes))) return false;
            UBSDemoSave* Loaded = Cast<UBSDemoSave>(UGameplayStatics::LoadGameFromMemory(Bytes));
            if (!TestNotNull(TEXT("Unreal restores the correct checkpoint class"), Loaded)) return false;
            TestTrue(TEXT("Every progression field survives serialization"), SameProgress(Save->Progress, Loaded->Progress));
            TestTrue(TEXT("Restored checkpoint passes domain validation"), Loaded->Progress.IsValid(Error));
            if (Phase == 0)
            {
                TestTrue(TEXT("Restored returning journey can finish once"), Loaded->Progress.TryComplete(Error));
                Save->Progress = Loaded->Progress;
            }
            else
            {
                TestFalse(TEXT("Restored completed journey cannot complete again"), Loaded->Progress.TryComplete(Error));
                TestFalse(TEXT("Restored installation has no carried Sunseed"), Loaded->Progress.bSunseedOwned);
            }
        }
    }
    return true;
}

#endif
