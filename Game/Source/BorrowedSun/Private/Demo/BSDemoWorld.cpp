#include "Demo/BSDemoWorld.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    const FLinearColor Chalk(0.54f, 0.64f, 0.60f);
    const FLinearColor Sand(0.47f, 0.24f, 0.13f);
    const FLinearColor Teal(0.025f, 0.24f, 0.26f);
    const FLinearColor Dark(0.025f, 0.043f, 0.060f);
    const FLinearColor Gold(1.0f, 0.34f, 0.045f);
    const FLinearColor Cyan(0.04f, 0.85f, 0.86f);
}

ABSDemoWorld::ABSDemoWorld()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RecipeRoot"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> C(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Y(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> S(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Cube = C.Object; Cylinder = Y.Object; Sphere = S.Object;
}

void ABSDemoWorld::BeginPlay()
{
    Super::BeginPlay();
    if (!Surface) Surface = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/BorrowedSun/Art/M_DemoSurface.M_DemoSurface"));
    BuildDistrict();
    BuildSky();
    BuildStateVisuals();
    SetSeedAvailable(bSeedAvailable);
    ApplyConsequence(AppliedChoice, bConsequenceComplete);
}

void ABSDemoWorld::Shape(UStaticMesh* Mesh, FVector Position, FVector S, FLinearColor Tint, float Glow, bool bCollision, FRotator Rotation)
{
    if (!Mesh) return;
    const FString Key = FString::Printf(TEXT("%s_%s_%.1f_%d"), *Mesh->GetName(), *Tint.ToString(), Glow, bCollision);
    int32 Index = INDEX_NONE;
    if (const int32* Found = GroupIndices.Find(Key)) Index = *Found;
    if (Index == INDEX_NONE)
    {
        auto* Group = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
        Group->SetupAttachment(RootComponent);
        Group->SetStaticMesh(Mesh);
        Group->SetMobility(EComponentMobility::Movable);
        Group->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
        Group->SetCollisionResponseToAllChannels(ECR_Block);
        Group->SetCanEverAffectNavigation(false);
        if (Surface)
        {
            auto* Material = UMaterialInstanceDynamic::Create(Surface, this);
            Material->SetVectorParameterValue(TEXT("Tint"), Tint);
            Material->SetScalarParameterValue(TEXT("Glow"), Glow);
            Group->SetMaterial(0, Material);
        }
        Group->RegisterComponent();
        Index = Groups.Add(Group);
        GroupIndices.Add(Key, Index);
    }
    Groups[Index]->AddInstance(FTransform(Rotation, Position, S), true);
}

void ABSDemoWorld::Box(FVector P, FVector S, FLinearColor C, float Glow, bool Collision)
{
    Shape(Cube, P, S, C, Glow, Collision);
}

UTextRenderComponent* ABSDemoWorld::Sign(const FString& Text, FVector P, float Size, FColor Color, FRotator Rotation)
{
    auto* Label = NewObject<UTextRenderComponent>(this);
    Label->SetupAttachment(RootComponent);
    Label->SetWorldLocation(P);
    Label->SetWorldRotation(Rotation);
    Label->SetText(FText::FromString(Text));
    Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetVerticalAlignment(EVRTA_TextCenter);
    Label->SetWorldSize(Size);
    Label->SetTextRenderColor(Color);
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Label->RegisterComponent();
    return Label;
}

