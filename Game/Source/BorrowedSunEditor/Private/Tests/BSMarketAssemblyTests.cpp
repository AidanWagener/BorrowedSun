#include "Demo/BSMarketAssembly.h"
#include "Engine/StaticMesh.h"
#include "Misc/AutomationTest.h"

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
    return true;
}
#endif
