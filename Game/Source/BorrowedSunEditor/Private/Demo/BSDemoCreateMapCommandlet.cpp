#include "Demo/BSDemoCreateMapCommandlet.h"

#include "Demo/BSDemoGameMode.h"
#include "Demo/BSDemoWorld.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Engine.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"

DEFINE_LOG_CATEGORY_STATIC(LogBSDemoCreateMap, Log, All);

namespace BSDemoMapAuthoring
{
    constexpr const TCHAR* MapPackageName = TEXT("/Game/BorrowedSun/Maps/L_BorrowedSunDemo");
    constexpr const TCHAR* MapAssetName = TEXT("L_BorrowedSunDemo");
    constexpr const TCHAR* MaterialPackageName = TEXT("/Game/BorrowedSun/Art/M_DemoSurface");
    constexpr const TCHAR* MaterialAssetName = TEXT("M_DemoSurface");

    template <typename TExpression>
    TExpression* AddExpression(UMaterial* Material, const FName Name, const int32 X, const int32 Y)
    {
        // Unique object names also permit rebuilding a material that was already loaded.
        const FName ObjectName = MakeUniqueObjectName(Material, TExpression::StaticClass(), Name);
        TExpression* Expression = NewObject<TExpression>(Material, ObjectName, RF_Transactional);
        Expression->Material = Material;
        Expression->MaterialExpressionEditorX = X;
        Expression->MaterialExpressionEditorY = Y;
        Expression->UpdateMaterialExpressionGuid(true, false);
        Material->GetExpressionCollection().AddExpression(Expression);
        return Expression;
    }

    UMaterial* CreateSurfaceMaterial()
    {
        UPackage* Package = CreatePackage(MaterialPackageName);
        if (FindObject<UObject>(Package, MaterialAssetName))
        {
            UE_LOG(LogBSDemoCreateMap, Error,
                TEXT("The destination material is already loaded. Run this commandlet in a fresh editor process."));
            return nullptr;
        }
        UMaterial* Material = NewObject<UMaterial>(Package, MaterialAssetName, RF_Public | RF_Standalone);

        // Main has checked overwrite permission. This is the complete regenerated package,
        // not a partial edit of an existing on-disk package or its unloaded exports.
        Package->MarkAsFullyLoaded();

        Material->PreEditChange(nullptr);
        Material->SetFlags(RF_Public | RF_Standalone);
        Material->GetExpressionCollection().Empty();
        Material->EditorParameters.Empty();
        Material->MaterialDomain = MD_Surface;
        Material->BlendMode = BLEND_Opaque;
        Material->SetShadingModel(MSM_DefaultLit);
        Material->bUsedWithInstancedStaticMeshes = true;

        UMaterialExpressionVectorParameter* Tint = AddExpression<UMaterialExpressionVectorParameter>(
            Material, TEXT("DemoTint"), -480, -80);
        Tint->ParameterName = TEXT("Tint");
        Tint->DefaultValue = FLinearColor(0.22f, 0.32f, 0.38f, 1.0f);
        Tint->UpdateParameterGuid(true, false);

        UMaterialExpressionScalarParameter* Glow = AddExpression<UMaterialExpressionScalarParameter>(
            Material, TEXT("DemoGlow"), -480, 120);
        Glow->ParameterName = TEXT("Glow");
        Glow->DefaultValue = 0.0f;
        Glow->UpdateParameterGuid(true, false);

        UMaterialExpressionMultiply* Emissive = AddExpression<UMaterialExpressionMultiply>(
            Material, TEXT("DemoEmissive"), -200, 100);
        Emissive->A.Connect(0, Tint);
        auto* DaylightGlow = AddExpression<UMaterialExpressionMultiply>(Material, TEXT("DaylightEmission"), -320, 240);
        DaylightGlow->A.Connect(0, Glow);
        DaylightGlow->ConstB = 2000.0f;
        Emissive->B.Connect(0, DaylightGlow);

        UMaterialEditorOnlyData* Data = Material->GetEditorOnlyData();
        Data->BaseColor.Connect(0, Tint);
        Data->EmissiveColor.Connect(0, Emissive);
        Data->Roughness.Expression = nullptr;
        Data->Roughness.UseConstant = true;
        Data->Roughness.Constant = 0.65f;
        Data->Metallic.Expression = nullptr;
        Data->Metallic.UseConstant = true;
        Data->Metallic.Constant = 0.0f;

        Material->AddExpressionParameter(Tint, Material->EditorParameters);
        Material->AddExpressionParameter(Glow, Material->EditorParameters);
        Material->PostEditChange();
        Material->MarkPackageDirty();
        return Material;
    }

