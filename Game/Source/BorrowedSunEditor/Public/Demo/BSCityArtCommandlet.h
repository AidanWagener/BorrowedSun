#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "BSCityArtCommandlet.generated.h"

/** Imports the reviewed CC0 maps and builds original reusable city meshes through editor APIs. */
UCLASS()
class BORROWEDSUNEDITOR_API UBSCityArtCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UBSCityArtCommandlet();
    virtual int32 Main(const FString& Params) override;
};
