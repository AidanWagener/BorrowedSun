#include "Demo/BSOrisonTerrainCommandlet.h"
#include "World/BSOrisonFrame.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeInfo.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionWorldPosition.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogBSOrisonTerrain, Log, All);

namespace BSOrisonTerrain
{
    constexpr int32 Resolution = 1009;
    const FName OwnedTag(TEXT("BorrowedSun.OrisonMacroTerrain"));
    const FName WaterTag(TEXT("BorrowedSun.OrisonPlanningSea"));
    const FString MapPath(TEXT("/Game/BorrowedSun/Maps/L_BorrowedSunDemo"));
    // Canonical city x east, y north, z metres above planning sea level.
    // Workshop (-4780,-1370,92)m retains its runtime anchor (-2570,-470,8)cm.
    const FVector RuntimeOriginMetres = FBSOrisonFrame::RuntimeOriginMetres();

    template<class T> T* Expression(UMaterial* Material)
    {
        T* E = NewObject<T>(Material);
        E->Material = Material;
        E->UpdateMaterialExpressionGuid(true, false);
        Material->GetExpressionCollection().AddExpression(E);
        return E;
    }

    bool Save(UObject* Asset)
    {
        if (!Asset) return false;
        UPackage* Package = Asset->GetOutermost();
        Package->MarkPackageDirty();
        const FString File = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(Package, Asset, *File, Args);
    }

    UMaterial* Material(const TCHAR* Name)
    {
        const FString Path = FString(TEXT("/Game/BorrowedSun/World/Orison/")) + Name;
        UMaterial* M = LoadObject<UMaterial>(nullptr, *(Path + TEXT(".") + Name));
        if (!M)
        {
            UPackage* Package = CreatePackage(*Path);
            Package->MarkAsFullyLoaded();
            M = NewObject<UMaterial>(Package, Name, RF_Public | RF_Standalone);
        }
        M->PreEditChange(nullptr);
        M->GetExpressionCollection().Empty();
        M->EditorParameters.Empty();
        M->SetShadingModel(MSM_DefaultLit);
        return M;
    }

    UMaterial* CreateTerrainMaterial()
    {
        UMaterial* M = Material(TEXT("M_OrisonMacroGround"));
        auto* Ground = Expression<UMaterialExpressionCustom>(M);
        Ground->OutputType = CMOT_Float3;
        Ground->Description = TEXT("Orison coarse terrain: triplanar sandstone, restrained elevation and slope variation");
        auto Input = [&](const TCHAR* Name, UMaterialExpression* Value)
        {
            FCustomInput I; I.InputName = Name; I.Input.Connect(0, Value); Ground->Inputs.Add(I);
        };
        auto* Texture = Expression<UMaterialExpressionTextureObject>(M);
        Texture->Texture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/BorrowedSun/City/Textures/T_seaworn_sandstone_brick_diff.T_seaworn_sandstone_brick_diff"));
        if (!Texture->Texture) return nullptr;
        Texture->SamplerType = SAMPLERTYPE_Color;
        Input(TEXT("D"), Texture);
        Input(TEXT("P"), Expression<UMaterialExpressionWorldPosition>(M));
        Input(TEXT("N"), Expression<UMaterialExpressionVertexNormalWS>(M));
        Ground->Code = TEXT(R"HLSL(
float3 weights=pow(abs(N),4.0); weights/=max(dot(weights,1.0),.0001);
float3 uv=P/1200.0;
float3 textureColor=Texture2DSample(D,DSampler,uv.yz).rgb*weights.x
 +Texture2DSample(D,DSampler,uv.xz).rgb*weights.y
 +Texture2DSample(D,DSampler,uv.xy).rgb*weights.z;
float grain=dot(textureColor,float3(.2126,.7152,.0722));
float altitude=P.z/100.0+91.92;
float slope=1.0-saturate(N.z);
float bands=.5+.5*sin(P.z/2300.0+sin(P.x/32000.0)*1.4+sin(P.y/47000.0));
float macro=.5+.25*sin(P.x/15700.0+P.y/32100.0)+.25*sin(P.y/19300.0-P.x/53900.0);
float3 earth=lerp(float3(.105,.126,.065),float3(.23,.205,.14),saturate(macro*.7+altitude/2200.0));
float3 rock=lerp(float3(.21,.185,.145),float3(.36,.31,.235),bands);
float rockWeight=saturate(slope*3.4+smoothstep(650.0,1350.0,altitude)*.55);
return lerp(earth,rock,rockWeight)*lerp(.74,1.2,saturate(grain*1.9));
)HLSL");
        auto* Data = M->GetEditorOnlyData();
        Data->BaseColor.Connect(0, Ground);
        Data->Roughness.UseConstant = true; Data->Roughness.Constant = .93f;
        M->PostEditChange();
        return Save(M) ? M : nullptr;
    }

    UMaterial* CreateSeaMaterial()
    {
        UMaterial* M = Material(TEXT("M_OrisonPlanningSea"));
        auto* Color = Expression<UMaterialExpressionConstant3Vector>(M);
        Color->Constant = FLinearColor(.016f, .061f, .070f);
        auto* Data = M->GetEditorOnlyData();
        Data->BaseColor.Connect(0, Color);
        Data->Roughness.UseConstant = true; Data->Roughness.Constant = .24f;
        Data->Metallic.UseConstant = true; Data->Metallic.Constant = .0f;
        // Opaque datum surface: waves, shoreline foam and swimming remain separate work.
        M->PostEditChange();
        return Save(M) ? M : nullptr;
    }
}

