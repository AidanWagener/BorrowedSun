#include "Objects/BSCarryComponent.h"

#include "Objects/BSPhysicalObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

UBSCarryComponent::UBSCarryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UBSCarryComponent::BeginPlay()
{
    Super::BeginPlay();
    Handle = NewObject<UPhysicsHandleComponent>(GetOwner(), TEXT("PropHandConstraint"));
    Handle->RegisterComponent();
    Handle->AddTickPrerequisiteComponent(this);
    Handle->SetLinearStiffness(2200.f); Handle->SetLinearDamping(180.f);
    Handle->SetAngularStiffness(1800.f); Handle->SetAngularDamping(160.f);
    Handle->SetInterpolationSpeed(12.f);
    Handle->SetComponentTickEnabled(false);
}

bool UBSCarryComponent::GetView(FVector& Eye, FRotator& View) const
{
    const auto* Pawn = Cast<APawn>(GetOwner());
    if (!Pawn || !Pawn->GetController()) return false;
    Pawn->GetActorEyesViewPoint(Eye, View);
    return !Eye.ContainsNaN() && !View.ContainsNaN();
}

ABSPhysicalObject* UBSCarryComponent::TraceObject() const
{
    FVector Eye; FRotator View;
    if (!GetView(Eye, View)) return nullptr;
    FHitResult Hit; FCollisionQueryParams Query(SCENE_QUERY_STAT(PropReach), false, GetOwner());
    // First opaque hit wins: a prop behind a shelf or wall cannot be reached through it.
    GetWorld()->LineTraceSingleByChannel(Hit, Eye, Eye + View.Vector() * 225.f, ECC_Visibility, Query);
    return Hit.bBlockingHit ? Cast<ABSPhysicalObject>(Hit.GetActor()) : nullptr;
}

bool UBSCarryComponent::ResolveTarget(const FVector& Eye, const FRotator& View, FVector& Target) const
{
    Target = Eye + View.Vector() * HoldDistance;
    FCollisionQueryParams Query(SCENE_QUERY_STAT(PropHold), false, GetOwner());
    Query.AddIgnoredActor(Held.Get());
    FHitResult Hit;
    GetWorld()->SweepSingleByChannel(Hit, Eye, Target, FQuat::Identity, ECC_PhysicsBody,
        FCollisionShape::MakeSphere(Radius), Query);
    if (Hit.bStartPenetrating) return false;
    if (Hit.bBlockingHit)
    {
        if (Hit.Distance < Radius + 20.f) return false;
        Target = Hit.Location - View.Vector() * 2.f;
    }
    return true;
}

bool UBSCarryComponent::TryGrab(FString& Error)
{
    Error.Reset();
    if (Held.IsValid() || !Handle) { Error = TEXT("The hand is already occupied or unavailable."); return false; }
    ABSPhysicalObject* Object = TraceObject();
    if (!Object || !Object->CanCarry()) { Error = TEXT("Look at a movable prop within arm's reach."); return false; }
    FVector Eye; FRotator View;
    if (!GetView(Eye, View) || !Object->SetHolder(this)) { Error = TEXT("This prop is already held."); return false; }
    Held = Object;
    const FVector Grip = Object->GripLocation();
    Radius = FMath::Max(4.f, float(Object->GetBody()->Bounds.SphereRadius + FVector::Distance(Grip, Object->GetBody()->Bounds.Origin)));
    HoldDistance = FMath::Clamp(float(FVector::Distance(Eye, Grip)), Radius + 45.f, 180.f);
    if (!ResolveTarget(Eye, View, LastTarget)) { Release(false); Error = TEXT("There is not enough room to lift this prop."); return false; }
    RelativeRotation = View.Quaternion().Inverse() * Object->GetActorQuat();
    Handle->GrabComponentAtLocationWithRotation(Object->GetBody(), NAME_None, Grip, Object->GetActorRotation());
    if (Handle->GetGrabbedComponent() != Object->GetBody()) { Release(false); Error = TEXT("The physical grip could not be created."); return false; }
    Handle->SetTargetLocationAndRotation(LastTarget, Object->GetActorRotation());
    Handle->SetComponentTickEnabled(true); SetComponentTickEnabled(true);
    return true;
}

void UBSCarryComponent::TickComponent(float Dt, ELevelTick TickType, FActorComponentTickFunction* TickFunction)
{
    Super::TickComponent(Dt, TickType, TickFunction);
    FVector Eye; FRotator View;
    ABSPhysicalObject* Object = Held.Get();
    if (!Object || !Handle || !GetView(Eye, View) || !Object->CanCarry()
        || Handle->GetGrabbedComponent() != Object->GetBody() || !ResolveTarget(Eye, View, LastTarget)
        || FVector::DistSquared(Object->GripLocation(), LastTarget) > FMath::Square(150.f))
    { Release(false); return; }
    Handle->SetTargetLocationAndRotation(LastTarget, (View.Quaternion() * RelativeRotation).Rotator());
}

void UBSCarryComponent::RotateHeld(float Yaw, float Pitch)
{
    if (!Held.IsValid() || !FMath::IsFinite(Yaw) || !FMath::IsFinite(Pitch)) return;
    const FQuat Delta = FRotator(FMath::Clamp(Pitch, -12.f, 12.f), FMath::Clamp(Yaw, -12.f, 12.f), 0).Quaternion();
    RelativeRotation = (Delta * RelativeRotation).GetNormalized();
}

void UBSCarryComponent::Release(bool bThrow)
{
    ABSPhysicalObject* Object = Held.Get();
    if (Handle) { Handle->ReleaseComponent(); Handle->SetComponentTickEnabled(false); }
    Held.Reset(); SetComponentTickEnabled(false);
    if (!Object) return;
    Object->ClearHolder();
    FVector Eye; FRotator View;
    if (bThrow && Object->CanCarry() && GetView(Eye, View))
    {
        // Cap pre-existing spring velocity before a mass-scaled impulse. No charge accumulator.
        Object->GetBody()->SetPhysicsLinearVelocity(Object->GetBody()->GetPhysicsLinearVelocity().GetClampedToMaxSize(150.f));
        Object->GetBody()->AddImpulse(View.Vector() * Object->GetPlacement().MassKg * 650.f);
    }
}

bool UBSCarryComponent::Collect(const FBSStableId& Destination, int32 Capacity, FString& Error)
{
    ABSPhysicalObject* Object = Held.IsValid() ? Held.Get() : TraceObject();
    if (!Object) { Error = TEXT("Look at or hold a collectible prop."); return false; }
    if (Object->IsHeld() && Object != Held.Get()) { Error = TEXT("Someone else is holding this prop."); return false; }
    return Object->CommitCollection(Destination, Capacity, Error);
}

void UBSCarryComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    Release(false);
    if (Handle) { Handle->DestroyComponent(); Handle = nullptr; }
    Super::EndPlay(Reason);
}
