#pragma once

#include "CoreMinimal.h"
#include "Core/BSStableId.h"
#include "GameFramework/SaveGame.h"
#include "BSObjectLedger.generated.h"

UENUM()
enum class EBSObjectCustody : uint8 { World, Collection };

UENUM()
enum class EBSObjectOperation : uint8 { None, Move, Collect };

/** One durable identity; the collection is a location in this ledger, not a second item copy. */
USTRUCT()
struct BORROWEDSUN_API FBSObjectRecord
{
    GENERATED_BODY()

    UPROPERTY(SaveGame) FBSStableId InstanceId;
    UPROPERTY(SaveGame) FBSStableId DefinitionId;
    UPROPERTY(SaveGame) int32 DefinitionRevision = 1;
    UPROPERTY(SaveGame) FTransform RestTransform = FTransform::Identity;
    UPROPERTY(SaveGame) EBSObjectCustody Custody = EBSObjectCustody::World;
    UPROPERTY(SaveGame) FBSStableId CollectionId;
    UPROPERTY(SaveGame) bool bCollectible = false;
    UPROPERTY(SaveGame) bool bMissionCustody = false;
    UPROPERTY(SaveGame) int64 Revision = 0;
    UPROPERTY(SaveGame) FGuid LastOperationId;
    UPROPERTY(SaveGame) EBSObjectOperation LastOperation = EBSObjectOperation::None;

    bool IsValid(FString& OutError) const;
    bool Equals(const FBSObjectRecord& Other) const;
    bool ShouldSpawnInWorld() const { return Custody == EBSObjectCustody::World; }
};

USTRUCT()
struct BORROWEDSUN_API FBSObjectSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame) int32 SchemaVersion = 1;
    UPROPERTY(SaveGame) FBSStableId WorldId;
    UPROPERTY(SaveGame) int64 Generation = 0;
    UPROPERTY(SaveGame) TArray<FBSObjectRecord> Objects;

    bool IsValid(FString& OutError) const;
    bool Equals(const FBSObjectSnapshot& Other) const;
    const FBSObjectRecord* Find(const FBSStableId& InstanceId) const;
};

UCLASS()
class BORROWEDSUN_API UBSObjectSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) FBSObjectSnapshot Snapshot;
};

/** Synchronous storage seam. Read distinguishes absent slots from unreadable existing data.
 *  Callers serialize access on the game thread; this is not a cross-process database lock. */
class BORROWEDSUN_API IBSObjectStorage
{
public:
    virtual ~IBSObjectStorage() = default;
    virtual bool Read(int32 Slot, FBSObjectSnapshot& Out, bool& bExists, FString& OutError) = 0;
    virtual bool Write(int32 Slot, const FBSObjectSnapshot& Snapshot, FString& OutError) = 0;
};

/** Separate slots from mission checkpoints. Namespace validation occurs before any disk access. */
class BORROWEDSUN_API FBSObjectSaveSlots final : public IBSObjectStorage
{
public:
    explicit FBSObjectSaveSlots(FString InNamespace);
    virtual bool Read(int32 Slot, FBSObjectSnapshot& Out, bool& bExists, FString& OutError) override;
    virtual bool Write(int32 Slot, const FBSObjectSnapshot& Snapshot, FString& OutError) override;
    FString SlotName(int32 Slot) const;
private:
    FString Namespace;
};

/** Value-only authority for ordinary props. No actor pointers, held velocities or inventory UI.
 *  Actors must update visibility only after a successful durable operation. RestTransform is
 *  an already collision-validated resting/recovery pose supplied by the physical-object layer. */
class BORROWEDSUN_API FBSObjectLedger
{
public:
    explicit FBSObjectLedger(IBSObjectStorage& InStorage) : Storage(InStorage) {}
    bool Open(const FBSStableId& InWorldId, FString& OutError);
    const FBSObjectSnapshot& GetSnapshot() const { return Current; }
    const FString& GetRecoveryWarning() const { return RecoveryWarning; }
    bool IsOpen() const { return bOpen; }

    /** Idempotent authored placement. Existing moved or collected records win over the spawn recipe. */
    bool Register(const FBSObjectRecord& Authored, FString& OutError);
    bool Move(const FBSStableId& InstanceId, int64 ExpectedRevision, const FGuid& OperationId,
        const FTransform& RestTransform, FString& OutError);
    /** Capacity belongs to this explicit collection stub, not the future game's inventory design. */
    bool Collect(const FBSStableId& InstanceId, int64 ExpectedRevision, const FGuid& OperationId,
        const FBSStableId& CollectionId, int32 StubCapacity, FString& OutError);

private:
    IBSObjectStorage& Storage;
    FBSObjectSnapshot Current;
    bool bOpen = false;
    int32 CurrentSlot = INDEX_NONE;
    FString RecoveryWarning;
    bool ReadLatest(const FBSStableId& WorldId, FBSObjectSnapshot& Out, int32& OutSlot,
        FString& OutWarning, FString& OutError);
    bool RequireCurrent(FString& OutError);
    bool Commit(FBSObjectSnapshot Candidate, FString& OutError);
};
