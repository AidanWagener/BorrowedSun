#pragma once

#include "CoreMinimal.h"

/** A mathematical sample only: movement/physics synchronization belongs to the future travel service. */
struct BORROWEDSUN_API FBSMotionSample
{
    FVector Position = FVector::ZeroVector;
    FQuat Orientation = FQuat::Identity;
    FVector LinearVelocity = FVector::ZeroVector;
    FVector AngularVelocity = FVector::ZeroVector;
};

namespace BorrowedSun
{
/** Fixed rigid anchors only; scaled or invalid transforms fail without changing OutSample. */
BORROWEDSUN_API bool TryMapSectorMotion(
    const FTransform& SourceAnchor,
    const FTransform& DestinationAnchor,
    const FBSMotionSample& Input,
    FBSMotionSample& OutSample);
}
