#include "Demo/BSKestrelPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

UBSKestrelMovementComponent::UBSKestrelMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoRegisterUpdatedComponent = false;
}

void UBSKestrelMovementComponent::MoveFlightStep(const FVector& Delta, const FQuat& Rotation)
{
    if (!UpdatedComponent) return;
    FHitResult Hit;
    SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);
    if (Hit.bStartPenetrating)
    {
        StopMovementImmediately();
    }
    else if (Hit.IsValidBlockingHit())
    {
        // Shed the velocity into the wall. Tangential movement remains controllable.
        Velocity = FVector::VectorPlaneProject(Velocity, Hit.Normal) * 0.85;
        SlideAlongSurface(Delta, 1.0f - Hit.Time, Hit.Normal, Hit, true);
        if (Hit.bStartPenetrating) StopMovementImmediately();
        else if (Hit.IsValidBlockingHit()) Velocity = FVector::VectorPlaneProject(Velocity, Hit.Normal);
    }
    UpdateComponentVelocity();
}

ABSKestrelPawn::ABSKestrelPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("FlightCollision"));
    Collision->InitSphereRadius(205.0f);
    Collision->SetCollisionProfileName(TEXT("Pawn"));
    Collision->SetSimulatePhysics(false);
    Collision->SetCanEverAffectNavigation(false);
    SetRootComponent(Collision);

    FlightMovement = CreateDefaultSubobject<UBSKestrelMovementComponent>(TEXT("FlightMovement"));
    FlightMovement->SetUpdatedComponent(Collision);
    VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShipVisuals"));
    VisualRoot->SetupAttachment(Collision);
    VisualRoot->SetRelativeScale3D(FVector(0.75));

    CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseArm"));
    CameraArm->SetupAttachment(Collision);
    CameraArm->SetRelativeLocation(FVector(0.0, 0.0, 115.0));
    CameraArm->SetRelativeRotation(FRotator(-9.0, 0.0, 0.0));
    CameraArm->TargetArmLength = 900.0f;
    CameraArm->bEnableCameraLag = true;
    CameraArm->CameraLagSpeed = 8.0f;
    CameraArm->CameraLagMaxDistance = 170.0f;
    CameraArm->bEnableCameraRotationLag = true;
    CameraArm->CameraRotationLagSpeed = 10.0f;
    CameraArm->bDoCollisionTest = true;
    CameraArm->ProbeSize = 16.0f;
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
    ViewCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
    ViewCamera->SetFieldOfView(82.0f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cone(TEXT("/Engine/BasicShapes/Cone.Cone"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    FallbackMaterial = BasicMaterial.Object;
    SurfaceMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/BorrowedSun/Art/M_DemoSurface.M_DemoSurface")));

    AddShipPart(TEXT("IvoryFuselage"), Cube.Object, FVector(-10, 0, 0), FRotator::ZeroRotator, FVector(3.4, 1.35, 0.6), TEXT("Ivory"));
    AddShipPart(TEXT("TealKeel"), Cube.Object, FVector(-20, 0, -30), FRotator::ZeroRotator, FVector(3.0, 1.2, 0.35), TEXT("Teal"));
    AddShipPart(TEXT("TaperedNose"), Cone.Object, FVector(170, 0, 0), FRotator(90, 0, 0), FVector(0.9, 0.9, 1.5), TEXT("Teal"));
    AddShipPart(TEXT("CockpitCanopy"), Sphere.Object, FVector(65, 0, 45), FRotator::ZeroRotator, FVector(1.3, 0.9, 0.48), TEXT("Glass"));
    AddShipPart(TEXT("RescueStripe"), Cube.Object, FVector(30, 0, 34), FRotator::ZeroRotator, FVector(0.35, 1.4, 0.08), TEXT("Orange"));
    AddShipPart(TEXT("TailFin"), Cube.Object, FVector(-120, 0, 65), FRotator(12, 0, 0), FVector(0.85, 0.12, 0.8), TEXT("Teal"));

    AddShipPart(TEXT("PortWing"), Cube.Object, FVector(-35, -110, -12), FRotator(0, -18, 0), FVector(1.5, 1.5, 0.12), TEXT("Ivory"));
    AddShipPart(TEXT("StarboardWing"), Cube.Object, FVector(-35, 110, -12), FRotator(0, 18, 0), FVector(1.5, 1.5, 0.12), TEXT("Ivory"));
    AddShipPart(TEXT("PortWingtip"), Cube.Object, FVector(-35, -172, -8), FRotator(0, -18, 0), FVector(0.9, 0.18, 0.16), TEXT("Orange"));
    AddShipPart(TEXT("StarboardWingtip"), Cube.Object, FVector(-35, 172, -8), FRotator(0, 18, 0), FVector(0.9, 0.18, 0.16), TEXT("Orange"));
    AddShipPart(TEXT("PortEngine"), Cylinder.Object, FVector(-130, -105, 0), FRotator(90, 0, 0), FVector(0.55, 0.55, 1.8), TEXT("Teal"));
    AddShipPart(TEXT("StarboardEngine"), Cylinder.Object, FVector(-130, 105, 0), FRotator(90, 0, 0), FVector(0.55, 0.55, 1.8), TEXT("Teal"));
    AddShipPart(TEXT("PortThruster"), Cylinder.Object, FVector(-222, -105, 0), FRotator(90, 0, 0), FVector(0.43, 0.43, 0.08), TEXT("Engine"));
    AddShipPart(TEXT("StarboardThruster"), Cylinder.Object, FVector(-222, 105, 0), FRotator(90, 0, 0), FVector(0.43, 0.43, 0.08), TEXT("Engine"));
}

UStaticMeshComponent* ABSKestrelPawn::AddShipPart(FName Name, UStaticMesh* Mesh, const FVector& Location,
    const FRotator& Rotation, const FVector& Scale, FName Finish)
{
    UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
    Part->SetupAttachment(VisualRoot);
    Part->SetStaticMesh(Mesh);
    Part->SetRelativeLocation(Location);
    Part->SetRelativeRotation(Rotation);
    Part->SetRelativeScale3D(Scale);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Part->SetGenerateOverlapEvents(false);
    Part->SetCanEverAffectNavigation(false);
    Part->ComponentTags.Add(Finish);
    return Part;
}

void ABSKestrelPawn::ApplyMaterials()
{
    UMaterialInterface* Material = SurfaceMaterial.LoadSynchronous();
    if (!Material) Material = FallbackMaterial;
    if (!Material) return;

    EngineMaterials.Reset();
    TInlineComponentArray<UStaticMeshComponent*> Parts(this);
    for (UStaticMeshComponent* Part : Parts)
    {
        FLinearColor Tint(0.78f, 0.86f, 0.79f);
        float Glow = 0.0f;
        if (Part->ComponentHasTag(TEXT("Teal"))) Tint = FLinearColor(0.018f, 0.24f, 0.28f);
        if (Part->ComponentHasTag(TEXT("Orange"))) { Tint = FLinearColor(1.0f, 0.26f, 0.045f); Glow = 0.15f; }
        if (Part->ComponentHasTag(TEXT("Glass"))) Tint = FLinearColor(0.014f, 0.055f, 0.085f);
        if (Part->ComponentHasTag(TEXT("Engine"))) { Tint = FLinearColor(0.02f, 0.6f, 1.0f); Glow = 4.0f; }
        UMaterialInstanceDynamic* Instance = Part->CreateDynamicMaterialInstance(0, Material);
        if (!Instance) continue;
        Instance->SetVectorParameterValue(TEXT("Tint"), Tint);
        Instance->SetScalarParameterValue(TEXT("Glow"), Glow);
        if (Part->ComponentHasTag(TEXT("Engine"))) EngineMaterials.Add(Instance);
    }
}

void ABSKestrelPawn::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyMaterials();
}

void ABSKestrelPawn::BeginPlay()
{
    Super::BeginPlay();
    ApplyMaterials();
    DesiredHeading = GetActorRotation();
    DesiredHeading.Roll = 0.0;
    LastSafeTransform = GetActorTransform();
}

UPawnMovementComponent* ABSKestrelPawn::GetMovementComponent() const { return FlightMovement; }
FVector ABSKestrelPawn::GetVelocity() const { return GetLinearVelocity(); }
FVector ABSKestrelPawn::GetLinearVelocity() const { return FlightMovement ? FlightMovement->Velocity : FVector::ZeroVector; }
float ABSKestrelPawn::GetSpeed() const { return static_cast<float>(GetLinearVelocity().Size()); }
bool ABSKestrelPawn::IsNearlyStopped() const { return GetSpeed() <= 120.0f; }

void ABSKestrelPawn::MoveForward(float Value) { FlightInput.X = FMath::IsFinite(Value) ? FMath::Clamp(Value, -1.0f, 1.0f) : 0.0f; }
void ABSKestrelPawn::MoveRight(float Value) { FlightInput.Y = FMath::IsFinite(Value) ? FMath::Clamp(Value, -1.0f, 1.0f) : 0.0f; }
void ABSKestrelPawn::MoveUp(float Value) { FlightInput.Z = FMath::IsFinite(Value) ? FMath::Clamp(Value, -1.0f, 1.0f) : 0.0f; }

void ABSKestrelPawn::LookYaw(float Degrees)
{
    if (!FMath::IsFinite(Degrees)) return;
    DesiredHeading.Yaw = FMath::UnwindDegrees(DesiredHeading.Yaw + Degrees);
    if (Controller) Controller->SetControlRotation(DesiredHeading);
}

void ABSKestrelPawn::LookPitch(float Degrees)
{
    if (!FMath::IsFinite(Degrees)) return;
    DesiredHeading.Pitch = FMath::Clamp(FMath::UnwindDegrees(DesiredHeading.Pitch) + Degrees, -65.0, 65.0);
    if (Controller) Controller->SetControlRotation(DesiredHeading);
}

void ABSKestrelPawn::StopFlight()
{
    FlightInput = FVector::ZeroVector;
    bBoosting = false;
    bBraking = false;
    if (FlightMovement) FlightMovement->StopMovementImmediately();
    ConsumeMovementInputVector();
}

void ABSKestrelPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    StopFlight();
    DesiredHeading = GetActorRotation();
    DesiredHeading.Pitch = FMath::Clamp(FMath::UnwindDegrees(DesiredHeading.Pitch), -65.0, 65.0);
    DesiredHeading.Roll = 0.0;
    if (Controller) Controller->SetControlRotation(DesiredHeading);
}

