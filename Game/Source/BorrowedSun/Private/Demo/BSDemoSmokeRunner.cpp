#include "Demo/BSDemoSmokeRunner.h"

#include "Demo/BSDemoGameMode.h"
#include "Demo/BSDemoPlayerController.h"
#include "Demo/BSDemoProgress.h"
#include "Demo/BSDemoWorld.h"
#include "Demo/BSExplorerCharacter.h"
#include "Demo/BSKestrelPawn.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/HUD.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformTime.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"
#if WITH_EDITOR
#include "AssetCompilingManager.h"
#include "ShaderCompiler.h"
#endif

namespace
{
    bool IsSafeNamespace(const FString& Value)
    {
        if (Value.IsEmpty() || Value.Len() > 48) return false;
        for (const TCHAR Character : Value)
        {
            const bool bLetter = (Character >= TEXT('A') && Character <= TEXT('Z'))
                || (Character >= TEXT('a') && Character <= TEXT('z'));
            const bool bDigit = Character >= TEXT('0') && Character <= TEXT('9');
            if (!bLetter && !bDigit && Character != TEXT('_') && Character != TEXT('-')) return false;
        }
        return true;
    }

    FString JsonString(const FString& Value)
    {
        FString Result(TEXT("\""));
        for (const TCHAR Character : Value)
        {
            switch (Character)
            {
            case TEXT('"'): Result += TEXT("\\\""); break;
            case TEXT('\\'): Result += TEXT("\\\\"); break;
            case TEXT('\n'): Result += TEXT("\\n"); break;
            case TEXT('\r'): Result += TEXT("\\r"); break;
            case TEXT('\t'): Result += TEXT("\\t"); break;
            default:
                if (Character < 0x20) Result += FString::Printf(TEXT("\\u%04x"), static_cast<uint32>(Character));
                else Result.AppendChar(Character);
                break;
            }
        }
        Result += TEXT("\"");
        return Result;
    }
}

ABSDemoSmokeRunner::ABSDemoSmokeRunner()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    PrimaryActorTick.TickGroup = TG_PostPhysics;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ABSDemoSmokeRunner::BeginPlay()
{
    Super::BeginPlay();
    Start();
}

void ABSDemoSmokeRunner::Start()
{
    if (bStarted) return;
    bStarted = true;
    bExplicitOptIn = FParse::Param(FCommandLine::Get(), TEXT("BSDemoAutoTest"));
    if (!bExplicitOptIn)
    {
        SetActorTickEnabled(false);
        Destroy();
        return;
    }

    StartedAt = PhaseStartedAt = FPlatformTime::Seconds();
    ReportDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("DemoValidation"));
    bNullRHIRequested = FParse::Param(FCommandLine::Get(), TEXT("nullrhi"));
    bCanRender = FApp::CanEverRender();
    bScreenshots = FParse::Param(FCommandLine::Get(), TEXT("BSDemoScreenshots"));
    bMeasureFrames = FParse::Param(FCommandLine::Get(), TEXT("BSDemoMeasureFrames"));
    FParse::Value(FCommandLine::Get(), TEXT("-BSDemoSaveNamespace="), SaveNamespace, false);

    // Invalid namespace values are ignored by the save subsystem, so the harness must refuse to proceed.
    if (!Check(TEXT("IsolatedSaveNamespace"), IsSafeNamespace(SaveNamespace),
        TEXT("Requires 1-48 ASCII letters, digits, underscores or hyphens before any gameplay/save mutation."))) return;
    if (!Check(TEXT("RenderedRuntimeRequired"), bCanRender && !bNullRHIRequested && GetWorld() && GetWorld()->IsGameWorld(),
        TEXT("Checks FApp::CanEverRender and command-line NullRHI request; this is not a visual-quality assertion."))) return;
    if (bMeasureFrames && !Check(TEXT("FrameCaptureWithoutScreenshots"), !bScreenshots,
        TEXT("Capture screenshots separately so image readback does not distort the frame interval sample."))) return;
    int32 Width=0,Height=0;
    const bool WidthSet=FParse::Value(FCommandLine::Get(),TEXT("BSDemoQAWidth="),Width);
    const bool HeightSet=FParse::Value(FCommandLine::Get(),TEXT("BSDemoQAHeight="),Height);
    if(WidthSet||HeightSet)
    {
        if(!Check(TEXT("RequestedViewportValid"),WidthSet&&HeightSet&&Width>=640&&Width<=3840&&Height>=360&&Height<=2160&&GEngine&&GEngine->GetGameUserSettings()))return;
        RequestedViewport=FIntPoint(Width,Height);
        auto* Settings=GEngine->GetGameUserSettings();Settings->SetScreenResolution(RequestedViewport);
        Settings->SetFullscreenMode(EWindowMode::Windowed);Settings->ApplyResolutionSettings(true);
    }
    UE_LOG(LogTemp, Display, TEXT("BSDemoAutoTest: Starting isolated rendered routes, namespace=%s"), *SaveNamespace);
}

