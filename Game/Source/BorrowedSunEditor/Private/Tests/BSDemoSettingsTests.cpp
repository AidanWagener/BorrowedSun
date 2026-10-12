#include "Demo/BSDemoUserSettings.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
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
#endif
