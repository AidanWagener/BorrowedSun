#include "Demo/BSMarketArtCommandlet.h"
#include "Demo/BSMarketAssembly.h"
#include "AssetImportTask.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionWorldPosition.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace BSMarketArt
{
    const FString Base=TEXT("/Game/BorrowedSun/Market/");
    TArray<FString> SavedAssets;
    FString Source(const FString& Relative){return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()/TEXT("..")/Relative);}
    TSharedPtr<FJsonObject> Read(const FString& Path)
    {
        FString Text;TSharedPtr<FJsonObject> Object;
        if(!FFileHelper::LoadFileToString(Text,*Source(Path))||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Object))return nullptr;
        return Object;
    }
    bool Save(UObject* Asset)
    {
        if(!Asset)return false;
        auto* P=Asset->GetOutermost();P->MarkAsFullyLoaded();P->MarkPackageDirty();
        const FString File=FPackageName::LongPackageNameToFilename(P->GetName(),FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);
        FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;
        if(!UPackage::SavePackage(P,Asset,*File,Args))return false;
        SavedAssets.AddUnique(Asset->GetPathName());return true;
    }
    template<class T> T* Expr(UMaterial* M)
    {
        auto* E=NewObject<T>(M);E->Material=M;E->UpdateMaterialExpressionGuid(true,false);M->GetExpressionCollection().AddExpression(E);return E;
    }
    UMaterialExpressionConstant* Scalar(UMaterial* M,float V){auto* E=Expr<UMaterialExpressionConstant>(M);E->R=V;return E;}
    UMaterialExpressionConstant3Vector* Color(UMaterial* M,FLinearColor V){auto* E=Expr<UMaterialExpressionConstant3Vector>(M);E->Constant=V;return E;}
    UTexture2D* Texture(const FString& Name,const FString& Relative,const FString& Kind,bool Flip=false)
    {
        auto* P=CreatePackage(*(Base+TEXT("Textures/")+Name));auto* Factory=NewObject<UTextureFactory>();Factory->SuppressImportOverwriteDialog();
        auto* T=Cast<UTexture2D>(UFactory::StaticImportObject(UTexture2D::StaticClass(),P,*Name,RF_Public|RF_Standalone,*Source(Relative),nullptr,Factory));
        if(!T)return nullptr;
        const bool Normal=Kind==TEXT("normal");T->SRGB=Kind==TEXT("diff");T->bFlipGreenChannel=Normal&&Flip;
        T->CompressionSettings=Normal?TC_Normalmap:(T->SRGB?TC_Default:TC_Grayscale);
        T->LODGroup=Normal?TEXTUREGROUP_WorldNormalMap:TEXTUREGROUP_World;T->MaxTextureSize=2048;
        T->PowerOfTwoMode=ETexturePowerOfTwoSetting::StretchToPowerOfTwo;
        T->PostEditChange();return Save(T)?T:nullptr;
    }
    UMaterial* NewMaterial(const FString& Name)
    {
        auto* P=CreatePackage(*(Base+TEXT("Materials/")+Name));auto* M=NewObject<UMaterial>(P,*Name,RF_Public|RF_Standalone);
        M->PreEditChange(nullptr);M->bUsedWithInstancedStaticMeshes=true;M->bUsedWithNanite=true;return M;
    }
    UMaterialExpressionTextureSample* Sample(UMaterial* M,UTexture2D* T,EMaterialSamplerType Type)
    {
        auto* S=Expr<UMaterialExpressionTextureSample>(M);S->Texture=T;S->SamplerType=Type;return S;
    }
    UMaterial* UvMaterial(const FString& Name,const TMap<FString,UTexture2D*>& Maps,bool Masked)
    {
        auto* M=NewMaterial(Name);auto* E=M->GetEditorOnlyData();
        if(!Maps.FindRef(TEXT("diff")))return nullptr;
        auto* D=Sample(M,Maps.FindRef(TEXT("diff")),SAMPLERTYPE_Color);E->BaseColor.Connect(0,D);
        if(auto* T=Maps.FindRef(TEXT("normal")))E->Normal.Connect(0,Sample(M,T,SAMPLERTYPE_Normal));
        if(auto* T=Maps.FindRef(TEXT("rough")))E->Roughness.Connect(1,Sample(M,T,SAMPLERTYPE_LinearGrayscale));
        if(auto* T=Maps.FindRef(TEXT("metal")))E->Metallic.Connect(1,Sample(M,T,SAMPLERTYPE_LinearGrayscale));
        if(auto* T=Maps.FindRef(TEXT("ao")))E->AmbientOcclusion.Connect(1,Sample(M,T,SAMPLERTYPE_LinearGrayscale));
        if(Masked)
        {
            auto* T=Maps.FindRef(TEXT("opacity"));if(!T)return nullptr;
            M->BlendMode=BLEND_Masked;M->TwoSided=true;M->SetShadingModel(MSM_TwoSidedFoliage);M->OpacityMaskClipValue=.4f;
            E->OpacityMask.Connect(1,Sample(M,T,SAMPLERTYPE_LinearGrayscale));
            auto* Sub=Expr<UMaterialExpressionMultiply>(M);Sub->A.Connect(0,D);Sub->B.Connect(0,Scalar(M,.45f));E->SubsurfaceColor.Connect(0,Sub);
        }
        M->PostEditChange();return Save(M)?M:nullptr;
    }
    UMaterial* TiledMaterial(const FString& Name,const TMap<FString,UTexture2D*>& Maps,float Size,FLinearColor Tint,float Metal=0,float NormalStrength=.5f)
    {
        auto* M=NewMaterial(Name);M->bTangentSpaceNormal=false;
        auto* C=Expr<UMaterialExpressionCustom>(M);C->Description=TEXT("World-scaled market PBR, no stretched facade textures");C->OutputType=CMOT_Float3;
        auto Input=[&](const TCHAR* Name,UMaterialExpression* E){FCustomInput I;I.InputName=Name;I.Input.Connect(0,E);C->Inputs.Add(I);};
        for(const auto& Pair:TMap<FString,FString>{{TEXT("D"),TEXT("diff")},{TEXT("N"),TEXT("normal")},{TEXT("R"),TEXT("rough")}})
        {
            auto* T=Maps.FindRef(Pair.Value);if(!T)return nullptr;
            auto* O=Expr<UMaterialExpressionTextureObject>(M);O->Texture=T;O->SamplerType=Pair.Key==TEXT("D")?SAMPLERTYPE_Color:(Pair.Key==TEXT("N")?SAMPLERTYPE_Normal:SAMPLERTYPE_LinearGrayscale);
            Input(*Pair.Key,O);
        }
        Input(TEXT("P"),Expr<UMaterialExpressionWorldPosition>(M));Input(TEXT("V"),Expr<UMaterialExpressionVertexNormalWS>(M));
        Input(TEXT("Tint"),Color(M,Tint));Input(TEXT("Size"),Scalar(M,Size));Input(TEXT("Strength"),Scalar(M,NormalStrength));
        FCustomOutput Rough;Rough.OutputName=TEXT("RoughOut");Rough.OutputType=CMOT_Float1;C->AdditionalOutputs.Add(Rough);
        FCustomOutput Norm;Norm.OutputName=TEXT("NormalOut");Norm.OutputType=CMOT_Float3;C->AdditionalOutputs.Add(Norm);
        C->Code=TEXT(R"HLSL(
float3 p=P/max(Size,1.0); float3 w=pow(abs(V),4.0);w/=max(dot(w,1.0),0.001);
float3 col=Texture2DSample(D,DSampler,p.yz).rgb*w.x+Texture2DSample(D,DSampler,p.xz).rgb*w.y+Texture2DSample(D,DSampler,p.xy).rgb*w.z;
RoughOut=clamp(Texture2DSample(R,RSampler,p.yz).r*w.x+Texture2DSample(R,RSampler,p.xz).r*w.y+Texture2DSample(R,RSampler,p.xy).r*w.z,.20,.96);
float2 nx=(Texture2DSample(N,NSampler,p.yz).rg*2-1)*Strength;
float2 ny=(Texture2DSample(N,NSampler,p.xz).rg*2-1)*Strength;
float2 nz=(Texture2DSample(N,NSampler,p.xy).rg*2-1)*Strength;
NormalOut=normalize(float3(sign(V.x)*sqrt(saturate(1-dot(nx,nx))),nx.x,nx.y)*w.x+float3(ny.x,sign(V.y)*sqrt(saturate(1-dot(ny,ny))),ny.y)*w.y+float3(nz.x,nz.y,sign(V.z)*sqrt(saturate(1-dot(nz,nz))))*w.z);
return col*Tint.rgb;
)HLSL");
        if(Name==TEXT("M_MarketPlaster"))C->Code.ReplaceInline(TEXT("return col*Tint.rgb;"),TEXT("return lerp(col*Tint.rgb,float3(.68,.60,.47),.68);"));
        C->RebuildOutputs();auto* E=M->GetEditorOnlyData();E->BaseColor.Connect(0,C);E->Roughness.Connect(1,C);E->Normal.Connect(2,C);E->Metallic.Connect(0,Scalar(M,Metal));
        M->PostEditChange();return Save(M)?M:nullptr;
    }
    UMaterial* Solid(const FString& Name,FLinearColor Tint,float Metal,float Rough)
    {
        auto* M=NewMaterial(Name);auto* E=M->GetEditorOnlyData();E->BaseColor.Connect(0,Color(M,Tint));E->Roughness.Connect(0,Scalar(M,Rough));E->Metallic.Connect(0,Scalar(M,Metal));
        M->PostEditChange();return Save(M)?M:nullptr;
    }
    UMaterial* Fabric(UTexture2D* Albedo)
    {
        auto* M=NewMaterial(TEXT("M_MarketCloth"));M->TwoSided=true;M->SetShadingModel(MSM_TwoSidedFoliage);
        auto* E=M->GetEditorOnlyData();auto* D=Sample(M,Albedo,SAMPLERTYPE_Color);
        auto* Pattern=Expr<UMaterialExpressionMultiply>(M);Pattern->A.Connect(0,D);Pattern->B.Connect(0,Scalar(M,.65f));
        auto* Dyed=Expr<UMaterialExpressionAdd>(M);Dyed->A.Connect(0,Pattern);Dyed->B.Connect(0,Color(M,FLinearColor(.01f,.025f,.065f)));E->BaseColor.Connect(0,Dyed);
        E->Roughness.Connect(0,Scalar(M,.9f));E->Opacity.Connect(0,Scalar(M,.38f));
        auto* Sub=Expr<UMaterialExpressionMultiply>(M);Sub->A.Connect(0,Dyed);Sub->B.Connect(0,Scalar(M,.65f));E->SubsurfaceColor.Connect(0,Sub);
        auto* N=LoadObject<UTexture2D>(nullptr,TEXT("/Game/BorrowedSun/City/Textures/T_denmin_fabric_02_nor_dx.T_denmin_fabric_02_nor_dx"));
        if(N){auto* Weave=Expr<UMaterialExpressionMultiply>(M);Weave->A.Connect(0,Sample(M,N,SAMPLERTYPE_Normal));Weave->B.Connect(0,Color(M,FLinearColor(.15f,.15f,1.f)));E->Normal.Connect(0,Weave);}
        M->PostEditChange();return Save(M)?M:nullptr;
    }
    bool ImportMesh(const FString& Name,const FString& Relative,const TMap<FString,UMaterial*>& Materials,bool Nanite)
    {
        auto* P=CreatePackage(*(Base+TEXT("Meshes/")+Name));auto* Factory=NewObject<UFbxFactory>();Factory->SetDetectImportTypeOnImport(false);
        auto* Task=NewObject<UAssetImportTask>();Task->bAutomated=true;Factory->SetAssetImportTask(Task);
        auto* UI=Factory->ImportUI.Get();UI->MeshTypeToImport=FBXIT_StaticMesh;UI->bAutomatedImportShouldDetectType=false;UI->bImportMaterials=false;UI->bImportTextures=false;
        UI->StaticMeshImportData->bCombineMeshes=true;
        UI->StaticMeshImportData->bAutoGenerateCollision=Name==TEXT("SM_MarketTable")||Name==TEXT("SM_CarvedPlanter")||Name==TEXT("SM_wooden_crate_02")||Name==TEXT("SM_wooden_stool_01")||Name==TEXT("SM_WorkshopBench")||Name==TEXT("SM_WorkshopRack");
        UI->StaticMeshImportData->NormalImportMethod=FBXNIM_ImportNormals;
        auto* SM=Cast<UStaticMesh>(UFactory::StaticImportObject(UStaticMesh::StaticClass(),P,*Name,RF_Public|RF_Standalone,*Source(Relative),nullptr,Factory));
        if(!SM)return false;
        for(int I=0;I<SM->GetStaticMaterials().Num();++I)
        {
            const FString Slot=SM->GetStaticMaterials()[I].MaterialSlotName.ToString();UMaterial* Match=nullptr;
            if(Materials.Num()==1)for(const auto& Pair:Materials)Match=Pair.Value;
            else for(const auto& Pair:Materials)if(Slot.Equals(Pair.Key,ESearchCase::IgnoreCase)||Slot.EndsWith(TEXT("_")+Pair.Key,ESearchCase::IgnoreCase)){Match=Pair.Value;break;}
            if(!Match){UE_LOG(LogTemp,Error,TEXT("No reviewed material for %s slot %s"),*Name,*Slot);return false;}
            SM->SetMaterial(I,Match);UE_LOG(LogTemp,Display,TEXT("Market slot %s: %s -> %s"),*Name,*Slot,*Match->GetName());
        }
        SM->NaniteSettings.bEnabled=Nanite;
        SM->NaniteSettings.ShapePreservation=Name==TEXT("SM_tree_small_02")?ENaniteShapePreservation::PreserveArea:ENaniteShapePreservation::None;
        SM->PostEditChange();
        UE_LOG(LogTemp,Display,TEXT("Market mesh %s: bounds cm %s; Nanite %d"),*Name,*SM->GetBoundingBox().GetSize().ToString(),Nanite);
        return Save(SM);
    }

    bool ImportWorkshop()
    {
        const auto Manifest=Read(TEXT("SourceArt/LanternWorkshop/workshop-manifest.json"));
        if(!Manifest)return false;
        TMap<FString,UMaterial*> Materials;
        for(const TCHAR* Key:{TEXT("Wood"),TEXT("Brass"),TEXT("Copper"),TEXT("Shadow")})
        {
            const FString Name=FString(TEXT("M_Market"))+Key;
            auto* M=LoadObject<UMaterial>(nullptr,*(Base+TEXT("Materials/")+Name+TEXT(".")+Name));
            if(!M)return false;Materials.Add(Key,M);
        }
        Materials.Add(TEXT("Enamel"),Solid(TEXT("M_WorkshopEnamel"),FLinearColor(.026f,.12f,.16f),.25f,.31f));
        Materials.Add(TEXT("Iron"),Solid(TEXT("M_WorkshopIron"),FLinearColor(.075f,.085f,.085f),.8f,.52f));
        Materials.Add(TEXT("Ivory"),Solid(TEXT("M_WorkshopIvory"),FLinearColor(.72f,.60f,.36f),0,.57f));
        Materials.Add(TEXT("MilkGlass"),Solid(TEXT("M_WorkshopMilkGlass"),FLinearColor(.82f,.68f,.39f),0,.25f));
        for(const auto& Pair:Materials)if(!Pair.Value)return false;
        for(const auto& Entry:Manifest->GetArrayField(TEXT("assets")))
        {
            const auto A=Entry->AsObject();
            if(!ImportMesh(A->GetStringField(TEXT("name")),A->GetStringField(TEXT("path")),Materials,true))return false;
        }
        const auto Definition=Read(Manifest->GetStringField(TEXT("assembly")));
        if(!Definition)return false;
        const FString Name=Definition->GetStringField(TEXT("name"));
        if(Name!=TEXT("DA_SecondDawnWorkshop"))return false;
        auto* Package=CreatePackage(*(Base+TEXT("Assemblies/")+Name));
        auto* Assembly=NewObject<UBSMarketAssembly>(Package,*Name,RF_Public|RF_Standalone);
        Assembly->AssemblyId=FName(*Definition->GetStringField(TEXT("id")));
        for(const auto& Entry:Definition->GetArrayField(TEXT("parts")))
        {
            const auto P=Entry->AsObject();FBSMarketAssemblyPart Part;
            auto ReadVector=[&](const TCHAR* Field,FVector& Out)
            {
                const TArray<TSharedPtr<FJsonValue>>* Array=nullptr;
                if(!P->TryGetArrayField(Field,Array)||Array->Num()!=3)return false;
                double V[3];for(int I=0;I<3;++I)if(!(*Array)[I]->TryGetNumber(V[I])||!FMath::IsFinite(V[I]))return false;
                Out=FVector(V[0],V[1],V[2]);return true;
            };
            FVector Position,Scale,Rotation;
            if(!ReadVector(TEXT("position"),Position)||!ReadVector(TEXT("scale"),Scale)||!ReadVector(TEXT("rotation"),Rotation))return false;
            const FString Mesh=P->GetStringField(TEXT("mesh"));
            Part.Key=FName(*P->GetStringField(TEXT("key")));
            Part.Mesh=LoadObject<UStaticMesh>(nullptr,*(Base+TEXT("Meshes/")+Mesh+TEXT(".")+Mesh));
            Part.LocalTransform=FTransform(FRotator(Rotation.X,Rotation.Y,Rotation.Z),Position,Scale);
            Part.bCollision=P->GetBoolField(TEXT("collision"));Assembly->Parts.Add(Part);
        }
        FString Reason;
        if(!Assembly->Validate(Reason)){UE_LOG(LogTemp,Error,TEXT("Workshop rejected: %s"),*Reason);return false;}
        UE_LOG(LogTemp,Display,TEXT("Second Dawn workshop: %d validated editable parts"),Assembly->Parts.Num());
        return Save(Assembly);
    }

    int32 WriteReport()
    {
        TArray<TSharedPtr<FJsonValue>> Paths;for(const auto& Path:SavedAssets)Paths.Add(MakeShared<FJsonValueString>(Path));
        TSharedPtr<FJsonObject> Result=MakeShared<FJsonObject>();Result->SetArrayField(TEXT("assets"),Paths);Result->SetNumberField(TEXT("assetCount"),SavedAssets.Num());Result->SetBoolField(TEXT("success"),true);
        FString Text;FJsonSerializer::Serialize(Result.ToSharedRef(),TJsonWriterFactory<>::Create(&Text));
        IFileManager::Get().MakeDirectory(*(FPaths::ProjectSavedDir()/TEXT("MarketAuthoring")),true);
        if(!FFileHelper::SaveStringToFile(Text,*(FPaths::ProjectSavedDir()/TEXT("MarketAuthoring/market-assets.json"))))return 9;
        UE_LOG(LogTemp,Display,TEXT("Market authoring complete: %d packages"),SavedAssets.Num());return 0;
    }
}

