#include "Demo/BSDemoGameMode.h"
#include "Demo/BSDemoWorld.h"
#include "Demo/BSExplorerCharacter.h"
#include "Demo/BSKestrelPawn.h"
#include "Demo/BSDemoPlayerController.h"
#include "Demo/BSDemoHUD.h"
#include "Demo/BSDemoSmokeRunner.h"
#include "Demo/BSDemoAudioComponent.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ABSDemoGameMode::ABSDemoGameMode()
{
    PrimaryActorTick.bCanEverTick=true;
    DefaultPawnClass=ABSExplorerCharacter::StaticClass();
    PlayerControllerClass=ABSDemoPlayerController::StaticClass();
    HUDClass=ABSDemoHUD::StaticClass();
    Audio=CreateDefaultSubobject<UBSDemoAudioComponent>(TEXT("DemoAudio"));
}

void ABSDemoGameMode::BeginPlay()
{
    Super::BeginPlay();
    State=GetGameInstance()->GetSubsystem<UBSDemoProgressSubsystem>();
    State->LoadCheckpoint();
    for(TActorIterator<ABSDemoWorld> It(GetWorld());It;++It) { District=*It; break; }
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Ship=GetWorld()->SpawnActor<ABSKestrelPawn>(ABSDemoWorld::DockLocation(),FRotator::ZeroRotator,Params);
    if(Ship) Ship->SetFlightBounds(FBox(FVector(-45000,-45000,220),FVector(45000,45000,28000)));
    else { Say(TEXT("Kestrel could not be created. Please relaunch the demo."),120); return; }
    Say(TEXT("IONA  //  A city that rents its daylight. One stolen sunseed. Two places that need it."),12);
    if(FParse::Param(FCommandLine::Get(),TEXT("BSDemoAutoTest")))
        GetWorld()->SpawnActor<ABSDemoSmokeRunner>();
}

const FBSDemoProgress& ABSDemoGameMode::Progress() const
{
    static const FBSDemoProgress Empty;
    return State?State->GetProgress():Empty;
}

void ABSDemoGameMode::Tick(float Dt)
{
    Super::Tick(Dt);
    MessageSeconds=FMath::Max(0.f,MessageSeconds-Dt);
    RelayRetryDelay=FMath::Max(0.f,RelayRetryDelay-Dt);
    auto* PC=GetWorld()->GetFirstPlayerController();
    if(!PC || !State || !Ship) return;
    if(!bRestored)
    {
        ReadinessDelay-=Dt;
        if(ReadinessDelay<=0 && PC->GetPawn()) { bRestored=true; RestorePose(PC); }
        return;
    }
    if(Progress().Stage==EBSDemoStage::Outbound && IsFlying(PC) && IsAtRelay(PC) && RelayRetryDelay<=0)
    {
        if(State->ApplyReachRelay())
        {
            Ship->StopFlight();
            if(Audio) Audio->PlayCue();
            Say(TEXT("VENN  //  Relay acquired. One lease only: [1] the clinic, or [2] the evacuation lift."),15);
        }
        else { RelayRetryDelay=8; Ship->StopFlight(); Say(State->LastError); }
    }
    if(PC->GetPawn() && PC->GetPawn()->GetActorLocation().Z < -500)
        Recover(PC);
}

bool ABSDemoGameMode::IsFlying(const APlayerController* PC) const { return PC && PC->GetPawn()==Ship; }
bool ABSDemoGameMode::IsAtRelay(const APlayerController* PC) const
{
    return PC && PC->GetPawn() && FVector::Dist(PC->GetPawn()->GetActorLocation(),ABSDemoWorld::RelayLocation())<2200;
}
bool ABSDemoGameMode::CanInteractAt(const APlayerController* PC,FVector Point,float Range) const
{
    return PC && PC->GetPawn() && FVector::Dist(PC->GetPawn()->GetActorLocation(),Point)<Range;
}

