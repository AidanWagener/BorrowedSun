#include "Demo/BSDemoWorld.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

namespace
{
    const FLinearColor CityIvory(.93f,.79f,.57f), CityPale(1.12f,1.02f,.83f), CityOchre(.75f,.45f,.21f);
    const FLinearColor CityIndigo(.12f,.19f,.39f), CityCopper(.32f,.49f,.39f), CityBrass(.70f,.39f,.11f);
    const FLinearColor CityInk(.025f,.038f,.060f), CityAmber(1.f,.52f,.12f), CityLeaf(.12f,.24f,.10f);
}

void ABSDemoWorld::LoadCityArt()
{
    for(const TCHAR* Name:{TEXT("SM_Arcade400"),TEXT("SM_Column400"),TEXT("SM_CopperDome"),TEXT("SM_Jali240"),TEXT("SM_ShadeSail"),TEXT("SM_Planter"),TEXT("SM_MothWing"),TEXT("SM_PalmFrond"),TEXT("SM_Cornice"),TEXT("SM_KeeperMask"),TEXT("SM_KeeperRobe"),TEXT("SM_SolarPetal"),TEXT("SM_brass_pot_01"),TEXT("SM_brass_diya_lantern")})
    {
        if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Game/BorrowedSun/City/Meshes/%s.%s"),Name,Name)))CityMeshes.Add(FName(Name),Mesh);
    }
    for(const TCHAR* Name:{TEXT("M_CityPlaster"),TEXT("M_CityStone"),TEXT("M_CityPaving"),TEXT("M_CityCloth"),TEXT("M_CityCopper"),TEXT("M_brass_pot_01"),TEXT("M_brass_diya_lantern")})
    {
        if(auto* Mat=LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/BorrowedSun/City/Materials/%s.%s"),Name,Name)))CityMaterials.Add(FName(Name),Mat);
    }
    UE_LOG(LogTemp,Display,TEXT("Brass Sanctuary city art: %d meshes, %d materials"),CityMeshes.Num(),CityMaterials.Num());
}

void ABSDemoWorld::CityPart(FName Name,FVector P,FVector S,FName Mat,FLinearColor Tint,FRotator Rot)
{
    if(auto* Mesh=CityMeshes.Find(Name))Shape(*Mesh,P,S,Tint,0,false,Rot,CityMaterials.FindRef(Mat));
}

void ABSDemoWorld::CityBox(FVector P,FVector S,FName Mat,FLinearColor Tint,bool Collision)
{
    Shape(Cube,P,S,Tint,0,Collision,FRotator::ZeroRotator,CityMaterials.FindRef(Mat));
}

void ABSDemoWorld::Beam(FVector A,FVector B,float Radius,FLinearColor Tint)
{
    const FVector D=B-A;
    Shape(Cylinder,(A+B)*.5,FVector(Radius/50,Radius/50,D.Size()/100),Tint,0,false,FRotationMatrix::MakeFromZ(D).Rotator(),CityMaterials.FindRef(TEXT("M_CityCopper")));
}

void ABSDemoWorld::BuildDistrict()
{
    // The inherited 84 x 64 m quay and all mission anchors retain their collision envelope.
    CityBox({0,0,-125},{86,68,2.5},TEXT("M_CityStone"),CityOchre);
    CityBox({0,0,-19},{82,64,.38},TEXT("M_CityPaving"),CityPale);
    // Brass guide inlays leave the ground readable without a neon runway.
    for(int I=-16;I<=(HasMarketArt()?-17:16);++I)
    {
        CityBox({I*220.,-520,2},{1.1,.055,.035},TEXT("M_CityCopper"),CityBrass,false);
        CityBox({I*220.,520,2},{1.1,.055,.035},TEXT("M_CityCopper"),CityBrass,false);
    }
    // Five deliberately composed building bays per street edge. Arcades, balcony rhythm,
    // repeated lattice meshes and broad plaster planes carry the kit's silhouette.
    for(int Row=0;Row<2;++Row)for(int I=0;I<6;++I)
    {
        const double X=-3250+I*1250, Y=Row==0?-2150.:2570.;
        const double Front=Row==0?-1530.:1950., SignY=Row==0?1.:-1.;
        const double H=1050+(I%3)*260+(Row?140:0);
        const FLinearColor Wall=I%3==1?CityIvory:CityPale;
        CityBox({X,Y,H*.5},{11.4,10.8,H/100},TEXT("M_CityPlaster"),Wall);
        CityBox({X,Y,95},{11.65,11.05,1.9},TEXT("M_CityStone"),CityIvory);
        for(double Z:{500.,950.})
        {
            CityPart(TEXT("SM_Cornice"),{X,Front-SignY*22,Z},{11.9,.7,1.5},TEXT("M_CityPlaster"),CityPale,FRotator(0,Row?180:0,0));
            CityBox({X,Front+SignY*74,Z-18},{11.6,1.9,.28},TEXT("M_CityStone"),CityIvory,false);
        }
        for(int Bay=-1;Bay<=1;++Bay)
        {
            const double BX=X+Bay*375;
            // Inset shadow reveals are cosmetic; original building solids supply collision.
            Box({BX,Front-SignY*24,230},{2.6,.16,4.6},CityInk,0,false);
            CityPart(TEXT("SM_Arcade400"),{BX,Front,0},{.90,1,1},TEXT("M_CityPlaster"),CityIvory);
            CityPart(TEXT("SM_Column400"),{BX-178,Front+SignY*40,0},{.50,.50,1},TEXT("M_CityStone"),CityPale);
            for(int Floor=0;Floor<2;++Floor)
            {
                const double Z=610+Floor*450;
                if(Z+260>H)continue;
                Box({BX,Front+SignY*10,Z+125},{2.55,.13,2.9},CityInk,0,false);
                CityPart(TEXT("SM_Jali240"),{BX,Front+SignY*24,Z},{.9,1,.9},TEXT("M_CityCopper"),I%2?CityBrass:CityCopper);
                CityBox({BX,Front+SignY*28,Z-25},{2.8,.7,.20},TEXT("M_CityStone"),CityPale,false);
            }
        }
        // Roof coping and crenellation, with a roof pavilion on alternating bays.
        CityBox({X,Y,H+25},{11.9,11.3,.5},TEXT("M_CityStone"),CityIvory);
        // Rear elevations matter from flight as much as the street façades do on foot.
        const double Back=Y-SignY*546;
        for(int Bay=-1;Bay<=1;++Bay)for(int Floor=0;Floor<2;++Floor)
        {
            const double Z=280+Floor*440;
            Box({X+Bay*325,Back-SignY*5,Z+120},{2.3,.12,2.6},CityInk,0,false);
            CityPart(TEXT("SM_Jali240"),{X+Bay*325,Back-SignY*16,Z},{.85,1,.85},TEXT("M_CityCopper"),CityCopper);
        }
        for(double DX:{-540.,540.})CityBox({X+DX,Back,H*.5},{.3,.5,H/100},TEXT("M_CityStone"),CityIvory,false);
        CityPart(TEXT("SM_Cornice"),{X,Back,H-55},{11.9,.7,1.6},TEXT("M_CityStone"),CityIvory,FRotator(0,Row?0:180,0));
        for(int C=-5;C<=5;++C)CityBox({X+C*105,Front,H+73},{.52,.35,.48},TEXT("M_CityPlaster"),CityPale,false);
        if(I%2==0)
        {
            const FVector Roof(X,Y,H+50);
            for(double DX:{-190.,190.})for(double DY:{-190.,190.})CityPart(TEXT("SM_Column400"),Roof+FVector(DX,DY,0),{.8,.8,1},TEXT("M_CityStone"),CityIvory);
            CityBox(Roof+FVector(0,0,402),{5.6,5.6,.4},TEXT("M_CityStone"),CityPale,false);
            CityPart(TEXT("SM_CopperDome"),Roof+FVector(0,0,420),{2.65,2.65,2.05},TEXT("M_CityCopper"),CityCopper);
            Shape(Sphere,Roof+FVector(0,0,700),{.40,.40,.65},CityBrass,0,false);
            Beam(Roof+FVector(0,0,675),Roof+FVector(0,0,820),5,CityBrass);
        }
        else
        {
            Shape(Cylinder,{X+260,Y,H+170},{1.8,1.8,2.8},CityCopper,0,false,FRotator::ZeroRotator,CityMaterials.FindRef(TEXT("M_CityCopper")));
            for(int Ring=0;Ring<3;++Ring)Shape(Cylinder,{X+260,Y,H+70+Ring*80},{1.85,1.85,.08},CityBrass,0,false);
            Beam({X+310,Front,H},{X+310,Front,120},14,CityCopper);
        }
    }
    // Draped market canopies occupy the edge, with open clearance down the mission promenade.
    for(int I=0;I<(HasMarketArt()?0:5);++I)
    {
        const double X=-2950+I*870;
        CityPart(TEXT("SM_ShadeSail"),{X,-1060,495},{7.9,8,1.9},TEXT("M_CityCloth"),I%3==0?FLinearColor(1.2,.80,.33):FLinearColor(.55,.85,1.3),FRotator(0,0,I%2?4:-4));
        for(double DX:{-370.,370.})Beam({X+DX,-730,0},{X+DX,-730,520},6,CityBrass);
        Beam({X-390,-720,510},{X+390,-720,510},4,CityBrass);
        CityBox({X,-1110,48},{6.5,1.8,.96},TEXT("M_CityStone"),CityIvory);
        CityBox({X,-1110,102},{6.8,2,.12},TEXT("M_CityCopper"),CityCopper,false);
        for(int C=0;C<5;++C)
        {
            if(C%2==0)CityPart(TEXT("SM_brass_pot_01"),{X-245+C*117,-1090,110},{1.4,1.4,1.4},TEXT("M_brass_pot_01"),FLinearColor::White,FRotator(0,C*47,0));
            else CityPart(TEXT("SM_Planter"),{X-245+C*117,-1130,109},{.55,.55,.7},TEXT("M_CityPlaster"),CityOchre);
        }
        CityPart(TEXT("SM_brass_diya_lantern"),{X+285,-810,300},{2,2,2},TEXT("M_brass_diya_lantern"),FLinearColor::White);
        auto* Light=NewObject<UPointLightComponent>(this);Light->SetupAttachment(RootComponent);Light->SetMobility(EComponentMobility::Movable);
        Light->SetWorldLocation({X,-1150,310});Light->SetLightColor(FLinearColor(1,.68,.34));Light->SetIntensity(1100);Light->SetAttenuationRadius(650);Light->SetCastShadows(false);Light->RegisterComponent();
    }
    // Two high shade sails produce a strong indigo frame over the market approach.
    for(int I=0;I<(HasMarketArt()?0:2);++I)
    {
        const double X=-2600+I*1100;
        CityPart(TEXT("SM_ShadeSail"),{X,-220,780.+I*120},{10,18,4},TEXT("M_CityCloth"),FLinearColor(.55,.8,1.25),FRotator(0,-12+I*24,0));
        for(double DY:{-1050.,620.})Beam({X-490,DY,0},{X-490,DY,900.+I*80},7,CityCopper);
    }
    // A cable line and hanging lamps link the street edge to its public power supply.
    for(int I=0;I<22;++I)
    {
        const double X=-3500+I*320; const double Z=700-110*FMath::Sin(I*PI/21);
        if(I<21)Beam({X,710,Z},{X+320,710,700-110*FMath::Sin((I+1)*PI/21)},2,CityInk);
        if(I%3==0)
        {
            Beam({X,710,Z},{X,710,Z-90},2,CityBrass);
            Shape(Sphere,{X,710,Z-115},{.32,.32,.50},CityAmber,2,false);
            for(int Rib=0;Rib<6;++Rib){double A=Rib*PI/3;Beam({X+18*FMath::Cos(A),710+18*FMath::Sin(A),Z-90},{X+18*FMath::Cos(A),710+18*FMath::Sin(A),Z-142},2,CityBrass);}
        }
    }
    // Palms are a small shared mesh kit, no foliage simulation or individual ticking actors.
    for(FVector P:{FVector(-3600,1080,0),FVector(-450,1280,0),FVector(3450,1150,0),FVector(3550,-1100,0)})
    {
        if(HasMarketArt())continue;
        CityPart(TEXT("SM_Planter"),P,{2.5,2.5,1.6},TEXT("M_CityStone"),CityIvory);
        Beam(P+FVector(0,0,80),P+FVector(25,0,610),17,CityOchre);
        for(int I=0;I<9;++I)CityPart(TEXT("SM_PalmFrond"),P+FVector(25,0,580),{3.8,3.8,3.8},TEXT("M_CityCloth"),FLinearColor(.30,.50,.15),FRotator(12+(I%3)*9,I*40,0));
    }
    // Relic plinth and the original docking footprint.
    if(!HasMarketArt())
    {
        Shape(Cylinder,SeedLocation()-FVector(0,0,105),{1.6,1.6,.9},CityIvory,0,true,FRotator::ZeroRotator,CityMaterials.FindRef(TEXT("M_CityStone")));
        Shape(Cylinder,SeedLocation()-FVector(0,0,50),{1.8,1.8,.15},CityBrass,0,false);
    }
    for(int I=0;I<8;++I){double A=I*PI/4;Shape(Sphere,SeedLocation()+FVector(FMath::Cos(A)*90,FMath::Sin(A)*90,-120),{.10,.10,.10},CityAmber,2,false);}
    if(!HasMarketArt())Sign(TEXT("SUNSEED / RECLAIMED LIGHT"),{-1100,190,190},26,FColor(255,217,150));
    Shape(Cylinder,{1800,0,6},{11,11,.12},CityCopper,0,true,FRotator::ZeroRotator,CityMaterials.FindRef(TEXT("M_CityCopper")));
    Shape(Cylinder,{1800,0,15},{9.6,9.6,.08},CityInk,0,false);
    for(int I=0;I<24;++I){double A=I*PI/12;Shape(Cube,{1800+FMath::Cos(A)*510,FMath::Sin(A)*510,20},{.35,.10,.08},CityAmber,1.5,false,FRotator(0,FMath::RadiansToDegrees(A)+90,0));}
    // Clinic and lift remain reachable at their unchanged saved mission coordinates.
    for(int I=0;I<2;++I)
    {
        const double X=I?2200:-1800, Y=I?1700:1550;
        CityBox({X,Y,185},{8,3.6,3.7},TEXT("M_CityPlaster"),CityPale);
        CityBox({X,Y,385},{8.5,4,.3},TEXT("M_CityStone"),CityIvory,false);
        Box({X,Y-190,170},{3,.05,2.8},CityInk,0,false);
        CityPart(TEXT("SM_Arcade400"),{X,Y-205,0},{.9,.5,.70},TEXT("M_CityStone"),CityIvory);
        CityPart(TEXT("SM_ShadeSail"),{X,Y-360,395},{7.6,3.8,1.1},TEXT("M_CityCloth"),FLinearColor(.55,.8,1.2));
        CityBox({X,I?1390.:1320.,75},{1.3,.65,1.5},TEXT("M_CityCopper"),CityCopper);
    }
    Sign(TEXT("IONA'S CLINIC"),{-1800,1305,355},46,FColor(236,214,161));
    ClinicStatus=Sign(TEXT("POWER LEASE EXPIRED"),{-1800,1300,250},16,FColor(246,162,77));
    Sign(TEXT("EVACUATION LIFT"),{2200,1435,355},46,FColor(236,214,161));
    EvacStatus=Sign(TEXT("FAMILIES WAITING"),{2200,1460,250},16,FColor(246,162,77));
    Sign(TEXT("ORISON  /  LOWER QUAY"),{-2800,-1475,555},46,FColor(236,214,161),FRotator(0,90,0));
    Sign(TEXT("03   KESTREL"),{2300,-1475,555},50,FColor(236,214,161),FRotator(0,90,0));
    // Detailed edge balustrades retain the prototype's low collision boundary.
    for(double Y:{-3200.,3200.})
    {
        CityBox({0,Y,45},{84,.30,.90},TEXT("M_CityStone"),CityIvory);
        CityBox({0,Y,115},{84,.34,.16},TEXT("M_CityStone"),CityPale,false);
        for(int I=-20;I<=20;++I)CityPart(TEXT("SM_Column400"),{I*200.,Y,45},{.26,.26,.18},TEXT("M_CityStone"),CityPale);
    }
    for(double X:{-4200.,4200.})CityBox({X,0,60},{.18,64,1.2},TEXT("M_CityStone"),CityIvory);
    // Stepped masonry foundations ground the high quay in the harbor below.
    CityBox({0,0,-1590},{86,68,29},TEXT("M_CityStone"),CityIvory,false);
    for(int I=0;I<4;++I)CityBox({0,3400.+I*180,-340.-I*250},{90.,4.5,2.5},TEXT("M_CityStone"),CityIvory,false);
}

void ABSDemoWorld::BuildLanternKeeper()
{
    if(HasMarketArt())
    {
        const FVector O(15500,0,-1800);
        MarketPart(TEXT("SM_KeeperSculpt"),O,FVector(7.5),FRotator(0,90,0));
        MarketPart(TEXT("SM_SolarHalo"),O,FVector(7.5),FRotator(0,90,0));
        for(int Step=0;Step<6;++Step)MarketBox(O+FVector(0,0,Step*135.),{40.-Step*3,43.-Step*3,1.4},TEXT("M_MarketStone"));
        Shape(Sphere,O+FVector(-1800,0,3750),FVector(5.5),CityAmber,4,false);
        for(int Ring=0;Ring<3;++Ring)for(int I=0;I<64;++I)
        {
            const double A=I*2*PI/64,B=(I+1)*2*PI/64;
            auto Point=[&](double T){const FVector P=Ring==0?FVector(0,FMath::Cos(T)*310,FMath::Sin(T)*310):Ring==1?FVector(FMath::Cos(T)*310,0,FMath::Sin(T)*310):FVector(FMath::Cos(T)*310,FMath::Sin(T)*310,0);return O+FVector(-1800,0,3750)+P;};
            Beam(Point(A),Point(B),8,CityBrass);
        }
        return;
    }
    const FVector O(11200,6500,-2100);
    CityBox(O+FVector(0,0,1000),{30,24,20},TEXT("M_CityStone"),CityIvory,false);
    for(int I=0;I<5;++I)CityBox(O+FVector(0,0,I*260.),{49.-I*4,40.-I*3,2.6},TEXT("M_CityStone"),CityPale,false);
    // Robe and mask form a deliberately simple first sculptural silhouette.
    CityPart(TEXT("SM_KeeperRobe"),O+FVector(0,0,1800),{40,25,45},TEXT("M_CityCopper"),CityCopper);
    Shape(Cylinder,O+FVector(0,0,6350),{7,5,5},CityBrass,0,false,FRotator::ZeroRotator,CityMaterials.FindRef(TEXT("M_CityCopper")));
    CityPart(TEXT("SM_KeeperMask"),O+FVector(0,-130,6320),{16,16,12},TEXT("M_CityCopper"),CityBrass);
    for(double S:{-1.,1.})
    {
        Shape(Sphere,O+FVector(S*240,-565,7210),{2.4,.35,1.25},CityInk,0,false,FRotator(0,0,S*35));
        Beam(O+FVector(S*180,0,7550),O+FVector(S*800,30,9000),28,CityBrass);
        for(int I=0;I<8;++I){const FVector A=O+FVector(S*(230+I*70),30,7670+I*155);Beam(A,A+FVector(S*300,0,90),12,CityBrass);}
        Beam(O+FVector(S*800,0,5700),O+FVector(S*1100,-300,4700),220,CityCopper);
        Beam(O+FVector(S*1100,-300,4700),O+FVector(S*530,-820,5050),180,CityBrass);
        Shape(Sphere,O+FVector(S*460,-820,5150),{4.0,3.0,5.8},CityBrass,0,false,FRotator(0,0,S*45));
        for(int Finger=0;Finger<4;++Finger)Beam(O+FVector(S*(360+Finger*70),-990,5050),O+FVector(S*(280+Finger*68),-1030,5370-Finger*25),35,CityBrass);
        // Segmented wing collectors read as a broken circular halo.
        for(int I=0;I<15;++I)
        {
            const double Degrees=(S<0?180:0)-70+I*10, A=Degrees*PI/180;
            const FVector Center=O+FVector(0,450,5700);
            CityPart(TEXT("SM_SolarPetal"),Center,{38,38,38},TEXT("M_CityCopper"),I%3?CityBrass:CityCopper,FRotator(-Degrees,0,0));
            Beam(Center+FVector(FMath::Cos(A)*1500,0,FMath::Sin(A)*1500),Center+FVector(FMath::Cos(A)*3900,0,FMath::Sin(A)*3900),20,CityBrass);
        }
    }
    Shape(Sphere,O+FVector(0,-850,5450),{11,11,11},CityAmber,4,false);
    for(int I=0;I<36;++I)
    {
        const double A=I*2*PI/36,B=(I+1)*2*PI/36;
        Beam(O+FVector(FMath::Cos(A)*4050,700,5800+FMath::Sin(A)*4050),O+FVector(FMath::Cos(B)*4050,700,5800+FMath::Sin(B)*4050),35,CityBrass);
    }
}

void ABSDemoWorld::BuildSky()
{
    Box({0,0,-5000},{1500,1500,4},FLinearColor(.025,.13,.17),0,false);
    // Outer city is only a silhouette layer; all detailed art is concentrated on the quay.
    FRandomStream R(102620);
    for(int I=0;I<48;++I)
    {
        const double A=I*2*PI/48, Radius=R.FRandRange(12000,24500), H=R.FRandRange(1700,4600);
        const FVector P(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,H*.5-3400);
        const double WX=R.FRandRange(9,19),WY=R.FRandRange(9,17);
        CityBox(P,{WX,WY,H/100},TEXT("M_CityPlaster"),I%3?CityIvory:CityOchre,false);
        CityBox(P+FVector(0,0,H*.5),{WX+.6,WY+.6,.8},TEXT("M_CityStone"),CityPale,false);
        // Buildings meet a harbor foundation instead of terminating above the water plane.
        CityBox({P.X,P.Y,-4140},{19,18,16},TEXT("M_CityStone"),CityIvory,false);
        if(HasMarketArt())for(int Floor=0;Floor<3;++Floor)for(double DX:{-320.,320.})
        {
            const double Z=-3200+Floor*550;if(Z+420>P.Z+H*.5)continue;
            MarketPart(TEXT("SM_MarketWindow"),{P.X+DX,P.Y-WY*50-15,Z},{1.3,1.3,1.3},FRotator(0,180,0));
            MarketPart(TEXT("SM_MarketWindow"),{P.X-WX*50-15,P.Y+DX,Z},{1.3,1.3,1.3},FRotator(0,90,0));
        }
        if(I%3==0)CityPart(TEXT("SM_CopperDome"),P+FVector(0,0,H*.5+50),{6,6,4},TEXT("M_CityCopper"),CityCopper);
    }
    BuildLanternKeeper();
    if(HasMarketArt())
    {
        // Terraced harbor neighborhoods establish a real middle distance behind the market.
        FRandomStream Harbor(70412);
        for(int Row=0;Row<4;++Row)for(int I=-5;I<=5;++I)
        {
            if(FMath::Abs(I)<2&&Row<2)continue;
            const double X=10200+Row*2300+Harbor.FRandRange(-350,350),Y=I*1400.+Harbor.FRandRange(-220,220);
            const double H=Harbor.FRandRange(1700,4100),Base=-1950+Row*150;
            MarketBox({X,Y,(Base-4800)*.5},{16.,18.,(Base+4800)/100},TEXT("M_MarketStone"));
            MarketBox({X,Y,Base+H*.5},{10.,11.,H/100},TEXT("M_MarketPlaster"));
            for(double Z:{Base+500,Base+1100,Base+H-40})MarketBox({X,Y,Z},{10.6,11.6,.35},TEXT("M_MarketStone"));
            for(int Floor=0;Floor<4;++Floor)for(double DY:{-300.,300.})
            {
                const double Z=Base+200+Floor*560;if(Z+380>Base+H)continue;
                MarketPart(TEXT("SM_MarketWindow"),{X-515,Y+DY,Z},{1.3,1.3,1.3},FRotator(0,90,0));
            }
            if(I%2==0)CityPart(TEXT("SM_CopperDome"),{X,Y,Base+H+20},{5.3,5.3,4},TEXT("M_CityCopper"),CityCopper);
            else for(int C=-4;C<=4;++C)MarketBox({X-505,Y+C*112,Base+H+65},{.50,.65,1.1},TEXT("M_MarketStone"));
        }
    }
    const FVector Relay=RelayLocation();
    Shape(Cylinder,Relay-FVector(0,0,2500),{30,30,1.5},CityInk);
    Shape(Cylinder,Relay-FVector(0,0,2420),{27,27,.12},CityCopper,0,false,FRotator::ZeroRotator,CityMaterials.FindRef(TEXT("M_CityCopper")));
    for(int I=0;I<32;++I){double A=I*2*PI/32;Shape(Cube,Relay+FVector(0,FMath::Cos(A)*1900,FMath::Sin(A)*1900+800),{.7,3.85,.7},CityAmber,2.5,false,FRotator(0,0,FMath::RadiansToDegrees(A)));}
    CityBox(Relay+FVector(1000,0,900),{1.3,12,24},TEXT("M_CityCopper"),CityCopper);
    Sign(TEXT("THE HELIARCH / SKY RELAY"),Relay+FVector(-400,0,2300),170,FColor(255,220,145),FRotator(0,180,0));
    for(int I=1;I<8;++I)
    {
        const FVector P=FMath::Lerp(DockLocation()+FVector(0,0,650),Relay,I/8.);
        for(int Side=0;Side<2;++Side)
        {
            const FVector Offset(0,Side?850:-850,0);
            FlightBeacons.Add(StateVisual(FName(*FString::Printf(TEXT("FlightBeacon_%d_%d"),I,Side)),Sphere,P+Offset,{.9,.9,.9},CityAmber,4));
            FlightBeacons.Add(StateVisual(FName(*FString::Printf(TEXT("FlightBeaconStem_%d_%d"),I,Side)),Cylinder,P+Offset-FVector(0,0,145),{.1,.1,1.9},CityCopper,0));
        }
    }
    // Navigation is an on-board aid; floating guide symbols do not intrude on the walking view.
    SetFlightBeaconsVisible(false);
}
