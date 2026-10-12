#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSDemoAudioComponent.generated.h"

class UAudioComponent;
class USoundWaveProcedural;

/** Original, runtime-generated ambience and milestone tones; no imported audio assets. */
UCLASS(ClassGroup = (BorrowedSun))
class BORROWEDSUN_API UBSDemoAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBSDemoAudioComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Short two-note acknowledgment, or a rising completion cue. Rapid duplicates coalesce. */
    void PlayCue(bool bSuccess = false);

private:
    UPROPERTY(Transient) TObjectPtr<USoundWaveProcedural> AmbientWave;
    UPROPERTY(Transient) TObjectPtr<USoundWaveProcedural> CueWave;
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> AmbientAudio;
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> CueAudio;

    TArray<int16> AmbientPCM;
    TArray<int16> InteractionPCM;
    TArray<int16> CompletionPCM;
    int32 AmbientSampleOffset = 0;
    float CueElapsed = -1.0f;
    bool bPendingCompletion = false;

    void GenerateBuffers();
    void QueueAmbientSecond();
    UAudioComponent* CreatePlayer(USoundWaveProcedural* Wave, FName Name, float Volume);
};