void ABSKestrelPawn::UnPossessed()
{
    StopFlight();
    Super::UnPossessed();
}

void ABSKestrelPawn::SetFlightBounds(const FBox& Bounds)
{
    if (!Bounds.IsValid || Bounds.Min.ContainsNaN() || Bounds.Max.ContainsNaN()
        || Bounds.Min.X >= Bounds.Max.X || Bounds.Min.Y >= Bounds.Max.Y || Bounds.Min.Z >= Bounds.Max.Z) return;
    FlightBoundsMin = Bounds.Min;
    FlightBoundsMax = Bounds.Max;
    ResetFlight(GetActorTransform());
}

void ABSKestrelPawn::ResetFlight(const FTransform& Transform)
{
    StopFlight();
    if (Transform.ContainsNaN() || !Transform.GetRotation().IsNormalized()) return;
    const FVector Location = Transform.GetLocation().BoundToBox(FlightBoundsMin, FlightBoundsMax);
    DesiredHeading = Transform.Rotator();
    DesiredHeading.Pitch = FMath::Clamp(FMath::UnwindDegrees(DesiredHeading.Pitch), -65.0, 65.0);
    DesiredHeading.Roll = 0.0;
    SetActorLocationAndRotation(Location, DesiredHeading, false, nullptr, ETeleportType::TeleportPhysics);
    if (Controller) Controller->SetControlRotation(DesiredHeading);
    LastSafeTransform = GetActorTransform();
}