FString ABSDemoSmokeRunner::RouteLabel() const
{
    return RouteIndex == 0 ? TEXT("Clinic") : TEXT("Evacuation");
}

void ABSDemoSmokeRunner::SetPhase(EPhase NewPhase)
{
    Phase = NewPhase;
    PhaseStartedAt = FPlatformTime::Seconds();
    bHasWalkSample = false;
}

bool ABSDemoSmokeRunner::Check(const TCHAR* Name, bool bPassed, const FString& Details)
{
    Checks.Add({ FString::Printf(TEXT("%s.%s"), *RouteLabel(), Name), Details, bPassed,
        FPlatformTime::Seconds() - StartedAt });
    UE_LOG(LogTemp, Display, TEXT("BSDemoAutoTest: %s %s - %s"), bPassed ? TEXT("PASS") : TEXT("FAIL"), Name, *Details);
    if (!bPassed) Finish(false);
    return bPassed;
}

void ABSDemoSmokeRunner::BeginRoute(ABSDemoGameMode* Mode, ABSDemoPlayerController* Controller)
{
    Controller->Confirm();
    Controller->bGuidance = false;
    Mode->NewExpedition(Controller);
    UBSDemoProgressSubsystem* State = GetGameInstance()->GetSubsystem<UBSDemoProgressSubsystem>();
    if (!Check(TEXT("ResetCheckpoint"), State && State->LastError.IsEmpty(), State ? State->LastError : TEXT("Missing subsystem."))) return;
    const FBSDemoProgress& Progress = Mode->Progress();
    if (!Check(TEXT("FreshJourney"), Progress.Stage == EBSDemoStage::Explore && Progress.Revision == 0
        && Progress.Choice == EBSDemoChoice::None && !Progress.bSunseedOwned
        && Cast<ABSExplorerCharacter>(Controller->GetPawn()) != nullptr && !Controller->IsOverlayOpen())) return;
    RouteStartedAt = FPlatformTime::Seconds();
    SetPhase(bScreenshots?EPhase::StreetStartPause:EPhase::WalkToSeed);
}

bool ABSDemoSmokeRunner::WalkTo(ABSDemoPlayerController* Controller, const FVector& Target, float Tolerance, float DeltaSeconds)
{
    ABSExplorerCharacter* Character = Cast<ABSExplorerCharacter>(Controller->GetPawn());
    if (!Character)
    {
        Check(TEXT("WalkingPawnPresent"), false);
        return false;
    }
    const FVector Location = Character->GetActorLocation();
    FVector To = Target - Location;
    To.Z = 0;
    const float Distance = static_cast<float>(To.Size());
    if (Distance <= Tolerance)
    {
        Character->MoveForward(0);
        Character->MoveRight(0);
        return true;
    }

    const double Now = FPlatformTime::Seconds();
    if (!bHasWalkSample)
    {
        bHasWalkSample = true;
        LastWalkSample = Location;
        LastWalkSampleAt = Now;
    }
    else if (Now - LastWalkSampleAt > 6.0)
    {
        if (!Check(TEXT("WalkingMakesProgress"), FVector::DistSquared2D(Location, LastWalkSample) > 25.0,
            FString::Printf(TEXT("Distance to waypoint %.1f cm; position %s"), Distance, *Location.ToCompactString()))) return false;
        LastWalkSample = Location;
        LastWalkSampleAt = Now;
    }

    const FVector Direction = To.GetSafeNormal();
    const float CurrentYaw = static_cast<float>(Controller->GetControlRotation().Yaw);
    const float DesiredYaw = static_cast<float>(Direction.Rotation().Yaw);
    Character->LookYaw(FMath::Clamp(FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw), -120.f * DeltaSeconds, 120.f * DeltaSeconds));
    const float Pitch = static_cast<float>(FMath::UnwindDegrees(Controller->GetControlRotation().Pitch));
    Character->LookPitch(FMath::Clamp(-Pitch, -90.f * DeltaSeconds, 90.f * DeltaSeconds));
    const FRotator Heading(0, Controller->GetControlRotation().Yaw, 0);
    const float Scale = FMath::Clamp(Distance / 160.f, .2f, 1.f);
    Character->SetSprinting(false);
    Character->MoveForward(static_cast<float>(FVector::DotProduct(Heading.Vector(), Direction)) * Scale);
    Character->MoveRight(static_cast<float>(FVector::DotProduct(FRotationMatrix(Heading).GetUnitAxis(EAxis::Y), Direction)) * Scale);
    return false;
}

