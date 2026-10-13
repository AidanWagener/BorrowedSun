#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSDemoSmokeRunner.generated.h"

class ABSDemoGameMode;
class ABSDemoPlayerController;

/** Explicit -BSDemoAutoTest only. Drives the real demo through both leases in isolated save slots. */
UCLASS()
class BORROWEDSUN_API ABSDemoSmokeRunner : public AActor
{
    GENERATED_BODY()

public:
    ABSDemoSmokeRunner();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    void Start();

private:
    enum class EPhase : uint8
    {
        Startup,
        RegionalGroundPause,
        WalkToWorkshop,
        HoldWorkshopProp,
        StreetStartPause,
        WalkToSeed,
        LeavePlinth,
        WalkToDock,
        FlyOutbound,
        RelayPause,
        FlyHome,
        LeaveDock,
        CrossStreet,
        WalkToService,
        CompletionPause,
        VerifyCheckpoint,
        RouteFinished,
        ArtTour
    };

    struct FCheck
    {
        FString Name;
        FString Details;
        bool bPassed = false;
        double Seconds = 0;
    };

    bool bStarted = false;
    bool bExplicitOptIn = false;
    bool bFinished = false;
    bool bScreenshots = false;
    bool bNullRHIRequested = false;
    bool bCanRender = false;
    bool bMeasureFrames = false;
    bool bFrameSampleOverflow = false;
    double PreviousFrameAt = 0;
    TArray<double> GroundFrameMs;
    TArray<double> FlightFrameMs;
    bool bHasWalkSample = false;
    int32 RouteIndex = 0;
    int32 StartupFrames = 0;
    int32 StartupVisualReadyFrames = 0;
    FIntPoint RequestedViewport = FIntPoint::ZeroValue;
    int32 ArtViewIndex = 0;
    bool bArtViewCaptured = false;
    TWeakObjectPtr<class ABSPhysicalObject> WorkshopProp;
    bool bWorkshopCaptured = false;
    UPROPERTY(Transient) TObjectPtr<class ACameraActor> ArtCamera;
    EPhase Phase = EPhase::Startup;
    double StartedAt = 0;
    double RouteStartedAt = 0;
    double PhaseStartedAt = 0;
    double LastWalkSampleAt = 0;
    FVector LastWalkSample = FVector::ZeroVector;
    FVector RegionalGroundProbe = FVector::ZeroVector;
    FString SaveNamespace;
    FString ReportDirectory;
    TArray<FCheck> Checks;

    void BeginRoute(ABSDemoGameMode* Mode, ABSDemoPlayerController* Controller);
    void SetPhase(EPhase NewPhase);
    bool WalkTo(ABSDemoPlayerController* Controller, const FVector& Target, float Tolerance, float DeltaSeconds);
    bool Check(const TCHAR* Name, bool bPassed, const FString& Details = FString());
    void Capture(const TCHAR* Milestone);
    void Finish(bool bSuccess);
    FString RouteLabel() const;
    FString FrameReport() const;
    void SetArtView(ABSDemoPlayerController* Controller);
};
