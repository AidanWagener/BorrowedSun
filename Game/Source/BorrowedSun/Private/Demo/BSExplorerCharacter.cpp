#include "Demo/BSExplorerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

ABSExplorerCharacter::ABSExplorerCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    BaseEyeHeight = 72.0f;
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(GetCapsuleComponent());
    ViewCamera->SetRelativeLocation(FVector(0.0, 0.0, 72.0));
    ViewCamera->bUsePawnControlRotation = true;
    ViewCamera->SetFieldOfView(80.0f);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->MaxWalkSpeed = WalkSpeed;
    Movement->JumpZVelocity = 480.0f;
    Movement->GravityScale = 1.6f;
    Movement->AirControl = 0.3f;
    Movement->MaxAcceleration = 2400.0f;
    Movement->BrakingDecelerationWalking = 2200.0f;
    Movement->MaxStepHeight = 45.0f;
    Movement->bOrientRotationToMovement = false;
}

void ABSExplorerCharacter::MoveForward(float Value)
{
    if (Controller && FMath::IsFinite(Value))
    {
        const FRotator Heading(0.0, Controller->GetControlRotation().Yaw, 0.0);
        AddMovementInput(Heading.Vector(), FMath::Clamp(Value, -1.0f, 1.0f));
    }
}

void ABSExplorerCharacter::MoveRight(float Value)
{
    if (Controller && FMath::IsFinite(Value))
    {
        const FRotator Heading(0.0, Controller->GetControlRotation().Yaw, 0.0);
        AddMovementInput(FRotationMatrix(Heading).GetUnitAxis(EAxis::Y), FMath::Clamp(Value, -1.0f, 1.0f));
    }
}

void ABSExplorerCharacter::LookYaw(float Degrees)
{
    if (Controller && FMath::IsFinite(Degrees))
    {
        FRotator View = Controller->GetControlRotation();
        View.Yaw = FMath::UnwindDegrees(View.Yaw + Degrees);
        View.Roll = 0.0;
        Controller->SetControlRotation(View);
    }
}

void ABSExplorerCharacter::LookPitch(float Degrees)
{
    if (Controller && FMath::IsFinite(Degrees))
    {
        FRotator View = Controller->GetControlRotation();
        View.Pitch = FMath::Clamp(FMath::UnwindDegrees(View.Pitch) + Degrees, -80.0, 80.0);
        View.Roll = 0.0;
        Controller->SetControlRotation(View);
    }
}

void ABSExplorerCharacter::SetSprinting(bool bEnabled)
{
    GetCharacterMovement()->MaxWalkSpeed = bEnabled ? SprintSpeed : WalkSpeed;
}

void ABSExplorerCharacter::UnPossessed()
{
    StopJumping();
    ConsumeMovementInputVector();
    GetCharacterMovement()->StopMovementImmediately();
    SetSprinting(false);
    Super::UnPossessed();
}
