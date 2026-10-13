#include "Objects/BSObjectLedger.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"

namespace BSObjectLedgerTests
{
namespace
{
    FBSStableId Id(const TCHAR* Namespace, const FString& Key)
    {
        FBSStableId Value; Value.Namespace = Namespace; Value.Key = Key; return Value;
    }
    FBSStableId World() { return Id(TEXT("orison"), TEXT("object_lab")); }
    FBSStableId Collection() { return Id(TEXT("collection"), TEXT("test_bin")); }
    FBSObjectRecord Prop(const TCHAR* Name)
    {
        FBSObjectRecord Record; Record.InstanceId = Id(TEXT("object_lab"), Name);
        Record.DefinitionId = Id(TEXT("prop"), Name); Record.bCollectible = true;
        return Record;
    }
    struct FTemporarySlots
    {
        FBSObjectSaveSlots Slots{TEXT("QA_Objects_") + FGuid::NewGuid().ToString(EGuidFormats::Digits)};
        ~FTemporarySlots()
        {
            // Only these two unique QA slots are ours; never enumerate or delete player saves.
            for (int32 Slot = 0; Slot < 2; ++Slot) UGameplayStatics::DeleteGameInSlot(Slots.SlotName(Slot), 0);
        }
    };
    struct FMemoryStorage final : IBSObjectStorage
    {
        TMap<int32, FBSObjectSnapshot> Slots;
        bool bFailWrite = false;
        bool bFailReadBack = false;
        bool bReadFailurePending = false;
        int32 Writes = 0;
        virtual bool Read(int32 Slot, FBSObjectSnapshot& Out, bool& Exists, FString& Error) override
        {
            Error.Reset(); Exists = Slots.Contains(Slot);
            if (bReadFailurePending) { bReadFailurePending = false; Error = TEXT("Injected read failure"); return false; }
            if (Exists) Out = Slots[Slot];
            return true;
        }
        virtual bool Write(int32 Slot, const FBSObjectSnapshot& Snapshot, FString& Error) override
        {
            if (bFailWrite) { Error = TEXT("Injected write failure"); return false; }
            Slots.Add(Slot, Snapshot); ++Writes;
            bReadFailurePending = bFailReadBack;
            return true;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSObjectDiskTest, "BorrowedSun.Smoke.Objects.DiskCustodyAndReconstruction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSObjectDiskTest::RunTest(const FString& Parameters)
{
    FTemporarySlots Files; FBSObjectLedger Ledger(Files.Slots); FString Error;
    if (!TestTrue(TEXT("Open isolated real SaveGame slots"), Ledger.Open(World(), Error))) return false;
    const TCHAR* Names[] = {TEXT("cup"), TEXT("bowl"), TEXT("lantern"), TEXT("tool"), TEXT("cell"),
        TEXT("crate"), TEXT("basket"), TEXT("bottle"), TEXT("book"), TEXT("bundle")};
    for (const TCHAR* Name : Names)
        if (!TestTrue(TEXT("Authored identity persisted to disk"), Ledger.Register(Prop(Name), Error))) return false;
    TestEqual(TEXT("Ten distinct records, not ten copies of one identity"), Ledger.GetSnapshot().Objects.Num(), 10);
    const FBSStableId Cup = Prop(TEXT("cup")).InstanceId;
    const FTransform Moved(FRotator(0, 67, 0), FVector(153, -28, 91));
    const FGuid MoveId = FGuid::NewGuid();
    TestTrue(TEXT("World resting pose commits"), Ledger.Move(Cup, 0, MoveId, Moved, Error));
    const int64 MovedGeneration = Ledger.GetSnapshot().Generation;
    TestTrue(TEXT("Identical move retry returns durable result"), Ledger.Move(Cup, 0, MoveId, Moved, Error));
    TestEqual(TEXT("Retry adds no generation"), Ledger.GetSnapshot().Generation, MovedGeneration);
    TestFalse(TEXT("Same operation cannot change its pose"), Ledger.Move(Cup, 0, MoveId, FTransform::Identity, Error));
    const FBSObjectSnapshot BeforeRefusal = Ledger.GetSnapshot();
    const FGuid CollectId = FGuid::NewGuid();
    TestFalse(TEXT("Full recipient refuses collection"), Ledger.Collect(Cup, 1, CollectId, Collection(), 0, Error));
    TestTrue(TEXT("Refusal preserves all records and world representation"), BeforeRefusal.Equals(Ledger.GetSnapshot())
        && Ledger.GetSnapshot().Find(Cup)->ShouldSpawnInWorld());
    TestTrue(TEXT("Accepting recipient commits one location"), Ledger.Collect(Cup, 1, CollectId, Collection(), 1, Error));
    TestFalse(TEXT("Successful collection suppresses world reconstruction"), Ledger.GetSnapshot().Find(Cup)->ShouldSpawnInWorld());
    const int64 CollectedGeneration = Ledger.GetSnapshot().Generation;
    TestTrue(TEXT("Retried accepted operation is idempotent even when recipient is now full"), Ledger.Collect(Cup, 1, CollectId, Collection(), 0, Error));
    TestEqual(TEXT("Collection retry does not duplicate the item"), Ledger.GetSnapshot().Generation, CollectedGeneration);
    TestFalse(TEXT("Second object cannot exceed stub capacity"), Ledger.Collect(Prop(TEXT("bowl")).InstanceId, 0, FGuid::NewGuid(), Collection(), 1, Error));
    TestFalse(TEXT("A different transaction cannot recollect the same identity"), Ledger.Collect(Cup, 2, FGuid::NewGuid(), Collection(), 10, Error));

    FBSObjectLedger Reopened(Files.Slots);
    if (!TestTrue(TEXT("Fresh ledger loads the actual disk checkpoint"), Reopened.Open(World(), Error))) return false;
    TestTrue(TEXT("All IDs, transforms, custody and receipts survive disk reload"), Reopened.GetSnapshot().Equals(Ledger.GetSnapshot()));
    TestTrue(TEXT("Last collection receipt also survives reload"), Reopened.Collect(Cup, 1, CollectId, Collection(), 1, Error));
    for (const TCHAR* Name : Names) TestTrue(TEXT("Repeated assembly registration respects saved placement"), Reopened.Register(Prop(Name), Error));
    TestEqual(TEXT("Regenerating the scene does not add object records"), Reopened.GetSnapshot().Objects.Num(), 10);
    TestEqual(TEXT("Regenerating the scene makes no save changes"), Reopened.GetSnapshot().Generation, CollectedGeneration);
    TestFalse(TEXT("Collected prop does not respawn from its original recipe"), Reopened.GetSnapshot().Find(Cup)->ShouldSpawnInWorld());
    TestTrue(TEXT("Last world pose is retained for future explicit release"), Reopened.GetSnapshot().Find(Cup)->RestTransform.Equals(Moved, 0.0));
    FBSObjectRecord ChangedDefinition = Prop(TEXT("cup")); ++ChangedDefinition.DefinitionRevision;
    TestFalse(TEXT("Definition changes require migration instead of silently resetting custody"), Reopened.Register(ChangedDefinition, Error));
    FBSObjectRecord Mission = Prop(TEXT("protected_core")); Mission.bMissionCustody = true;
    TestFalse(TEXT("Mission-owned records cannot enter the generic ledger"), Reopened.Register(Mission, Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSObjectAtomicityTest, "BorrowedSun.Smoke.Objects.FailureAtomicityAndStaleWrites",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSObjectAtomicityTest::RunTest(const FString& Parameters)
{
    FMemoryStorage Storage; FBSObjectLedger A(Storage), B(Storage); FString Error;
    const FBSObjectRecord Cup = Prop(TEXT("cup"));
    TestTrue(TEXT("Open primary ledger"), A.Open(World(), Error));
    TestTrue(TEXT("Initial placement committed"), A.Register(Cup, Error));
    TestTrue(TEXT("Second session sees same initial generation"), B.Open(World(), Error));
    const FBSObjectSnapshot Before = A.GetSnapshot();
    Storage.bFailWrite = true;
    TestFalse(TEXT("Injected write failure refuses collection"), A.Collect(Cup.InstanceId, 0, FGuid::NewGuid(), Collection(), 1, Error));
    TestTrue(TEXT("Failed save retains every in-memory field and world custody"), A.GetSnapshot().Equals(Before));
    Storage.bFailWrite = false;
    const FGuid MoveId = FGuid::NewGuid();
    TestTrue(TEXT("A moves after storage recovery"), A.Move(Cup.InstanceId, 0, MoveId, FTransform(FVector(20, 10, 90)), Error));
    TestFalse(TEXT("Stale session cannot overwrite the newer saved pose with a collection"), B.Collect(Cup.InstanceId, 0, FGuid::NewGuid(), Collection(), 1, Error));
    TestTrue(TEXT("Stale session retains its prior state until explicit reload"), B.GetSnapshot().Equals(Before));
    TestTrue(TEXT("Stale session can reload current state"), B.Open(World(), Error));
    TestFalse(TEXT("Old expected revision cannot mutate reloaded object"), B.Move(Cup.InstanceId, 0, FGuid::NewGuid(), FTransform::Identity, Error));
    const FBSObjectSnapshot BeforeReadFailure = B.GetSnapshot();
    Storage.bFailReadBack = true;
    const FGuid CollectId = FGuid::NewGuid();
    TestFalse(TEXT("Ambiguous read-back does not report collection success"), B.Collect(Cup.InstanceId, 1, CollectId, Collection(), 1, Error));
    TestTrue(TEXT("Read-back failure does not prematurely change in-memory representation"), B.GetSnapshot().Equals(BeforeReadFailure));
    Storage.bFailReadBack = false;
    TestFalse(TEXT("Ambiguous disk outcome requires reload before another mutation"), B.Move(Cup.InstanceId, 1, FGuid::NewGuid(), FTransform::Identity, Error));
    TestTrue(TEXT("Reload reconciles the successfully written collection"), B.Open(World(), Error));
    TestFalse(TEXT("Reconciled object has one collection owner and no world spawn"), B.GetSnapshot().Find(Cup.InstanceId)->ShouldSpawnInWorld());
    TestTrue(TEXT("Retry uses the persisted transaction receipt"), B.Collect(Cup.InstanceId, 1, CollectId, Collection(), 1, Error));
    TestEqual(TEXT("Only placement, movement and collection were written"), Storage.Writes, 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSObjectRecoveryTest, "BorrowedSun.Smoke.Objects.RecoveryAndValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSObjectRecoveryTest::RunTest(const FString& Parameters)
{
    FTemporarySlots Files; FString Error; FBSObjectLedger Ledger(Files.Slots);
    TestTrue(TEXT("Open isolated disk slots"), Ledger.Open(World(), Error));
    const FBSObjectRecord Cup = Prop(TEXT("cup"));
    if (!TestTrue(TEXT("First generation is durable"), Ledger.Register(Cup, Error))) return false;
    const FBSObjectSnapshot GenerationOne = Ledger.GetSnapshot();
    TestTrue(TEXT("Second generation stored in other slot"), Ledger.Move(Cup.InstanceId, 0, FGuid::NewGuid(), FTransform(FVector(10, 20, 90)), Error));
    UBSObjectSave* Invalid = NewObject<UBSObjectSave>(); Invalid->Snapshot = Ledger.GetSnapshot();
    const FBSObjectRecord Duplicate = Invalid->Snapshot.Objects[0];
    Invalid->Snapshot.Objects.Add(Duplicate);
    Invalid->Snapshot.Generation = 4; // Keep event totals plausible so duplicate identity is the rejected invariant.
    TestTrue(TEXT("Inject duplicate-identity corruption into latest disk slot"), UGameplayStatics::SaveGameToSlot(Invalid, Files.Slots.SlotName(1), 0));
    FBSObjectLedger Recovered(Files.Slots);
    TestTrue(TEXT("An intact previous generation remains recoverable"), Recovered.Open(World(), Error));
    TestTrue(TEXT("Recovery selects precisely the previous complete snapshot"), Recovered.GetSnapshot().Equals(GenerationOne));
    TestFalse(TEXT("Recovery is reported, not silently treated as latest state"), Recovered.GetRecoveryWarning().IsEmpty());
    TestTrue(TEXT("Inject corruption into remaining slot"), UGameplayStatics::SaveGameToSlot(Invalid, Files.Slots.SlotName(0), 0));
    FBSObjectLedger Broken(Files.Slots);
    TestFalse(TEXT("Two invalid slots do not start an empty world"), Broken.Open(World(), Error));
    TestFalse(TEXT("Unopened ledger cannot replace damaged saves"), Broken.Register(Cup, Error));

    FMemoryStorage Memory; Memory.Slots.Add(0, GenerationOne);
    FBSObjectSnapshot Future = GenerationOne; Future.SchemaVersion = 2; Memory.Slots.Add(1, Future);
    FBSObjectLedger FutureReader(Memory);
    TestFalse(TEXT("Future schema is preserved rather than overwritten with older data"), FutureReader.Open(World(), Error));
    Future = GenerationOne; Future.Objects[0].RestTransform.SetLocation(FVector(100, 0, 0)); Memory.Slots[1] = Future;
    TestFalse(TEXT("Different payloads at same generation fail closed"), FutureReader.Open(World(), Error));
    Future = GenerationOne; Future.WorldId = Id(TEXT("orison"), TEXT("other_world")); Memory.Slots[1] = Future;
    TestFalse(TEXT("A namespace cannot silently change its world"), FutureReader.Open(World(), Error));
    FBSObjectSaveSlots Unsafe(TEXT("../player")); FBSObjectLedger UnsafeLedger(Unsafe);
    TestTrue(TEXT("Unsafe namespace has no resolved slot name"), Unsafe.SlotName(0).IsEmpty());
    TestFalse(TEXT("Unsafe namespace rejected before disk access"), UnsafeLedger.Open(World(), Error));

    FBSObjectSnapshot Bad = GenerationOne; Bad.Objects[0].RestTransform.SetScale3D(FVector::ZeroVector);
    TestFalse(TEXT("Zero-scale saved object rejected"), Bad.IsValid(Error));
    Bad = GenerationOne; Bad.Objects[0].Custody = static_cast<EBSObjectCustody>(255);
    TestFalse(TEXT("Unknown custody enum rejected"), Bad.IsValid(Error));
    Bad = GenerationOne; Bad.Objects[0].CollectionId = Collection();
    TestFalse(TEXT("One record cannot occupy world and collection simultaneously"), Bad.IsValid(Error));
    Bad = GenerationOne; Bad.Objects[0].Revision = MAX_int64;
    TestFalse(TEXT("Exhausted revision rejected without integer overflow"), Bad.IsValid(Error));
    return true;
}
} // namespace BSObjectLedgerTests
#endif
