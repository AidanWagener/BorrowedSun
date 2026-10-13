#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/WorldSubsystem.h"
#include "Objects/BSObjectLedger.h"
#include "BSPhysicalObject.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UBSCarryComponent;

USTRUCT()
struct BORROWEDSUN_API FBSPhysicalObjectPlacement
{
    GENERATED_BODY()
    UPROPERTY() FBSStableId InstanceId;
    UPROPERTY() FBSStableId DefinitionId;
    UPROPERTY() int32 DefinitionRevision = 1;
    UPROPERTY() FString DisplayName;
    UPROPERTY() TObjectPtr<UStaticMesh> Mesh;
    UPROPERTY() FTransform Transform = FTransform::Identity;
    UPROPERTY() float MassKg = 1.0f;
    UPROPERTY() bool bCanCarry = true;
    UPROPERTY() bool bCanCollect = true;
    UPROPERTY() bool bMissionCustody = false;
    UPROPERTY() FName GripSocket;
};

/** One independently simulated, addressable ordinary prop. Its mesh must already have simple collision. */
UCLASS()
class BORROWEDSUN_API ABSPhysicalObject : public AActor
{
    GENERATED_BODY()
public:
    ABSPhysicalObject();
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UStaticMeshComponent* GetBody() const { return Body; }
    const FBSPhysicalObjectPlacement& GetPlacement() const { return Placement; }
    bool IsHeld() const { return Holder.IsValid(); }
    bool CanCarry() const;
    FVector GripLocation() const;
    bool SetHolder(UBSCarryComponent* InHolder);
    void ClearHolder();
    /** Math, recovery bounds and actual component collision checked; small floor contact tolerance only. */
    bool IsPoseClear(const FTransform& Pose) const;
    bool PersistRestingPose(FString& OutError);
    bool CommitCollection(const FBSStableId& Destination, int32 StubCapacity, FString& OutError);
    bool InitializePlacement(const FBSPhysicalObjectPlacement& InPlacement, const FBSObjectRecord& Record, FString& OutError);
    void SuppressWorldRepresentation();
private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Body;
    UPROPERTY() FBSPhysicalObjectPlacement Placement;
    TWeakObjectPtr<UBSCarryComponent> Holder;
    FTimerHandle MotionTimer;
    FGuid PendingMoveOperation;
    FTransform PendingMovePose;
    FGuid PendingCollectOperation;
    bool bConfigured = false;
    bool bSuppressed = false;
    UFUNCTION() void OnWake(UPrimitiveComponent* Component, FName BoneName);
    UFUNCTION() void OnSleep(UPrimitiveComponent* Component, FName BoneName);
    void WatchMotion();
    bool RecoverToAnchor(FString& OutError);
};

/** Explicitly opened per-world store and live-actor registry. Idle ordinary demo worlds do not create object saves. */
UCLASS()
class BORROWEDSUN_API UBSObjectWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    bool Open(const FBSStableId& WorldId, const FString& SaveNamespace, const FBox& RecoveryRegion, FString& OutError);
    bool OpenWithStorage(const FBSStableId& WorldId, TUniquePtr<IBSObjectStorage> InStorage,
        const FBox& RecoveryRegion, FString& OutError);
    ABSPhysicalObject* RegisterPlacement(const FBSPhysicalObjectPlacement& Placement, FString& OutError);
    bool Reload(FString& OutError);
    bool SaveSettledObjects(FString& OutError);
    void Unregister(ABSPhysicalObject* Actor);
    FBSObjectLedger* GetLedger() const { return Ledger.Get(); }
    const FBox& GetRecoveryRegion() const { return Region; }
    FString LastError;
private:
    TUniquePtr<IBSObjectStorage> Storage;
    TUniquePtr<FBSObjectLedger> Ledger;
    FBSStableId WorldIdentity;
    FBox Region = FBox(EForceInit::ForceInit);
    UPROPERTY() TMap<FString, FBSPhysicalObjectPlacement> Placements;
    UPROPERTY() TMap<FString, TWeakObjectPtr<ABSPhysicalObject>> LiveObjects;
    ABSPhysicalObject* SpawnSaved(const FBSPhysicalObjectPlacement& Placement, FString& OutError);
};
