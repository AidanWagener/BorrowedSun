#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BSKestrelPawn.generated.h"

class UCameraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USceneComponent;
class USphereComponent;
class USpringArmComponent;
class UStaticMesh;
class UStaticMeshComponent;

/** Collision utility. The pawn runs a bounded substep simulation, so this never ticks itself. */
UCLASS()
class BORROWEDSUN_API UBSKestrelMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()
public:
    UBSKestrelMovementComponent();
    void MoveFlightStep(const FVector& Delta, const FQuat& Rotation);
};

/** Inertial hover craft with no automatic forward throttle and no physics simulation dependency. */
UCLASS()
class BORROWEDSUN_API ABSKestrelPawn : public APawn
{
    GENERATED_BODY()

public:
    ABSKestrelPawn();
    virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;
    virtual UPawnMovementComponent* GetMovementComponent() const override;
    virtual FVector GetVelocity() const override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void MoveUp(float Value);
    /** Look values are degrees already scaled by the player controller. */
    void LookYaw(float Degrees);
    void LookPitch(float Degrees);
    void SetBoosting(bool bEnabled) { bBoosting = bEnabled; }
    void SetBraking(bool bEnabled) { bBraking = bEnabled; }
    FVector GetLinearVelocity() const;
    float GetSpeed() const;
    bool IsNearlyStopped() const;
    void StopFlight();
    void ResetFlight(const FTransform& Transform);
    void SetFlightBounds(const FBox& Bounds);
    FBox GetFlightBounds() const { return FBox(FlightBoundsMin, FlightBoundsMax); }
    UCameraComponent* GetViewCamera() const { return ViewCamera; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Flight") TObjectPtr<USphereComponent> Collision;
    UPROPERTY(VisibleAnywhere, Category = "Flight") TObjectPtr<UBSKestrelMovementComponent> FlightMovement;
    UPROPERTY(VisibleAnywhere, Category = "Appearance") TObjectPtr<USceneComponent> VisualRoot;
    UPROPERTY(VisibleAnywhere, Category = "View") TObjectPtr<USpringArmComponent> CameraArm;
    UPROPERTY(VisibleAnywhere, Category = "View") TObjectPtr<UCameraComponent> ViewCamera;
    UPROPERTY(EditDefaultsOnly, Category = "Appearance") TSoftObjectPtr<UMaterialInterface> SurfaceMaterial;
    UPROPERTY() TObjectPtr<UMaterialInterface> FallbackMaterial;
    UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> EngineMaterials;

    UPROPERTY(EditAnywhere, Category = "Flight", meta = (ClampMin = "1")) float CruiseSpeed = 1800.0f;
    UPROPERTY(EditAnywhere, Category = "Flight", meta = (ClampMin = "1")) float BoostSpeed = 3500.0f;
    UPROPERTY(EditAnywhere, Category = "Flight", meta = (ClampMin = "1")) float Acceleration = 2200.0f;
    UPROPERTY(EditAnywhere, Category = "Flight", meta = (ClampMin = "1")) float CoastDeceleration = 300.0f;
    UPROPERTY(EditAnywhere, Category = "Flight", meta = (ClampMin = "1")) float BrakeDeceleration = 9000.0f;
    UPROPERTY(EditAnywhere, Category = "Flight") FVector FlightBoundsMin = FVector(-45000.0, -45000.0, 100.0);
    UPROPERTY(EditAnywhere, Category = "Flight") FVector FlightBoundsMax = FVector(45000.0, 45000.0, 28000.0);

    FVector FlightInput = FVector::ZeroVector;
    FRotator DesiredHeading = FRotator::ZeroRotator;
    FTransform LastSafeTransform = FTransform::Identity;
    bool bBoosting = false;
    bool bBraking = false;

    void ApplyMaterials();
    void SimulateFlight(float DeltaSeconds);
    UStaticMeshComponent* AddShipPart(FName Name, UStaticMesh* Mesh, const FVector& Location,
        const FRotator& Rotation, const FVector& Scale, FName Finish);
};
