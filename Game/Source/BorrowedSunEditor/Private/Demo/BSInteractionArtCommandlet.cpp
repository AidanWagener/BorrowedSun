#include "Demo/BSInteractionArtCommandlet.h"
#include "Objects/BSObjectLab.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Engine.h"
#include "Engine/SkyLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "AssetImportTask.h"
#include "Engine/StaticMesh.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "HAL/FileManager.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "PhysicsEngine/BodySetup.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/SavePackage.h"

UBSInteractionArtCommandlet::UBSInteractionArtCommandlet()
{ IsClient = false; IsServer = false; IsEditor = true; LogToConsole = true; }

int32 UBSInteractionArtCommandlet::Main(const FString& Params)
{
    FString Root, Report;
    if (!FParse::Value(*Params,TEXT("SourceRoot="),Root) || !FParse::Value(*Params,TEXT("Report="),Report)) return 1;
    FString Text; TSharedPtr<FJsonObject> Manifest;
    if (!FFileHelper::LoadFileToString(Text, *FPaths::Combine(Root,TEXT("SourceArt/InteractionProps/interaction-props-manifest.json")))
        || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Manifest) || !Manifest.IsValid()) return 2;
    const FString Base = TEXT("/Game/BorrowedSun/Interaction/Meshes/");
    const FString MapPackage=TEXT("/Game/BorrowedSun/Maps/L_ObjectLab");
    const FString MapFile=FPackageName::LongPackageNameToFilename(MapPackage,FPackageName::GetMapPackageExtension());
    if (IFileManager::Get().FileExists(*MapFile) && !FParse::Param(*Params,TEXT("Force"))) return 5;
    const TArray<FString> Names = {TEXT("Cup"),TEXT("Bowl"),TEXT("Lantern"),TEXT("Tool"),TEXT("Cell"),
        TEXT("Crate"),TEXT("Basket"),TEXT("Bottle"),TEXT("Book"),TEXT("Bundle")};
    const auto& Assets = Manifest->GetArrayField(TEXT("assets"));
    if (Assets.Num() != Names.Num()) return 3;
    // Preflight every owned destination before writing any package.
    for (int32 I=0; I<Names.Num(); ++I)
    {
        const auto A=Assets[I]->AsObject(); const FString Name=TEXT("SM_Hand")+Names[I];
        if (!A || A->GetStringField(TEXT("name"))!=Name
            || A->GetStringField(TEXT("path"))!=TEXT("SourceArt/InteractionProps/")+Name+TEXT(".fbx")) return 4;
        const FString File=FPackageName::LongPackageNameToFilename(Base+Name,FPackageName::GetAssetPackageExtension());
        if (IFileManager::Get().FileExists(*File) && !FParse::Param(*Params,TEXT("Force"))) return 5;
    }
    TArray<TSharedPtr<FJsonValue>> Results;
    for (int32 I=0; I<Names.Num(); ++I)
    {
        const auto A=Assets[I]->AsObject(); const FString Name=TEXT("SM_Hand")+Names[I];
        auto* Package=CreatePackage(*(Base+Name));
        auto* Factory=NewObject<UFbxFactory>(); Factory->SetDetectImportTypeOnImport(false);
        auto* Task=NewObject<UAssetImportTask>(); Task->bAutomated=true; Factory->SetAssetImportTask(Task);
        auto* UI=Factory->ImportUI.Get(); UI->MeshTypeToImport=FBXIT_StaticMesh;
        UI->bAutomatedImportShouldDetectType=false; UI->bImportMaterials=false; UI->bImportTextures=false;
        UI->StaticMeshImportData->bCombineMeshes=true; UI->StaticMeshImportData->bAutoGenerateCollision=false;
        UI->StaticMeshImportData->bOneConvexHullPerUCX=true;
        UI->StaticMeshImportData->NormalImportMethod=FBXNIM_ImportNormals;
        UI->StaticMeshImportData->VertexColorImportOption=EVertexColorImportOption::Replace;
        auto* Mesh=Cast<UStaticMesh>(UFactory::StaticImportObject(UStaticMesh::StaticClass(),Package,*Name,
            RF_Public|RF_Standalone,*FPaths::Combine(Root,A->GetStringField(TEXT("path"))),nullptr,Factory));
        if (!Mesh || !Mesh->GetBodySetup() || Mesh->GetBodySetup()->AggGeom.GetElementCount()!=A->GetIntegerField(TEXT("collisionHulls")))
        { UE_LOG(LogTemp,Error,TEXT("Hand prop collision import did not match authored hull count: %s"),*Name); return 6; }
        for (int32 Slot=0;Slot<Mesh->GetStaticMaterials().Num();++Slot)
        {
            const FString SlotName=Mesh->GetStaticMaterials()[Slot].MaterialSlotName.ToString();
            UMaterialInterface* Match=nullptr;
            for (const TCHAR* Key : {TEXT("Wood"),TEXT("Brass"),TEXT("Copper"),TEXT("Shadow"),TEXT("Enamel"),TEXT("Iron"),TEXT("Ivory"),TEXT("MilkGlass")})
                if (SlotName.Equals(Key,ESearchCase::IgnoreCase) || SlotName.EndsWith(TEXT("_")+FString(Key),ESearchCase::IgnoreCase))
                {
                    const FString MatName=FString(Key)==TEXT("Shadow")?TEXT("M_MarketShadow"):TEXT("M_Workshop")+FString(Key);
                    Match=LoadObject<UMaterialInterface>(nullptr,*(TEXT("/Game/BorrowedSun/Market/Materials/")+MatName+TEXT(".")+MatName)); break;
                }
            if (!Match) { UE_LOG(LogTemp,Error,TEXT("Unmapped prop material: %s %s"),*Name,*SlotName); return 7; }
            Mesh->SetMaterial(Slot,Match);
        }
        Mesh->NaniteSettings.bEnabled=false;
        Mesh->GetBodySetup()->CollisionTraceFlag=CTF_UseSimpleAndComplex;
        Mesh->PostEditChange(); Package->MarkPackageDirty();
        const FString File=FPackageName::LongPackageNameToFilename(Base+Name,FPackageName::GetAssetPackageExtension());
        FSavePackageArgs Save; Save.TopLevelFlags=RF_Public|RF_Standalone; Save.SaveFlags=SAVE_NoError;
        if (!UPackage::SavePackage(Package,Mesh,*File,Save)) return 8;
        auto Item=MakeShared<FJsonObject>(); Item->SetStringField(TEXT("name"),Name);
        Item->SetNumberField(TEXT("collisionHulls"),Mesh->GetBodySetup()->AggGeom.GetElementCount());
        Item->SetStringField(TEXT("dimensionsCm"),Mesh->GetBoundingBox().GetSize().ToString());
        Item->SetNumberField(TEXT("materials"),Mesh->GetStaticMaterials().Num());
        Results.Add(MakeShared<FJsonValueObject>(Item));
    }
    auto Result=MakeShared<FJsonObject>(); Result->SetStringField(TEXT("status"),TEXT("Passed"));
    auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("L_ObjectLab"),CreatePackage(*MapPackage));
    if (!World) return 10;
    World->SetFlags(RF_Public|RF_Standalone);
    GEngine->CreateNewWorldContext(EWorldType::Editor).SetCurrentWorld(World);
    World->GetWorldSettings()->DefaultGameMode=ABSObjectLabGameMode::StaticClass();
    World->GetWorldSettings()->bForceNoPrecomputedLighting=true;
    World->SpawnActor<APlayerStart>(FVector(0,-240,100),FRotator(-12,90,0));
    auto* Sun=World->SpawnActor<ADirectionalLight>(FVector(0,0,2000),FRotator(-48,-35,0));
    Sun->GetComponent()->SetMobility(EComponentMobility::Movable);
    Sun->GetComponent()->SetIntensity(18000.f); Sun->GetComponent()->SetLightColor(FLinearColor(1,.89f,.73f));
    Sun->GetComponent()->SetAtmosphereSunLight(true);
    auto* Fill=World->SpawnActor<ADirectionalLight>(FVector(0,0,2000),FRotator(-60,140,0));
    Fill->GetComponent()->SetMobility(EComponentMobility::Movable); Fill->GetComponent()->SetIntensity(2500.f);
    Fill->GetComponent()->SetLightColor(FLinearColor(.65f,.8f,1)); Fill->GetComponent()->SetCastShadows(false);
    auto* Sky=World->SpawnActor<ASkyLight>(); Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Sky->GetLightComponent()->SetRealTimeCaptureEnabled(true); Sky->GetLightComponent()->SetIntensity(1.f);
    World->SpawnActor<ASkyAtmosphere>();
    auto* Exposure=World->SpawnActor<APostProcessVolume>(); Exposure->bUnbound=true;
    Exposure->Settings.bOverride_AutoExposureMinBrightness=true; Exposure->Settings.bOverride_AutoExposureMaxBrightness=true;
    Exposure->Settings.AutoExposureMinBrightness=10.f; Exposure->Settings.AutoExposureMaxBrightness=10.f;
    Exposure->Settings.bOverride_AutoExposureBias=true; Exposure->Settings.AutoExposureBias=0.f;
    FSavePackageArgs MapSave; MapSave.TopLevelFlags=RF_Public|RF_Standalone;
    const bool MapSaved=UPackage::SavePackage(World->GetOutermost(),World,*MapFile,MapSave);
    GEngine->DestroyWorldContext(World); World->DestroyWorld(false);
    if (!MapSaved) return 11;
    Result->SetStringField(TEXT("map"),MapPackage);
    Result->SetArrayField(TEXT("assets"),Results); FString Json;
    FJsonSerializer::Serialize(Result,TJsonWriterFactory<>::Create(&Json));
    return FFileHelper::SaveStringToFile(Json,*Report)?0:9;
}
