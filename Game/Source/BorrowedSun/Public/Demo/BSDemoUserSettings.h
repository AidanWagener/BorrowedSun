#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BSDemoUserSettings.generated.h"

/** Player preferences are independent of mission checkpoints and survive a new expedition. */
UCLASS(Config=GameUserSettings)
class BORROWEDSUN_API UBSDemoUserSettings : public UGameUserSettings
{
    GENERATED_BODY()
public:
    static UBSDemoUserSettings* Get();
    virtual void SetToDefaults() override;
    /** One-time project visual preset upgrade; never changes input, audio, display or mission data. */
    bool UpgradeCityVisualPreset();
    float GetLookSensitivity() const;
    float GetMasterVolume() const;
    bool IsLookInverted() const { return bInvertLook; }
    void SetLookSensitivity(float Value);
    void SetMasterVolume(float Value);
    void SetLookInverted(bool Value) { bInvertLook=Value; }
private:
    UPROPERTY(Config) float LookSensitivity=0.65f;
    UPROPERTY(Config) float MasterVolume=0.8f;
    UPROPERTY(Config) bool bInvertLook=false;
    UPROPERTY(Config) int32 VisualPresetVersion=0;
};