void ABSDemoGameMode::RestorePose(APlayerController* PC)
{
    if(!PC || !Ship) return;
    if(!Ground) Ground=Cast<ABSExplorerCharacter>(PC->GetPawn());
    if(!Ground)
    {
        FActorSpawnParameters P;
        P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Ground=GetWorld()->SpawnActor<ABSExplorerCharacter>(ABSDemoWorld::StartLocation(),FRotator::ZeroRotator,P);
    }
    if(!Ground) return;
    const EBSDemoStage Stage=Progress().Stage;
    if(Stage==EBSDemoStage::Outbound || Stage==EBSDemoStage::AtRelay || Stage==EBSDemoStage::Returning)
    {
        const bool AtSky=Stage==EBSDemoStage::AtRelay || Stage==EBSDemoStage::Returning;
        Ship->ResetFlight(FTransform(FRotator(0,AtSky?180:0,0),AtSky?ABSDemoWorld::RelayLocation():ABSDemoWorld::DockLocation()+FVector(0,0,600)));
        PC->Possess(Ship);
        Ground->SetActorHiddenInGame(true);
        Ground->SetActorEnableCollision(false);
        Ground->GetCharacterMovement()->DisableMovement();
        PC->SetControlRotation(Ship->GetActorRotation());
    }
    else
    {
        Ship->ResetFlight(FTransform(FRotator::ZeroRotator,ABSDemoWorld::DockLocation()));
        Ground->SetActorLocation(Stage==EBSDemoStage::Complete?ABSDemoWorld::DockLocation()+FVector(-600,0,-100):ABSDemoWorld::StartLocation(),false,nullptr,ETeleportType::TeleportPhysics);
        Ground->SetActorHiddenInGame(false);
        Ground->SetActorEnableCollision(true);
        Ground->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        PC->Possess(Ground);
        PC->SetControlRotation(FRotator::ZeroRotator);
    }
    RefreshConsequence();
}

bool ABSDemoGameMode::Board(APlayerController* PC)
{
    if(!Ship || !PC || !CanInteractAt(PC,Ship->GetActorLocation(),850)) return false;
    Ground=Cast<ABSExplorerCharacter>(PC->GetPawn());
    if(!Ground) return false;
    if(Progress().Stage==EBSDemoStage::Explore) { Say(TEXT("IONA  //  The sunseed is still on the plinth. Take it before we leave.")); return false; }
    PC->Possess(Ship);
    if(PC->GetPawn()!=Ship) return false;
    if(Progress().Stage==EBSDemoStage::Carrying && !State->ApplyDepart())
    {
        PC->Possess(Ground); Say(State->LastError); return false;
    }
    Ground->SetActorHiddenInGame(true);
    Ground->SetActorEnableCollision(false);
    Ground->GetCharacterMovement()->DisableMovement();
    PC->SetControlRotation(Ship->GetActorRotation());
    Say(TEXT("KESTREL  //  W/S thrust. A/D strafe. SPACE/C altitude. SHIFT boost. CTRL brake. G toggles flight assist."),12);
    return true;
}

bool ABSDemoGameMode::Disembark(APlayerController* PC)
{
    if(!PC || !Ship || !Ground || !IsFlying(PC)) return false;
    if(FVector::Dist(Ship->GetActorLocation(),ABSDemoWorld::DockLocation())>1400 || Ship->GetSpeed()>650)
    { Say(TEXT("KESTREL  //  Return to the marked quay dock and brake before leaving the cockpit.")); return false; }
    Ship->ResetFlight(FTransform(FRotator::ZeroRotator,ABSDemoWorld::DockLocation()));
    Ground->SetActorLocation(ABSDemoWorld::DockLocation()+FVector(-650,0,-100),false,nullptr,ETeleportType::TeleportPhysics);
    Ground->SetActorHiddenInGame(false);
    Ground->SetActorEnableCollision(true);
    Ground->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    PC->Possess(Ground);
    if(PC->GetPawn()!=Ground) return false;
    PC->SetControlRotation(FRotator(0,90,0));
    if(auto* DemoPC=Cast<ABSDemoPlayerController>(PC)) DemoPC->bGuidance=false;
    Say(Progress().Stage==EBSDemoStage::Returning?TEXT("IONA  //  You're home. Install the seed at the service you chose. Let them see the light."):TEXT("KESTREL  //  Dock secure. Your expedition can continue whenever you're ready."));
    return true;
}

