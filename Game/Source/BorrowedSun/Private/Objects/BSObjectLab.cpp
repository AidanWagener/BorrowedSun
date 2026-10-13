#include "Objects/BSObjectLab.h"
#include "Objects/BSPhysicalObject.h"
#include "Objects/BSCarryComponent.h"
#include "Demo/BSExplorerCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace BSObjectLabPrivate
{
    FBSStableId Id(const TCHAR* Space, const FString& Key)
    { FBSStableId Value; Value.Namespace=Space; Value.Key=Key; return Value; }
}

ABSObjectLabGameMode::ABSObjectLabGameMode()
{
    DefaultPawnClass=ABSExplorerCharacter::StaticClass(); PlayerControllerClass=ABSObjectLabController::StaticClass();
    HUDClass=ABSObjectLabHUD::StaticClass();
}

void ABSObjectLabGameMode::StartPlay()
{
    UStaticMesh* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Wood=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/BorrowedSun/Market/Materials/M_WorkshopWood.M_WorkshopWood"));
    auto* Enamel=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/BorrowedSun/Market/Materials/M_WorkshopEnamel.M_WorkshopEnamel"));
    auto* Ivory=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/BorrowedSun/Market/Materials/M_WorkshopIvory.M_WorkshopIvory"));
    auto Solid=[&](FVector Position,FVector Size,UMaterialInterface* Material)
    {
        auto* Actor=GetWorld()->SpawnActor<AStaticMeshActor>(Position,FRotator::ZeroRotator);
        auto* Body=Actor->GetStaticMeshComponent(); Body->SetMobility(EComponentMobility::Movable);
        Body->SetStaticMesh(Cube); Body->SetMaterial(0,Material); Body->SetCollisionProfileName(TEXT("BlockAll"));
        Actor->SetActorScale3D(Size/100); Body->SetMobility(EComponentMobility::Static);
    };
    Solid(FVector(0,0,-20),FVector(1200,1400,40),Ivory);
    Solid(FVector(0,85,86),FVector(700,290,8),Wood);
    for (double X:{-320.,320.}) for (double Y:{-35.,205.}) Solid(FVector(X,Y,41),FVector(14,14,82),Enamel);
    for (double X:{-600.,600.}) Solid(FVector(X,0,125),FVector(20,1400,250),Enamel);
    Solid(FVector(0,700,125),FVector(1200,20,250),Enamel);
    Solid(FVector(0,-700,60),FVector(1200,20,120),Enamel);
    // Shelves, stairs and a narrow opening provide real collision situations for carrying.
    for (double Z:{70.,140.,210.}) Solid(FVector(400,400,Z),FVector(240,70,6),Wood);
    for (int32 I=0;I<5;++I) Solid(FVector(-430,250+I*45,10+I*10),FVector(150,45,20+I*20),Ivory);
    for (double X:{-70.,70.}) Solid(FVector(X,400,120),FVector(50,20,240),Enamel);
    Solid(FVector(0,400,230),FVector(190,20,20),Enamel);
    auto* Store=GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>(); FString Error;
    FString Namespace=TEXT("ObjectLab_v1"); FParse::Value(FCommandLine::Get(),TEXT("BSObjectNamespace="),Namespace);
    if (!Store->Open(BSObjectLabPrivate::Id(TEXT("orison"),TEXT("object_lab_v1")),Namespace,
        FBox(FVector(-580,-680,-180),FVector(580,680,900)),Error)) Status=Error;
    else
    {
        const TCHAR* Names[]={TEXT("Cup"),TEXT("Bowl"),TEXT("Lantern"),TEXT("Tool"),TEXT("Cell"),TEXT("Crate"),TEXT("Basket"),TEXT("Bottle"),TEXT("Book"),TEXT("Bundle")};
        const TCHAR* Labels[]={TEXT("Enamel tea cup"),TEXT("Glazed sharing bowl"),TEXT("Caged service lantern"),TEXT("Insulated service spanner"),TEXT("Workshop power cell"),TEXT("Strapped parts crate"),TEXT("Woven parts basket"),TEXT("Ceramic flask"),TEXT("Repair ledger"),TEXT("Tied cloth parcel")};
        const float Masses[]={.28f,.65f,1.1f,.55f,.8f,3.2f,.45f,.9f,.7f,1.4f};
        for (int32 I=0;I<10;++I)
        {
            const FString Key=FString(Names[I]).ToLower(); const FString MeshName=TEXT("SM_Hand")+FString(Names[I]);
            FBSPhysicalObjectPlacement P; P.InstanceId=BSObjectLabPrivate::Id(TEXT("object_lab"),Key);
            P.DefinitionId=BSObjectLabPrivate::Id(TEXT("hand_prop"),Key); P.DisplayName=Labels[I]; P.MassKg=Masses[I];
            P.Mesh=LoadObject<UStaticMesh>(nullptr,*(TEXT("/Game/BorrowedSun/Interaction/Meshes/")+MeshName+TEXT(".")+MeshName));
            P.Transform.SetLocation(FVector(-240+(I%5)*120,(I/5)*150,94));
            Store->RegisterPlacement(P,Error); if (!Error.IsEmpty()) { Status=Error; break; }
        }
        if (Status.IsEmpty()) Status=TEXT("Ten ordinary props. Try the table, shelves, stairs and narrow doorway.");
    }
    Super::StartPlay();
}

