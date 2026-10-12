#pragma once

#include "CoreMinimal.h"
#include "BSStableId.generated.h"

/** Authored identity, independent of Actor names, paths, array positions and representations. */
USTRUCT(BlueprintType)
struct BORROWEDSUN_API FBSStableId
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Identity")
    FString Namespace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Identity")
    FString Key;

    bool IsValid() const;
    FString ToString() const;
    bool operator==(const FBSStableId& Other) const;
};
