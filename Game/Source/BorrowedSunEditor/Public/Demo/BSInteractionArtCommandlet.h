#pragma once
#include "Commandlets/Commandlet.h"
#include "BSInteractionArtCommandlet.generated.h"

UCLASS()
class BORROWEDSUNEDITOR_API UBSInteractionArtCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UBSInteractionArtCommandlet();
    virtual int32 Main(const FString& Params) override;
};
