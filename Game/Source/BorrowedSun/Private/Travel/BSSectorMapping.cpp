#include "Travel/BSSectorMapping.h"

bool BorrowedSun::TryMapSectorMotion(
    const FTransform& SourceAnchor,
    const FTransform& DestinationAnchor,
    const FBSMotionSample& Input,
    FBSMotionSample& OutSample)
{
    const bool bValidAnchors = SourceAnchor.IsValid() && DestinationAnchor.IsValid()
        && SourceAnchor.GetScale3D().Equals(FVector::OneVector, UE_SMALL_NUMBER)
        && DestinationAnchor.GetScale3D().Equals(FVector::OneVector, UE_SMALL_NUMBER);
    if (!bValidAnchors || Input.Position.ContainsNaN() || Input.LinearVelocity.ContainsNaN()
        || Input.AngularVelocity.ContainsNaN() || Input.Orientation.ContainsNaN()
        || !Input.Orientation.IsNormalized())
    {
        return false;
    }

    const FQuat Rotation = DestinationAnchor.GetRotation() * SourceAnchor.GetRotation().Inverse();
    FBSMotionSample Result;
    Result.Position = DestinationAnchor.GetLocation()
        + Rotation.RotateVector(Input.Position - SourceAnchor.GetLocation());
    Result.Orientation = (Rotation * Input.Orientation).GetNormalized();
    Result.LinearVelocity = Rotation.RotateVector(Input.LinearVelocity);
    Result.AngularVelocity = Rotation.RotateVector(Input.AngularVelocity);
    OutSample = Result;
    return true;
}
