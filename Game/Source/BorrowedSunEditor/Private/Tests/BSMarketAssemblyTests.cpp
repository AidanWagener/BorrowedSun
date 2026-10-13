#include "Demo/BSMarketAssembly.h"
#include "Engine/StaticMesh.h"
#include "Misc/AutomationTest.h"
#include "StaticMeshResources.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSWorkshopAssemblyTest, "BorrowedSun.Smoke.Content.WorkshopAssembly",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSWorkshopAssemblyTest::RunTest(const FString& Parameters)
{
    auto* Assembly=LoadObject<UBSMarketAssembly>(nullptr,TEXT("/Game/BorrowedSun/Market/Assemblies/DA_SecondDawnWorkshop.DA_SecondDawnWorkshop"));
    if(!TestNotNull(TEXT("Authored workshop package survives editor restart"),Assembly))return false;
    FString Reason;
    if(!TestTrue(TEXT("Every persisted part is valid"),Assembly->Validate(Reason)))return false;
    TestEqual(TEXT("Stable assembly ID"),Assembly->AssemblyId,FName(TEXT("orison.second-dawn")));
    for(const auto& Part:Assembly->Parts)
    {
        if(Part.bCollision)
        {
            const FBox Bounds=Part.Mesh->GetBoundingBox().TransformBy(Part.LocalTransform);
            // Anchor is at street Y=-470; the walking validation route is Y=-250.
            // Leave at least a capsule diameter between furniture and that route.
            TestTrue(*FString::Printf(TEXT("%s leaves the street clear"),*Part.Key.ToString()),Bounds.Max.Y<140);
            TestNotNull(TEXT("Furniture has an authored collision body"),Part.Mesh->GetBodySetup());
        }
        for(const auto& Material:Part.Mesh->GetStaticMaterials())TestNotNull(TEXT("Material references survive import"),Material.MaterialInterface.Get());
    }
    auto* Invalid=DuplicateObject<UBSMarketAssembly>(Assembly,GetTransientPackage());
    const FBSMarketAssemblyPart DuplicatePart = Invalid->Parts[0];
    Invalid->Parts.Add(DuplicatePart);
    TestFalse(TEXT("Duplicate authored keys are rejected"),Invalid->Validate(Reason));
    Invalid->Parts.Pop();Invalid->Parts[0].LocalTransform.SetScale3D(FVector(-1,1,1));
    TestFalse(TEXT("Mirrored scale is rejected before placement"),Invalid->Validate(Reason));
    auto* Canopy=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/BorrowedSun/Market/Meshes/SM_MarketCanopy.SM_MarketCanopy"));
    if(!TestNotNull(TEXT("Authored canopy package loads"),Canopy))return false;
    // FBX import stores sRGB-decoded values in MeshDescription; the mesh builder
    // re-encodes them into UNORM bytes. The material consumes this built buffer.
    // Checking the intermediate .01298 instead of the material's 30/255 mask
    // would falsely reject the correct linear FBX export or encourage double gamma.
    const auto* RenderData=Canopy->GetRenderData();
    if(!TestNotNull(TEXT("Canopy has built render data"),RenderData))return false;
    if(!TestTrue(TEXT("Canopy has LOD0"),RenderData->LODResources.Num()>0))return false;
    const auto& Colors=RenderData->LODResources[0].VertexBuffers.ColorVertexBuffer;
    if(!TestTrue(TEXT("Canopy has resident vertex colors"),Colors.GetNumVertices()>0&&Colors.GetVertexData()!=nullptr))return false;
    uint8 Minimum=255,Maximum=0;uint32 Quiet=0,Border=0;bool OtherChannelsWhite=true;
    for(uint32 Index=0;Index<Colors.GetNumVertices();++Index)
    {
        const FColor Color=Colors.VertexColor(Index);
        Minimum=FMath::Min(Minimum,Color.R);Maximum=FMath::Max(Maximum,Color.R);
        Quiet+=Color.R<=51;Border+=Color.R==255;
        OtherChannelsWhite&=Color.G==255&&Color.B==255&&Color.A==255;
    }
    TestEqual(TEXT("Quiet panel mask reaches material without double gamma"),Minimum,uint8(30));
    TestEqual(TEXT("Patterned borders retain the full mask"),Maximum,uint8(255));
    TestTrue(TEXT("Quiet panels and decorative borders both survive mesh build"),Quiet>0&&Border>0);
    TestTrue(TEXT("Unused mask channels stay neutral"),OtherChannelsWhite);
    return true;
}
#endif
