#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "BSDemoPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class BORROWEDSUN_API ABSDemoPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ABSDemoPlayerController();
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void PlayerTick(float DeltaTime) override;
    virtual void ProcessPlayerInput(float DeltaTime, bool bGamePaused) override;
    bool bWelcome = true;
    bool bMenu = false;
    bool bConfirmRestart = false;
    bool bShowHelp = false;
    bool bGuidance = false;
    bool bSettings = false;
    bool bInvertLook = false;
    float MasterVolume = 0.8f;
    float LookSensitivity = 0.65f;
    bool IsOverlayOpen() const { return bWelcome || bMenu; }
    void Confirm();
    void ToggleMenu();
private:
    bool bAutomatedInput = false;
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TArray<TObjectPtr<UInputAction>> Actions;
    UInputAction* Axis(const TCHAR* Name, FKey Positive, FKey Negative, FKey Gamepad);
    UInputAction* Button(const TCHAR* Name, FKey Key, FKey Gamepad);
    void MoveForward(const FInputActionValue& V);
    void MoveRight(const FInputActionValue& V);
    void MoveUp(const FInputActionValue& V);
    void LookYaw(const FInputActionValue& V);
    void LookPitch(const FInputActionValue& V);
    void PadYaw(const FInputActionValue& V);
    void PadPitch(const FInputActionValue& V);
    void Boost(const FInputActionValue& V);
    void Brake(const FInputActionValue& V);
    void Interact();
    void SelectClinic();
    void SelectEvacuation();
    void Save();
    void Load();
    void Recover();
    void Restart();
    void Quit();
    void Help();
    void Guidance();
    void SensitivityDown();
    void SensitivityUp();
    void Settings();
    void InvertLook();
    void VolumeDown();
    void VolumeUp();
    void SavePreferences();
};
