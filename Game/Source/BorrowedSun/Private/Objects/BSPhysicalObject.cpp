#include "Objects/BSPhysicalObject.h"

#include "Objects/BSCarryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"

namespace BSPhysicalObjectPrivate
{
    bool Reject(FString& Error, const TCHAR* Message) { Error = Message; return false; }
    bool ValidatePlacement(const FBSPhysicalObjectPlacement& P, FString& Error)
    {
        FBSObjectRecord Record; Record.InstanceId = P.InstanceId; Record.DefinitionId = P.DefinitionId;
        Record.DefinitionRevision = P.DefinitionRevision; Record.RestTransform = P.Transform;
        Record.bCollectible = P.bCanCollect; Record.bMissionCustody = P.bMissionCustody;
        if (!Record.IsValid(Error)) return false;
        if (!P.Mesh || !P.Mesh->GetBodySetup() || P.Mesh->GetBodySetup()->CollisionTraceFlag == CTF_UseComplexAsSimple
            || P.Mesh->GetBodySetup()->AggGeom.GetElementCount() == 0)
            return Reject(Error, TEXT("Physical prop requires an authored mesh with simple collision."));
        if (!FMath::IsFinite(P.MassKg) || P.MassKg < .01f || P.MassKg > 1000.f || !P.Transform.GetScale3D().Equals(FVector(1), .0001))
            return Reject(Error, TEXT("Physical placement requires a bounded mass and geometry authored at unit scale."));
        return true;
    }
}

ABSPhysicalObject::ABSPhysicalObject()
{
    PrimaryActorTick.bCanEverTick = false;
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectBody"));
    SetRootComponent(Body); Body->SetMobility(EComponentMobility::Movable);
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Body->SetCollisionObjectType(ECC_PhysicsBody);
    Body->SetCollisionResponseToAllChannels(ECR_Block);
    // Small carried props cannot form a physics elevator or launch the player capsule.
    Body->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    Body->SetCanEverAffectNavigation(false);
    Body->CanCharacterStepUpOn = ECB_No;
    Body->BodyInstance.bGenerateWakeEvents = true;
    Body->SetLinearDamping(.2f); Body->SetAngularDamping(3.f);
    // Centimetre-scale curved props need enough contact iterations to settle instead
    // of continually rocking at seams between their authored convex hulls.
    Body->BodyInstance.SetPositionSolverIterationCount(16);
    Body->BodyInstance.SetVelocitySolverIterationCount(4);
    Body->BodyInstance.SetOverrideIterationCounts(true);
}

bool ABSPhysicalObject::InitializePlacement(const FBSPhysicalObjectPlacement& P, const FBSObjectRecord& Record, FString& Error)
{
    if (bConfigured || !BSPhysicalObjectPrivate::ValidatePlacement(P, Error) || !Record.IsValid(Error)) return false;
    if (!(Record.InstanceId == P.InstanceId) || !(Record.DefinitionId == P.DefinitionId)
        || Record.DefinitionRevision != P.DefinitionRevision || Record.bCollectible != P.bCanCollect
        || !Record.RestTransform.GetScale3D().Equals(FVector(1), .0001))
        return BSPhysicalObjectPrivate::Reject(Error, TEXT("Saved object does not match its physical definition."));
    Placement = P; Body->SetStaticMesh(P.Mesh);
    auto* Contact = NewObject<UPhysicalMaterial>(this);
    Contact->Friction = .7f; Contact->StaticFriction = .7f; Contact->Restitution = .05f;
    // Filter sub-centimetre contact chatter for 30 consecutive physics steps before sleeping.
    // Angular threshold is radians/s; ordinary movement and impacts still wake the body.
    Contact->SleepLinearVelocityThreshold = 1.f;
    Contact->SleepAngularVelocityThreshold = .2f;
    Contact->SleepCounterThreshold = 30;
    Body->SetPhysMaterialOverride(Contact);
    Body->SetMassOverrideInKg(NAME_None, P.MassKg, true);
    Body->SetUseCCD(true); Body->SetPhysicsMaxAngularVelocityInDegrees(360.f);
    Body->OnComponentWake.AddDynamic(this, &ThisClass::OnWake);
    Body->OnComponentSleep.AddDynamic(this, &ThisClass::OnSleep);
    bConfigured = true;
    if (!Record.ShouldSpawnInWorld()) { SuppressWorldRepresentation(); return true; }
    SetActorTransform(Record.RestTransform, false, nullptr, ETeleportType::TeleportPhysics);
    Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    if (!IsPoseClear(Record.RestTransform))
    {
        if (!RecoverToAnchor(Error)) return false;
    }
    Body->SetSimulatePhysics(true);
    Body->SetPhysicsLinearVelocity(FVector::ZeroVector); Body->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    GetWorldTimerManager().SetTimer(MotionTimer, this, &ThisClass::WatchMotion, .25f, true);
    return Body->IsSimulatingPhysics();
}

