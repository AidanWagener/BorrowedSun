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
    LoadCityArt();
    BuildDistrict();
    BuildSky();
    BuildStateVisuals();
    SetSeedAvailable(bSeedAvailable);
    ApplyConsequence(AppliedChoice, bConsequenceComplete);
}

void ABSDemoWorld::Shape(UStaticMesh* Mesh, FVector Position, FVector S, FLinearColor Tint, float Glow, bool bCollision, FRotator Rotation, UMaterialInterface* Override)
{
    if (!Mesh) return;
    const FString Key = FString::Printf(TEXT("%s_%s_%s_%.1f_%d"), *Mesh->GetName(), Override ? *Override->GetName() : TEXT("Demo"), *Tint.ToString(), Glow, bCollision);
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
        if (Override || Surface)
        {
            auto* Material = UMaterialInstanceDynamic::Create(Override ? Override : Surface.Get(), this);
            Material->SetVectorParameterValue(TEXT("Tint"), Tint);
            Material->SetScalarParameterValue(TEXT("Glow"), Glow);
            for(int32 Slot=0;Slot<FMath::Max(1,Mesh->GetStaticMaterials().Num());++Slot)Group->SetMaterial(Slot, Material);
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
