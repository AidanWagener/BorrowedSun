#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Demo/BSDemoProgress.h"
#include "BSDemoGameMode.generated.h"

class ABSExplorerCharacter;
class ABSKestrelPawn;
class ABSDemoWorld;
class UBSDemoProgressSubsystem;

UCLASS()
class BORROWEDSUN_API ABSDemoGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ABSDemoGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    void Interact(APlayerController* PC);
    void Choose(APlayerController* PC, EBSDemoChoice Choice);
    void Recover(APlayerController* PC);
    void Save(APlayerController* PC);
    void Load(APlayerController* PC);
    void NewExpedition(APlayerController* PC);
    bool IsFlying(const APlayerController* PC) const;
    bool IsAtRelay(const APlayerController* PC) const;
    FString Objective() const;
    FString InteractionPrompt(const APlayerController* PC) const;
    FVector ObjectiveLocation() const;
    FString ObjectiveLabel() const;
    const FBSDemoProgress& Progress() const;
    void Say(const FString& Text, float Seconds = 8.0f);
    FString Message;
    float MessageSeconds = 0;
    UPROPERTY() TObjectPtr<ABSKestrelPawn> Ship;
private:
    UPROPERTY() TObjectPtr<ABSExplorerCharacter> Ground;
    UPROPERTY() TObjectPtr<ABSDemoWorld> District;
    UPROPERTY() TObjectPtr<UBSDemoProgressSubsystem> State;
    UPROPERTY() TObjectPtr<class UBSDemoAudioComponent> Audio;
    float ReadinessDelay = 0.25f;
    float RelayRetryDelay = 0;
    bool bRestored = false;
    void RestorePose(APlayerController* PC);
    bool Board(APlayerController* PC);
    bool Disembark(APlayerController* PC);
    bool CanInteractAt(const APlayerController* PC, FVector Point, float Range) const;
    void RefreshConsequence();
};
