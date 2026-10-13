#pragma once
#include "Objects/BSPhysicalObject.h"

/** Shared definitions for city placements and the isolated court; identities do not depend on array position. */
namespace BSHandProps
{
    constexpr int32 Count = 10;
    BORROWEDSUN_API bool MakePlacement(int32 Index, const FString& PlacementNamespace, const FTransform& Pose,
        FBSPhysicalObjectPlacement& Out, FString& Error);
    BORROWEDSUN_API FBSStableId Id(const TCHAR* Space, const FString& Key);
}