void ABSDemoWorld::BuildDistrict()
{
    // Dimensions are centimeters. The broad promenade and dock remain free of scattering.
    Box(FVector(0, 0, -120), FVector(86, 68, 2.4), Dark);
    Box(FVector(0, 0, -18), FVector(82, 64, .36), FLinearColor(.19f,.24f,.25f));
    Box(FVector(0, 0, 2), FVector(74, 10, .04), FLinearColor(.31f,.35f,.32f), 0, false);
    for (int32 I = -16; I <= 16; ++I)
    {
        Box(FVector(I * 220, -500, 4), FVector(1.2f,.05f,.04f), Gold, 1.5f, false);
        Box(FVector(I * 220, 500, 4), FVector(1.2f,.05f,.04f), Cyan, .8f, false);
        Box(FVector(I * 220, 0, 3), FVector(.035f,9.8f,.025f), Dark, 0, false);
    }
    // Two distinct elevations around the playable route, with inset window bands.
    FRandomStream Random(81226);
    for (int32 Row = 0; Row < 2; ++Row)
    {
        for (int32 I = 0; I < 9; ++I)
        {
            const float X = -3200 + I * 800;
            const float Y = Row == 0 ? -1900.f : 2400.f;
            const float H = Random.FRandRange(900, 2450);
            const FLinearColor Color = I % 3 == 0 ? Teal : (I % 3 == 1 ? Chalk : Sand);
            Box(FVector(X,Y,H*.5f), FVector(6.2f,8.f,H/100), Color);
            Box(FVector(X,Y,H+30), FVector(6.7f,8.5f,.6f), Dark);
            for (int32 Floor=0; Floor < FMath::FloorToInt(H/290); ++Floor)
            {
                const float Z=220+Floor*290;
                const float Front=Y+(Row==0 ? 406 : -406);
                Box(FVector(X,Front,Z), FVector(5.2f,.04f,1.1f), Dark,0,false);
                for (int32 W=-2; W<=2; ++W)
                    Box(FVector(X+W*105,Front+(Row==0?3:-3),Z), FVector(.72f,.025f,.75f), I%2?Gold:Cyan,.55f,false);
            }
            if (I%2==0)
            {
                Shape(Cylinder,FVector(X,Y,H+200),FVector(2.2f,2.2f,3.5f),Teal);
                Box(FVector(X,Y,H+430),FVector(.12f,.12f,2),Gold,2,false);
            }
        }
    }
    // Market shelters on the south side; open fronts face the promenade.
    for (int32 I=0; I<5; ++I)
    {
        const float X=-2600+I*730;
        Box(FVector(X,-850,280),FVector(5.8f,4.5f,.18f),I%2?Gold:Teal);
        for (float DX:{-260.f,260.f}) Box(FVector(X+DX,-1040,140),FVector(.12f,.12f,2.8f),Dark);
        Box(FVector(X,-1020,55),FVector(4.8f,1.2f,1.1f),Sand);
        for (int32 C=0; C<4; ++C) Box(FVector(X-170+C*110,-990,135),FVector(.7f,.75f,.5f),C%2?Gold:Chalk);
    }
    Sign(TEXT("ORISON / LOWER QUAY"),FVector(-2500,-1490,720),110,FColor(240,200,136),FRotator(0,90,0));
    Sign(TEXT("DAYLIGHT IS A SERVICE"),FVector(0,-1490,570),52,FColor(160,210,205),FRotator(0,90,0));
    Sign(TEXT("03  KESTREL"),FVector(2050,-1480,450),85,FColor(255,194,99),FRotator(0,90,0));

    // Sunseed shrine and visible docking geometry.
    Shape(Cylinder,SeedLocation()-FVector(0,0,105),FVector(1.6f,1.6f,.9f),Dark);
    for (int32 I=0;I<8;++I)
    {
        const float A=I*PI/4;
        Box(SeedLocation()+FVector(FMath::Cos(A)*95,FMath::Sin(A)*95,-130),FVector(.12f,.12f,.2f),Gold,3,false);
    }
    Sign(TEXT("SUNSEED / RECLAIMED LIGHT"),FVector(-1100,190,190),32,FColor(255,214,116));
    Shape(Cylinder,FVector(1800,0,6),FVector(11,11,.12f),Teal);
    Shape(Cylinder,FVector(1800,0,15),FVector(8.7f,8.7f,.08f),Dark,0,false);
    for (int32 I=0;I<12;++I)
    {
        const float A=I*2*PI/12;
        Shape(Cube,FVector(1800+FMath::Cos(A)*510,FMath::Sin(A)*510,20),FVector(.65f,.15f,.1f),Gold,3,false,FRotator(0,FMath::RadiansToDegrees(A)+90,0));
    }
    // Service forecourts; the seed is installed here after flight.
    Box(FVector(-1800,1550,180),FVector(8,3.6f,3.6f),Teal);
    Box(FVector(-1800,1360,170),FVector(3,.05f,2.8f),Dark);
    Box(FVector(-1800,1320,75),FVector(1.3f,.65f,1.5f),Chalk);
    Sign(TEXT("IONA'S CLINIC"),FVector(-1800,1340,360),58,FColor(150,240,235));
    ClinicStatus=Sign(TEXT("POWER LEASE EXPIRED"),FVector(-1800,1300,250),16,FColor(246,162,77));
    Box(FVector(2200,1700,180),FVector(8,3.6f,3.6f),Sand);
    Box(FVector(2200,1500,170),FVector(3,.05f,2.8f),Dark);
    Box(FVector(2200,1390,75),FVector(1.3f,.65f,1.5f),Chalk);
    Sign(TEXT("EVACUATION LIFT"),FVector(2200,1500,360),58,FColor(255,208,138));
    EvacStatus=Sign(TEXT("FAMILIES WAITING"),FVector(2200,1460,250),16,FColor(246,162,77));
    // Road-end rails, lamps and an identifiable skyline beyond the playable quay.
    for(float Y:{-3200.f,3200.f})
        Box(FVector(0,Y,60),FVector(84,.18f,1.2f),Teal);
    for(float X:{-4200.f,4200.f})
        Box(FVector(X,0,60),FVector(.18f,64,1.2f),Teal);
    for (int32 I=-3;I<=3;++I)
    {
        Box(FVector(I*1000,630,180),FVector(.1f,.1f,3.6f),Dark);
        Box(FVector(I*1000,630,370),FVector(.8f,.8f,.12f),Cyan,3,false);
    }
}