void ABSDemoSmokeRunner::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bExplicitOptIn || bFinished) return;
    const double Now = FPlatformTime::Seconds();
    if (Now - StartedAt > 300.0)
    {
        Check(TEXT("TotalTimeBudget"), false, TEXT("Exceeded 300 seconds."));
        return;
    }
    if (Phase != EPhase::Startup && Now - RouteStartedAt > 120.0)
    {
        Check(TEXT("RouteTimeBudget"), false, FString::Printf(TEXT("Exceeded 120 seconds in phase %d."), static_cast<int32>(Phase)));
        return;
    }

    ABSDemoGameMode* Mode = GetWorld()->GetAuthGameMode<ABSDemoGameMode>();
    ABSDemoPlayerController* Controller = Cast<ABSDemoPlayerController>(GetWorld()->GetFirstPlayerController());
    if (bMeasureFrames)
    {
        if (PreviousFrameAt > 0 && Now - StartedAt >= 5.0 && Mode && Controller && !Controller->IsOverlayOpen())
        {
            if (GroundFrameMs.Num() + FlightFrameMs.Num() < 120000)
                (Mode->IsFlying(Controller)?FlightFrameMs:GroundFrameMs).Add((Now-PreviousFrameAt)*1000.0);
            else bFrameSampleOverflow=true;
        }
        PreviousFrameAt=Now;
    }
    if (Phase == EPhase::Startup)
    {
        // Time spent loading does not count as rendered warmup for the first screenshot.
        if (++StartupFrames < 120 || Now - StartedAt < 2.0) return;
#if WITH_EDITOR
        // Editor gameplay can render fallback materials while asynchronous compilation runs.
        // Wait for the real assets and another settled second before the first art capture.
        if(bScreenshots)
        {
            if((GShaderCompilingManager&&GShaderCompilingManager->IsCompiling())||FAssetCompilingManager::Get().GetNumRemainingAssets()>0)
            {StartupVisualReadyFrames=0;return;}
            if(++StartupVisualReadyFrames<60)return;
        }
#endif
        if (!Mode || !Controller || !Controller->GetPawn() || !Mode->Ship || !GEngine || !GEngine->GameViewport)
        {
            if (Now - StartedAt > 15.0) Check(TEXT("RuntimeReady"), false, TEXT("Missing mode, controller, pawn, ship or game viewport."));
            return;
        }
        if (!Check(TEXT("RuntimeReady"), true)) return;
        const FIntPoint Actual=GEngine->GameViewport->Viewport?GEngine->GameViewport->Viewport->GetSizeXY():FIntPoint::ZeroValue;
        if(RequestedViewport.X>0&&!Check(TEXT("RequestedViewportApplied"),Actual==RequestedViewport,FString::Printf(TEXT("Requested %dx%d; actual %dx%d."),RequestedViewport.X,RequestedViewport.Y,Actual.X,Actual.Y)))return;
        TActorIterator<ABSDemoWorld> City(GetWorld());
        if (!Check(TEXT("CityArtReady"), City && City->HasCityArt(), TEXT("All 14 city meshes and seven PBR materials loaded in this runtime."))) return;
        if (!Check(TEXT("MarketArtReady"), City && City->HasMarketArt(), TEXT("All 30 market meshes and nine architectural materials loaded in this runtime."))) return;
        BeginRoute(Mode, Controller);
        return;
    }
    if (!Mode || !Controller || !Controller->GetPawn() || !Mode->Ship)
    {
        Check(TEXT("RuntimeRemainsValid"), false);
        return;
    }

    const EBSDemoChoice Lease = RouteIndex == 0 ? EBSDemoChoice::Clinic : EBSDemoChoice::Evacuation;
    const FVector Service = RouteIndex == 0 ? ABSDemoWorld::ClinicLocation() : ABSDemoWorld::EvacuationLocation();
    switch (Phase)
    {
    case EPhase::StreetStartPause:
        // Capture the real idle gameplay view after the opening dialogue naturally fades.
        if(Mode->MessageSeconds<=0)
        {
            const FVector P=Controller->GetPawn()->GetActorLocation();
            if(!Check(TEXT("IdleStartPose"),FVector::Dist2D(P,ABSDemoWorld::StartLocation())<5.f&&FMath::Abs(Controller->GetControlRotation().Yaw)<1.f,
                FString::Printf(TEXT("Idle pose %s; view %s"),*P.ToString(),*Controller->GetControlRotation().ToString())))return;
            Capture(TEXT("street-start"));
            SetPhase(EPhase::WalkToSeed);
        }
        break;
    case EPhase::WalkToSeed:
        if (WalkTo(Controller, ABSDemoWorld::SeedLocation() - FVector(280, 0, 0), 25.f, DeltaSeconds))
        {
            Mode->Interact(Controller);
            if (!Check(TEXT("AcquireThroughInteraction"), Mode->Progress().Stage == EBSDemoStage::Carrying
                && Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 1)) return;
            SetPhase(EPhase::LeavePlinth);
        }
        break;
    case EPhase::LeavePlinth:
        if (WalkTo(Controller, ABSDemoWorld::SeedLocation() - FVector(280, 250, 0), 30.f, DeltaSeconds))
            SetPhase(EPhase::WalkToDock);
        break;
    case EPhase::WalkToDock:
        if (WalkTo(Controller, ABSDemoWorld::DockLocation() + FVector(-600, -250, 0), 30.f, DeltaSeconds))
        {
            Mode->Interact(Controller);
            if (!Check(TEXT("BoardThroughInteraction"), Mode->IsFlying(Controller)
                && Mode->Progress().Stage == EBSDemoStage::Outbound && Mode->Progress().Revision == 2)) return;
            Controller->bGuidance = true;
            SetPhase(EPhase::FlyOutbound);
        }
        break;
    case EPhase::FlyOutbound:
        if (Mode->Progress().Stage == EBSDemoStage::AtRelay)
        {
            if (!Check(TEXT("ReachRelayByFlight"), Mode->IsFlying(Controller) && Mode->IsAtRelay(Controller)
                && Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 3)) return;
            Capture(TEXT("sky-relay"));
            SetPhase(EPhase::RelayPause);
        }
        else if (Mode->Progress().Stage != EBSDemoStage::Outbound)
            Check(TEXT("OutboundStageInvariant"), false);
        break;
    case EPhase::RelayPause:
        if (Now - PhaseStartedAt >= 1.0)
        {
            Mode->Choose(Controller, Lease);
            if (!Check(TEXT("AuthorizeSingleLease"), Mode->Progress().Stage == EBSDemoStage::Returning
                && Mode->Progress().Choice == Lease && Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 4)) return;
            Controller->bGuidance = true;
            SetPhase(EPhase::FlyHome);
        }
        break;
    case EPhase::FlyHome:
        if (FVector::Dist(Mode->Ship->GetActorLocation(), ABSDemoWorld::DockLocation()) < 1400.0 && Mode->Ship->GetSpeed() < 650.f)
        {
            Mode->Interact(Controller);
            if (!Check(TEXT("ReturnAndDisembark"), !Mode->IsFlying(Controller)
                && Cast<ABSExplorerCharacter>(Controller->GetPawn()) != nullptr
                && Mode->Progress().Stage == EBSDemoStage::Returning && Mode->Progress().bSunseedOwned)) return;
            SetPhase(EPhase::LeaveDock);
        }
        break;
    case EPhase::LeaveDock:
        if (WalkTo(Controller, ABSDemoWorld::DockLocation() + FVector(-650, -250, 0), 30.f, DeltaSeconds))
            SetPhase(EPhase::CrossStreet);
        break;
    case EPhase::CrossStreet:
        if (WalkTo(Controller, FVector(Service.X, -250, Service.Z), 30.f, DeltaSeconds))
            SetPhase(EPhase::WalkToService);
        break;
    case EPhase::WalkToService:
        if (WalkTo(Controller, Service - FVector(0, 310, 0), 25.f, DeltaSeconds))
        {
            Mode->Interact(Controller);
            if (!Check(TEXT("InstallThroughInteraction"), Mode->Progress().Stage == EBSDemoStage::Complete
                && Mode->Progress().Choice == Lease && !Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 5)) return;
            Capture(TEXT("service-complete"));
            SetPhase(EPhase::CompletionPause);
        }
        break;
    case EPhase::CompletionPause:
        if (Now - PhaseStartedAt >= 1.0)
        {
            Mode->Interact(Controller);
            if (!Check(TEXT("DuplicateInteractionHarmless"), Mode->Progress().Stage == EBSDemoStage::Complete
                && Mode->Progress().Choice == Lease && !Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 5)) return;
            SetPhase(EPhase::VerifyCheckpoint);
        }
        break;
    case EPhase::VerifyCheckpoint:
        {
            UBSDemoProgressSubsystem* State = GetGameInstance()->GetSubsystem<UBSDemoProgressSubsystem>();
            Mode->Save(Controller);
            if (!Check(TEXT("ManualCheckpointSaved"), State && State->LastError.IsEmpty(), State ? State->LastError : TEXT("Missing subsystem."))) return;
            // Explicit boolean confirms a real disk load; unchanged in-memory state alone cannot prove persistence.
            if (!Check(TEXT("CheckpointReadFromDisk"), State->LoadCheckpoint(), State->LastError)) return;
            Mode->Load(Controller);
            if (!Check(TEXT("CompletedLeaseSurvivesLoad"), State->LastError.IsEmpty()
                && Mode->Progress().SchemaVersion == 1 && Mode->Progress().Stage == EBSDemoStage::Complete
                && Mode->Progress().Choice == Lease && !Mode->Progress().bSunseedOwned && Mode->Progress().Revision == 5,
                State->LastError)) return;
            SetPhase(EPhase::RouteFinished);
        }
        break;
    case EPhase::RouteFinished:
        if (Now - PhaseStartedAt >= 1.0)
        {
            if (!Check(TEXT("RouteWithinBudget"), Now - RouteStartedAt <= 120.0)) return;
            if (RouteIndex == 0)
            {
                ++RouteIndex;
                BeginRoute(Mode, Controller);
            }
            else if(bScreenshots)
            {
                ArtCamera=GetWorld()->SpawnActor<ACameraActor>();
                if(!Check(TEXT("CityReviewCamera"),ArtCamera!=nullptr))return;
                if(auto* Hud=Controller->GetHUD())Hud->bShowHUD=false;
                SetArtView(Controller);
            }
            else Finish(true);
        }
        break;
    case EPhase::ArtTour:
        if(!bArtViewCaptured && Now-PhaseStartedAt>3.0)
        {
            const TCHAR* Names[]={TEXT("city-overview"),TEXT("market-arcade"),TEXT("lantern-keeper"),TEXT("dock-courtyard")};
            Capture(Names[ArtViewIndex]); bArtViewCaptured=true;
        }
        if(Now-PhaseStartedAt>3.6)
        {
            if(++ArtViewIndex>=4)Finish(true);
            else SetArtView(Controller);
        }
        break;
    default: break;
    }
}

