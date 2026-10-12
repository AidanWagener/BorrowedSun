#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSDemoProgress.generated.h"

UENUM(BlueprintType)
enum class EBSDemoStage : uint8
{
    Explore,
    Carrying,
    Outbound,
    AtRelay,
    Returning,
    Complete
};

UENUM(BlueprintType)
enum class EBSDemoChoice : uint8
{
    None,
    Clinic,
    Evacuation
};

/** One demo journey. Revision counts accepted events within this journey, not across resets. */
USTRUCT(BlueprintType)
struct BORROWEDSUN_API FBSDemoProgress
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Demo")
    int32 SchemaVersion = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Demo")
    EBSDemoStage Stage = EBSDemoStage::Explore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Demo")
    EBSDemoChoice Choice = EBSDemoChoice::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Demo")
    int32 Revision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Demo")
    bool bSunseedOwned = false;

    bool TryAcquire(FString& OutError);
    bool TryDepart(FString& OutError);
    bool TryReachRelay(FString& OutError);
    bool TryChoose(EBSDemoChoice InChoice, FString& OutError);
    bool TryComplete(FString& OutError);
    bool IsValid(FString& OutError) const;

private:
    bool CanTransitionFrom(EBSDemoStage ExpectedStage, FString& OutError) const;
};

UCLASS()
class BORROWEDSUN_API UBSDemoSave : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(SaveGame)
    FBSDemoProgress Progress;
};

/** Synchronous demo checkpoints, user index zero. Optional -BSDemoSaveNamespace isolates QA slots.
 *  Namespace accepts only 1-48 ASCII letters, digits, underscores or hyphens; invalid values are ignored. */
UCLASS()
class BORROWEDSUN_API UBSDemoProgressSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    const FBSDemoProgress& GetProgress() const { return Progress; }

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool ApplyAcquire();

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool ApplyDepart();

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool ApplyReachRelay();

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool ApplyChoose(EBSDemoChoice InChoice);

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool ApplyComplete();

    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool SaveCheckpoint();

    /** Valid primary wins; backup is consulted only if primary is missing or invalid. */
    UFUNCTION(BlueprintCallable, Category = "Demo")
    bool LoadCheckpoint();

    /** Starts a new journey and replaces both checkpoint slots. Inspect LastError for write failures. */
    UFUNCTION(BlueprintCallable, Category = "Demo")
    void ResetProgress();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Demo")
    FString LastError;

private:
    UPROPERTY(Transient)
    FBSDemoProgress Progress;

    bool CommitCandidate(const FBSDemoProgress& Candidate);
};
