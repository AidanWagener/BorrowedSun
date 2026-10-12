#pragma once
#include "Commandlets/Commandlet.h"
#include "BSMarketArtCommandlet.generated.h"

UCLASS()
class BORROWEDSUNEDITOR_API UBSMarketArtCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UBSMarketArtCommandlet();
    virtual int32 Main(const FString& Params) override;
};