UBSMarketArtCommandlet::UBSMarketArtCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;ShowErrorCount=true;}
int32 UBSMarketArtCommandlet::Main(const FString& Params)
{
    using namespace BSMarketArt;SavedAssets.Reset();
    if(IFileManager::Get().DirectoryExists(*(FPaths::ProjectContentDir()/TEXT("BorrowedSun/Market")))&&!FParse::Param(*Params,TEXT("Force")))return 2;
    if(FParse::Param(*Params,TEXT("WorkshopOnly")))return ImportWorkshop()?WriteReport():10;
    const auto Intake=Read(TEXT("Docs/Registers/market-asset-intake.json"));const auto Kit=Read(TEXT("SourceArt/MarketKit/market-kit-manifest.json"));
    if(!Intake||!Kit)return 3;
    TMap<FString,TMap<FString,UTexture2D*>> Surfaces;TMap<FString,UMaterial*> KitMats;
    for(const auto& Value:Intake->GetArrayField(TEXT("assets")))
    {
        const auto Asset=Value->AsObject();const FString Id=Asset->GetStringField(TEXT("id"));const bool Model=Asset->GetStringField(TEXT("type"))==TEXT("model");
        TMap<FString,UMaterial*> PropMats;bool MaskedModel=false;
        for(const auto& MatValue:Asset->GetArrayField(TEXT("materials")))
        {
            const auto Definition=MatValue->AsObject();const FString Key=Definition->GetStringField(TEXT("name"));TMap<FString,UTexture2D*> Maps;
            MaskedModel|=Definition->GetBoolField(TEXT("masked"));
            for(const FString& Kind:TArray<FString>{TEXT("diff"),TEXT("normal"),TEXT("rough"),TEXT("metal"),TEXT("ao"),TEXT("opacity")})
            {
                FString Path;if(!Definition->GetObjectField(TEXT("maps"))->TryGetStringField(Kind,Path))continue;
                auto* T=Texture(TEXT("T_")+Id+TEXT("_")+Key+TEXT("_")+Kind,Path,Kind,Definition->GetBoolField(TEXT("flipNormalGreen")));
                if(!T)return 4;Maps.Add(Kind,T);
            }
            if(Model)
            {
                auto* M=UvMaterial(TEXT("M_")+Id+TEXT("_")+Key,Maps,Definition->GetBoolField(TEXT("masked")));if(!M)return 5;PropMats.Add(Key,M);
            }
            else Surfaces.Add(Id,Maps);
        }
        if(Model)
        {
            FString File;for(const auto& F:Asset->GetArrayField(TEXT("files")))if(F->AsObject()->GetStringField(TEXT("kind"))==TEXT("mesh"))File=F->AsObject()->GetStringField(TEXT("path"));
            if(File.IsEmpty()||!ImportMesh(TEXT("SM_")+Id,File,PropMats,!MaskedModel||Id==TEXT("tree_small_02")))return 6;
        }
    }
    KitMats.Add(TEXT("Stone"),TiledMaterial(TEXT("M_MarketStone"),Surfaces.FindChecked(TEXT("sandstone_cracks")),220,FLinearColor(1.50f,1.47f,1.32f),0,.25f));
    KitMats.Add(TEXT("Plaster"),TiledMaterial(TEXT("M_MarketPlaster"),Surfaces.FindChecked(TEXT("worn_cracked_plaster")),420,FLinearColor(1.75f,1.60f,1.34f),0,.20f));
    KitMats.Add(TEXT("Wood"),TiledMaterial(TEXT("M_MarketWood"),Surfaces.FindChecked(TEXT("medieval_wood")),280,FLinearColor(.85f,.69f,.5f),0,.45f));
    KitMats.Add(TEXT("Brass"),Solid(TEXT("M_MarketBrass"),FLinearColor(.65f,.38f,.11f),.85f,.32f));
    KitMats.Add(TEXT("Copper"),TiledMaterial(TEXT("M_MarketCopper"),Surfaces.FindChecked(TEXT("worn_cracked_plaster")),170,FLinearColor(.16f,.34f,.27f),.65f,.12f));
    KitMats.Add(TEXT("Shadow"),Solid(TEXT("M_MarketShadow"),FLinearColor(.025f,.035f,.039f),0,.96f));
    KitMats.Add(TEXT("Ceramic"),Solid(TEXT("M_MarketCeramic"),FLinearColor(.36f,.12f,.036f),0,.29f));
    auto* Cloth=Texture(TEXT("T_IndigoSunWeave"),TEXT("SourceArt/Generated/IndigoSunWeave.png"),TEXT("diff"));if(!Cloth)return 7;
    KitMats.Add(TEXT("Cloth"),Fabric(Cloth));
    auto* Floor=TiledMaterial(TEXT("M_MarketPaving"),Surfaces.FindChecked(TEXT("monastery_stone_floor")),400,FLinearColor(1.15f,1.06f,.94f),0,.3f);
    if(!Floor)return 7;for(const auto& Pair:KitMats)if(!Pair.Value)return 7;
    for(const auto& Value:Kit->GetArrayField(TEXT("assets")))
    {
        const auto Asset=Value->AsObject();const FString Name=Asset->GetStringField(TEXT("name"));auto Materials=KitMats;
        if(Name.Contains(TEXT("Flagstone")))Materials[TEXT("Stone")]=Floor;
        const bool Nanite=!Name.Contains(TEXT("Canopy"))&&!Name.Contains(TEXT("Banner"))&&!Name.Contains(TEXT("Sack"))&&!Name.Contains(TEXT("Drape"));
        if(!ImportMesh(Name,Asset->GetStringField(TEXT("path")),Materials,Nanite))return 8;
    }
    if(!ImportWorkshop())return 10;
    return WriteReport();
}
