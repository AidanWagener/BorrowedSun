#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "BSStartingAreaLookCommandlet.generated.h"

/** Adjusts the existing main map's named daylight actors without rebuilding the map. */
UCLASS()
class BORROWEDSUNEDITOR_API UBSStartingAreaLookCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UBSStartingAreaLookCommandlet();
    virtual int32 Main(const FString& Params) override;
};
