#include "Demo/BSDemoPlayerController.h"
#include "Demo/BSDemoGameMode.h"
#include "Demo/BSExplorerCharacter.h"
#include "Demo/BSKestrelPawn.h"
#include "Demo/BSDemoUserSettings.h"
#include "Objects/BSCarryComponent.h"
#include "Objects/BSHandProps.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

ABSDemoPlayerController::ABSDemoPlayerController()
{
    PrimaryActorTick.bCanEverTick=true;
    bShowMouseCursor=false;
}
void ABSDemoPlayerController::BeginPlay()
{
    Super::BeginPlay();
    bAutomatedInput=FParse::Param(FCommandLine::Get(),TEXT("BSDemoAutoTest"));
    if(auto* Settings=UBSDemoUserSettings::Get())
    {
        if(Settings->UpgradeCityVisualPreset())Settings->ApplySettings(false);
        LookSensitivity=Settings->GetLookSensitivity();
        MasterVolume=Settings->GetMasterVolume();
        bInvertLook=Settings->IsLookInverted();
    }
    SetInputMode(FInputModeGameOnly());
    if(auto* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        if(Mapping) Sub->AddMappingContext(Mapping,0);
}

void ABSDemoPlayerController::ProcessPlayerInput(float DeltaTime, bool bGamePaused)
{
    // The explicit QA harness drives gameplay methods. Desktop/controller input must
    // not move its camera or invoke save/menu actions while the machine is in use.
    if(!bAutomatedInput)Super::ProcessPlayerInput(DeltaTime,bGamePaused);
}

UInputAction* ABSDemoPlayerController::Axis(const TCHAR* Name,FKey Positive,FKey Negative,FKey Pad)
{
    auto* A=NewObject<UInputAction>(this,FName(Name));
    A->ValueType=EInputActionValueType::Axis1D;
    Actions.Add(A);
    if(Positive.IsValid()) Mapping->MapKey(A,Positive);
    if(Negative.IsValid()) Mapping->MapKey(A,Negative).Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
    if(Pad.IsValid())
    {
        auto& Key=Mapping->MapKey(A,Pad);
        Key.Modifiers.Add(NewObject<UInputModifierDeadZone>(Mapping));
    }
    return A;
}
UInputAction* ABSDemoPlayerController::Button(const TCHAR* Name,FKey Key,FKey Pad)
{
    auto* A=NewObject<UInputAction>(this,FName(Name));
    A->ValueType=EInputActionValueType::Boolean;
    Actions.Add(A);
    Mapping->MapKey(A,Key);
    if(Pad.IsValid()) Mapping->MapKey(A,Pad);
    return A;
}

void ABSDemoPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    auto* E=Cast<UEnhancedInputComponent>(InputComponent);
    if(!E) return;
    Mapping=NewObject<UInputMappingContext>(this,TEXT("DemoControls"));
    auto BindAxis=[&](UInputAction* A,void(ABSDemoPlayerController::*Fn)(const FInputActionValue&))
    {
        E->BindAction(A,ETriggerEvent::Triggered,this,Fn);
        E->BindAction(A,ETriggerEvent::Completed,this,Fn);
        E->BindAction(A,ETriggerEvent::Canceled,this,Fn);
    };
    BindAxis(Axis(TEXT("Forward"),EKeys::W,EKeys::S,EKeys::Gamepad_LeftY),&ThisClass::MoveForward);
    BindAxis(Axis(TEXT("Right"),EKeys::D,EKeys::A,EKeys::Gamepad_LeftX),&ThisClass::MoveRight);
    auto* Up=Axis(TEXT("Altitude"),EKeys::SpaceBar,EKeys::C,FKey());
    Mapping->MapKey(Up,EKeys::Gamepad_FaceButton_Bottom);
    Mapping->MapKey(Up,EKeys::Gamepad_FaceButton_Right).Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
    BindAxis(Up,&ThisClass::MoveUp);
    BindAxis(Axis(TEXT("MouseYaw"),EKeys::MouseX,FKey(),FKey()),&ThisClass::LookYaw);
    BindAxis(Axis(TEXT("MousePitch"),EKeys::MouseY,FKey(),FKey()),&ThisClass::LookPitch);
    BindAxis(Axis(TEXT("PadYaw"),FKey(),FKey(),EKeys::Gamepad_RightX),&ThisClass::PadYaw);
    BindAxis(Axis(TEXT("PadPitch"),FKey(),FKey(),EKeys::Gamepad_RightY),&ThisClass::PadPitch);
    BindAxis(Button(TEXT("Boost"),EKeys::LeftShift,EKeys::Gamepad_RightTrigger),&ThisClass::Boost);
    BindAxis(Button(TEXT("Brake"),EKeys::LeftControl,EKeys::Gamepad_LeftTrigger),&ThisClass::Brake);
    auto Bind=[&](const TCHAR* Name,FKey Key,FKey Pad,void(ABSDemoPlayerController::*Fn)())
    { E->BindAction(Button(Name,Key,Pad),ETriggerEvent::Started,this,Fn); };
    Bind(TEXT("Interact"),EKeys::E,EKeys::Gamepad_FaceButton_Left,&ThisClass::Interact);
    Bind(TEXT("Clinic"),EKeys::One,EKeys::Gamepad_LeftShoulder,&ThisClass::SelectClinic);
    Bind(TEXT("Evacuation"),EKeys::Two,EKeys::Gamepad_RightShoulder,&ThisClass::SelectEvacuation);
    Bind(TEXT("Confirm"),EKeys::Enter,EKeys::Gamepad_FaceButton_Bottom,&ThisClass::Confirm);
    Bind(TEXT("Menu"),EKeys::Escape,EKeys::Gamepad_Special_Right,&ThisClass::ToggleMenu);
    Bind(TEXT("Save"),EKeys::F5,FKey(),&ThisClass::Save);
    Bind(TEXT("Load"),EKeys::F9,FKey(),&ThisClass::Load);
    Bind(TEXT("Recover"),EKeys::R,EKeys::Gamepad_DPad_Down,&ThisClass::Recover);
    Bind(TEXT("Restart"),EKeys::N,FKey(),&ThisClass::Restart);
    Bind(TEXT("Quit"),EKeys::Q,FKey(),&ThisClass::Quit);
    Bind(TEXT("Help"),EKeys::F1,EKeys::Gamepad_Special_Left,&ThisClass::Help);
    Bind(TEXT("Guidance"),EKeys::G,EKeys::Gamepad_FaceButton_Top,&ThisClass::Guidance);
    Bind(TEXT("GrabProp"),EKeys::F,EKeys::Gamepad_DPad_Up,&ThisClass::GrabProp);
    Bind(TEXT("ThrowProp"),EKeys::LeftMouseButton,EKeys::Gamepad_DPad_Right,&ThisClass::ThrowProp);
    Bind(TEXT("CollectProp"),EKeys::Z,EKeys::Gamepad_DPad_Left,&ThisClass::CollectProp);
    BindAxis(Button(TEXT("RotateProp"),EKeys::RightMouseButton,EKeys::Gamepad_RightThumbstick),&ThisClass::RotateProp);
    Bind(TEXT("SensitivityDown"),EKeys::LeftBracket,FKey(),&ThisClass::SensitivityDown);
    Bind(TEXT("SensitivityUp"),EKeys::RightBracket,FKey(),&ThisClass::SensitivityUp);
    Bind(TEXT("Settings"),EKeys::O,FKey(),&ThisClass::Settings);
    Bind(TEXT("InvertLook"),EKeys::I,FKey(),&ThisClass::InvertLook);
    Bind(TEXT("VolumeDown"),EKeys::Hyphen,FKey(),&ThisClass::VolumeDown);
    Bind(TEXT("VolumeUp"),EKeys::Equals,FKey(),&ThisClass::VolumeUp);
    if(auto* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) Sub->AddMappingContext(Mapping,0);
}