bool ABSPhysicalObject::CanCarry() const
{
    return bConfigured && !bSuppressed && Placement.bCanCarry && !Placement.bMissionCustody
        && Placement.MassKg <= 8.f && Body->IsSimulatingPhysics()
        && Body->GetStaticMesh()->GetBoundingBox().GetExtent().GetMax() <= 60.0;
}

FVector ABSPhysicalObject::GripLocation() const
{
    return !Placement.GripSocket.IsNone() && Body->DoesSocketExist(Placement.GripSocket)
        ? Body->GetSocketLocation(Placement.GripSocket) : Body->GetCenterOfMass();
}

bool ABSPhysicalObject::SetHolder(UBSCarryComponent* InHolder)
{
    if (!InHolder || IsHeld() || !CanCarry()) return false;
    Holder = InHolder; Body->WakeAllRigidBodies();
    GetWorldTimerManager().SetTimer(MotionTimer, this, &ThisClass::WatchMotion, .25f, true);
    return true;
}

void ABSPhysicalObject::ClearHolder()
{
    Holder.Reset();
    if (bConfigured && !bSuppressed) GetWorldTimerManager().SetTimer(MotionTimer, this, &ThisClass::WatchMotion, .25f, true);
}

bool ABSPhysicalObject::IsPoseClear(const FTransform& Pose) const
{
    const auto* Store = GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>();
    if (!Store || Pose.ContainsNaN() || !Pose.GetRotation().IsNormalized() || !Pose.GetScale3D().Equals(FVector(1), .0001)) return false;
    const FBox Bounds = Body->GetStaticMesh()->GetBoundingBox().TransformBy(Pose);
    if (!Store->GetRecoveryRegion().IsInsideOrOn(Bounds.Min) || !Store->GetRecoveryRegion().IsInsideOrOn(Bounds.Max)) return false;
    TArray<FOverlapResult> Hits; FComponentQueryParams Query(SCENE_QUERY_STAT(ObjectPose), this);
    Body->ComponentOverlapMulti(Hits, GetWorld(), Pose.GetLocation() + FVector(0, 0, .5), Pose.GetRotation(), ECC_PhysicsBody, Query);
    for (const FOverlapResult& Hit : Hits)
        if (const UPrimitiveComponent* Other = Hit.GetComponent())
            if (Other->GetOwner() != this && Other->GetCollisionObjectType() != ECC_Pawn
                && Other->GetCollisionResponseToChannel(ECC_PhysicsBody) == ECR_Block && Hit.bBlockingHit) return false;
    return true;
}

bool ABSPhysicalObject::PersistRestingPose(FString& Error)
{
    Error.Reset();
    if (bSuppressed || IsHeld() || !bConfigured || Body->IsAnyRigidBodyAwake()) return true;
    auto* Store = GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>();
    if (!Store || !Store->GetLedger()) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object store is unavailable."));
    const FBSObjectRecord* Record = Store->GetLedger()->GetSnapshot().Find(Placement.InstanceId);
    if (!Record || !Record->ShouldSpawnInWorld()) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object no longer has world custody."));
    const FTransform Pose = GetActorTransform();
    if (!IsPoseClear(Pose)) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Settled object pose intersects geometry or leaves the recovery region."));
    if (Record->RestTransform.Equals(Pose, .05)) { PendingMoveOperation.Invalidate(); return true; }
    if (!PendingMoveOperation.IsValid() || !PendingMovePose.Equals(Pose, .001))
    { PendingMoveOperation = FGuid::NewGuid(); PendingMovePose = Pose; }
    if (!Store->GetLedger()->Move(Placement.InstanceId, Record->Revision, PendingMoveOperation, Pose, Error)) return false;
    PendingMoveOperation.Invalidate(); return true;
}

