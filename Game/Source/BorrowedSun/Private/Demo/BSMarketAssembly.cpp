#include "Demo/BSMarketAssembly.h"
#include "Engine/StaticMesh.h"

bool UBSMarketAssembly::Validate(FString& Reason) const
{
    if (AssemblyId.IsNone() || Parts.IsEmpty()) { Reason = TEXT("Assembly needs an ID and parts."); return false; }
    TSet<FName> Keys;
    for (const auto& Part : Parts)
    {
        const FVector Scale = Part.LocalTransform.GetScale3D();
        if (Part.Key.IsNone() || Keys.Contains(Part.Key) || !Part.Mesh || !Part.LocalTransform.IsValid()
            || Scale.GetMin() <= 0 || Scale.GetMax() > 10)
        {
            Reason = FString::Printf(TEXT("Invalid or duplicate part: %s"), *Part.Key.ToString()); return false;
        }
        Keys.Add(Part.Key);
    }
    Reason.Reset(); return true;
}