void ABSObjectLabController::BeginPlay()
{
    Super::BeginPlay(); SetInputMode(FInputModeGameOnly()); bShowMouseCursor=false;
    bAutomated=FParse::Param(FCommandLine::Get(),TEXT("BSObjectLabAutoTest"));
}

void ABSObjectLabController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::F,IE_Pressed,this,&ThisClass::Grab);
    InputComponent->BindKey(EKeys::LeftMouseButton,IE_Pressed,this,&ThisClass::Throw);
    InputComponent->BindKey(EKeys::Z,IE_Pressed,this,&ThisClass::Collect);
    InputComponent->BindKey(EKeys::F5,IE_Pressed,this,&ThisClass::Save);
    InputComponent->BindKey(EKeys::F9,IE_Pressed,this,&ThisClass::Load);
    InputComponent->BindKey(EKeys::SpaceBar,IE_Pressed,this,&ThisClass::Jump);
    InputComponent->BindKey(EKeys::Escape,IE_Pressed,this,&ThisClass::Menu).bExecuteWhenPaused=true;
    InputComponent->BindKey(EKeys::Q,IE_Pressed,this,&ThisClass::Quit).bExecuteWhenPaused=true;
    InputComponent->BindAxisKey(EKeys::MouseX,this,&ThisClass::Yaw);
    InputComponent->BindAxisKey(EKeys::MouseY,this,&ThisClass::Pitch);
}