bool ABSPhysicalObject::CommitCollection(const FBSStableId& Destination, int32 Capacity, FString& Error)
{
    auto* Store = GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>();
    if (bSuppressed || !bConfigured || !Store || !Store->GetLedger()) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object is not available in the world."));
    const FBSObjectRecord* Record = Store->GetLedger()->GetSnapshot().Find(Placement.InstanceId);
    if (!Record) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object identity was not registered."));
    if (!PendingCollectOperation.IsValid()) PendingCollectOperation = FGuid::NewGuid();
    if (!Store->GetLedger()->Collect(Placement.InstanceId, Record->Revision, PendingCollectOperation, Destination, Capacity, Error)) return false;
    SuppressWorldRepresentation(); return true;
}

void ABSPhysicalObject::SuppressWorldRepresentation()
{
    if (Holder.IsValid()) Holder->Release(false);
    bSuppressed = true; Holder.Reset();
    GetWorldTimerManager().ClearTimer(MotionTimer);
    Body->SetSimulatePhysics(false); Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorHiddenInGame(true);
}

bool ABSPhysicalObject::RecoverToAnchor(FString& Error)
{
    if (Holder.IsValid()) Holder->Release(false);
    const FVector Offsets[] = {FVector::ZeroVector, FVector(0,0,30), FVector(50,0,0), FVector(-50,0,0), FVector(0,50,0), FVector(0,-50,0), FVector(0,0,80)};
    for (const FVector& Offset : Offsets)
    {
        FTransform Pose = Placement.Transform; Pose.AddToTranslation(Offset);
        if (!IsPoseClear(Pose)) continue;
        Body->SetPhysicsLinearVelocity(FVector::ZeroVector); Body->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        SetActorTransform(Pose, false, nullptr, ETeleportType::TeleportPhysics); Body->WakeAllRigidBodies();
        Error.Reset(); return true;
    }
    return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object recovery anchors are obstructed; saved identity retained."));
}

void ABSPhysicalObject::OnWake(UPrimitiveComponent*, FName)
{
    if (bConfigured && !bSuppressed) GetWorldTimerManager().SetTimer(MotionTimer, this, &ThisClass::WatchMotion, .25f, true);
}

void ABSPhysicalObject::OnSleep(UPrimitiveComponent*, FName)
{
    // Delay disk I/O until after the physics callback has returned.
    if (bConfigured && !bSuppressed) GetWorldTimerManager().SetTimer(MotionTimer, this, &ThisClass::WatchMotion, .25f, true);
}

void ABSPhysicalObject::WatchMotion()
{
    if (!bConfigured || bSuppressed) { GetWorldTimerManager().ClearTimer(MotionTimer); return; }
    auto* Store = GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>(); if (!Store) return;
    if (!Store->GetRecoveryRegion().IsInsideOrOn(GetActorLocation()))
    {
        if (!RecoverToAnchor(Store->LastError)) { Body->SetSimulatePhysics(false); GetWorldTimerManager().ClearTimer(MotionTimer); }
        return;
    }
    if (!IsHeld() && !Body->IsAnyRigidBodyAwake())
    {
        PersistRestingPose(Store->LastError);
        GetWorldTimerManager().ClearTimer(MotionTimer); // Failed writes are retried explicitly; no disk-error polling loop.
    }
}

void ABSPhysicalObject::EndPlay(const EEndPlayReason::Type Reason)
{
    if (Holder.IsValid()) Holder->Release(false);
    GetWorldTimerManager().ClearTimer(MotionTimer);
    if (auto* Store = GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>()) Store->Unregister(this);
    Super::EndPlay(Reason);
}

bool UBSObjectWorldSubsystem::Open(const FBSStableId& WorldId, const FString& Namespace, const FBox& Bounds, FString& Error)
{
    return OpenWithStorage(WorldId, MakeUnique<FBSObjectSaveSlots>(Namespace), Bounds, Error);
}

