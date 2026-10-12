#include "Demo/BSDemoProgress.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace
{
    constexpr int32 DemoUserIndex = 0;

    struct FDemoCheckpointSlots
    {
        FString Primary;
        FString Backup;
    };

    FDemoCheckpointSlots GetCheckpointSlots()
    {
        FString SaveNamespace;
        FString Suffix;
        if (FParse::Value(FCommandLine::Get(), TEXT("-BSDemoSaveNamespace="), SaveNamespace, false)
            && !SaveNamespace.IsEmpty() && SaveNamespace.Len() <= 48)
        {
            bool bValid = true;
            for (const TCHAR Character : SaveNamespace)
            {
                const bool bAsciiLetter = (Character >= TEXT('A') && Character <= TEXT('Z'))
                    || (Character >= TEXT('a') && Character <= TEXT('z'));
                const bool bAsciiDigit = Character >= TEXT('0') && Character <= TEXT('9');
                if (!bAsciiLetter && !bAsciiDigit && Character != TEXT('_') && Character != TEXT('-'))
                {
                    bValid = false;
                    break;
                }
            }
            if (bValid) Suffix = TEXT("_") + SaveNamespace;
        }
        return { FString(TEXT("BorrowedSun_Demo_Primary")) + Suffix,
            FString(TEXT("BorrowedSun_Demo_Backup")) + Suffix };
    }

    UBSDemoSave* ReadValidCheckpoint(const FString& Slot, FString& OutError)
    {
        if (!UGameplayStatics::DoesSaveGameExist(Slot, DemoUserIndex))
        {
            OutError = TEXT("Checkpoint does not exist.");
            return nullptr;
        }

        UBSDemoSave* Save = Cast<UBSDemoSave>(UGameplayStatics::LoadGameFromSlot(Slot, DemoUserIndex));
        if (!Save)
        {
            OutError = TEXT("Checkpoint could not be read or has the wrong save class.");
            return nullptr;
        }
        return Save->Progress.IsValid(OutError) ? Save : nullptr;
    }

    bool SameProgress(const FBSDemoProgress& A, const FBSDemoProgress& B)
    {
        return A.SchemaVersion == B.SchemaVersion && A.Stage == B.Stage && A.Choice == B.Choice
            && A.Revision == B.Revision && A.bSunseedOwned == B.bSunseedOwned;
    }
}

bool FBSDemoProgress::IsValid(FString& OutError) const
{
    OutError.Reset();
    if (SchemaVersion != 1)
    {
        OutError = FString::Printf(TEXT("Unsupported demo checkpoint schema: %d."), SchemaVersion);
        return false;
    }
    if (Choice != EBSDemoChoice::None && Choice != EBSDemoChoice::Clinic && Choice != EBSDemoChoice::Evacuation)
    {
        OutError = TEXT("Checkpoint contains an unknown power lease choice.");
        return false;
    }

    int32 ExpectedRevision = INDEX_NONE;
    bool bExpectedOwned = false;
    bool bRequiresChoice = false;
    switch (Stage)
    {
    case EBSDemoStage::Explore: ExpectedRevision = 0; break;
    case EBSDemoStage::Carrying: ExpectedRevision = 1; bExpectedOwned = true; break;
    case EBSDemoStage::Outbound: ExpectedRevision = 2; bExpectedOwned = true; break;
    case EBSDemoStage::AtRelay: ExpectedRevision = 3; bExpectedOwned = true; break;
    case EBSDemoStage::Returning: ExpectedRevision = 4; bExpectedOwned = true; bRequiresChoice = true; break;
    case EBSDemoStage::Complete: ExpectedRevision = 5; bRequiresChoice = true; break;
    default:
        OutError = TEXT("Checkpoint contains an unknown demo stage.");
        return false;
    }

    if (Revision != ExpectedRevision)
    {
        OutError = TEXT("Checkpoint revision does not match its demo stage.");
        return false;
    }
    if (bSunseedOwned != bExpectedOwned)
    {
        OutError = TEXT("Checkpoint Sunseed custody does not match its demo stage.");
        return false;
    }
    if ((Choice != EBSDemoChoice::None) != bRequiresChoice)
    {
        OutError = TEXT("Checkpoint power lease choice does not match its demo stage.");
        return false;
    }
    return true;
}