void ABSKestrelPawn::SimulateFlight(float DeltaSeconds)
{
    const FRotator Heading = FMath::RInterpTo(GetActorRotation(), DesiredHeading, DeltaSeconds, 5.5f);
    const FRotationMatrix Basis(Heading);
    const FVector Direction = (Basis.GetUnitAxis(EAxis::X) * FlightInput.X
        + Basis.GetUnitAxis(EAxis::Y) * FlightInput.Y + FVector::UpVector * FlightInput.Z).GetClampedToMaxSize(1.0);
    const float MaximumSpeed = bBoosting ? BoostSpeed : CruiseSpeed;
    const FVector TargetVelocity = bBraking ? FVector::ZeroVector : Direction * MaximumSpeed;
    const float ChangeRate = bBraking ? BrakeDeceleration : (Direction.IsNearlyZero() ? CoastDeceleration : Acceleration);
    FlightMovement->Velocity = FMath::VInterpConstantTo(FlightMovement->Velocity, TargetVelocity, DeltaSeconds, ChangeRate);
    FlightMovement->Velocity = FlightMovement->Velocity.GetClampedToMaxSize(FMath::Max(CruiseSpeed, BoostSpeed));

    const FVector Position = GetActorLocation();
    const FVector Intended = Position + FlightMovement->Velocity * DeltaSeconds;
    const FVector Bounded = Intended.BoundToBox(FlightBoundsMin, FlightBoundsMax);
    for (int32 Axis = 0; Axis < 3; ++Axis)
    {
        if (!FMath::IsNearlyEqual(Intended[Axis], Bounded[Axis])) FlightMovement->Velocity[Axis] = 0.0;
    }
    FlightMovement->MoveFlightStep(Bounded - Position, Heading.Quaternion());
}

void ABSKestrelPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f) return;
    if (GetActorTransform().ContainsNaN() || FlightMovement->Velocity.ContainsNaN())
    {
        ResetFlight(LastSafeTransform);
        return;
    }
    if (Controller)
    {
        // Discard excess hitch time rather than allowing an unbounded collision sweep or catch-up loop.
        const float SimulatedTime = FMath::Min(DeltaSeconds, 0.125f);
        const int32 Steps = FMath::Clamp(FMath::CeilToInt(SimulatedTime / (1.0f / 60.0f)), 1, 8);
        for (int32 Step = 0; Step < Steps; ++Step) SimulateFlight(SimulatedTime / Steps);
        LastSafeTransform = GetActorTransform();
    }
    const float Thrust = FMath::Clamp(GetSpeed() / FMath::Max(1.0f, BoostSpeed), 0.0f, 1.0f);
    for (UMaterialInstanceDynamic* Material : EngineMaterials)
    {
        if (Material) Material->SetScalarParameterValue(TEXT("Glow"), 4.0f + Thrust * 14.0f);
    }
}