void ABSDemoSmokeRunner::SetArtView(ABSDemoPlayerController* Controller)
{
    const FVector Eye[]={{-6500,-7900,5100},{-3120,-160,215},{11500,-6500,1600},{3700,-750,420}};
    const FVector Target[]={{0,400,1000},{-1800,-500,300},{15500,0,3000},{1200,800,250}};
    ArtCamera->SetActorLocation(Eye[ArtViewIndex]);
    ArtCamera->SetActorRotation((Target[ArtViewIndex]-Eye[ArtViewIndex]).Rotation());
    ArtCamera->GetCameraComponent()->SetFieldOfView(ArtViewIndex==0?68.f:74.f);
    Controller->SetViewTarget(ArtCamera); bArtViewCaptured=false; SetPhase(EPhase::ArtTour);
}

void ABSDemoSmokeRunner::Capture(const TCHAR* Milestone)
{
    if (!bScreenshots || !bCanRender || bNullRHIRequested) return;
    IFileManager::Get().MakeDirectory(*ReportDirectory, true);
    const FString Filename = ReportDirectory / FString::Printf(TEXT("%s-%s-%s.png"), *SaveNamespace, *RouteLabel(), Milestone);
    FScreenshotRequest::RequestScreenshot(Filename, true, false);
    UE_LOG(LogTemp, Display, TEXT("BSDemoAutoTest: Requested real viewport screenshot %s"), *Filename);
}