UBSOrisonTerrainCommandlet::UBSOrisonTerrainCommandlet()
{
    IsClient = false; IsServer = false; IsEditor = true;
    LogToConsole = true; ShowErrorCount = true;
}

int32 UBSOrisonTerrainCommandlet::Main(const FString& Params)
{
    using namespace BSOrisonTerrain;
    const FString HeightFile = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../SourceArt/World/Orison/Orison_MacroTerrain_1009.r16"));
    TArray<uint8> Bytes;
    if (!FFileHelper::LoadFileToArray(Bytes, *HeightFile) || Bytes.Num() != Resolution * Resolution * 2)
    {
        UE_LOG(LogBSOrisonTerrain, Error, TEXT("Expected 1009 square little-endian R16: %s"), *HeightFile);
        return 1;
    }
    TArray<uint16> Heights; Heights.SetNumUninitialized(Resolution * Resolution);
    for (int32 I = 0; I < Heights.Num(); ++I) Heights[I] = uint16(Bytes[I * 2]) | (uint16(Bytes[I * 2 + 1]) << 8);
    const FString MapFile = FPackageName::LongPackageNameToFilename(MapPath, FPackageName::GetMapPackageExtension());
    UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFile);
    if (!World) return 2;
    TArray<AActor*> Previous;
    for (TActorIterator<AActor> It(World); It; ++It)
        if (It->ActorHasTag(OwnedTag) || It->ActorHasTag(WaterTag)) Previous.Add(*It);
    if (!Previous.IsEmpty() && !FParse::Param(*Params, TEXT("ReplaceOwned")))
    {
        UE_LOG(LogBSOrisonTerrain, Error, TEXT("Owned regional terrain exists; supply -ReplaceOwned to regenerate it. Other actors are preserved."));
        return 3;
    }
    UMaterial* GroundMaterial = CreateTerrainMaterial();
    UMaterial* SeaMaterial = CreateSeaMaterial();
    UStaticMesh* Plane = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (!GroundMaterial || !SeaMaterial || !Plane) return 4;
    for (AActor* Actor : Previous)
    {
        World->DestroyActor(Actor);
        Actor->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
    }

    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("Nacre_OrisonTerrain");
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    const FVector Position = (FVector(-13500, -12000, 0) - RuntimeOriginMetres) * 100.0;
    ALandscape* Landscape = World->SpawnActor<ALandscape>(Position, FRotator::ZeroRotator, Spawn);
    if (!Landscape) return 5;
    Landscape->Tags.Add(OwnedTag);
    Landscape->SetActorLabel(TEXT("Nacre / Orison 27 x 24 km macro terrain"));
    Landscape->SetActorScale3D(FVector(2700000.0 / 1008.0, 2400000.0 / 1008.0, 800.0));
    Landscape->LandscapeMaterial = GroundMaterial;
    Landscape->StaticLightingLOD = 0;
    TMap<FGuid, TArray<uint16>> HeightsByLayer;
    HeightsByLayer.Add(FGuid(), MoveTemp(Heights));
    TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialsByLayer;
    MaterialsByLayer.Add(FGuid(), {});
    Landscape->Import(FGuid(0x4f726973, 0x6f6e4e61, 0x63726501, 0x00000001), 0, 0, Resolution - 1, Resolution - 1,
        1, 63, HeightsByLayer, *HeightFile, MaterialsByLayer, ELandscapeImportAlphamapType::Additive, TArrayView<const FLandscapeLayer>());
    if (!Landscape->GetLandscapeInfo() || Landscape->LandscapeComponents.Num() != 256) return 6;
    Landscape->GetLandscapeInfo()->UpdateLayerInfoMap(Landscape);

    Spawn.Name = TEXT("Nacre_OrisonPlanningSea");
    AStaticMeshActor* Sea = World->SpawnActor<AStaticMeshActor>((FVector(0, 0, 0) - RuntimeOriginMetres) * 100.0, FRotator::ZeroRotator, Spawn);
    if (!Sea) return 7;
    Sea->Tags.Add(WaterTag);
    Sea->SetActorLabel(TEXT("Orison / planning sea datum (no water simulation)"));
    Sea->GetStaticMeshComponent()->SetStaticMesh(Plane);
    Sea->GetStaticMeshComponent()->SetMaterial(0, SeaMaterial);
    Sea->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Sea->SetActorScale3D(FVector(30000, 28000, 1));
    World->UpdateWorldComponents(false, false);
    if (!UEditorLoadingAndSavingUtils::SaveMap(World, MapPath)) return 8;
    const FString ReportDirectory = FPaths::ProjectSavedDir() / TEXT("OrisonTerrain");
    IFileManager::Get().MakeDirectory(*ReportDirectory, true);
    const FString Report = TEXT("{\"schemaVersion\":1,\"success\":true,\"resolution\":1009,\"components\":256,\"extentMetres\":[27000,24000],\"runtimeOriginMetres\":[-4754.3,-1365.3,91.92],\"map\":\"/Game/BorrowedSun/Maps/L_BorrowedSunDemo\",\"status\":\"macro terrain; water simulation and planetary streaming not implemented\"}");
    if (!FFileHelper::SaveStringToFile(Report, *(ReportDirectory / TEXT("authoring.json")))) return 9;
    UE_LOG(LogBSOrisonTerrain, Display, TEXT("Preserved main game map; imported 256 Landscape components and canonical sea datum."));
    return 0;
}
