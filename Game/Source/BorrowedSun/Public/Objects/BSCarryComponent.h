#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BSStableId.h"
#include "BSCarryComponent.generated.h"

class ABSPhysicalObject;
class UPhysicsHandleComponent;

/** Local single-player hand. Only ticks while it owns a simulated prop; custody stays in the world ledger. */
UCLASS(ClassGroup=(BorrowedSun))
class BORROWEDSUN_API UBSCarryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UBSCarryComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void TickComponent(float Dt, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;
    ABSPhysicalObject* TraceObject() const;
    ABSPhysicalObject* GetHeldObject() const { return Held.Get(); }
    bool TryGrab(FString& OutError);
    void Release(bool bThrow);
    void RotateHeld(float YawDegrees, float PitchDegrees);
    bool Collect(const FBSStableId& Destination, int32 Capacity, FString& OutError);
    bool GetView(FVector& Location, FRotator& Rotation) const;
    FVector GetLastTarget() const { return LastTarget; }
private:
    UPROPERTY() TObjectPtr<UPhysicsHandleComponent> Handle;
    TWeakObjectPtr<ABSPhysicalObject> Held;
    FQuat RelativeRotation = FQuat::Identity;
    FVector LastTarget = FVector::ZeroVector;
    float Radius = 10.f;
    float HoldDistance = 140.f;
    bool ResolveTarget(const FVector& Eye, const FRotator& View, FVector& Target) const;
};