void ABSDemoSmokeRunner::Finish(bool bSuccess)
{
    if (bFinished || !bExplicitOptIn) return;
    bFinished = true;
    SetActorTickEnabled(false);
    const FString Bool = bSuccess ? TEXT("true") : TEXT("false");
    FString Report = FString::Printf(TEXT("{\n  \"schemaVersion\": 1,\n  \"namespace\": %s,\n  \"success\": %s,\n  \"elapsedSeconds\": %.3f,\n  \"canEverRender\": %s,\n  \"nullRHIRequested\": %s,\n  \"screenshotsRequested\": %s,\n  \"checks\": [\n"),
        *JsonString(SaveNamespace), *Bool, FPlatformTime::Seconds() - StartedAt,
        bCanRender ? TEXT("true") : TEXT("false"), bNullRHIRequested ? TEXT("true") : TEXT("false"), bScreenshots ? TEXT("true") : TEXT("false"));
    for (int32 Index = 0; Index < Checks.Num(); ++Index)
    {
        const FCheck& Entry = Checks[Index];
        Report += FString::Printf(TEXT("    {\"name\": %s, \"passed\": %s, \"elapsedSeconds\": %.3f, \"details\": %s}%s\n"),
            *JsonString(Entry.Name), Entry.bPassed ? TEXT("true") : TEXT("false"), Entry.Seconds, *JsonString(Entry.Details),
            Index + 1 < Checks.Num() ? TEXT(",") : TEXT(""));
    }
    Report += TEXT("  ]");
    if (bMeasureFrames) Report += TEXT(",\n  \"frameIntervals\": ") + FrameReport();
    Report += TEXT("\n}\n");
    IFileManager::Get().MakeDirectory(*ReportDirectory, true);
    const FString SafeReportName = IsSafeNamespace(SaveNamespace) ? SaveNamespace : TEXT("invalid-namespace");
    const FString ReportPath = ReportDirectory / FString::Printf(TEXT("Result-%s.json"), *SafeReportName);
    if (!FFileHelper::SaveStringToFile(Report, *ReportPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
    {
        UE_LOG(LogTemp, Error, TEXT("BSDemoAutoTest: Failed to write result %s"), *ReportPath);
        bSuccess = false;
    }
    UE_LOG(LogTemp, Display, TEXT("BSDemoAutoTest: %s. Report: %s"), bSuccess ? TEXT("PASS") : TEXT("FAIL"), *ReportPath);
    FPlatformMisc::RequestExitWithStatus(false, bSuccess ? 0 : 1);
}

FString ABSDemoSmokeRunner::FrameReport() const
{
    auto Summary=[](TArray<double> Samples)
    {
        if(Samples.IsEmpty()) return FString(TEXT("{\"sampleCount\":0}"));
        Samples.Sort();
        double Total=0;
        int32 Over33=0,Over50=0,Over100=0;
        for(double Ms:Samples) { Total+=Ms; Over33+=Ms>33.333; Over50+=Ms>50; Over100+=Ms>100; }
        auto Percentile=[&](double P) { return Samples[FMath::Clamp(FMath::CeilToInt(P*Samples.Num())-1,0,Samples.Num()-1)]; };
        return FString::Printf(TEXT("{\"sampleCount\":%d,\"sampleSeconds\":%.3f,\"meanMs\":%.3f,\"medianMs\":%.3f,\"p95Ms\":%.3f,\"p99Ms\":%.3f,\"maxMs\":%.3f,\"over33Ms\":%d,\"over50Ms\":%d,\"over100Ms\":%d}"),
            Samples.Num(),Total/1000.0,Total/Samples.Num(),Percentile(.5),Percentile(.95),Percentile(.99),Samples.Last(),Over33,Over50,Over100);
    };
    TArray<double> All=GroundFrameMs;
    All.Append(FlightFrameMs);
    const FIntPoint Size=GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport?GEngine->GameViewport->Viewport->GetSizeXY():FIntPoint::ZeroValue;
    const auto* Cap=IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
    return FString::Printf(TEXT("{\"method\":\"Wall-clock interval between rendered game ticks; includes pacing and scheduling, not isolated CPU or GPU cost\",\"warmupSeconds\":5,\"screenshotsExcluded\":true,\"overflow\":%s,\"width\":%d,\"height\":%d,\"frameCap\":%.1f,\"overall\":%s,\"onFoot\":%s,\"flight\":%s}"),
        bFrameSampleOverflow?TEXT("true"):TEXT("false"),Size.X,Size.Y,Cap?Cap->GetFloat():0.f,*Summary(All),*Summary(GroundFrameMs),*Summary(FlightFrameMs));
}