void ABSDemoWorld::BuildSky()
{
    // Sea and distant architecture establish scale without importing any vendor content.
    Box(FVector(0,0,-5000),FVector(1500,1500,4),FLinearColor(.018f,.075f,.092f),0,false);
    FRandomStream R(1012);
    for(int32 I=0;I<70;++I)
    {
        const float A=I*2*PI/70;
        const float Radius=R.FRandRange(9500,26000);
        const float H=R.FRandRange(1500,6800);
        const FVector P(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,H*.5f-2600);
        Box(P,FVector(R.FRandRange(5,12),R.FRandRange(5,12),H/100),I%3?Teal:Sand);
        Box(P+FVector(0,0,H*.5f+50),FVector(5,5,1),Gold,.65f,false);
    }
    // A ring-shaped sky relay and its floating platform. The first demo uses one continuous map.
    const FVector Relay=RelayLocation();
    Shape(Cylinder,Relay-FVector(0,0,2500),FVector(30,30,1.5f),Dark);
    Shape(Cylinder,Relay-FVector(0,0,2420),FVector(27,27,.12f),Teal);
    for(int32 I=0;I<32;++I)
    {
        const float A=I*2*PI/32;
        const FVector P=Relay+FVector(0,FMath::Cos(A)*1900,FMath::Sin(A)*1900+800);
        Shape(Cube,P,FVector(.7f,3.85f,.7f),Gold,2.5f,false,FRotator(0,0,FMath::RadiansToDegrees(A)));
    }
    Box(Relay+FVector(1000,0,900),FVector(1.3f,12,24),Teal);
    Sign(TEXT("THE HELIARCH / SKY RELAY"),Relay+FVector(-400,0,2300),170,FColor(255,220,145),FRotator(0,180,0));
    // Sparse approach beacons, readable both through the cockpit and from the ground.
    for(int32 I=1;I<8;++I)
    {
        const float T=I/8.f;
        const FVector P=FMath::Lerp(DockLocation()+FVector(0,0,650),Relay,T);
        for(float S:{-1.f,1.f})
        {
            Shape(Sphere,P+FVector(0,S*850,0),FVector(.9f),Cyan,5,false);
            Box(P+FVector(0,S*850,-140),FVector(.12f,.12f,2.5f),Teal,0,false);
        }
    }
}

