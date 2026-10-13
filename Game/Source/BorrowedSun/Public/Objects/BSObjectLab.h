#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "BSObjectLab.generated.h"

class UBSCarryComponent;

/** Isolated interaction court. It does not load or mutate the expedition's mission checkpoint. */
UCLASS()
class BORROWEDSUN_API ABSObjectLabGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ABSObjectLabGameMode();
    virtual void StartPlay() override;
    FString Status;
};

UCLASS()
class BORROWEDSUN_API ABSObjectLabController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void PlayerTick(float Dt) override;
    virtual void ProcessPlayerInput(float Dt, bool Paused) override;
    virtual void FlushPressedKeys() override;
    UBSCarryComponent* Hand() const;
    bool bMenu = false;
    void Grab();
    void Throw();
    void Collect();
    void Save();
    void Load();
    void Menu();
private:
    bool bAutomated = false;
    void Yaw(float Value);
    void Pitch(float Value);
    void Jump();
    void Quit();
    void Say(const FString& Message);
};

UCLASS()
class BORROWEDSUN_API ABSObjectLabHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
