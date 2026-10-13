#include "Objects/BSObjectLedger.h"

#include "Kismet/GameplayStatics.h"

namespace
{
    constexpr int32 MaxObjectRecords = 4096;

    bool ObjectLedgerFail(FString& Error, const TCHAR* Message) { Error = Message; return false; }
    bool EmptyObjectOwner(const FBSStableId& Id) { return Id.Namespace.IsEmpty() && Id.Key.IsEmpty(); }
    bool ValidObjectRestTransform(const FTransform& Transform)
    {
        const FVector Scale = Transform.GetScale3D();
        return !Transform.ContainsNaN() && Transform.GetRotation().IsNormalized()
            && Scale.GetMin() > 0.0 && Scale.GetMax() <= 10.0;
    }
    bool ValidObjectSaveNamespace(const FString& Value)
    {
        if (Value.IsEmpty() || Value.Len() > 48) return false;
        for (const TCHAR C : Value)
            if (!((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') || (C >= '0' && C <= '9') || C == '_' || C == '-')) return false;
        return true;
    }
}

bool FBSObjectRecord::IsValid(FString& Error) const
{
    Error.Reset();
    if (!InstanceId.IsValid() || !DefinitionId.IsValid() || DefinitionRevision < 1)
        return ObjectLedgerFail(Error, TEXT("Object identity or definition revision is invalid."));
    if (bMissionCustody) return ObjectLedgerFail(Error, TEXT("Mission custody must stay with the mission authority."));
    if (!ValidObjectRestTransform(RestTransform)) return ObjectLedgerFail(Error, TEXT("Object resting transform is invalid."));
    if (Revision < 0 || Revision == MAX_int64) return ObjectLedgerFail(Error, TEXT("Object revision is invalid or exhausted."));
    if (Custody == EBSObjectCustody::World)
    {
        if (!EmptyObjectOwner(CollectionId)) return ObjectLedgerFail(Error, TEXT("World object cannot also belong to a collection."));
    }
    else if (Custody == EBSObjectCustody::Collection)
    {
        if (!bCollectible || !CollectionId.IsValid()) return ObjectLedgerFail(Error, TEXT("Collected object lacks eligibility or a valid owner."));
    }
    else return ObjectLedgerFail(Error, TEXT("Unknown object custody."));
    if (Revision == 0)
    {
        if (LastOperationId.IsValid() || LastOperation != EBSObjectOperation::None || Custody != EBSObjectCustody::World)
            return ObjectLedgerFail(Error, TEXT("Fresh placement has an inconsistent transaction receipt."));
    }
    else if (!LastOperationId.IsValid()
        || (LastOperation != EBSObjectOperation::Move && LastOperation != EBSObjectOperation::Collect)
        || ((LastOperation == EBSObjectOperation::Collect) != (Custody == EBSObjectCustody::Collection)))
        return ObjectLedgerFail(Error, TEXT("Object transaction receipt contradicts custody."));
    return true;
}

bool FBSObjectRecord::Equals(const FBSObjectRecord& B) const
{
    return InstanceId == B.InstanceId && DefinitionId == B.DefinitionId && DefinitionRevision == B.DefinitionRevision
        && RestTransform.Equals(B.RestTransform, 0.0) && Custody == B.Custody && CollectionId == B.CollectionId
        && bCollectible == B.bCollectible && bMissionCustody == B.bMissionCustody && Revision == B.Revision
        && LastOperationId == B.LastOperationId && LastOperation == B.LastOperation;
}

bool FBSObjectSnapshot::IsValid(FString& Error) const
{
    Error.Reset();
    if (SchemaVersion != 1 || !WorldId.IsValid() || Generation < 0 || Generation == MAX_int64
        || Objects.Num() > MaxObjectRecords || Generation < Objects.Num())
        return ObjectLedgerFail(Error, TEXT("Unsupported or invalid object checkpoint header."));
    TSet<FString> Ids;
    int64 AccountedGeneration = Objects.Num();
    for (const FBSObjectRecord& Object : Objects)
    {
        if (!Object.IsValid(Error)) return false;
        const FString Id = Object.InstanceId.ToString();
        if (Ids.Contains(Id) || Object.Revision >= Generation)
            return ObjectLedgerFail(Error, TEXT("Duplicate identity or impossible object revision."));
        Ids.Add(Id);
        if (Object.Revision > Generation - AccountedGeneration)
            return ObjectLedgerFail(Error, TEXT("Object revisions exceed the checkpoint generation."));
        AccountedGeneration += Object.Revision;
    }
    if (AccountedGeneration != Generation)
        return ObjectLedgerFail(Error, TEXT("Checkpoint generation does not match its placement and transaction history."));
    return true;
}

bool FBSObjectSnapshot::Equals(const FBSObjectSnapshot& B) const
{
    if (SchemaVersion != B.SchemaVersion || !(WorldId == B.WorldId) || Generation != B.Generation || Objects.Num() != B.Objects.Num()) return false;
    for (int32 I = 0; I < Objects.Num(); ++I) if (!Objects[I].Equals(B.Objects[I])) return false;
    return true;
}

const FBSObjectRecord* FBSObjectSnapshot::Find(const FBSStableId& Id) const
{
    return Objects.FindByPredicate([&](const FBSObjectRecord& Object) { return Object.InstanceId == Id; });
}

FBSObjectSaveSlots::FBSObjectSaveSlots(FString InNamespace) : Namespace(MoveTemp(InNamespace)) {}

FString FBSObjectSaveSlots::SlotName(int32 Slot) const
{
    return ValidObjectSaveNamespace(Namespace) && (Slot == 0 || Slot == 1)
        ? FString::Printf(TEXT("BorrowedSun_Objects_%s_%d"), *Namespace, Slot) : FString();
}

bool FBSObjectSaveSlots::Read(int32 Slot, FBSObjectSnapshot& Out, bool& bExists, FString& Error)
{
    Error.Reset(); bExists = false;
    const FString Name = SlotName(Slot);
    if (Name.IsEmpty()) return ObjectLedgerFail(Error, TEXT("Invalid object checkpoint namespace or slot."));
    bExists = UGameplayStatics::DoesSaveGameExist(Name, 0);
    if (!bExists) return true;
    const UBSObjectSave* Save = Cast<UBSObjectSave>(UGameplayStatics::LoadGameFromSlot(Name, 0));
    if (!Save) return ObjectLedgerFail(Error, TEXT("Object checkpoint is unreadable or has the wrong save class."));
    Out = Save->Snapshot;
    return true;
}

bool FBSObjectSaveSlots::Write(int32 Slot, const FBSObjectSnapshot& Snapshot, FString& Error)
{
    const FString Name = SlotName(Slot);
    if (Name.IsEmpty()) return ObjectLedgerFail(Error, TEXT("Invalid object checkpoint namespace or slot."));
    if (!Snapshot.IsValid(Error)) return false;
    UBSObjectSave* Save = NewObject<UBSObjectSave>(); Save->Snapshot = Snapshot;
    if (!UGameplayStatics::SaveGameToSlot(Save, Name, 0)) return ObjectLedgerFail(Error, TEXT("Could not write object checkpoint."));
    return true;
}

bool FBSObjectLedger::ReadLatest(const FBSStableId& WorldId, FBSObjectSnapshot& Out, int32& OutSlot, FString& Warning, FString& Error)
{
    Warning.Reset(); Error.Reset(); OutSlot = INDEX_NONE;
    bool AnyExisting = false, AnyInvalid = false;
    for (int32 Slot = 0; Slot < 2; ++Slot)
    {
        FBSObjectSnapshot Candidate; bool Exists = false; FString SlotError;
        const bool Read = Storage.Read(Slot, Candidate, Exists, SlotError);
        AnyExisting |= Exists;
        if (!Read && !Exists) return ObjectLedgerFail(Error, TEXT("Object checkpoint storage could not be accessed."));
        if (!Exists) continue;
        // Do not downgrade a future schema or overwrite another world's namespace.
        if (Read && (Candidate.SchemaVersion != 1 || !(Candidate.WorldId == WorldId)))
            return ObjectLedgerFail(Error, TEXT("Object checkpoint belongs to another world or an unsupported schema."));
        if (!Read || !Candidate.IsValid(SlotError)) { AnyInvalid = true; continue; }
        if (OutSlot != INDEX_NONE && Candidate.Generation == Out.Generation && !Candidate.Equals(Out))
            return ObjectLedgerFail(Error, TEXT("Object checkpoint slots disagree at the same generation."));
        if (OutSlot == INDEX_NONE || Candidate.Generation > Out.Generation) { Out = MoveTemp(Candidate); OutSlot = Slot; }
    }
    if (AnyExisting && OutSlot == INDEX_NONE) return ObjectLedgerFail(Error, TEXT("No valid object checkpoint; existing files retained."));
    if (OutSlot == INDEX_NONE) { Out = {}; Out.WorldId = WorldId; }
    if (AnyInvalid) Warning = TEXT("Recovered an intact object checkpoint; the other generation was unreadable or invalid.");
    return true;
}

bool FBSObjectLedger::Open(const FBSStableId& WorldId, FString& Error)
{
    if (!WorldId.IsValid()) return ObjectLedgerFail(Error, TEXT("A canonical world identity is required."));
    FBSObjectSnapshot Loaded; int32 Slot; FString Warning;
    if (!ReadLatest(WorldId, Loaded, Slot, Warning, Error)) return false;
    Current = MoveTemp(Loaded); CurrentSlot = Slot; RecoveryWarning = MoveTemp(Warning); bOpen = true;
    return true;
}

bool FBSObjectLedger::RequireCurrent(FString& Error)
{
    if (!bOpen) return ObjectLedgerFail(Error, TEXT("Open the object ledger before mutation."));
    FBSObjectSnapshot Disk; int32 Slot; FString Warning;
    if (!ReadLatest(Current.WorldId, Disk, Slot, Warning, Error)) return false;
    if (!Disk.Equals(Current)) return ObjectLedgerFail(Error, TEXT("Object checkpoint changed; reload before applying this action."));
    CurrentSlot = Slot;
    return true;
}

bool FBSObjectLedger::Commit(FBSObjectSnapshot Candidate, FString& Error)
{
    if (Current.Generation >= MAX_int64 - 1) return ObjectLedgerFail(Error, TEXT("Object checkpoint generation is exhausted."));
    Candidate.Generation = Current.Generation + 1;
    if (!Candidate.IsValid(Error)) return false;
    const int32 Slot = CurrentSlot == 0 ? 1 : 0;
    if (!Storage.Write(Slot, Candidate, Error)) return false;
    FBSObjectSnapshot Verified; bool Exists = false;
    if (!Storage.Read(Slot, Verified, Exists, Error) || !Exists || !Candidate.Equals(Verified))
        return ObjectLedgerFail(Error, TEXT("Object checkpoint read-back failed; reload to reconcile disk before further actions."));
    Current = MoveTemp(Candidate); CurrentSlot = Slot; Error.Reset();
    return true;
}

bool FBSObjectLedger::Register(const FBSObjectRecord& Authored, FString& Error)
{
    if (!RequireCurrent(Error) || !Authored.IsValid(Error)) return false;
    if (Authored.Revision != 0 || Authored.Custody != EBSObjectCustody::World)
        return ObjectLedgerFail(Error, TEXT("Registration requires a fresh authored placement."));
    if (const FBSObjectRecord* Existing = Current.Find(Authored.InstanceId))
    {
        if (!(Existing->DefinitionId == Authored.DefinitionId) || Existing->DefinitionRevision != Authored.DefinitionRevision
            || Existing->bCollectible != Authored.bCollectible)
            return ObjectLedgerFail(Error, TEXT("Placed identity changed definition; an explicit migration is required."));
        return true; // Saved movement and collection win over repeated construction.
    }
    FBSObjectSnapshot Candidate = Current; Candidate.Objects.Add(Authored);
    return Commit(MoveTemp(Candidate), Error);
}

bool FBSObjectLedger::Move(const FBSStableId& Id, int64 ExpectedRevision, const FGuid& OperationId,
    const FTransform& RestTransform, FString& Error)
{
    if (!RequireCurrent(Error)) return false;
    const FBSObjectRecord* Existing = Current.Find(Id);
    if (!Existing || !OperationId.IsValid() || !ValidObjectRestTransform(RestTransform))
        return ObjectLedgerFail(Error, TEXT("Move requires a known object, operation ID and valid resting pose."));
    if (Existing->LastOperationId == OperationId)
        return Existing->LastOperation == EBSObjectOperation::Move && ExpectedRevision == Existing->Revision - 1
            && Existing->RestTransform.Equals(RestTransform, 0.0) ? true : ObjectLedgerFail(Error, TEXT("Operation replay changed its payload."));
    if (Existing->Revision != ExpectedRevision || Existing->Custody != EBSObjectCustody::World)
        return ObjectLedgerFail(Error, TEXT("Move has stale revision or the object is not in the world."));
    FBSObjectSnapshot Candidate = Current;
    FBSObjectRecord& Object = Candidate.Objects[Existing - Current.Objects.GetData()];
    Object.RestTransform = RestTransform; ++Object.Revision;
    Object.LastOperationId = OperationId; Object.LastOperation = EBSObjectOperation::Move;
    return Commit(MoveTemp(Candidate), Error);
}

bool FBSObjectLedger::Collect(const FBSStableId& Id, int64 ExpectedRevision, const FGuid& OperationId,
    const FBSStableId& CollectionId, int32 StubCapacity, FString& Error)
{
    if (!RequireCurrent(Error)) return false;
    const FBSObjectRecord* Existing = Current.Find(Id);
    if (!Existing || !OperationId.IsValid() || !CollectionId.IsValid() || StubCapacity < 0 || StubCapacity > MaxObjectRecords)
        return ObjectLedgerFail(Error, TEXT("Collection requires known identities, an operation ID and a bounded stub capacity."));
    if (Existing->LastOperationId == OperationId)
        return Existing->LastOperation == EBSObjectOperation::Collect && ExpectedRevision == Existing->Revision - 1
            && Existing->CollectionId == CollectionId ? true : ObjectLedgerFail(Error, TEXT("Operation replay changed its payload."));
    if (Existing->Revision != ExpectedRevision || Existing->Custody != EBSObjectCustody::World || !Existing->bCollectible)
        return ObjectLedgerFail(Error, TEXT("Collection is stale, ineligible or already owned elsewhere."));
    int32 Occupied = 0;
    for (const FBSObjectRecord& Object : Current.Objects)
        if (Object.Custody == EBSObjectCustody::Collection && Object.CollectionId == CollectionId) ++Occupied;
    if (Occupied >= StubCapacity) return ObjectLedgerFail(Error, TEXT("Collection stub refused the transfer because it is full."));
    FBSObjectSnapshot Candidate = Current;
    FBSObjectRecord& Object = Candidate.Objects[Existing - Current.Objects.GetData()];
    Object.Custody = EBSObjectCustody::Collection; Object.CollectionId = CollectionId; ++Object.Revision;
    Object.LastOperationId = OperationId; Object.LastOperation = EBSObjectOperation::Collect;
    return Commit(MoveTemp(Candidate), Error);
}
