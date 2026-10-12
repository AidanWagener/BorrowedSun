#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSDemoWorld.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;
class UTextRenderComponent;

/** Deterministic, project-owned prototype architecture. One authored actor owns the scene recipe. */
UCLASS()
class BORROWEDSUN_API ABSDemoWorld : public AActor
{
    GENERATED_BODY()
public:
    ABSDemoWorld();
    virtual void BeginPlay() override;
    void SetSurfaceMaterial(UMaterialInterface* InSurface) { Surface = InSurface; }
    void ApplyConsequence(uint8 Choice, bool bComplete);
    void SetSeedAvailable(bool bAvailable);
    static FVector SeedLocation() { return FVector(-1100, 0, 150); }
    static FVector DockLocation() { return FVector(1800, 0, 220); }
    static FVector RelayLocation() { return FVector(18000, 0, 15000); }
    static FVector ClinicLocation() { return FVector(-1800, 1400, 110); }
    static FVector EvacuationLocation() { return FVector(2200, 1500, 110); }
    static FVector StartLocation() { return FVector(-2900, 0, 120); }
private:
    UPROPERTY() TObjectPtr<UMaterialInterface> Surface;
    UPROPERTY() TObjectPtr<UStaticMesh> Cube;
    UPROPERTY() TObjectPtr<UStaticMesh> Cylinder;
    UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
    UPROPERTY() TArray<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> Groups;
    UPROPERTY() TObjectPtr<UTextRenderComponent> ClinicStatus;
    UPROPERTY() TObjectPtr<UTextRenderComponent> EvacStatus;
    UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> SeedVisual;
    UPROPERTY(Transient) TArray<TObjectPtr<UStaticMeshComponent>> ClinicLights;
    UPROPERTY(Transient) TArray<TObjectPtr<UStaticMeshComponent>> EvacLights;
    bool bSeedAvailable = true;
    uint8 AppliedChoice = 0;
    bool bConsequenceComplete = false;
    TMap<FString, int32> GroupIndices;
    void Shape(UStaticMesh* Mesh, FVector Position, FVector Scale, FLinearColor Tint, float Glow = 0, bool bCollision = true, FRotator Rotation = FRotator::ZeroRotator);
    void Box(FVector P, FVector S, FLinearColor C, float Glow = 0, bool bCollision = true);
    UTextRenderComponent* Sign(const FString& Text, FVector Position, float Size, FColor Color, FRotator Rotation = FRotator(0, -90, 0));
    void BuildDistrict();
    void BuildSky();
    void BuildStateVisuals();
    UStaticMeshComponent* StateVisual(FName Name, UStaticMesh* Mesh, FVector Position, FVector Scale, FLinearColor Tint, float Glow);
};
