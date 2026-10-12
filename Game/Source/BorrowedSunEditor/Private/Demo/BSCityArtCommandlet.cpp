#include "Demo/BSCityArtCommandlet.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxStaticMeshImportData.h"
#include "AssetImportTask.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionVertexNormalWS.h"
#include "Materials/MaterialExpressionWorldPosition.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"

namespace BSCityArt
{
    const FString Base = TEXT("/Game/BorrowedSun/City/");
    bool Save(UObject* Asset)
    {
        if (!Asset) return false;
        auto* Package = Asset->GetOutermost();
        Package->MarkAsFullyLoaded();
        Package->MarkPackageDirty();
        const FString File = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args; Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(Package, Asset, *File, Args);
    }

    template<class T> T* Expr(UMaterial* M)
    {
        T* E=NewObject<T>(M); E->Material=M;
        E->UpdateMaterialExpressionGuid(true,false);
        M->GetExpressionCollection().AddExpression(E);
        return E;
    }

    UTexture2D* Import(const FString& Asset, const FString& Suffix, bool Normal, bool Color, const FString& Resolution=TEXT("2k"), const FString& Extension=TEXT("jpg"))
    {
        const FString Name = TEXT("T_") + Asset + TEXT("_") + Suffix;
        const FString Filename=FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()/TEXT("../SourceArt/PolyHaven")/Asset/(Asset+TEXT("_")+Suffix+TEXT("_")+Resolution+TEXT(".")+Extension));
        auto* P=CreatePackage(*(Base+TEXT("Textures/")+Name));
        auto* Factory=NewObject<UTextureFactory>(); Factory->SuppressImportOverwriteDialog();
        auto* T=Cast<UTexture2D>(UFactory::StaticImportObject(UTexture2D::StaticClass(),P,*Name,RF_Public|RF_Standalone,*Filename,nullptr,Factory));
        if (!T) return nullptr;
        T->SRGB=Color; T->CompressionSettings=Normal?TC_Normalmap:(Color?TC_Default:TC_Grayscale);
        T->LODGroup=Normal?TEXTUREGROUP_WorldNormalMap:TEXTUREGROUP_World;
        T->MaxTextureSize=2048; T->PostEditChange();
        return Save(T)?T:nullptr;
    }

    bool ImportProp(const FString& Id, bool Masked)
    {
        UTexture2D* Diff=Import(Id,TEXT("diff"),false,true,TEXT("1k"));
        UTexture2D* Normal=Import(Id,TEXT("nor_dx"),true,false,TEXT("1k"));
        UTexture2D* Rough=Import(Id,TEXT("rough"),false,false,TEXT("1k"));
        UTexture2D* Metal=Import(Id,TEXT("metal"),false,false,TEXT("1k"));
        UTexture2D* Opacity=Masked?Import(Id,TEXT("opacity"),false,false,TEXT("1k"),TEXT("png")):nullptr;
        if(!Diff||!Normal||!Rough||!Metal||(Masked&&!Opacity))return false;
        const FString MatName=TEXT("M_")+Id;
        auto* MP=CreatePackage(*(Base+TEXT("Materials/")+MatName));
        auto* Mat=NewObject<UMaterial>(MP,*MatName,RF_Public|RF_Standalone);
        Mat->PreEditChange(nullptr);Mat->bUsedWithInstancedStaticMeshes=true;
        auto Sample=[&](UTexture2D* T,EMaterialSamplerType Type){auto* S=Expr<UMaterialExpressionTextureSample>(Mat);S->Texture=T;S->SamplerType=Type;return S;};
        auto* E=Mat->GetEditorOnlyData();E->BaseColor.Connect(0,Sample(Diff,SAMPLERTYPE_Color));E->Normal.Connect(0,Sample(Normal,SAMPLERTYPE_Normal));
        E->Roughness.Connect(1,Sample(Rough,SAMPLERTYPE_LinearGrayscale));E->Metallic.Connect(1,Sample(Metal,SAMPLERTYPE_LinearGrayscale));
        if(Masked){Mat->BlendMode=BLEND_Masked;Mat->TwoSided=true;Mat->OpacityMaskClipValue=.4f;E->OpacityMask.Connect(1,Sample(Opacity,SAMPLERTYPE_LinearGrayscale));}
        Mat->PostEditChange();if(!Save(Mat))return false;
        const FString MeshName=TEXT("SM_")+Id;
        auto* P=CreatePackage(*(Base+TEXT("Meshes/")+MeshName));
        auto* Factory=NewObject<UFbxFactory>();Factory->SetDetectImportTypeOnImport(false);
        auto* Task=NewObject<UAssetImportTask>();Task->bAutomated=true;Factory->SetAssetImportTask(Task);
        Factory->ImportUI->MeshTypeToImport=FBXIT_StaticMesh;Factory->ImportUI->bAutomatedImportShouldDetectType=false;
        Factory->ImportUI->bImportMaterials=false;Factory->ImportUI->bImportTextures=false;
        Factory->ImportUI->StaticMeshImportData->bCombineMeshes=true;Factory->ImportUI->StaticMeshImportData->bAutoGenerateCollision=false;
        const FString File=FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()/TEXT("../SourceArt/PolyHaven")/Id/(Id+TEXT("_1k.fbx")));
        auto* SM=Cast<UStaticMesh>(UFactory::StaticImportObject(UStaticMesh::StaticClass(),P,*MeshName,RF_Public|RF_Standalone,*File,nullptr,Factory));
        if(!SM)return false;
        for(int I=0;I<SM->GetStaticMaterials().Num();++I)SM->SetMaterial(I,Mat);
        UE_LOG(LogTemp,Display,TEXT("Imported CC0 prop %s, bounds cm: %s, slots %d"),*Id,*SM->GetBoundingBox().GetSize().ToString(),SM->GetStaticMaterials().Num());
        return Save(SM);
    }

    bool Material(const FString& Name, UTexture2D* Diff, UTexture2D* Normal, UTexture2D* Rough, float Size, float Metallic, bool TwoSided)
    {
        auto* P=CreatePackage(*(Base+TEXT("Materials/")+Name));
        auto* M=NewObject<UMaterial>(P,*Name,RF_Public|RF_Standalone);
        M->PreEditChange(nullptr); M->bUsedWithInstancedStaticMeshes=true;
        M->TwoSided=TwoSided; M->bTangentSpaceNormal=false;
        auto* D=Expr<UMaterialExpressionTextureObject>(M); D->Texture=Diff; D->SamplerType=SAMPLERTYPE_Color;
        auto* N=Expr<UMaterialExpressionTextureObject>(M); N->Texture=Normal; N->SamplerType=SAMPLERTYPE_Normal;
        auto* R=Expr<UMaterialExpressionTextureObject>(M); R->Texture=Rough; R->SamplerType=SAMPLERTYPE_LinearGrayscale;
        auto* Pos=Expr<UMaterialExpressionWorldPosition>(M);
        auto* Norm=Expr<UMaterialExpressionVertexNormalWS>(M);
        auto* Tint=Expr<UMaterialExpressionVectorParameter>(M); Tint->ParameterName=TEXT("Tint"); Tint->DefaultValue=FLinearColor::White; Tint->UpdateParameterGuid(true,false);
        auto* Scale=Expr<UMaterialExpressionScalarParameter>(M); Scale->ParameterName=TEXT("TileSize"); Scale->DefaultValue=Size; Scale->UpdateParameterGuid(true,false);
        auto* C=Expr<UMaterialExpressionCustom>(M); C->Description=TEXT("World-scale PBR with BC5 normal reorientation"); C->OutputType=CMOT_Float3;
        auto Input=[&](FName Key, UMaterialExpression* E){FCustomInput I;I.InputName=Key;I.Input.Connect(0,E);C->Inputs.Add(I);};
        Input(TEXT("D"),D); Input(TEXT("N"),N); Input(TEXT("R"),R); Input(TEXT("P"),Pos); Input(TEXT("V"),Norm); Input(TEXT("Tint"),Tint); Input(TEXT("Size"),Scale);
        FCustomOutput RoughOut; RoughOut.OutputName=TEXT("RoughOut"); RoughOut.OutputType=CMOT_Float1; C->AdditionalOutputs.Add(RoughOut);
        FCustomOutput NormalOut; NormalOut.OutputName=TEXT("NormalOut"); NormalOut.OutputType=CMOT_Float3; C->AdditionalOutputs.Add(NormalOut);
        C->Code=TEXT(R"HLSL(
float3 p=P/max(Size,1.0); float3 w=pow(abs(V),4.0); w/=max(dot(w,1.0),0.001);
float3 color=Texture2DSample(D,DSampler,p.yz).rgb*w.x+Texture2DSample(D,DSampler,p.xz).rgb*w.y+Texture2DSample(D,DSampler,p.xy).rgb*w.z;
RoughOut=clamp(Texture2DSample(R,RSampler,p.yz).r*w.x+Texture2DSample(R,RSampler,p.xz).r*w.y+Texture2DSample(R,RSampler,p.xy).r*w.z,0.22,0.95);
float2 nx=Texture2DSample(N,NSampler,p.yz).rg*2-1;
float2 ny=Texture2DSample(N,NSampler,p.xz).rg*2-1;
float2 nz=Texture2DSample(N,NSampler,p.xy).rg*2-1;
nx*=0.45; ny*=0.45; nz*=0.45;
float3 a=float3(sign(V.x)*sqrt(saturate(1-dot(nx,nx))),nx.x,nx.y);
float3 b=float3(ny.x,sign(V.y)*sqrt(saturate(1-dot(ny,ny))),ny.y);
float3 c=float3(nz.x,nz.y,sign(V.z)*sqrt(saturate(1-dot(nz,nz))));
NormalOut=normalize(a*w.x+b*w.y+c*w.z);
return color*Tint.rgb;
)HLSL");
        C->RebuildOutputs();
        auto* E=M->GetEditorOnlyData(); E->BaseColor.Connect(0,C); E->Roughness.Connect(1,C); E->Normal.Connect(2,C);
        E->Metallic.UseConstant=true; E->Metallic.Constant=Metallic;
        M->AddExpressionParameter(Tint,M->EditorParameters); M->AddExpressionParameter(Scale,M->EditorParameters);
        M->PostEditChange(); return Save(M);
    }

    struct Mesh
    {
        FMeshDescription Data; FMeshDescriptionBuilder Builder; FPolygonGroupID Group;
        Mesh(){FStaticMeshAttributes A(Data);A.Register();Builder.SetMeshDescription(&Data);Builder.SetNumUVLayers(1);Group=Builder.AppendPolygonGroup(TEXT("Surface"));}
        void Tri(FVector A,FVector B,FVector C)
        {
            const FVector Normal=FVector::CrossProduct(B-A,C-A).GetSafeNormal();
            if(Normal.IsNearlyZero()) return;
            FVertexInstanceID V[3]; const FVector P[3]={A,B,C};
            for(int I=0;I<3;++I){V[I]=Builder.AppendInstance(Builder.AppendVertex(P[I]));Builder.SetInstanceNormal(V[I],Normal);Builder.SetInstanceUV(V[I],FVector2D(P[I].X/100,P[I].Z/100));}
            Builder.AppendTriangle(V[0],V[1],V[2],Group);
        }
        void Quad(FVector A,FVector B,FVector C,FVector D){Tri(A,B,C);Tri(A,C,D);}
        void Box(FVector P,FVector Half,FRotator Rot=FRotator::ZeroRotator)
        {
            FVector V[8];for(int I=0;I<8;++I)V[I]=P+Rot.RotateVector(FVector((I&1)?Half.X:-Half.X,(I&2)?Half.Y:-Half.Y,(I&4)?Half.Z:-Half.Z));
            Quad(V[0],V[4],V[6],V[2]);Quad(V[1],V[3],V[7],V[5]);Quad(V[0],V[1],V[5],V[4]);Quad(V[2],V[6],V[7],V[3]);Quad(V[0],V[2],V[3],V[1]);Quad(V[4],V[5],V[7],V[6]);
        }
        void Lathe(const TArray<FVector2D>& Profile,int Sides=32)
        {
            for(int I=0;I<Sides;++I)for(int J=0;J<Profile.Num()-1;++J){
                const double A=I*2*PI/Sides,B=(I+1)*2*PI/Sides;
                auto Point=[](FVector2D V,double T){return FVector(V.X*FMath::Cos(T),V.X*FMath::Sin(T),V.Y);};
                Quad(Point(Profile[J],A),Point(Profile[J],B),Point(Profile[J+1],B),Point(Profile[J+1],A));
            }
        }
        bool Write(const FString& Name)
        {
            auto* P=CreatePackage(*(Base+TEXT("Meshes/")+Name));
            auto* SM=NewObject<UStaticMesh>(P,*Name,RF_Public|RF_Standalone);
            SM->GetStaticMaterials().Add(FStaticMaterial(nullptr,TEXT("Surface")));
            UStaticMesh::FBuildMeshDescriptionsParams Params; Params.bBuildSimpleCollision=false; Params.bFastBuild=false;
            if(!SM->BuildFromMeshDescriptions({&Data},Params))return false;
            SM->SetLightMapResolution(32); SM->MarkPackageDirty();
            UE_LOG(LogTemp,Display,TEXT("City mesh %s: %d triangles"),*Name,Data.Triangles().Num());
            return Save(SM);
        }
    };

    bool BuildMeshes()
    {
        Mesh Arch;
        Arch.Box(FVector(-170,0,150),FVector(30,40,150)); Arch.Box(FVector(170,0,150),FVector(30,40,150));
        for(int I=0;I<24;++I){
            const double A=I*PI/24,B=(I+1)*PI/24;
            auto P=[](double R,double T,double Y){return FVector(R*FMath::Cos(T),Y,300+R*FMath::Sin(T));};
            Arch.Quad(P(140,A,-40),P(140,B,-40),P(200,B,-40),P(200,A,-40));
            Arch.Quad(P(140,A,40),P(200,A,40),P(200,B,40),P(140,B,40));
            Arch.Quad(P(200,A,-40),P(200,B,-40),P(200,B,40),P(200,A,40));
            Arch.Quad(P(140,A,-40),P(140,A,40),P(140,B,40),P(140,B,-40));
        }
        if(!Arch.Write(TEXT("SM_Arcade400")))return false;
        Mesh Column; Column.Lathe({{0,0},{50,0},{50,20},{39,32},{32,55},{26,75},{26,340},{34,360},{46,375},{50,385},{50,400},{0,400}});
        if(!Column.Write(TEXT("SM_Column400")))return false;
        Mesh Dome; TArray<FVector2D> DomeProfile; DomeProfile.Add({0,0});DomeProfile.Add({100,0});DomeProfile.Add({100,10});
        for(int I=0;I<=18;++I){double A=I*PI/36;DomeProfile.Add({96*FMath::Cos(A),10+110*FMath::Sin(A)});}Dome.Lathe(DomeProfile,40);
        if(!Dome.Write(TEXT("SM_CopperDome")))return false;
        Mesh Jali; Jali.Box({-120,0,130},{8,6,138});Jali.Box({120,0,130},{8,6,138});Jali.Box({0,0,0},{120,6,8});Jali.Box({0,0,260},{120,6,8});
        for(int X=-90;X<=90;X+=60)for(int Z=40;Z<=220;Z+=60){
            const FVector P(X,0,Z); const FVector A(0,0,28),B(28,0,0);
            auto Beam=[&](FVector From,FVector To){FVector D=To-From;Jali.Box((From+To)*.5,FVector(D.Size()*.5,5,4),D.Rotation());};
            Beam(P+A,P+B);Beam(P+B,P-A);Beam(P-A,P-B);Beam(P-B,P+A);
        }
        if(!Jali.Write(TEXT("SM_Jali240")))return false;
        Mesh Canopy;
        for(int X=0;X<16;++X)for(int Y=0;Y<12;++Y){
            auto P=[](double U,double V){return FVector((U-.5)*100,(V-.5)*100,-14*FMath::Sin(U*PI)*FMath::Sin(V*PI)+2*FMath::Sin(U*6*PI)*FMath::Sin(V*PI));};
            Canopy.Quad(P(X/16.,Y/12.),P((X+1)/16.,Y/12.),P((X+1)/16.,(Y+1)/12.),P(X/16.,(Y+1)/12.));
        }
        if(!Canopy.Write(TEXT("SM_ShadeSail")))return false;
        Mesh Planter;Planter.Lathe({{0,0},{32,0},{38,7},{48,65},{54,68},{54,80},{43,80},{39,65},{26,10},{0,10}},24);
        if(!Planter.Write(TEXT("SM_Planter")))return false;
        Mesh Wing;
        const TArray<FVector> Ring={{0,0,0},{-20,0,18},{-28,0,55},{-20,0,90},{0,0,125},{20,0,90},{28,0,55},{20,0,18}};
        for(int I=0;I<Ring.Num();++I){int J=(I+1)%Ring.Num();Wing.Tri(FVector(0,-7,55),Ring[I],Ring[J]);Wing.Tri(FVector(0,7,55),Ring[J],Ring[I]);}
        if(!Wing.Write(TEXT("SM_MothWing")))return false;
        Mesh Leaf;
        for(int I=0;I<8;++I){auto P=[](double T,double Side){return FVector(T*100,Side*14*FMath::Sin(T*PI),FMath::Sin(T*PI)*24-T*T*25);};
            Leaf.Quad(P(I/8.,-1),P((I+1)/8.,-1),P((I+1)/8.,1),P(I/8.,1));}
        if(!Leaf.Write(TEXT("SM_PalmFrond")))return false;
        Mesh Trim;
        const TArray<FVector2D> Profile={{-50,-12},{50,-12},{50,-6},{44,-2},{44,2},{38,5},{38,12},{-50,12}};
        for(int I=0;I<Profile.Num();++I){int J=(I+1)%Profile.Num();FVector A(-50,Profile[I].X,Profile[I].Y),B(50,Profile[I].X,Profile[I].Y),C(50,Profile[J].X,Profile[J].Y),D(-50,Profile[J].X,Profile[J].Y);Trim.Quad(A,B,C,D);}
        for(int I=1;I<Profile.Num()-1;++I){Trim.Tri({-50,Profile[0].X,Profile[0].Y},{-50,Profile[I+1].X,Profile[I+1].Y},{-50,Profile[I].X,Profile[I].Y});Trim.Tri({50,Profile[0].X,Profile[0].Y},{50,Profile[I].X,Profile[I].Y},{50,Profile[I+1].X,Profile[I+1].Y});}
        if(!Trim.Write(TEXT("SM_Cornice")))return false;
        Mesh Mask;
        const TArray<FVector> Outline={{0,0,0},{-18,0,20},{-34,0,65},{-26,0,100},{0,0,120},{26,0,100},{34,0,65},{18,0,20}};
        for(int I=0;I<Outline.Num();++I){int J=(I+1)%Outline.Num();Mask.Tri({0,-30,72},Outline[I],Outline[J]);Mask.Tri({0,10,65},Outline[J],Outline[I]);}
        if(!Mask.Write(TEXT("SM_KeeperMask")))return false;
        Mesh Robe;
        const TArray<FVector2D> Folds={{0,0},{34,0},{33,7},{28,45},{22,60},{25,72},{20,89},{9,100},{0,100}};
        for(int I=0;I<96;++I)for(int J=0;J<Folds.Num()-1;++J){
            auto P=[](FVector2D F,double T){double R=F.X*(1+.065*FMath::Cos(T*16));return FVector(R*FMath::Cos(T),R*FMath::Sin(T),F.Y);};
            double A=I*2*PI/96,B=(I+1)*2*PI/96;
            Robe.Quad(P(Folds[J],A),P(Folds[J],B),P(Folds[J+1],B),P(Folds[J+1],A));
        }
        if(!Robe.Write(TEXT("SM_KeeperRobe")))return false;
        Mesh Solar;
        for(int I=0;I<6;++I){double A=(-4.4+I*8.8/6)*PI/180,B=(-4.4+(I+1)*8.8/6)*PI/180;
            auto P=[](double R,double A,double Y){return FVector(R*FMath::Cos(A),Y,R*FMath::Sin(A));};
            Solar.Quad(P(45,A,-1),P(100,A,-1),P(100,B,-1),P(45,B,-1));
            Solar.Quad(P(45,A,1),P(45,B,1),P(100,B,1),P(100,A,1));
            Solar.Quad(P(100,A,-1),P(100,A,1),P(100,B,1),P(100,B,-1));
        }
        return Solar.Write(TEXT("SM_SolarPetal"));
    }
}

