#include "Demo/BSDemoUserSettings.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoSettingsBounds,"BorrowedSun.Smoke.Settings.RejectsUnsafeValues",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FBSDemoSettingsBounds::RunTest(const FString&)
{
    auto* Settings=NewObject<UBSDemoUserSettings>();
    Settings->SetToDefaults();
    TestEqual(TEXT("Reset retains the 60 FPS player target"),Settings->GetFrameRateLimit(),60.f);
    TestTrue(TEXT("Reset retains windowed presentation"),Settings->GetFullscreenMode()==EWindowMode::Windowed);
    Settings->SetMasterVolume(-1.f);
    TestEqual(TEXT("Negative volume clamps to mute"),Settings->GetMasterVolume(),0.f);
    Settings->SetMasterVolume(4.f);
    TestEqual(TEXT("Volume cannot amplify above full"),Settings->GetMasterVolume(),1.f);
    Settings->SetLookSensitivity(-1.f);
    TestEqual(TEXT("Look cannot become reversed by negative sensitivity"),Settings->GetLookSensitivity(),.15f);
    Settings->SetLookSensitivity(std::numeric_limits<float>::quiet_NaN());
    TestEqual(TEXT("NaN sensitivity uses safe default"),Settings->GetLookSensitivity(),.65f);
    Settings->SetMasterVolume(std::numeric_limits<float>::infinity());
    TestEqual(TEXT("Infinite volume uses safe default"),Settings->GetMasterVolume(),.8f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoSettingsPersistence,"BorrowedSun.Smoke.Settings.IsolatedConfigRoundTrip",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FBSDemoSettingsPersistence::RunTest(const FString&)
{
    const FString Directory=FPaths::ProjectSavedDir()/TEXT("Automation/PreferenceFixtures");
    IFileManager::Get().MakeDirectory(*Directory,true);
    const FString File=FPaths::ConvertRelativePathToFull(Directory/(FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".ini")));
    auto* Source=NewObject<UBSDemoUserSettings>();
    Source->SetLookSensitivity(1.25f);
    Source->SetMasterVolume(.3f);
    Source->SetLookInverted(true);
    Source->SaveConfig(CPF_Config,*File);
    TestTrue(TEXT("A real isolated config file was written"),IFileManager::Get().FileExists(*File));
    auto* Restored=NewObject<UBSDemoUserSettings>();
    Restored->SetToDefaults();
    Restored->LoadConfig(nullptr,*File);
    TestEqual(TEXT("Sensitivity persists"),Restored->GetLookSensitivity(),1.25f);
    TestEqual(TEXT("Volume persists"),Restored->GetMasterVolume(),.3f);
    TestTrue(TEXT("Invert persists"),Restored->IsLookInverted());
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSDemoCityPresetMigration,"BorrowedSun.Smoke.Settings.CityPresetMigration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FBSDemoCityPresetMigration::RunTest(const FString&)
{
    const FString Directory=FPaths::ProjectSavedDir()/TEXT("Automation/PreferenceFixtures");
    IFileManager::Get().MakeDirectory(*Directory,true);
    const FString File=FPaths::ConvertRelativePathToFull(Directory/(FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".ini")));
    const FString Legacy=TEXT("[/Script/BorrowedSun.BSDemoUserSettings]\nVisualPresetVersion=0\nLookSensitivity=1.25\nMasterVolume=0.3\nbInvertLook=True\n");
    TestTrue(TEXT("Legacy fixture written"),FFileHelper::SaveStringToFile(Legacy,*File));
    auto* Settings=NewObject<UBSDemoUserSettings>();Settings->SetToDefaults();Settings->LoadConfig(nullptr,*File);
    Settings->SetGlobalIlluminationQuality(0);Settings->SetReflectionQuality(0);
    Settings->SetScreenResolution(FIntPoint(1600,900));Settings->SetFrameRateLimit(45);
    TestTrue(TEXT("Legacy preset upgraded once"),Settings->UpgradeCityVisualPreset());
    TestEqual(TEXT("Lumen quality enabled"),Settings->GetGlobalIlluminationQuality(),2);
    TestEqual(TEXT("Reflection quality enabled"),Settings->GetReflectionQuality(),2);
    TestEqual(TEXT("Sensitivity retained"),Settings->GetLookSensitivity(),1.25f);
    TestEqual(TEXT("Audio retained"),Settings->GetMasterVolume(),.3f);
    TestTrue(TEXT("Inversion retained"),Settings->IsLookInverted());
    TestEqual(TEXT("Frame limit retained"),Settings->GetFrameRateLimit(),45.f);
    TestTrue(TEXT("Resolution retained"),Settings->GetScreenResolution()==FIntPoint(1600,900));
    Settings->SetGlobalIlluminationQuality(1);
    TestFalse(TEXT("Later launches do not overwrite chosen quality"),Settings->UpgradeCityVisualPreset());
    TestEqual(TEXT("Later quality choice retained"),Settings->GetGlobalIlluminationQuality(),1);
    Settings->SaveConfig(CPF_Config,*File);
    auto* Reloaded=NewObject<UBSDemoUserSettings>();Reloaded->LoadConfig(nullptr,*File);
    TestFalse(TEXT("Migration version survives disk round trip"),Reloaded->UpgradeCityVisualPreset());
    return true;
}
#endif
