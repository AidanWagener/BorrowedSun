#include "Demo/BSDemoHUD.h"
#include "Demo/BSDemoGameMode.h"
#include "Demo/BSDemoPlayerController.h"
#include "Demo/BSKestrelPawn.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    const FLinearColor Ink(.018f,.035f,.047f,.94f);
    const FLinearColor Paper(.9f,.94f,.88f,1);
    const FLinearColor Mint(.25f,.9f,.83f,1);
    const FLinearColor Amber(1,.64f,.22f,1);
    const FLinearColor Muted(.57f,.68f,.69f,1);
}
ABSDemoHUD::ABSDemoHUD()
{
    static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
    DisplayFont=Font.Object;
}
void ABSDemoHUD::Text(const FString& S,float X,float Y,float Size,FLinearColor Color)
{
    UFont* Font=DisplayFont?DisplayFont.Get():GEngine->GetMediumFont();
    DrawText(S,Color,X*Scale,Y*Scale,Font,(Size/FMath::Max(1.f,Font->GetMaxCharHeight()))*Scale,false);
}
void ABSDemoHUD::Panel(float X,float Y,float W,float H,FLinearColor C)
{
    DrawRect(C,X*Scale,Y*Scale,W*Scale,H*Scale);
}
void ABSDemoHUD::DrawHUD()
{
    Super::DrawHUD();
    if(!Canvas || !GEngine) return;
    auto* PC=Cast<ABSDemoPlayerController>(GetOwningPlayerController());
    auto* G=GetWorld()->GetAuthGameMode<ABSDemoGameMode>();
    if(!PC || !G) return;
    Scale=FMath::Min(Canvas->SizeX/1280.f,Canvas->SizeY/720.f);
    const float W=Canvas->SizeX/Scale;
    const float H=Canvas->SizeY/Scale;
    if(PC->IsOverlayOpen())
    {
        Panel(0,0,W,H,FLinearColor(.008f,.022f,.032f,.88f));
        const float X=FMath::Max(60.f,(W-1080)/2);
        Panel(X,90,5,490,Amber);
        Text(TEXT("ORISON / A FIRST PLAYABLE EXPEDITION"),X+30,90,18,Mint);
        Text(TEXT("BORROWED SUN"),X+25,132,66,Paper);
        Text(PC->bWelcome?TEXT("The city rents its daylight. Tonight, you decide who gets it."):TEXT("EXPEDITION PAUSED"),X+30,227,25,Amber);
        if(PC->bConfirmRestart)
        {
            Text(TEXT("START A NEW EXPEDITION?"),X+30,310,30,Paper);
            Text(TEXT("This replaces this demo's current and backup checkpoints."),X+30,362,20,Muted);
            Text(TEXT("ENTER confirms    /    ESC cancels"),X+30,430,24,Mint);
        }
        else
        {
            Text(TEXT("Recover the sunseed. Take Kestrel to the sky relay."),X+30,301,23,Paper);
            Text(TEXT("Choose a power lease. Return home and make it real."),X+30,337,23,Paper);
            Text(TEXT("WASD move  /  MOUSE look  /  E interact  /  SPACE jump or ascend"),X+30,400,18,Muted);
            Text(TEXT("IN FLIGHT   C descend  /  SHIFT boost  /  CTRL brake  /  G flight assist"),X+30,431,18,Muted);
            Text(TEXT("ENTER / A  CONTINUE"),X+30,495,29,Mint);
            Text(TEXT("N new expedition   /   F9 restore checkpoint   /   Q quit"),X+30,548,17,Muted);
        }
        Text(TEXT("FIRST LIGHT  |  Original prototype  |  Auto-checkpoints at each mission step"),X+30,H-63,16,Muted);
        return;
    }

    Panel(28,24,760,108,Ink);
    Panel(28,24,4,108,Amber);
    Text(TEXT("BORROWED SUN  /  FIRST LIGHT"),48,37,17,Mint);
    Text(G->Objective(),48,72,21,Paper);
    Text(FString::Printf(TEXT("SUNSEED  %s     |     LEASE  %s"),G->Progress().bSunseedOwned?TEXT("IN CUSTODY"):G->Progress().Stage==EBSDemoStage::Complete?TEXT("INSTALLED"):TEXT("UNCLAIMED"),G->Progress().Choice==EBSDemoChoice::Clinic?TEXT("CLINIC"):G->Progress().Choice==EBSDemoChoice::Evacuation?TEXT("EVACUATION"):TEXT("UNASSIGNED")),48,106,13,Muted);
    Panel(W-247,24,219,66,Ink);
    Text(G->IsFlying(PC)?TEXT("KESTREL / FLIGHT"):TEXT("LOWER QUAY / ON FOOT"),W-231,36,16,Amber);
    if(G->IsFlying(PC) && G->Ship)
        Text(FString::Printf(TEXT("%03.0f km/h   /   %s"),G->Ship->GetSpeed()*.036f,PC->bGuidance?TEXT("ASSIST"):TEXT("MANUAL")),W-231,62,16,Paper);
    else Text(TEXT("F1 controls   /   ESC menu"),W-231,62,15,Muted);

    if(PC->GetPawn())
    {
        const FVector Target=G->ObjectiveLocation();
        const float Distance=FVector::Dist(PC->GetPawn()->GetActorLocation(),Target)/100.f;
        FVector2D Screen;
        const bool Front=PC->ProjectWorldLocationToScreen(Target+FVector(0,0,170),Screen);
        Screen/=Scale;
        if(!Front) Screen=FVector2D(W*.5f,H-185);
        Screen.X=FMath::Clamp(Screen.X,170.f,W-210);
        Screen.Y=FMath::Clamp(Screen.Y,165.f,H-205);
        Panel(Screen.X-3,Screen.Y-3,6,6,Amber);
        Panel(Screen.X-110,Screen.Y+12,245,44,Ink);
        Text(FString::Printf(TEXT("%s%s"),Front?TEXT(""):TEXT("TURN / "),*G->ObjectiveLabel()),Screen.X-99,Screen.Y+17,14,Amber);
        Text(FString::Printf(TEXT("%.0f m"),Distance),Screen.X-99,Screen.Y+36,13,Paper);
    }
    Panel(W*.5f-1,H*.5f-5,2,10,Paper*.6f);
    Panel(W*.5f-5,H*.5f-1,10,2,Paper*.6f);
    const FString Prompt=G->InteractionPrompt(PC);
    if(!Prompt.IsEmpty())
    {
        Panel(W*.5f-300,H-205,600,47,Ink);
        Text(Prompt,W*.5f-274,H-191,20,Mint);
    }
    if(G->MessageSeconds>0)
    {
        Panel(28,H-143,W-56,64,Ink);
        FString First=G->Message,Second;
        if(First.Len()>117)
        {
            int32 Break=117;
            while(Break>70 && First[Break]!=TCHAR(' ')) --Break;
            Second=First.Mid(Break+1); First=First.Left(Break);
        }
        Text(First,48,H-130,17,Paper);
        if(!Second.IsEmpty()) Text(Second,48,H-104,17,Paper);
    }
    Panel(0,H-51,W,51,Ink);
    Text(G->IsFlying(PC)?TEXT("WASD thrust / strafe    SPACE / C altitude    SHIFT boost    CTRL brake    G assist    E dock"):TEXT("WASD move    MOUSE look    SPACE jump    SHIFT sprint    E interact"),30,H-36,16,Paper);
    Text(TEXT("F5 save   F9 load   R recover   ESC menu"),W-365,H-16,12,Muted);

    if(G->Progress().Stage==EBSDemoStage::AtRelay)
    {
        const float X=W*.5f-440;
        Panel(X,220,880,237,Ink);
        Text(TEXT("ONE SUNSEED. TWO PROMISES."),X+24,238,31,Amber);
        Text(TEXT("[1 / LB]  IONA'S CLINIC"),X+24,300,24,Mint);
        Text(TEXT("Keep treatment running through the night."),X+24,340,17,Paper);
        Text(TEXT("The evacuation lift remains dark."),X+24,375,16,Muted);
        Text(TEXT("[2 / RB]  EVACUATION"),X+459,300,24,Mint);
        Text(TEXT("Give the waiting families a way out."),X+459,340,17,Paper);
        Text(TEXT("The clinic must ration its last reserves."),X+459,375,16,Muted);
        Text(TEXT("Your choice takes effect when you return and install the seed."),X+24,424,16,Amber);
    }
    if(G->Progress().Stage==EBSDemoStage::Complete && G->MessageSeconds>13)
    {
        const float X=W*.5f-315;
        Panel(X,245,630,157,Ink);
        Panel(X,245,4,157,Mint);
        Text(TEXT("FIRST LIGHT / EXPEDITION COMPLETE"),X+24,264,28,Mint);
        Text(G->Progress().Choice==EBSDemoChoice::Clinic?TEXT("You kept the clinic alive through the night."):TEXT("You gave the waiting families a way out."),X+24,318,22,Paper);
        Text(TEXT("The sunseed is installed. This choice survives your return."),X+24,366,16,Amber);
    }
    if(PC->bShowHelp)
    {
        Panel(W-495,153,467,280,Ink);
        Text(TEXT("CONTROLS / ACCESSIBILITY"),W-475,173,23,Mint);
        Text(TEXT("Stick left: move / strafe. Stick right: look."),W-475,218,17,Paper);
        Text(TEXT("A: jump / rise. B: descend. X: interact."),W-475,250,17,Paper);
        Text(TEXT("RT: boost. LT: brake. Y: flight assist."),W-475,282,17,Paper);
        Text(TEXT("D-pad down / R: recover to a safe position."),W-475,314,17,Paper);
        Text(FString::Printf(TEXT("[ / ]: mouse sensitivity (%.2f). No camera shake."),PC->LookSensitivity),W-475,346,16,Paper);
        Text(TEXT("Flight assist is optional. F1 / View closes this panel."),W-475,392,15,Muted);
    }
}