UStaticMeshComponent* ABSDemoWorld::StateVisual(FName Name, UStaticMesh* Mesh, FVector Position, FVector Scale, FLinearColor Tint, float Glow)
{
    auto* Visual = NewObject<UStaticMeshComponent>(this, Name);
    Visual->SetupAttachment(RootComponent);
    Visual->SetStaticMesh(Mesh);
    Visual->SetMobility(EComponentMobility::Movable);
    Visual->SetWorldLocation(Position);
    Visual->SetWorldScale3D(Scale);
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Visual->SetGenerateOverlapEvents(false);
    Visual->SetCanEverAffectNavigation(false);
    if (Surface)
    {
        auto* Material = UMaterialInstanceDynamic::Create(Surface, Visual);
        Material->SetVectorParameterValue(TEXT("Tint"), Tint);
        Material->SetScalarParameterValue(TEXT("Glow"), Glow);
        Visual->SetMaterial(0, Material);
    }
    Visual->RegisterComponent();
    return Visual;
}

void ABSDemoWorld::BuildStateVisuals()
{
    // Create stateful geometry once. Checkpoint refreshes only toggle these components.
    if (SeedVisual) return;
    SeedVisual = StateVisual(TEXT("Sunseed"), Sphere, SeedLocation(), FVector(.6f), Gold, 5);
    for (int32 ServiceIndex = 0; ServiceIndex < 2; ++ServiceIndex)
    {
        const FVector Service = ServiceIndex == 0 ? ClinicLocation() : EvacuationLocation();
        auto& Lights = ServiceIndex == 0 ? ClinicLights : EvacLights;
        const TCHAR* Prefix = ServiceIndex == 0 ? TEXT("Clinic") : TEXT("Evacuation");
        Lights.Add(StateVisual(FName(*FString::Printf(TEXT("%sSeedLight"), Prefix)), Sphere,
            Service + FVector(0, -90, 150), FVector(.55f), Cyan, 5));
        for (int32 I = -3; I <= 3; ++I)
        {
            Lights.Add(StateVisual(FName(*FString::Printf(TEXT("%sStrip%d"), Prefix, I + 3)), Cube,
                Service + FVector(I * 100, -120, 40), FVector(.75f, .07f, .08f), Cyan, 4));
        }
    }
}

void ABSDemoWorld::SetSeedAvailable(bool bAvailable)
{
    bSeedAvailable = bAvailable;
    if (SeedVisual) SeedVisual->SetVisibility(bAvailable, true);
}

void ABSDemoWorld::ApplyConsequence(uint8 Choice, bool Complete)
{
    AppliedChoice = Choice <= 2 ? Choice : 0;
    bConsequenceComplete = Complete;
    const bool bClinicLit = Complete && AppliedChoice == 1;
    const bool bEvacLit = Complete && AppliedChoice == 2;
    for (UStaticMeshComponent* Light : ClinicLights)
        if (Light) Light->SetVisibility(bClinicLit, true);
    for (UStaticMeshComponent* Light : EvacLights)
        if (Light) Light->SetVisibility(bEvacLit, true);

    if (ClinicStatus)
    {
        ClinicStatus->SetText(FText::FromString(bClinicLit ? TEXT("LIGHT RESTORED / TREATMENT RESUMED") :
            AppliedChoice == 1 ? TEXT("LEASE AUTHORIZED / AWAITING SEED") : TEXT("POWER LEASE EXPIRED")));
        ClinicStatus->SetTextRenderColor(bClinicLit ? FColor(150, 240, 235) : FColor(246, 162, 77));
    }
    if (EvacStatus)
    {
        EvacStatus->SetText(FText::FromString(bEvacLit ? TEXT("LIFT ONLINE / FAMILIES DEPARTING") :
            AppliedChoice == 2 ? TEXT("LEASE AUTHORIZED / AWAITING SEED") : TEXT("FAMILIES WAITING")));
        EvacStatus->SetTextRenderColor(bEvacLit ? FColor(150, 240, 235) : FColor(246, 162, 77));
    }
}