UBSCityArtCommandlet::UBSCityArtCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;ShowErrorCount=true;}
int32 UBSCityArtCommandlet::Main(const FString& Params)
{
    using namespace BSCityArt;
    if(IFileManager::Get().DirectoryExists(*(FPaths::ProjectContentDir()/TEXT("BorrowedSun/City")))&&!FParse::Param(*Params,TEXT("Force"))){UE_LOG(LogTemp,Error,TEXT("City assets already exist. Use -Force only to deliberately regenerate owned City assets."));return 2;}
    struct Source{const TCHAR* Id;const TCHAR* Material;float Size;};
    const Source Sources[]={{TEXT("plastered_wall_02"),TEXT("M_CityPlaster"),220},{TEXT("seaworn_sandstone_brick"),TEXT("M_CityStone"),170},{TEXT("cobblestone_floor_08"),TEXT("M_CityPaving"),250},{TEXT("denmin_fabric_02"),TEXT("M_CityCloth"),160}};
    for(const auto& S:Sources){
        UTexture2D* D=Import(S.Id,TEXT("diff"),false,true);UTexture2D* N=Import(S.Id,TEXT("nor_dx"),true,false);UTexture2D* R=Import(S.Id,TEXT("rough"),false,false);
        if(!D||!N||!R||!Material(S.Material,D,N,R,S.Size,0,FString(S.Material).Contains(TEXT("Cloth"))))return 3;
        if(FString(S.Id)==TEXT("plastered_wall_02")&&!Material(TEXT("M_CityCopper"),D,N,R,140,.72f,false))return 3;
    }
    if(!BuildMeshes())return 4;
    if(!ImportProp(TEXT("brass_pot_01"),false)||!ImportProp(TEXT("brass_diya_lantern"),true))return 5;
    UE_LOG(LogTemp,Display,TEXT("City art authored: 21 reviewed CC0 textures, 7 PBR materials, 12 original meshes and 2 scanned props."));
    return 0;
}
