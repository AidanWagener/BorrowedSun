#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BSExplorerCharacter.generated.h"

class UCameraComponent;
class UBSCarryComponent;

/** First-person walking pawn; input bindings live in the demo player controller. */
UCLASS()
class BORROWEDSUN_API ABSExplorerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABSExplorerCharacter();
    virtual void UnPossessed() override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    /** Look values are degrees already scaled by the player controller. */
    void LookYaw(float Degrees);
    void LookPitch(float Degrees);
    void SetSprinting(bool bEnabled);
    UCameraComponent* GetViewCamera() const { return ViewCamera; }
    UBSCarryComponent* GetCarryComponent() const { return Carry; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Interaction")
    TObjectPtr<UBSCarryComponent> Carry;

    UPROPERTY(VisibleAnywhere, Category = "View")
    TObjectPtr<UCameraComponent> ViewCamera;

    UPROPERTY(EditDefaultsOnly, Category = "Walking", meta = (ClampMin = "1"))
    float WalkSpeed = 450.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Walking", meta = (ClampMin = "1"))
    float SprintSpeed = 780.0f;
};