bool FBSDemoProgress::CanTransitionFrom(EBSDemoStage ExpectedStage, FString& OutError) const
{
    if (!IsValid(OutError))
    {
        return false;
    }
    if (Stage != ExpectedStage)
    {
        OutError = TEXT("This action is unavailable at the current demo stage.");
        return false;
    }
    return true;
}

bool FBSDemoProgress::TryAcquire(FString& OutError)
{
    if (!CanTransitionFrom(EBSDemoStage::Explore, OutError)) return false;
    Stage = EBSDemoStage::Carrying;
    bSunseedOwned = true;
    ++Revision;
    return true;
}

bool FBSDemoProgress::TryDepart(FString& OutError)
{
    if (!CanTransitionFrom(EBSDemoStage::Carrying, OutError)) return false;
    Stage = EBSDemoStage::Outbound;
    ++Revision;
    return true;
}

bool FBSDemoProgress::TryReachRelay(FString& OutError)
{
    if (!CanTransitionFrom(EBSDemoStage::Outbound, OutError)) return false;
    Stage = EBSDemoStage::AtRelay;
    ++Revision;
    return true;
}

bool FBSDemoProgress::TryChoose(EBSDemoChoice InChoice, FString& OutError)
{
    if (!CanTransitionFrom(EBSDemoStage::AtRelay, OutError)) return false;
    if (InChoice != EBSDemoChoice::Clinic && InChoice != EBSDemoChoice::Evacuation)
    {
        OutError = TEXT("Choose the clinic or evacuation power lease.");
        return false;
    }
    Choice = InChoice;
    Stage = EBSDemoStage::Returning;
    ++Revision;
    return true;
}

bool FBSDemoProgress::TryComplete(FString& OutError)
{
    if (!CanTransitionFrom(EBSDemoStage::Returning, OutError)) return false;
    Stage = EBSDemoStage::Complete;
    bSunseedOwned = false;
    ++Revision;
    return true;
}

bool UBSDemoProgressSubsystem::CommitCandidate(const FBSDemoProgress& Candidate)
{
    const FBSDemoProgress Before = Progress;
    Progress = Candidate;
    if (!SaveCheckpoint())
    {
        Progress = Before;
        return false;
    }
    return true;
}

bool UBSDemoProgressSubsystem::ApplyAcquire()
{
    FBSDemoProgress Candidate = Progress;
    return Candidate.TryAcquire(LastError) && CommitCandidate(Candidate);
}

bool UBSDemoProgressSubsystem::ApplyDepart()
{
    FBSDemoProgress Candidate = Progress;
    return Candidate.TryDepart(LastError) && CommitCandidate(Candidate);
}

bool UBSDemoProgressSubsystem::ApplyReachRelay()
{
    FBSDemoProgress Candidate = Progress;
    return Candidate.TryReachRelay(LastError) && CommitCandidate(Candidate);
}

bool UBSDemoProgressSubsystem::ApplyChoose(EBSDemoChoice InChoice)
{
    FBSDemoProgress Candidate = Progress;
    return Candidate.TryChoose(InChoice, LastError) && CommitCandidate(Candidate);
}

bool UBSDemoProgressSubsystem::ApplyComplete()
{
    FBSDemoProgress Candidate = Progress;
    return Candidate.TryComplete(LastError) && CommitCandidate(Candidate);
}