    template <typename TActor>
    TActor* SpawnAuthoredActor(UWorld* World, const FName Name, const FVector& Location,
        const FRotator& Rotation = FRotator::ZeroRotator)
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = Name;
        Spawn.ObjectFlags |= RF_Transactional;
        Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        TActor* Actor = World->SpawnActor<TActor>(Location, Rotation, Spawn);
        if (Actor)
        {
            Actor->SetActorLabel(Name.ToString());
        }
        return Actor;
    }

    bool SaveAsset(UObject* Asset, const FString& Filename)
    {
        UPackage* Package = Asset->GetOutermost();
        Package->MarkPackageDirty();
        FSavePackageArgs SaveArgs;
        SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
        SaveArgs.SaveFlags = SAVE_None;
        SaveArgs.Error = GWarn;
        SaveArgs.bSlowTask = false;
        if (!UPackage::SavePackage(Package, Asset, *Filename, SaveArgs))
        {
            UE_LOG(LogBSDemoCreateMap, Error, TEXT("Could not save package: %s"), *Filename);
            return false;
        }
        if (IFileManager::Get().FileSize(*Filename) <= 0)
        {
            UE_LOG(LogBSDemoCreateMap, Error, TEXT("Saved package is missing or empty: %s"), *Filename);
            return false;
        }
        UE_LOG(LogBSDemoCreateMap, Display, TEXT("Saved %s"), *Filename);
        return true;
    }
}

UBSDemoCreateMapCommandlet::UBSDemoCreateMapCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
    ShowErrorCount = true;
}

