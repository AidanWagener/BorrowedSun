#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "BSOrisonTerrainCommandlet.generated.h"

/** Adds the canonical Orison macro heightfield to the existing main map. */
UCLASS()
class BORROWEDSUNEDITOR_API UBSOrisonTerrainCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UBSOrisonTerrainCommandlet();
    virtual int32 Main(const FString& Params) override;
};