void ABSDemoGameMode::Interact(APlayerController* PC)
{
    if(!State || !PC || !PC->GetPawn() || !Ship) return;
    if(IsFlying(PC)) { Disembark(PC); return; }
    if(Progress().Stage==EBSDemoStage::Explore && CanInteractAt(PC,ABSDemoWorld::SeedLocation(),350))
    {
        if(State->ApplyAcquire()) { if(Audio) Audio->PlayCue(); RefreshConsequence(); Say(TEXT("IONA  //  The sunseed is yours. Kestrel is on the amber dock. The sky relay can authorize its light."),12); }
        else Say(State->LastError);
        return;
    }
    if(Progress().Stage==EBSDemoStage::Returning)
    {
        const FVector Service=Progress().Choice==EBSDemoChoice::Clinic?ABSDemoWorld::ClinicLocation():ABSDemoWorld::EvacuationLocation();
        if(CanInteractAt(PC,Service,420))
        {
            if(State->ApplyComplete())
            {
                RefreshConsequence();
                if(Audio) Audio->PlayCue(true);
                Say(Progress().Choice==EBSDemoChoice::Clinic?TEXT("IONA  //  The clinic has light again. We can keep them alive tonight. The lift will have to wait."):TEXT("SELI  //  The lift is moving. Those families get a tomorrow. Iona will do what she can in the dark."),20);
            }
            else Say(State->LastError);
            return;
        }
    }
    if(CanInteractAt(PC,Ship->GetActorLocation(),850)) { Board(PC); return; }
    Say(TEXT("Move closer to the marked objective. E interacts; F1 shows controls."),4);
}

void ABSDemoGameMode::Choose(APlayerController* PC,EBSDemoChoice Choice)
{
    if(Progress().Stage!=EBSDemoStage::AtRelay || !IsFlying(PC) || !IsAtRelay(PC)) return;
    if(State->ApplyChoose(Choice))
    {
        if(Audio) Audio->PlayCue();
        Say(Choice==EBSDemoChoice::Clinic?TEXT("VENN  //  Clinic lease signed. Return to the quay and bring Iona her daylight."):TEXT("VENN  //  Evacuation lease signed. Return to the quay. Those families are counting on you."),12);
    }
    else Say(State->LastError);
}

void ABSDemoGameMode::Recover(APlayerController* PC)
{
    if(!PC || !Ship) return;
    if(IsFlying(PC))
    {
        const bool Sky=Progress().Stage==EBSDemoStage::AtRelay;
        Ship->ResetFlight(FTransform(FRotator::ZeroRotator,Sky?ABSDemoWorld::RelayLocation():ABSDemoWorld::DockLocation()+FVector(0,0,500)));
        PC->SetControlRotation(FRotator::ZeroRotator);
        if(auto* D=Cast<ABSDemoPlayerController>(PC)) D->bGuidance=false;
    }
    else if(PC->GetPawn())
        PC->GetPawn()->SetActorLocation(ABSDemoWorld::StartLocation(),false,nullptr,ETeleportType::TeleportPhysics);
    Say(TEXT("KESTREL  //  Safe position restored. Your cargo and mission choice are unchanged."));
}

void ABSDemoGameMode::Save(APlayerController*)
{
    if(State) Say(State->SaveCheckpoint()?TEXT("CHECKPOINT SAVED  //  Reload resumes at the safe start of this mission stage."):State->LastError);
}
void ABSDemoGameMode::Load(APlayerController* PC)
{
    if(State && State->LoadCheckpoint()) { RestorePose(PC); Say(State->LastError.IsEmpty()?TEXT("CHECKPOINT RESTORED  //  Resuming the saved mission stage."):State->LastError); }
    else if(State) Say(State->LastError);
}
void ABSDemoGameMode::NewExpedition(APlayerController* PC)
{
    if(!State) return;
    State->ResetProgress();
    RestorePose(PC);
    if(!State->LastError.IsEmpty()) { Say(State->LastError); return; }
    Say(TEXT("IONA  //  One sunseed. Two promises. Take it from the plinth and find Kestrel."),12);
}
void ABSDemoGameMode::RefreshConsequence()
{
    if(District)
    {
        District->SetSeedAvailable(Progress().Stage==EBSDemoStage::Explore);
        District->ApplyConsequence(static_cast<uint8>(Progress().Choice),Progress().Stage==EBSDemoStage::Complete);
    }
}

