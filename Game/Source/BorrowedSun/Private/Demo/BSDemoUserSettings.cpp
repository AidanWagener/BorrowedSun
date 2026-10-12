#include "Demo/BSDemoUserSettings.h"
#include "Engine/Engine.h"

UBSDemoUserSettings* UBSDemoUserSettings::Get()
{
    return GEngine?Cast<UBSDemoUserSettings>(GEngine->GetGameUserSettings()):nullptr;
}
void UBSDemoUserSettings::SetToDefaults()
{
    Super::SetToDefaults();
    SetFrameRateLimit(60.f);
    SetScreenResolution(FIntPoint(1280,720));
    SetFullscreenMode(EWindowMode::Windowed);
    LookSensitivity=0.65f;
    MasterVolume=0.8f;
    bInvertLook=false;
}
float UBSDemoUserSettings::GetLookSensitivity() const
{
    return FMath::IsFinite(LookSensitivity)?FMath::Clamp(LookSensitivity,.15f,2.f):.65f;
}
float UBSDemoUserSettings::GetMasterVolume() const
{
    return FMath::IsFinite(MasterVolume)?FMath::Clamp(MasterVolume,0.f,1.f):.8f;
}
void UBSDemoUserSettings::SetLookSensitivity(float Value)
{
    LookSensitivity=FMath::IsFinite(Value)?FMath::Clamp(Value,.15f,2.f):.65f;
}
void UBSDemoUserSettings::SetMasterVolume(float Value)
{
    MasterVolume=FMath::IsFinite(Value)?FMath::Clamp(Value,0.f,1.f):.8f;
}
