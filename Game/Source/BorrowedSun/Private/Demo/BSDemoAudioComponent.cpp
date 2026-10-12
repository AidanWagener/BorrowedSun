#include "Demo/BSDemoAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "Sound/SoundWaveProcedural.h"

namespace
{
    constexpr int32 SampleRate = 24000;
    constexpr int32 LoopSeconds = 8;
    constexpr int32 BytesPerSecond = SampleRate * sizeof(int16);
    constexpr int32 LowWaterBytes = BytesPerSecond / 4;
    constexpr double Tau = 6.28318530717958647692;

    int16 ToPCM(double Sample)
    {
        return static_cast<int16>(FMath::RoundToInt(FMath::Clamp(Sample, -1.0, 1.0) * 32767.0));
    }

    double Note(double Time, double Start, double Frequency, double Length)
    {
        const double Age = Time - Start;
        if (Age < 0.0 || Age >= Length)
        {
            return 0.0;
        }
        const double Attack = FMath::Clamp(Age / 0.025, 0.0, 1.0);
        const double Release = FMath::Clamp((Length - Age) / 0.10, 0.0, 1.0);
        const double Envelope = Attack * Release * FMath::Exp(-3.0 * Age);
        return Envelope * (FMath::Sin(Tau * Frequency * Age)
            + 0.18 * FMath::Sin(Tau * Frequency * 2.0 * Age));
    }

    USoundWaveProcedural* CreateWave(UObject* Owner, FName Name)
    {
        USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>(Owner, Name, RF_Transient);
        Wave->SetSampleRate(SampleRate);
        Wave->NumChannels = 1;
        Wave->Duration = INDEFINITELY_LOOPING_DURATION;
        // The component supplies the loop. Procedural underflow produces silence, not an end marker.
        Wave->bLooping = false;
        Wave->SoundGroup = SOUNDGROUP_Default;
        Wave->VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
        return Wave;
    }
}

UBSDemoAudioComponent::UBSDemoAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}

void UBSDemoAudioComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!GetWorld() || !GetOwner() || !GetWorld()->AllowAudioPlayback()
        || GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        SetComponentTickEnabled(false);
        return;
    }

    GenerateBuffers();
    AmbientWave = CreateWave(this, TEXT("DemoAmbientWave"));
    CueWave = CreateWave(this, TEXT("DemoCueWave"));
    AmbientAudio = CreatePlayer(AmbientWave, TEXT("DemoAmbientPlayer"), 0.28f);
    CueAudio = CreatePlayer(CueWave, TEXT("DemoCuePlayer"), 0.30f);
    QueueAmbientSecond();
    AmbientAudio->FadeIn(1.0f);
}

UAudioComponent* UBSDemoAudioComponent::CreatePlayer(USoundWaveProcedural* Wave, FName Name, float Volume)
{
    UAudioComponent* Player = NewObject<UAudioComponent>(GetOwner(), Name, RF_Transient);
    Player->bAutoActivate = false;
    Player->bAutoDestroy = false;
    Player->bStopWhenOwnerDestroyed = true;
    Player->bAllowSpatialization = false;
    Player->SetUISound(false);
    Player->SetVolumeMultiplier(Volume);
    Player->SetSound(Wave);
    Player->RegisterComponent();
    return Player;
}

