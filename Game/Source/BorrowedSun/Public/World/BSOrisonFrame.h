#pragma once

#include "CoreMinimal.h"

/** First regional frame. This is an Orison planning projection, not planet ECEF or streaming. */
struct FBSOrisonFrame
{
    static FVector RuntimeOriginMetres() { return FVector(-4754.3, -1365.3, 91.92); }
    static FVector ToRuntime(const FVector& CityMetres) { return (CityMetres - RuntimeOriginMetres()) * 100.0; }
    static FVector ToCity(const FVector& RuntimeCentimetres) { return RuntimeCentimetres / 100.0 + RuntimeOriginMetres(); }
};