int32 UBSDemoCreateMapCommandlet::Main(const FString& Params)
{
    using namespace BSDemoMapAuthoring;

    const FString MapFilename = FPaths::ConvertRelativePathToFull(
        FPackageName::LongPackageNameToFilename(MapPackageName, FPackageName::GetMapPackageExtension()));
    const FString MaterialFilename = FPaths::ConvertRelativePathToFull(
        FPackageName::LongPackageNameToFilename(MaterialPackageName, FPackageName::GetAssetPackageExtension()));
    const bool bForce = FParse::Param(*Params, TEXT("Force"));

    // Check both outputs before modifying either one, including partial earlier runs.
    for (const FString& Filename : {MapFilename, MaterialFilename})
    {
        if (IFileManager::Get().FileExists(*Filename) && !bForce)
        {
            UE_LOG(LogBSDemoCreateMap, Error,
                TEXT("Destination exists; nothing was changed: %s. Use -Force to regenerate both demo packages."),
                *Filename);
            return 2;
        }
        if (IFileManager::Get().IsReadOnly(*Filename))
        {
            UE_LOG(LogBSDemoCreateMap, Error, TEXT("Destination is read-only: %s"), *Filename);
            return 2;
        }
    }

    if (!GEngine)
    {
        UE_LOG(LogBSDemoCreateMap, Error, TEXT("An initialized Unreal editor engine is required."));
        return 3;
    }

    UPackage* MapPackage = CreatePackage(MapPackageName);
    if (FindObject<UObject>(MapPackage, MapAssetName))
    {
        UE_LOG(LogBSDemoCreateMap, Error,
            TEXT("The destination world is already loaded. Run this commandlet in a fresh editor process without loading that map."));
        return 3;
    }

    for (const FString& Filename : {MapFilename, MaterialFilename})
    {
        const FString Directory = FPaths::GetPath(Filename);
        if (!IFileManager::Get().DirectoryExists(*Directory)
            && !IFileManager::Get().MakeDirectory(*Directory, true))
        {
            UE_LOG(LogBSDemoCreateMap, Error, TEXT("Could not create asset directory: %s"), *Directory);
            return 3;
        }
    }

    TStrongObjectPtr<UMaterial> SurfaceMaterial(CreateSurfaceMaterial());
    if (!SurfaceMaterial.IsValid())
    {
        return 4;
    }
    // -Force means a full replacement of this owned package. Do not load and mix old actors.
    MapPackage->MarkAsFullyLoaded();
    UWorld* World = UWorld::CreateWorld(EWorldType::Editor, false, FName(MapAssetName), MapPackage);
    if (!World)
    {
        UE_LOG(LogBSDemoCreateMap, Error, TEXT("Could not create the demo world."));
        return 4;
    }
    World->SetFlags(RF_Public | RF_Standalone);

    UWorld* PreviousWorld = GWorld;
    GEngine->CreateNewWorldContext(EWorldType::Editor).SetCurrentWorld(World);
    GWorld = World;
    ON_SCOPE_EXIT
    {
        GWorld = PreviousWorld;
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };

    World->GetWorldSettings()->DefaultGameMode = ABSDemoGameMode::StaticClass();
    World->GetWorldSettings()->bForceNoPrecomputedLighting = true;

    ABSDemoWorld* Demo = SpawnAuthoredActor<ABSDemoWorld>(World, TEXT("BorrowedSunDemoWorld"), FVector::ZeroVector);
    APlayerStart* PlayerStart = SpawnAuthoredActor<APlayerStart>(
        World, TEXT("DemoPlayerStart"), FVector(-2200.0, -600.0, 160.0), FRotator(0.0, 25.0, 0.0));
    ADirectionalLight* Sun = SpawnAuthoredActor<ADirectionalLight>(
        World, TEXT("DemoGoldenHourSun"), FVector(0.0, 0.0, 3000.0), FRotator(-27.0, -35.0, 0.0));
    ADirectionalLight* Fill = SpawnAuthoredActor<ADirectionalLight>(
        World, TEXT("CityCoolSkyFill"), FVector(0.0, 0.0, 3500.0), FRotator(-65.0, 140.0, 0.0));
    ASkyLight* Sky = SpawnAuthoredActor<ASkyLight>(World, TEXT("DemoSkyLight"), FVector(0.0, 0.0, 1500.0));
    ASkyAtmosphere* Atmosphere = SpawnAuthoredActor<ASkyAtmosphere>(World, TEXT("DemoSkyAtmosphere"), FVector::ZeroVector);
    AVolumetricCloud* Clouds = SpawnAuthoredActor<AVolumetricCloud>(World, TEXT("HarborCumulusClouds"), FVector::ZeroVector);
    AExponentialHeightFog* Fog = SpawnAuthoredActor<AExponentialHeightFog>(
        World, TEXT("DemoHeightFog"), FVector(0.0, 0.0, -300.0));
    APostProcessVolume* Exposure = SpawnAuthoredActor<APostProcessVolume>(World, TEXT("CityDaylightExposure"), FVector::ZeroVector);

    if (!Demo || !PlayerStart || !Sun || !Fill || !Sky || !Atmosphere || !Clouds || !Fog || !Exposure)
    {
        UE_LOG(LogBSDemoCreateMap, Error, TEXT("An authored demo actor failed to spawn; packages were not saved."));
        return 4;
    }

    // A serialized UPROPERTY reference makes the shared material a cook dependency.
    Demo->SetSurfaceMaterial(SurfaceMaterial.Get());

    UDirectionalLightComponent* SunComponent = Sun->GetComponent();
    SunComponent->SetMobility(EComponentMobility::Movable);
    SunComponent->SetIntensity(35000.0f);
    SunComponent->SetLightColor(FLinearColor(1.0f, 0.91f, 0.76f));
    SunComponent->SetAtmosphereSunLight(true);
    SunComponent->SetForwardShadingPriority(1);
    // Broad artistic sky fill keeps shaded interaction spaces readable without extra shadow passes.
    Fill->GetComponent()->SetMobility(EComponentMobility::Movable);
    Fill->GetComponent()->SetIntensity(3000.0f);
    Fill->GetComponent()->SetLightColor(FLinearColor(.65f,.78f,1.0f));
    Fill->GetComponent()->SetCastShadows(false);
    Fill->GetComponent()->SetAtmosphereSunLight(false);
    Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Sky->GetLightComponent()->SetIntensity(1.5f);
    Sky->GetLightComponent()->SetRealTimeCaptureEnabled(true);
    auto* CloudMaterial=LoadObject<UMaterialInterface>(nullptr,TEXT("/Engine/EngineSky/VolumetricClouds/m_SimpleVolumetricCloud_Inst.m_SimpleVolumetricCloud_Inst"));
    auto* CloudComponent=Clouds->FindComponentByClass<UVolumetricCloudComponent>();
    if(!CloudMaterial||!CloudComponent)return 4;
    CloudComponent->SetMaterial(CloudMaterial);
    CloudComponent->SetLayerBottomAltitude(1.4f);
    CloudComponent->SetLayerHeight(2.5f);
    CloudComponent->SetViewSampleCountScale(.6f);
    CloudComponent->SetReflectionViewSampleCountScale(.2f);
    Fog->GetComponent()->SetFogDensity(0.028f);
    Fog->GetComponent()->SetFogHeightFalloff(0.05f);
    Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.33f, 0.40f, 0.46f));
    Fog->GetComponent()->SetVolumetricFog(true);
    Exposure->bUnbound=true;
    Exposure->Settings.bOverride_AutoExposureMinBrightness=true;
    Exposure->Settings.bOverride_AutoExposureMaxBrightness=true;
    Exposure->Settings.AutoExposureMinBrightness=10.8f;
    Exposure->Settings.AutoExposureMaxBrightness=12.0f;
    Exposure->Settings.bOverride_AutoExposureBias=true;
    Exposure->Settings.AutoExposureBias=0.0f;

    World->UpdateWorldComponents(false, false);
    if (!SaveAsset(SurfaceMaterial.Get(), MaterialFilename))
    {
        return 5;
    }
    if (!SaveAsset(World, MapFilename))
    {
        UE_LOG(LogBSDemoCreateMap, Error,
            TEXT("Material saved, but map save failed. Resolve the reported error and retry with -Force."));
        return 5;
    }

    UE_LOG(LogBSDemoCreateMap, Display,
        TEXT("Demo authoring completed: %s and %s. This confirms package saves; rendering and gameplay still require verification."),
        MapPackageName, MaterialPackageName);
    return 0;
}