bool UBSDemoProgressSubsystem::SaveCheckpoint()
{
    if (!Progress.IsValid(LastError)) return false;

    const FDemoCheckpointSlots Slots = GetCheckpointSlots();
    const FString& PrimarySlot = Slots.Primary;
    const FString& BackupSlot = Slots.Backup;

    FString PrimaryError;
    FString BackupError;
    UBSDemoSave* Previous = ReadValidCheckpoint(PrimarySlot, PrimaryError);
    if (!Previous) Previous = ReadValidCheckpoint(BackupSlot, BackupError);

    if (!Previous && (UGameplayStatics::DoesSaveGameExist(PrimarySlot, DemoUserIndex)
        || UGameplayStatics::DoesSaveGameExist(BackupSlot, DemoUserIndex)))
    {
        LastError = TEXT("Existing checkpoints are invalid. Reset explicitly before replacing them.");
        return false;
    }

    if (Previous)
    {
        // A stale game instance must not overwrite a later checkpoint or change an accepted lease.
        if (Previous->Progress.Revision > Progress.Revision
            || (Previous->Progress.Revision == Progress.Revision && !SameProgress(Previous->Progress, Progress))
            || (Previous->Progress.Choice != EBSDemoChoice::None && Previous->Progress.Choice != Progress.Choice))
        {
            LastError = TEXT("The saved journey differs from this session. Reload the checkpoint or reset explicitly.");
            return false;
        }
        if (SameProgress(Previous->Progress, Progress))
        {
            // Re-saving an unchanged primary must not destroy the previous generation in backup.
            FString CurrentPrimaryError;
            if (ReadValidCheckpoint(PrimarySlot, CurrentPrimaryError))
            {
                LastError.Reset();
                return true;
            }
        }
        if (!UGameplayStatics::SaveGameToSlot(Previous, BackupSlot, DemoUserIndex))
        {
            LastError = TEXT("Could not preserve the previous checkpoint. Progress was not committed.");
            return false;
        }
    }

    UBSDemoSave* NewSave = NewObject<UBSDemoSave>();
    NewSave->Progress = Progress;
    if (!UGameplayStatics::SaveGameToSlot(NewSave, PrimarySlot, DemoUserIndex))
    {
        LastError = TEXT("Could not write the demo checkpoint. Progress was not committed.");
        return false;
    }
    LastError.Reset();
    return true;
}

bool UBSDemoProgressSubsystem::LoadCheckpoint()
{
    const FDemoCheckpointSlots Slots = GetCheckpointSlots();
    const FString& PrimarySlot = Slots.Primary;
    const FString& BackupSlot = Slots.Backup;
    FString PrimaryError;
    FString BackupError;
    if (const UBSDemoSave* Save = ReadValidCheckpoint(PrimarySlot, PrimaryError))
    {
        Progress = Save->Progress;
        LastError.Reset();
        return true;
    }
    if (const UBSDemoSave* Save = ReadValidCheckpoint(BackupSlot, BackupError))
    {
        Progress = Save->Progress;
        LastError = FString::Printf(TEXT("Recovered the previous checkpoint: %s"), *PrimaryError);
        return true;
    }
    LastError = FString::Printf(TEXT("No valid demo checkpoint. Primary: %s Backup: %s"), *PrimaryError, *BackupError);
    return false;
}

void UBSDemoProgressSubsystem::ResetProgress()
{
    const FDemoCheckpointSlots Slots = GetCheckpointSlots();
    const FString& PrimarySlot = Slots.Primary;
    const FString& BackupSlot = Slots.Backup;
    UBSDemoSave* NewSave = NewObject<UBSDemoSave>();
    if (!UGameplayStatics::SaveGameToSlot(NewSave, PrimarySlot, DemoUserIndex))
    {
        LastError = TEXT("Could not save a new journey. Existing progress was retained.");
        return;
    }
    Progress = NewSave->Progress;
    if (!UGameplayStatics::SaveGameToSlot(NewSave, BackupSlot, DemoUserIndex))
    {
        LastError = TEXT("New journey saved, but the backup could not be refreshed.");
        return;
    }
    LastError.Reset();
}