void UBSDemoAudioComponent::GenerateBuffers()
{
    AmbientPCM.SetNumUninitialized(SampleRate * LoopSeconds);
    InteractionPCM.SetNumUninitialized(SampleRate);
    CompletionPCM.SetNumUninitialized(SampleRate);

    // Two passes over the same noise sequence settle the filter into its periodic state.
    // Harmonic frequencies and modulation periods also close on the eight-second boundary.
    double Wind = 0.0;
    for (int32 Pass = 0; Pass < 2; ++Pass)
    {
        FRandomStream Random(472913);
        for (int32 Index = 0; Index < AmbientPCM.Num(); ++Index)
        {
            Wind = 0.985 * Wind + 0.015 * (Random.FRand() * 2.0 - 1.0);
            if (Pass == 1)
            {
                const double Time = static_cast<double>(Index) / SampleRate;
                const double Swell = 0.72 + 0.18 * FMath::Sin(Tau * Time / LoopSeconds);
                const double Tone = 0.045 * FMath::Sin(Tau * 55.0 * Time)
                    + 0.022 * FMath::Sin(Tau * 82.5 * Time)
                    + 0.008 * FMath::Sin(Tau * 110.0 * Time);
                AmbientPCM[Index] = ToPCM(Swell * (Tone + 0.24 * Wind));
            }
        }
    }

    for (int32 Index = 0; Index < SampleRate; ++Index)
    {
        const double Time = static_cast<double>(Index) / SampleRate;
        InteractionPCM[Index] = ToPCM(0.22 * (Note(Time, 0.0, 261.6256, 0.28)
            + Note(Time, 0.18, 391.9954, 0.34)));
        CompletionPCM[Index] = ToPCM(0.20 * (Note(Time, 0.0, 261.6256, 0.32)
            + Note(Time, 0.20, 329.6276, 0.34)
            + Note(Time, 0.40, 523.2511, 0.38)));
    }
}

void UBSDemoAudioComponent::QueueAmbientSecond()
{
    if (!AmbientWave || AmbientPCM.Num() != SampleRate * LoopSeconds)
    {
        return;
    }
    AmbientWave->QueueAudio(reinterpret_cast<const uint8*>(AmbientPCM.GetData() + AmbientSampleOffset),
        BytesPerSecond);
    AmbientSampleOffset = (AmbientSampleOffset + SampleRate) % AmbientPCM.Num();
}

void UBSDemoAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (AmbientWave && AmbientWave->GetAvailableAudioByteCount() < LowWaterBytes)
    {
        // One copy at most per tick: the queued PCM stays below 1.25 seconds, even after a hitch.
        QueueAmbientSecond();
    }
    if (CueElapsed >= 0.0f && CueWave && CueAudio)
    {
        CueElapsed += DeltaTime;
        if (CueElapsed >= 1.2f && CueWave->GetAvailableAudioByteCount() == 0)
        {
            CueAudio->Stop();
            CueElapsed = -1.0f;
            if (bPendingCompletion)
            {
                bPendingCompletion = false;
                PlayCue(true);
            }
        }
    }
}

void UBSDemoAudioComponent::PlayCue(bool bSuccess)
{
    if (!CueWave || !CueAudio)
    {
        return;
    }
    if (CueElapsed >= 0.0f || CueWave->GetAvailableAudioByteCount() > 0)
    {
        // Coalesce rapid interactions; keep at most one completion request behind the current cue.
        bPendingCompletion |= bSuccess;
        return;
    }
    const TArray<int16>& PCM = bSuccess ? CompletionPCM : InteractionPCM;
    CueWave->QueueAudio(reinterpret_cast<const uint8*>(PCM.GetData()), BytesPerSecond);
    CueElapsed = 0.0f;
    CueAudio->Play();
}

void UBSDemoAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetComponentTickEnabled(false);
    for (UAudioComponent* Player : { AmbientAudio.Get(), CueAudio.Get() })
    {
        if (Player)
        {
            Player->Stop();
            Player->SetSound(nullptr);
            Player->DestroyComponent();
        }
    }
    // Reset only at teardown. ResetAudio is asynchronous and must not race a subsequent refill.
    if (AmbientWave) AmbientWave->ResetAudio();
    if (CueWave) CueWave->ResetAudio();
    AmbientAudio = nullptr;
    CueAudio = nullptr;
    AmbientWave = nullptr;
    CueWave = nullptr;
    AmbientPCM.Empty();
    InteractionPCM.Empty();
    CompletionPCM.Empty();
    AmbientSampleOffset = 0;
    CueElapsed = -1.0f;
    bPendingCompletion = false;
    Super::EndPlay(EndPlayReason);
}