FString ABSDemoGameMode::Objective() const
{
    switch(Progress().Stage)
    {
    case EBSDemoStage::Explore:return TEXT("Recover the sunseed from the market plinth.");
    case EBSDemoStage::Carrying:return TEXT("Board Kestrel at dock 03.");
    case EBSDemoStage::Outbound:return TEXT("Fly through the beacons to the sky relay.");
    case EBSDemoStage::AtRelay:return TEXT("Authorize one lease: clinic [1] or evacuation [2].");
    case EBSDemoStage::Returning:return TEXT("Return to the quay. Install the seed at your chosen service.");
    case EBSDemoStage::Complete:return TEXT("Expedition complete. Your choice is saved. ESC opens the menu.");
    }
    return FString();
}
FVector ABSDemoGameMode::ObjectiveLocation() const
{
    switch(Progress().Stage)
    {
    case EBSDemoStage::Explore:return ABSDemoWorld::SeedLocation();
    case EBSDemoStage::Carrying:return Ship?Ship->GetActorLocation():ABSDemoWorld::DockLocation();
    case EBSDemoStage::Outbound: case EBSDemoStage::AtRelay:return ABSDemoWorld::RelayLocation();
    case EBSDemoStage::Returning:
        if(IsFlying(GetWorld()->GetFirstPlayerController())) return ABSDemoWorld::DockLocation();
        return Progress().Choice==EBSDemoChoice::Clinic?ABSDemoWorld::ClinicLocation():ABSDemoWorld::EvacuationLocation();
    default:return Progress().Choice==EBSDemoChoice::Clinic?ABSDemoWorld::ClinicLocation():ABSDemoWorld::EvacuationLocation();
    }
}
FString ABSDemoGameMode::ObjectiveLabel() const
{
    switch(Progress().Stage)
    {
    case EBSDemoStage::Explore:return TEXT("SUNSEED");
    case EBSDemoStage::Carrying:return TEXT("KESTREL / DOCK 03");
    case EBSDemoStage::Outbound:case EBSDemoStage::AtRelay:return TEXT("SKY RELAY");
    case EBSDemoStage::Returning:
        if(IsFlying(GetWorld()->GetFirstPlayerController())) return TEXT("HOME / QUAY DOCK");
        break;
    default:break;
    }
    return Progress().Choice==EBSDemoChoice::Clinic?TEXT("IONA'S CLINIC"):TEXT("EVACUATION LIFT");
}
FString ABSDemoGameMode::InteractionPrompt(const APlayerController* PC) const
{
    if(!PC || !PC->GetPawn()) return FString();
    if(IsFlying(PC))
    {
        if(Progress().Stage==EBSDemoStage::AtRelay && IsAtRelay(PC)) return TEXT("[1 / LB]  CLINIC        [2 / RB]  EVACUATION");
        if(FVector::Dist(Ship->GetActorLocation(),ABSDemoWorld::DockLocation())<1400)
            return Ship->GetSpeed()<650?TEXT("[E / X]  DOCK AND DISEMBARK"):TEXT("[CTRL / LT]  BRAKE TO DOCK");
        return FString();
    }
    if(Progress().Stage==EBSDemoStage::Explore && CanInteractAt(PC,ABSDemoWorld::SeedLocation(),350)) return TEXT("[E / X]  TAKE THE SUNSEED");
    if(Progress().Stage==EBSDemoStage::Returning && CanInteractAt(PC,ObjectiveLocation(),420)) return TEXT("[E / X]  INSTALL SUNSEED / RESTORE LIGHT");
    if(Ship && CanInteractAt(PC,Ship->GetActorLocation(),850)) return TEXT("[E / X]  BOARD KESTREL");
    return FString();
}
void ABSDemoGameMode::Say(const FString& Text,float Seconds)
{
    Message=Text; MessageSeconds=Seconds;
    UE_LOG(LogTemp,Display,TEXT("BSDemo: %s"),*Text);
}