void ABSDemoPlayerController::MoveForward(const FInputActionValue& V)
{
    if(IsOverlayOpen()) return;
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->MoveForward(V.Get<float>());
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->MoveForward(V.Get<float>());
}
void ABSDemoPlayerController::MoveRight(const FInputActionValue& V)
{
    if(IsOverlayOpen()) return;
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->MoveRight(V.Get<float>());
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->MoveRight(V.Get<float>());
}
void ABSDemoPlayerController::MoveUp(const FInputActionValue& V)
{
    if(IsOverlayOpen()) return;
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) { if(V.Get<float>()>0) P->Jump(); else P->StopJumping(); }
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->MoveUp(V.Get<float>());
}
void ABSDemoPlayerController::LookYaw(const FInputActionValue& V)
{
    if(IsOverlayOpen() || bGuidance) return;
    const float D=V.Get<float>()*LookSensitivity;
    if(bRotateProp&&Hand()&&Hand()->GetHeldObject()){Hand()->RotateHeld(D,0);return;}
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookYaw(D);
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->LookYaw(D);
}
void ABSDemoPlayerController::LookPitch(const FInputActionValue& V)
{
    if(IsOverlayOpen() || bGuidance) return;
    const float D=V.Get<float>()*LookSensitivity*(bInvertLook?1.f:-1.f);
    if(bRotateProp&&Hand()&&Hand()->GetHeldObject()){Hand()->RotateHeld(0,D);return;}
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookPitch(D);
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->LookPitch(D);
}
void ABSDemoPlayerController::PadYaw(const FInputActionValue& V)
{
    if(IsOverlayOpen() || bGuidance) return;
    const float D=V.Get<float>()*100*GetWorld()->GetDeltaSeconds();
    if(bRotateProp&&Hand()&&Hand()->GetHeldObject()){Hand()->RotateHeld(D,0);return;}
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookYaw(D);
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->LookYaw(D);
}
void ABSDemoPlayerController::PadPitch(const FInputActionValue& V)
{
    if(IsOverlayOpen() || bGuidance) return;
    const float D=V.Get<float>()*80*GetWorld()->GetDeltaSeconds()*(bInvertLook?-1.f:1.f);
    if(bRotateProp&&Hand()&&Hand()->GetHeldObject()){Hand()->RotateHeld(0,D);return;}
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookPitch(D);
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->LookPitch(D);
}
void ABSDemoPlayerController::Boost(const FInputActionValue& V)
{
    const bool B=!IsOverlayOpen() && V.Get<bool>();
    if(auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->SetSprinting(B);
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->SetBoosting(B);
}
void ABSDemoPlayerController::Brake(const FInputActionValue& V)
{
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->SetBraking(V.Get<bool>());
    if(V.Get<bool>()) bGuidance=false;
}

void ABSDemoPlayerController::PlayerTick(float Dt)
{
    Super::PlayerTick(Dt);
    auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>();
    auto* P=Cast<ABSKestrelPawn>(GetPawn());
    if(!P || !G) return;
    if(IsOverlayOpen()) { P->StopFlight(); return; }
    if(!bGuidance) return;
    if(G->Progress().Stage==EBSDemoStage::AtRelay) { bGuidance=false; P->StopFlight(); return; }
    const FVector Target=G->ObjectiveLocation();
    const FVector To=Target-P->GetActorLocation();
    const float Dist=To.Size();
    const bool Returning=G->Progress().Stage==EBSDemoStage::Returning;
    if(Dist < (Returning?850.f:1600.f))
    {
        bGuidance=false; P->StopFlight();
        if(Returning) G->Say(TEXT("KESTREL  //  Dock in range. Press E to set down and disembark."));
        return;
    }
    const FRotator Desired=To.Rotation();
    const FRotator Current=GetControlRotation();
    P->LookYaw(FMath::Clamp(FMath::FindDeltaAngleDegrees(Current.Yaw,Desired.Yaw),-70*Dt,70*Dt));
    P->LookPitch(FMath::Clamp(FMath::FindDeltaAngleDegrees(Current.Pitch,Desired.Pitch),-45*Dt,45*Dt));
    const float Facing=FVector::DotProduct(P->GetActorForwardVector(),To.GetSafeNormal());
    P->MoveForward(FMath::Clamp(Facing,0.f,1.f));
    P->MoveRight(0); P->MoveUp(0);
    P->SetBoosting(Dist>6500 && Facing>.9f);
    P->SetBraking(false);
}

void ABSDemoPlayerController::Confirm()
{
    if(bSettings) { bSettings=false; return; }
    if(bConfirmRestart)
    { if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->NewExpedition(this); bConfirmRestart=false; }
    bWelcome=false; bMenu=false;
    SetInputMode(FInputModeGameOnly());
}
void ABSDemoPlayerController::ToggleMenu()
{
    ReleaseProp();
    if(bSettings) { bSettings=false; return; }
    if(bConfirmRestart) { bConfirmRestart=false; return; }
    bWelcome=false; bMenu=!bMenu;
    bGuidance=false;
    if(auto* P=Cast<ABSKestrelPawn>(GetPawn())) P->StopFlight();
}
void ABSDemoPlayerController::Interact() { if(!IsOverlayOpen()) if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Interact(this); }
void ABSDemoPlayerController::SelectClinic() { if(!IsOverlayOpen()) if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Choose(this,EBSDemoChoice::Clinic); }
void ABSDemoPlayerController::SelectEvacuation() { if(!IsOverlayOpen()) if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Choose(this,EBSDemoChoice::Evacuation); }
void ABSDemoPlayerController::Save() { if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Save(this); }
void ABSDemoPlayerController::Load() { bGuidance=false; if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Load(this); }
void ABSDemoPlayerController::Recover() { bGuidance=false; if(!IsOverlayOpen()) if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>()) G->Recover(this); }
void ABSDemoPlayerController::Restart() { if(IsOverlayOpen() && !bSettings) bConfirmRestart=true; }
void ABSDemoPlayerController::Quit() { if(IsOverlayOpen()) UKismetSystemLibrary::QuitGame(this,this,EQuitPreference::Quit,false); }
void ABSDemoPlayerController::Help() { bShowHelp=!bShowHelp; }
void ABSDemoPlayerController::Guidance()
{
    if(IsOverlayOpen() || !Cast<ABSKestrelPawn>(GetPawn())) return;
    auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>();
    if(!G || (G->Progress().Stage!=EBSDemoStage::Outbound && G->Progress().Stage!=EBSDemoStage::Returning)) return;
    bGuidance=!bGuidance;
    if(!bGuidance) Cast<ABSKestrelPawn>(GetPawn())->StopFlight();
    G->Say(bGuidance?TEXT("KESTREL  //  Flight assist engaged. G or CTRL returns manual control."):TEXT("KESTREL  //  Manual flight."),5);
}
void ABSDemoPlayerController::SensitivityDown() { LookSensitivity=FMath::Max(.15f,LookSensitivity-.1f); SavePreferences(); }
void ABSDemoPlayerController::SensitivityUp() { LookSensitivity=FMath::Min(2.f,LookSensitivity+.1f); SavePreferences(); }
void ABSDemoPlayerController::Settings() { if(IsOverlayOpen() && !bConfirmRestart) bSettings=!bSettings; }
void ABSDemoPlayerController::InvertLook() { if(bSettings) { bInvertLook=!bInvertLook; SavePreferences(); } }
void ABSDemoPlayerController::VolumeDown() { if(bSettings) { MasterVolume=FMath::Max(0.f,MasterVolume-.1f); SavePreferences(); } }
void ABSDemoPlayerController::VolumeUp() { if(bSettings) { MasterVolume=FMath::Min(1.f,MasterVolume+.1f); SavePreferences(); } }
void ABSDemoPlayerController::SavePreferences()
{
    if(auto* Preferences=UBSDemoUserSettings::Get())
    {
        Preferences->SetLookSensitivity(LookSensitivity);
        Preferences->SetMasterVolume(MasterVolume);
        Preferences->SetLookInverted(bInvertLook);
        Preferences->SaveSettings();
    }
}

