#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "BSDemoCreateMapCommandlet.generated.h"

/** Creates the owned demo map and its shared material through editor package APIs. */
UCLASS()
class BORROWEDSUNEDITOR_API UBSDemoCreateMapCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UBSDemoCreateMapCommandlet();

    /** Existing destination files are protected unless the caller supplies -Force. */
    virtual int32 Main(const FString& Params) override;
};
