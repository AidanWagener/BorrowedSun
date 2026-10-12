#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSMarketAssembly.generated.h"

class UStaticMesh;

/** An authored decorative part. Its key survives layout changes. */
USTRUCT(BlueprintType)
struct FBSMarketAssemblyPart
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Key;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMesh> Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FTransform LocalTransform;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bCollision = false;
};

/** Reusable dressed shop, editable in the Content Browser without a C++ rebuild. */
UCLASS(BlueprintType)
class BORROWEDSUN_API UBSMarketAssembly : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AssemblyId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FBSMarketAssemblyPart> Parts;
    bool Validate(FString& Reason) const;
};