UBSCarryComponent* ABSDemoPlayerController::Hand() const
{ const auto* P=Cast<ABSExplorerCharacter>(GetPawn());return P?P->GetCarryComponent():nullptr; }
void ABSDemoPlayerController::ReleaseProp()
{ bRotateProp=false;if(Hand())Hand()->Release(false); }
void ABSDemoPlayerController::FlushPressedKeys()
{ ReleaseProp();Super::FlushPressedKeys(); }
void ABSDemoPlayerController::RotateProp(const FInputActionValue& V)
{ bRotateProp=!IsOverlayOpen()&&V.Get<bool>(); }
void ABSDemoPlayerController::GrabProp()
{
    if(IsOverlayOpen()||!Hand())return;
    if(Hand()->GetHeldObject()){ReleaseProp();return;}
    FString Error;
    if(!Hand()->TryGrab(Error))if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>())G->Say(Error,3);
}
void ABSDemoPlayerController::ThrowProp()
{ if(!IsOverlayOpen()&&Hand()){bRotateProp=false;Hand()->Release(true);} }
void ABSDemoPlayerController::CollectProp()
{
    if(IsOverlayOpen()||!Hand())return;
    FString Error;
    if(auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>())
        G->Say(Hand()->Collect(BSHandProps::Id(TEXT("collection"),TEXT("field_satchel")),10,Error)?TEXT("Collected and saved. Inventory browsing and retrieval are coming later."):Error,4);
}
