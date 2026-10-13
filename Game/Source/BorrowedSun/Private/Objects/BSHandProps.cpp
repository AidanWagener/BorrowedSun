#include "Objects/BSHandProps.h"
#include "Engine/StaticMesh.h"

FBSStableId BSHandProps::Id(const TCHAR* Space, const FString& Key)
{ FBSStableId Value; Value.Namespace=Space; Value.Key=Key; return Value; }

bool BSHandProps::MakePlacement(int32 Index,const FString& Space,const FTransform& Pose,FBSPhysicalObjectPlacement& Out,FString& Error)
{
    Error.Reset();
    struct FDefinition { const TCHAR* Name; const TCHAR* Label; float Mass; };
    static const FDefinition Definitions[]={
        {TEXT("Cup"),TEXT("Enamel tea cup"),.28f},{TEXT("Bowl"),TEXT("Glazed sharing bowl"),.65f},
        {TEXT("Lantern"),TEXT("Caged service lantern"),1.1f},{TEXT("Tool"),TEXT("Insulated service spanner"),.55f},
        {TEXT("Cell"),TEXT("Workshop power cell"),.8f},{TEXT("Crate"),TEXT("Strapped parts crate"),3.2f},
        {TEXT("Basket"),TEXT("Woven parts basket"),.45f},{TEXT("Bottle"),TEXT("Ceramic flask"),.9f},
        {TEXT("Book"),TEXT("Repair ledger"),.7f},{TEXT("Bundle"),TEXT("Tied cloth parcel"),1.4f}};
    static_assert(UE_ARRAY_COUNT(Definitions)==Count);
    if (Index<0 || Index>=Count) { Error=TEXT("Unknown hand prop definition."); return false; }
    const auto& D=Definitions[Index]; const FString Key=FString(D.Name).ToLower();
    const FString MeshName=TEXT("SM_Hand")+FString(D.Name);
    Out={}; Out.InstanceId=Id(*Space,Key); Out.DefinitionId=Id(TEXT("hand_prop"),Key);
    Out.DisplayName=D.Label; Out.MassKg=D.Mass; Out.Transform=Pose;
    Out.Mesh=LoadObject<UStaticMesh>(nullptr,*(TEXT("/Game/BorrowedSun/Interaction/Meshes/")+MeshName+TEXT(".")+MeshName));
    if (!Out.InstanceId.IsValid() || !Out.Mesh) { Error=TEXT("Hand prop identity or cooked mesh is unavailable."); return false; }
    return true;
}
