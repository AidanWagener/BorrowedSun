#include "Demo/BSDemoWorld.h"
#include "Demo/BSMarketAssembly.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Components/PointLightComponent.h"

void ABSDemoWorld::LoadMarketArt()
{
    for(const TCHAR* Name:{TEXT("SM_MarketArch_Round"),TEXT("SM_MarketArch_Tall"),TEXT("SM_MarketDoor"),TEXT("SM_MarketWindow"),TEXT("SM_MarketCorbel"),TEXT("SM_MarketBalcony"),TEXT("SM_MarketCanopy"),TEXT("SM_MarketBanner"),TEXT("SM_MarketTable"),TEXT("SM_SunseedReliquary"),TEXT("SM_FlagstonePatch_0"),TEXT("SM_FlagstonePatch_1"),TEXT("SM_FlagstonePatch_2"),TEXT("SM_CopperDownpipe"),TEXT("SM_DrainStrip"),TEXT("SM_CarvedPlanter"),TEXT("SM_WovenSackCluster"),TEXT("SM_wooden_crate_02"),TEXT("SM_wicker_basket_02"),TEXT("SM_food_pomegranate_01"),TEXT("SM_potted_plant_01"),TEXT("SM_ceramic_vase_02"),TEXT("SM_wooden_stool_01"),TEXT("SM_wooden_bowl_01"),TEXT("SM_brass_pan_01")})
        if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Game/BorrowedSun/Market/Meshes/%s.%s"),Name,Name)))MarketMeshes.Add(FName(Name),Mesh);
    for(const TCHAR* Name:{TEXT("M_MarketStone"),TEXT("M_MarketPlaster"),TEXT("M_MarketWood"),TEXT("M_MarketBrass"),TEXT("M_MarketCopper"),TEXT("M_MarketCloth"),TEXT("M_MarketShadow"),TEXT("M_MarketCeramic"),TEXT("M_MarketPaving")})
        if(auto* M=LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/BorrowedSun/Market/Materials/%s.%s"),Name,Name)))MarketMaterials.Add(FName(Name),M);
    for(const TCHAR* Name:{TEXT("SM_KeeperSculpt"),TEXT("SM_SolarHalo"),TEXT("SM_MarketDrape"),TEXT("SM_tree_small_02"),TEXT("SM_MarketLantern")})
        if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Game/BorrowedSun/Market/Meshes/%s.%s"),Name,Name)))MarketMeshes.Add(FName(Name),Mesh);
    UE_LOG(LogTemp,Display,TEXT("Starting market art: %d meshes, %d architectural materials"),MarketMeshes.Num(),MarketMaterials.Num());
    if(auto* Tree=MarketMeshes.FindRef(TEXT("SM_tree_small_02")).Get())UE_LOG(LogTemp,Display,TEXT("Market tree bounds: min %s max %s"),*Tree->GetBoundingBox().Min.ToString(),*Tree->GetBoundingBox().Max.ToString());
}

void ABSDemoWorld::MarketPart(FName Name,FVector P,FVector S,FRotator Rot)
{
    const bool Collision=Name==TEXT("SM_MarketTable")||Name==TEXT("SM_CarvedPlanter")||Name==TEXT("SM_wooden_crate_02")||Name==TEXT("SM_wooden_stool_01");
    if(auto* Mesh=MarketMeshes.Find(Name))Shape(*Mesh,P,S,FLinearColor::White,0,Collision,Rot,nullptr,true);
}

void ABSDemoWorld::MarketBox(FVector P,FVector S,FName Material,bool Collision)
{
    Shape(Cube,P,S,FLinearColor::White,0,Collision,FRotator::ZeroRotator,MarketMaterials.FindRef(Material));
}

void ABSDemoWorld::BuildStartingMarket()
{
    if(!HasMarketArt())return;
    const auto* Workshop=LoadObject<UBSMarketAssembly>(nullptr,TEXT("/Game/BorrowedSun/Market/Assemblies/DA_SecondDawnWorkshop.DA_SecondDawnWorkshop"));
    FString WorkshopError;
    const bool bValidWorkshop=Workshop&&Workshop->Validate(WorkshopError);
    const FLinearColor Brass(.65f,.40f,.14f),Rope(.08f,.055f,.03f);
    // A narrower street establishes human scale. The north break leads to the unchanged clinic.
    for(int Row=0;Row<2;++Row)for(int I=0;I<4;++I)
    {
        const double X=-3650+I*1030,Front=Row==0?-700.:900.,Out=Row==0?1.:-1.;
        const double H=1670+(I%3)*130+(Row?90:0);
        const FRotator R(0,Row==0?0:180,0);
        auto Part=[&](FName Name,double DX,double Depth,double Z,FVector S=FVector(1))
        {MarketPart(Name,{X+DX,Front+Out*Depth,Z},S,R);};
        const bool Passage=Row==1&&I==2;
        if(Passage)
        {
            MarketBox({X,Front-Out*355,(H+620)*.5},{9.7,7.1,(H-620)/100},TEXT("M_MarketPlaster"),true);
            MarketBox({-2090,Front-Out*355,310},{.85,7.1,6.2},TEXT("M_MarketStone"),true);
            MarketBox({-1260,Front-Out*355,310},{4.05,7.1,6.2},TEXT("M_MarketPlaster"),true);
            MarketPart(TEXT("SM_MarketArch_Tall"),{-1800,Front,8},{1.42,1.3,1.15},R);
        }
        else
        {
            MarketBox({X,Front-Out*355,H*.5},{9.7,7.1,H/100},TEXT("M_MarketPlaster"),true);
            MarketBox({X,Front+Out*4,60},{9.82,.28,1.2},TEXT("M_MarketStone"));
        }
        // Solid closed doors sit behind real arch depth; the tracery casts actual shadows.
        for(double DX:{-236.,236.})
        {
            if(!Passage)
            {
                Part(I%2?TEXT("SM_MarketArch_Tall"):TEXT("SM_MarketArch_Round"),DX,0,8,{.94,1,1.07});
                Part(TEXT("SM_MarketDoor"),DX,22,12,{.92,1,I%2?1.21:1.08});
            }
            for(int Floor=0;Floor<2;++Floor)
            {
                const double Z=665+Floor*480;
                Part(TEXT("SM_MarketWindow"),DX,26,Z,{1,1,.95});
                Part(TEXT("SM_MarketBalcony"),DX,75,Z-20,{1.03,1.2,1});
                for(double Support:{-140.,140.})Part(TEXT("SM_MarketCorbel"),DX+Support,55,Z-26,{.7,1,.85});
                if((Floor+I)%2==0)MarketPart(TEXT("SM_potted_plant_01"),{X+DX+130,Front+Out*84,Z+25},{1.25,1.25,1.25},FRotator(0,I*61,0));
            }
        }
        // Layered cornices and pilasters break broad planes without flattening the silhouette.
        for(double Z:{585.,1080.,H-40})
        {
            for(int Layer=0;Layer<3;++Layer)MarketBox({X,Front+Out*(20+Layer*10),Z+Layer*12},{9.95+Layer*.10,.50+Layer*.18,.12},TEXT("M_MarketStone"));
            for(int Tooth=-9;Tooth<=9;++Tooth)MarketBox({X+Tooth*51,Front+Out*36,Z-15},{.17,.38,.25},TEXT("M_MarketStone"));
        }
        for(double DX:{-477.,0.,477.})
        {
            const double Bottom=Passage?620:0;
            MarketBox({X+DX,Front+Out*8,(H+Bottom)*.5},{.22,.42,(H-Bottom)/100},TEXT("M_MarketStone"));
            for(int Course=Passage?7:0;Course<H/90;++Course)MarketBox({X+DX,Front+Out*25,Course*90.+44},{.32,.23,.018},TEXT("M_MarketBrass"));
        }
        for(int Segment=0;Segment<4;++Segment)Part(TEXT("SM_CopperDownpipe"),449,50,Segment*400.,{1,1,1});
        Part(TEXT("SM_MarketBanner"),0,120,1125,{.8,1,1.2});
        MarketBox({X,Front-Out*355,H+10},{10.2,7.6,.25},TEXT("M_MarketStone"));
        for(int B=-7;B<=7;++B)MarketBox({X+B*65,Front+Out*5,H+65},{.43,.45,.8},TEXT("M_MarketStone"));
        for(int P=-1;P<=1;++P)MarketPart(TEXT("SM_potted_plant_01"),{X+P*275,Front-Out*60,H+25},{1.6,1.6,1.6},FRotator(0,P*97+I*30,0));
    }
    // Deep shopfront awnings and separate overhead cloth frame the view without blocking the route.
    for(int Row=0;Row<2;++Row)for(int I=0;I<4;++I)
    {
        if(Row==1&&I==2)continue;
        const double X=-3430+I*860,Y=Row==0?-470.:680.,Out=Row==0?1.:-1.;
        const FRotator R(0,Row==0?0:180,0);
        MarketPart(TEXT("SM_MarketCanopy"),{X,Y,480},{1.65,1.4,1.25},R);
        for(double DX:{-405.,405.})
        {
            MarketBox({X+DX,Y+Out*170,252},{.09,.09,5.04},TEXT("M_MarketWood"));
            Beam({X+DX,Y-Out*155,493},{X+DX,Y+Out*170,477},4,Brass);
            const FVector Lamp(X+DX,Y+Out*172,345+(I%2)*22);
            MarketPart(TEXT("SM_MarketLantern"),Lamp,FVector(1.12));
            Shape(Cylinder,Lamp+FVector(0,0,38),{.29,.29,.45},FLinearColor(1,.64,.24),1.3,false);
            Beam({X+DX,Y+Out*172,477},Lamp+FVector(0,0,100),1.1,Brass);
            auto* Light=NewObject<UPointLightComponent>(this);Light->SetupAttachment(RootComponent);Light->SetMobility(EComponentMobility::Movable);
            Light->SetWorldLocation(Lamp+FVector(0,0,36));Light->SetLightColor(FLinearColor(1,.65,.30));Light->SetIntensity(320);Light->SetAttenuationRadius(190);Light->SetCastShadows(false);Light->RegisterComponent();
        }
        if(Row==0&&I==1&&bValidWorkshop)
        {
            const FTransform Anchor(R,FVector(X,Y,8));
            for(const auto& Part:Workshop->Parts)
            {
                const FTransform World=Part.LocalTransform*Anchor;
                Shape(Part.Mesh,World.GetLocation(),World.GetScale3D(),FLinearColor::White,0,Part.bCollision,World.Rotator(),nullptr,true);
            }
            // The trade sign's bracket returns to an existing canopy post.
            Beam({X+302,Y+110,425},{X+405,Y+170,425},2.5,Brass);
            bWorkshopReady=true;
            UE_LOG(LogTemp,Display,TEXT("Second Dawn assembly placed: %d parts"),Workshop->Parts.Num());
            continue;
        }
        MarketPart(TEXT("SM_MarketTable"),{X,Y,8},{1.45,1.25,1},R);
        MarketPart(TEXT("SM_MarketDrape"),{X,Y,112},{1,1,1},R);
        MarketPart(TEXT("SM_wooden_crate_02"),{X+265,Y,7},{1.4,1.4,1.4},FRotator(0,7+I*24,0));
        MarketPart(TEXT("SM_wooden_crate_02"),{X+265,Y,70},{1.1,1.1,1.1},FRotator(0,-8+I*24,0));
        MarketPart(TEXT("SM_WovenSackCluster"),{X-270,Y+Out*45,7},{1,1,1},R);
        MarketPart(TEXT("SM_wooden_stool_01"),{X-250,Y-Out*65,7},{1,1,1},FRotator(0,42,0));
        if(I%2==0)
        {
            MarketPart(TEXT("SM_wicker_basket_02"),{X-100,Y,113},{2.2,2.2,2.2});
            MarketPart(TEXT("SM_wooden_bowl_01"),{X+70,Y+5,109},{2,2,2});
            for(int Fruit=0;Fruit<24;++Fruit)
            {
                const double A=Fruit*2.39996,Radius=Fruit<17?26.:12.;
                MarketPart(TEXT("SM_food_pomegranate_01"),{X-100+FMath::Cos(A)*Radius,Y+FMath::Sin(A)*Radius*.65,156+(Fruit<17?0.:12.)},{1.15,1.15,1.15},FRotator(0,Fruit*51,0));
            }
            for(int Fruit=0;Fruit<9;++Fruit)MarketPart(TEXT("SM_food_pomegranate_01"),{X+60+(Fruit%3)*10,Y-10+(Fruit/3)*9,131},{1,1,1},FRotator(0,Fruit*67,0));
        }
        else
        {
            for(int Vessel=0;Vessel<4;++Vessel){const double Size=Vessel==1?1.85:Vessel==2?.95:1.3;CityPart(TEXT("SM_brass_pot_01"),{X-130+Vessel*81,Y+(Vessel%2)*25,117},FVector(Size),TEXT("M_brass_pot_01"),FLinearColor::White,FRotator(0,Vessel*93,0));}
            MarketPart(TEXT("SM_brass_pan_01"),{X-60,Y+Out*160,365},{1.7,1.7,1.7},FRotator(90,0,0));
            MarketPart(TEXT("SM_ceramic_vase_02"),{X+200,Y+Out*70,7},{2,2,2});
        }
        MarketPart(TEXT("SM_potted_plant_01"),{X+360,Y+Out*100,7},{1.8,1.8,1.8},FRotator(0,I*89,0));
        // Secondary stock sits under and beside the display, with different silhouettes per shop.
        for(int Stock=0;Stock<3+(I%2);++Stock)
        {
            const double PX=X-190+Stock*77,PY=Y-Out*130;
            if(I%3==0)MarketPart(TEXT("SM_wicker_basket_02"),{PX,PY,8},FVector(1.3+Stock*.23),FRotator(0,Stock*51+Row*13,0));
            else if(I%3==1)MarketPart(TEXT("SM_ceramic_vase_02"),{PX,PY,8},FVector(.9+Stock*.35),FRotator(0,Stock*29,0));
            else CityPart(TEXT("SM_brass_pot_01"),{PX,PY,8},FVector(.8+Stock*.26),TEXT("M_brass_pot_01"),FLinearColor::White,FRotator(0,Stock*79,0));
        }
        if(I==1||I==3)
        {
            MarketBox({X-275,Y-Out*105,190},{1.15,.62,.08},TEXT("M_MarketWood"));
            for(double DX:{-310.,-240.})MarketBox({X+DX,Y-Out*110,95},{.06,.06,1.90},TEXT("M_MarketWood"));
            for(int Shelf=0;Shelf<3;++Shelf)MarketPart(TEXT("SM_wooden_bowl_01"),{X-310+Shelf*35,Y-Out*105,198},FVector(1.1+Shelf*.2),FRotator(0,Shelf*25,0));
        }
    }
    for(int I=0;I<2;++I)
    {
        const double X=I?-1900:-2900,Z=I?790:560;
        MarketPart(TEXT("SM_MarketCanopy"),{X,100,Z},{1.45,4.7,2},FRotator(0,I?6:-5,0));
        for(double DX:{-365.,365.})for(double Y:{-695.,895.})Beam({X+DX,Y,Z+35},{X+DX*.99,100+(Y-100)*.82,Z},2,Rope);
    }
    // Geometric joints remain visible at player height, while a continuous collider avoids snagging.
    for(int X=0;X<15;++X)for(int Y=0;Y<5;++Y)
        MarketPart(FName(*FString::Printf(TEXT("SM_FlagstonePatch_%d"),(X+Y*2)%3)),{-4000+X*400.,-800+Y*400.,3},{1,1,1});
    for(int I=0;I<25;++I)for(double Y:{-220.,400.})MarketPart(TEXT("SM_DrainStrip"),{-3800+I*200.,Y,8},{1,1,1},FRotator(0,90,0));
    MarketPart(TEXT("SM_SunseedReliquary"),SeedLocation()-FVector(0,0,142));
    MarketBox(SeedLocation()-FVector(0,0,101),{1.2,1.2,.80},TEXT("M_MarketStone"),true);
    // Entrance plants and larger pottery make distinct near/mid-ground clusters.
    for(FVector P:{FVector(-3190,-410,8),FVector(-2780,530,8),FVector(-560,-440,8),FVector(-500,500,8)})
    {
        MarketPart(TEXT("SM_CarvedPlanter"),P,{.82,.82,.82});
        MarketPart(TEXT("SM_potted_plant_01"),P+FVector(0,0,70),{2,2,2},FRotator(0,P.X*.2,0));
        MarketPart(TEXT("SM_ceramic_vase_02"),P+FVector(70,-35,0),{1.4,1.4,1.4});
    }
    for(FVector P:{FVector(350,-530,0),FVector(400,760,0),FVector(-2620,-1050,1822)})
    {
        const double S=P.Z>1000?1.1:2.1;
        MarketPart(TEXT("SM_tree_small_02"),P+FVector(0,0,40),FVector(S),FRotator(0,P.Y*.11,0));
        MarketBox(P+FVector(0,0,40),{1.55,1.55,.12},TEXT("M_MarketShadow"));
        for(double Side:{-1.,1.})
        {
            MarketBox(P+FVector(Side*85,0,28),{.18,1.9,.56},TEXT("M_MarketStone"),true);
            MarketBox(P+FVector(0,Side*85,28),{1.55,.18,.56},TEXT("M_MarketStone"),true);
            MarketBox(P+FVector(Side*85,0,59),{.25,1.97,.09},TEXT("M_MarketStone"));
            MarketBox(P+FVector(0,Side*85,59),{1.55,.25,.09},TEXT("M_MarketStone"));
        }
    }
}