UBSCarryComponent* ABSObjectLabController::Hand() const
{ const auto* P=Cast<ABSExplorerCharacter>(GetPawn()); return P?P->GetCarryComponent():nullptr; }
void ABSObjectLabController::Say(const FString& Text)
{ if (auto* G=GetWorld()->GetAuthGameMode<ABSObjectLabGameMode>()) G->Status=Text; }
void ABSObjectLabController::ProcessPlayerInput(float Dt,bool Paused)
{ if (!bAutomated) Super::ProcessPlayerInput(Dt,Paused); }
void ABSObjectLabController::PlayerTick(float Dt)
{
    Super::PlayerTick(Dt);
    if (bMenu || bAutomated) return;
    if (auto* P=Cast<ABSExplorerCharacter>(GetPawn()))
    {
        P->MoveForward(float(IsInputKeyDown(EKeys::W))-float(IsInputKeyDown(EKeys::S)));
        P->MoveRight(float(IsInputKeyDown(EKeys::D))-float(IsInputKeyDown(EKeys::A)));
        P->SetSprinting(IsInputKeyDown(EKeys::LeftShift));
    }
}
void ABSObjectLabController::Yaw(float V)
{
    if (bMenu) return;
    if (Hand() && Hand()->GetHeldObject() && IsInputKeyDown(EKeys::RightMouseButton)) Hand()->RotateHeld(V*.5f,0);
    else if (auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookYaw(V*.2f);
}
void ABSObjectLabController::Pitch(float V)
{
    if (bMenu) return;
    if (Hand() && Hand()->GetHeldObject() && IsInputKeyDown(EKeys::RightMouseButton)) Hand()->RotateHeld(0,-V*.5f);
    else if (auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->LookPitch(-V*.2f);
}
void ABSObjectLabController::Grab()
{
    if (bMenu || !Hand()) return;
    if (Hand()->GetHeldObject()) { Hand()->Release(false); Say(TEXT("Released. The resting position saves after the prop settles.")); }
    else { FString Error; Say(Hand()->TryGrab(Error)?TEXT("Holding: right mouse + move mouse to rotate; left click to throw."):Error); }
}
void ABSObjectLabController::Throw() { if (!bMenu && Hand()) Hand()->Release(true); }
void ABSObjectLabController::Collect()
{
    if (bMenu || !Hand()) return; FString Error;
    Say(Hand()->Collect(BSObjectLabPrivate::Id(TEXT("collection"),TEXT("lab_bin")),10,Error)
        ?TEXT("Collected into the ten-place test bin. This is the inventory integration stub."):Error);
}
void ABSObjectLabController::Save()
{
    if (bMenu) return; if (Hand()) Hand()->Release(false); FString Error;
    auto* Store=GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>();
    Say(Store->SaveSettledObjects(Error)?TEXT("Saved resting props. Moving props retain their last safe resting pose."):Error);
}
void ABSObjectLabController::Load()
{
    if (bMenu) return; if (Hand()) Hand()->Release(false); FString Error;
    Say(GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>()->Reload(Error)?TEXT("Restored object positions and collection custody."):Error);
}
void ABSObjectLabController::Menu()
{
    if (Hand()) Hand()->Release(false); bMenu=!bMenu;
    if (auto* P=Cast<ABSExplorerCharacter>(GetPawn())) { P->ConsumeMovementInputVector(); P->StopJumping(); P->GetCharacterMovement()->StopMovementImmediately(); }
    SetPause(bMenu);
}
void ABSObjectLabController::FlushPressedKeys()
{ if (Hand()) Hand()->Release(false); Super::FlushPressedKeys(); }
void ABSObjectLabController::Jump() { if (!bMenu) if (auto* P=Cast<ABSExplorerCharacter>(GetPawn())) P->Jump(); }
void ABSObjectLabController::Quit() { if (bMenu) ConsoleCommand(TEXT("quit")); }

void ABSObjectLabHUD::DrawHUD()
{
    Super::DrawHUD(); if (!Canvas || !GEngine) return;
    auto* PC=Cast<ABSObjectLabController>(GetOwningPlayerController()); auto* G=GetWorld()->GetAuthGameMode<ABSObjectLabGameMode>();
    if (!PC || !G) return;
    const float S=FMath::Min(Canvas->SizeX/1280.f,Canvas->SizeY/720.f);
    auto Text=[&](const FString& T,float X,float Y,FLinearColor C,float Size=1.f)
    { DrawText(T,C,X*S,Y*S,GEngine->GetMediumFont(),S*Size,false); };
    DrawRect(FLinearColor(.012f,.03f,.035f,.88f),20*S,20*S,680*S,90*S);
    Text(TEXT("BORROWED SUN  /  SECOND DAWN OBJECT COURT"),36,34,FLinearColor(.95f,.7f,.3f),1.3f);
    Text(TEXT("F  pick up / drop     LMB  throw     RMB + mouse  rotate     Z  collect"),36,69,FLinearColor::White);
    Text(TEXT("WASD  walk   Shift  sprint   Space  jump   F5  save   F9  load   Esc  menu"),36,91,FLinearColor(.65f,.8f,.8f));
    if (PC->bMenu)
    {
        DrawRect(FLinearColor(0,0,0,.75f),0,0,Canvas->SizeX,Canvas->SizeY);
        Text(TEXT("OBJECT COURT PAUSED  /  Esc resume  /  Q quit"),270,320,FLinearColor::White,1.4f);
    }
    else
    {
        DrawRect(FLinearColor::White,Canvas->SizeX*.5f-2,Canvas->SizeY*.5f-2,4,4);
        if (PC->Hand())
        {
            auto* Object=PC->Hand()->GetHeldObject(); if (!Object) Object=PC->Hand()->TraceObject();
            if (Object) Text(Object->GetPlacement().DisplayName,480,430,FLinearColor(1,.8f,.42f),1.2f);
        }
        auto* Store=GetWorld()->GetSubsystem<UBSObjectWorldSubsystem>(); int32 Count=0;
        if (Store->GetLedger()) for (const auto& Object:Store->GetLedger()->GetSnapshot().Objects) Count+=Object.Custody==EBSObjectCustody::Collection;
        Text(FString::Printf(TEXT("Test bin  %d / 10"),Count),36,640,FLinearColor(.45f,.9f,.8f),1.2f);
        Text(Store->LastError.IsEmpty()?G->Status:Store->LastError,36,675,FLinearColor::White);
    }
}