bool UBSObjectWorldSubsystem::OpenWithStorage(const FBSStableId& WorldId, TUniquePtr<IBSObjectStorage> InStorage, const FBox& Bounds, FString& Error)
{
    if (Ledger || !InStorage || !Bounds.IsValid || Bounds.Min.ContainsNaN() || Bounds.Max.ContainsNaN()
        || Bounds.GetSize().GetMin() <= 0)
        return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object world is already open or its recovery region is invalid."));
    TUniquePtr<FBSObjectLedger> Candidate = MakeUnique<FBSObjectLedger>(*InStorage);
    if (!Candidate->Open(WorldId, Error)) return false;
    Storage = MoveTemp(InStorage); Ledger = MoveTemp(Candidate); WorldIdentity = WorldId; Region = Bounds;
    LastError = Ledger->GetRecoveryWarning(); return true;
}

ABSPhysicalObject* UBSObjectWorldSubsystem::RegisterPlacement(const FBSPhysicalObjectPlacement& P, FString& Error)
{
    Error.Reset();
    if (!Ledger || !BSPhysicalObjectPrivate::ValidatePlacement(P, Error))
    { if (Error.IsEmpty()) Error = TEXT("Object world must be opened before placement."); return nullptr; }
    const FString Key = P.InstanceId.ToString();
    if (LiveObjects.FindRef(Key).IsValid()) { Error = TEXT("A live actor already owns this placed identity."); return nullptr; }
    FBSObjectRecord Record; Record.InstanceId = P.InstanceId; Record.DefinitionId = P.DefinitionId;
    Record.DefinitionRevision = P.DefinitionRevision; Record.RestTransform = P.Transform; Record.bCollectible = P.bCanCollect;
    if (!Ledger->Register(Record, Error)) return nullptr;
    Placements.Add(Key, P);
    return SpawnSaved(P, Error);
}

ABSPhysicalObject* UBSObjectWorldSubsystem::SpawnSaved(const FBSPhysicalObjectPlacement& P, FString& Error)
{
    const FBSObjectRecord* Saved = Ledger->GetSnapshot().Find(P.InstanceId);
    if (!Saved || !Saved->ShouldSpawnInWorld()) return nullptr; // Collection tombstone is successful absence.
    const FBSObjectRecord Record = *Saved;
    FActorSpawnParameters Params; Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ABSPhysicalObject* Actor = GetWorld()->SpawnActor<ABSPhysicalObject>(ABSPhysicalObject::StaticClass(), Record.RestTransform, Params);
    if (!Actor || !Actor->InitializePlacement(P, Record, Error))
    { if (Actor) Actor->Destroy(); if (Error.IsEmpty()) Error = TEXT("Physical object could not be created."); return nullptr; }
    LiveObjects.Add(P.InstanceId.ToString(), Actor); return Actor;
}

void UBSObjectWorldSubsystem::Unregister(ABSPhysicalObject* Actor)
{
    if (!Actor) return;
    const FString Key = Actor->GetPlacement().InstanceId.ToString();
    if (LiveObjects.FindRef(Key).Get() == Actor) LiveObjects.Remove(Key);
}

bool UBSObjectWorldSubsystem::SaveSettledObjects(FString& Error)
{
    Error.Reset();
    for (const auto& Pair : LiveObjects)
        if (Pair.Value.IsValid() && !Pair.Value->PersistRestingPose(Error)) return false;
    return true;
}

bool UBSObjectWorldSubsystem::Reload(FString& Error)
{
    if (!Ledger) return BSPhysicalObjectPrivate::Reject(Error, TEXT("Object world has not been opened."));
    // Read and validate before removing any existing representation.
    if (!Ledger->Open(WorldIdentity, Error)) return false;
    TArray<TWeakObjectPtr<ABSPhysicalObject>> Previous; LiveObjects.GenerateValueArray(Previous);
    for (const auto& Actor : Previous) if (Actor.IsValid()) Actor->Destroy();
    LiveObjects.Empty();
    bool Success = true; FString FirstError;
    for (const auto& Pair : Placements)
    {
        FString SpawnError; SpawnSaved(Pair.Value, SpawnError);
        if (!SpawnError.IsEmpty()) { if (FirstError.IsEmpty()) FirstError = SpawnError; Success = false; }
    }
    Error = FirstError; LastError = Success ? Ledger->GetRecoveryWarning() : Error;
    return Success;
}
