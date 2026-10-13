#include "Demo/BSStartingAreaLookCommandlet.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "String/LexFromString.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"

DEFINE_LOG_CATEGORY_STATIC(LogBSStartingAreaLook, Log, All);

namespace BSStartingAreaLook
{
    const FString MapPath(TEXT("/Game/BorrowedSun/Maps/L_BorrowedSunDemo"));

    // This is a reviewable daylight look, not a simulation of Nacre's weather or solar orbit.
    struct FLook
    {
        // At the 17 m market walls, this angle clears the street center while preserving
        // roughly 5.8 m of north-edge shadow. The earlier -27/-35 look shaded the whole route.
        float SunPitch = -42.f;
        float SunYaw = -18.f;
        float SunIntensity = 35000.f;
        // Artistic bounce substitute: source on the street (+Y), facing the workshop frontage.
        // The original -65/140 fill came from behind that wall and left its face unreadable.
        float FillPitch = -45.f;
        float FillYaw = -90.f;
        float FillIntensity = 3500.f;
        float SkyIntensity = 1.8f;
        float FogDensity = .004f;
        float ExposureMin = 11.f;
        float ExposureMax = 11.6f;
        float CloudBottomKm = 1.25f;
        float CloudHeightKm = 2.5f;
    };

    bool ReadBounded(const FString& Params, const TCHAR* Key, float& Value, float Min, float Max)
    {
        FString Text;
        if (!FParse::Value(*Params, *(FString(Key) + TEXT("=")), Text)) return true;
        float Parsed = 0.f;
        if (!LexTryParseString(Parsed, *Text) || !FMath::IsFinite(Parsed) || Parsed < Min || Parsed > Max)
        {
            UE_LOG(LogBSStartingAreaLook, Error, TEXT("%s must be a finite number in [%g, %g]."), Key, Min, Max);
            return false;
        }
        Value = Parsed;
        return true;
    }

    template<class T> T* ExactActor(UWorld* World, const TCHAR* Name)
    {
        AActor* Match = nullptr;
        int32 Count = 0;
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            if (It->GetFName() == FName(Name)) { Match = *It; ++Count; }
        }
        T* Result = Count == 1 ? Cast<T>(Match) : nullptr;
        if (!Result) UE_LOG(LogBSStartingAreaLook, Error, TEXT("Expected exactly one %s named %s; found %d name matches."), *T::StaticClass()->GetName(), Name, Count);
        return Result;
    }

    bool WriteReport(const TSharedRef<FJsonObject>& Report)
    {
        const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("StartingAreaLook"));
        if (!IFileManager::Get().MakeDirectory(*Directory, true)) return false;
        FString Json;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
        if (!FJsonSerializer::Serialize(Report, Writer)) return false;
        return FFileHelper::SaveStringToFile(Json, *(Directory / TEXT("report.json")), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
}

UBSStartingAreaLookCommandlet::UBSStartingAreaLookCommandlet()
{
    IsClient = false; IsServer = false; IsEditor = true;
    LogToConsole = true; ShowErrorCount = true;
}

int32 UBSStartingAreaLookCommandlet::Main(const FString& Params)
{
    using namespace BSStartingAreaLook;
    const TSharedRef<FJsonObject> Report = MakeShared<FJsonObject>();
    Report->SetNumberField(TEXT("schema_version"), 1);
    Report->SetStringField(TEXT("map"), MapPath);
    Report->SetStringField(TEXT("started_utc"), FDateTime::UtcNow().ToIso8601());
    Report->SetBoolField(TEXT("saved"), false);
    Report->SetBoolField(TEXT("success"), false);
    Report->SetBoolField(TEXT("visually_approved"), false);
    Report->SetStringField(TEXT("status"), TEXT("started"));
    // Overwrite any older success before loading or mutating the map.
    if (!WriteReport(Report)) return 1;
    auto Fail = [&](int32 Code, const FString& Message)
    {
        Report->SetStringField(TEXT("status"), TEXT("failed"));
        Report->SetStringField(TEXT("error"), Message);
        Report->SetNumberField(TEXT("exit_code"), Code);
        if (!WriteReport(Report)) UE_LOG(LogBSStartingAreaLook, Error, TEXT("Could not write the failure report."));
        UE_LOG(LogBSStartingAreaLook, Error, TEXT("%s"), *Message);
        return Code;
    };

    FLook Look;
    if (!ReadBounded(Params, TEXT("SunPitch"), Look.SunPitch, -70.f, -5.f)
        || !ReadBounded(Params, TEXT("SunYaw"), Look.SunYaw, -180.f, 180.f)
        || !ReadBounded(Params, TEXT("SunIntensity"), Look.SunIntensity, 10000.f, 80000.f)
        || !ReadBounded(Params, TEXT("FillPitch"), Look.FillPitch, -85.f, -10.f)
        || !ReadBounded(Params, TEXT("FillYaw"), Look.FillYaw, -180.f, 180.f)
        || !ReadBounded(Params, TEXT("FillIntensity"), Look.FillIntensity, 0.f, 5000.f)
        || !ReadBounded(Params, TEXT("SkyIntensity"), Look.SkyIntensity, .2f, 3.f)
        || !ReadBounded(Params, TEXT("FogDensity"), Look.FogDensity, 0.f, .045f)
        || !ReadBounded(Params, TEXT("ExposureMin"), Look.ExposureMin, 9.f, 13.f)
        || !ReadBounded(Params, TEXT("ExposureMax"), Look.ExposureMax, 9.f, 14.f)
        || !ReadBounded(Params, TEXT("CloudBottomKm"), Look.CloudBottomKm, .5f, 4.f)
        || !ReadBounded(Params, TEXT("CloudHeightKm"), Look.CloudHeightKm, .3f, 6.f)
        || Look.ExposureMin > Look.ExposureMax)
        return Fail(2, TEXT("Invalid look parameters; map was not loaded or modified."));

    const FString MapFile = FPackageName::LongPackageNameToFilename(MapPath, FPackageName::GetMapPackageExtension());
    if (!FPaths::FileExists(MapFile)) return Fail(3, TEXT("Existing main map is missing; this commandlet never creates a replacement map."));
    UWorld* World = UEditorLoadingAndSavingUtils::LoadMap(MapFile);
    if (!World || World->GetOutermost()->GetName() != MapPath) return Fail(3, TEXT("Main map could not be loaded under its expected package name."));

    auto* Sun = ExactActor<ADirectionalLight>(World, TEXT("DemoGoldenHourSun"));
    auto* Fill = ExactActor<ADirectionalLight>(World, TEXT("CityCoolSkyFill"));
    auto* Sky = ExactActor<ASkyLight>(World, TEXT("DemoSkyLight"));
    auto* Fog = ExactActor<AExponentialHeightFog>(World, TEXT("DemoHeightFog"));
    auto* Exposure = ExactActor<APostProcessVolume>(World, TEXT("CityDaylightExposure"));
    auto* Clouds = ExactActor<AVolumetricCloud>(World, TEXT("HarborCumulusClouds"));
    auto* Demo = ExactActor<AActor>(World, TEXT("BorrowedSunDemoWorld"));
    if (!Sun || !Fill || !Sky || !Fog || !Exposure || !Clouds || !Demo)
        return Fail(4, TEXT("Required named actors are missing or have unexpected classes; nothing was modified."));
    auto* CloudComponent = Clouds->FindComponentByClass<UVolumetricCloudComponent>();
    if (!Sun->GetComponent() || !Fill->GetComponent() || !Sky->GetLightComponent() || !Fog->GetComponent() || !CloudComponent)
        return Fail(4, TEXT("A required actor has no expected light, fog or cloud component; nothing was modified."));
    UMaterialInterface* CloudMaterial = CloudComponent->Material.LoadSynchronous();
    if (!CloudMaterial) return Fail(4, TEXT("Existing cloud material is missing; nothing was modified."));

    int32 ActorCount = 0;
    for (TActorIterator<AActor> It(World); It; ++It) ++ActorCount;
    Report->SetNumberField(TEXT("actor_count_before"), ActorCount);
    const TSharedRef<FJsonObject> Before = MakeShared<FJsonObject>();
    Before->SetNumberField(TEXT("sun_lux"), Sun->GetComponent()->Intensity);
    Before->SetNumberField(TEXT("sun_pitch_degrees"), Sun->GetActorRotation().Pitch);
    Before->SetNumberField(TEXT("sun_yaw_degrees"), Sun->GetActorRotation().Yaw);
    Before->SetNumberField(TEXT("fill_lux"), Fill->GetComponent()->Intensity);
    Before->SetNumberField(TEXT("fill_pitch_degrees"), Fill->GetActorRotation().Pitch);
    Before->SetNumberField(TEXT("fill_yaw_degrees"), Fill->GetActorRotation().Yaw);
    Before->SetNumberField(TEXT("sky_intensity"), Sky->GetLightComponent()->Intensity);
    Before->SetNumberField(TEXT("fog_density"), Fog->GetComponent()->FogDensity);
    Before->SetNumberField(TEXT("exposure_min_ev100"), Exposure->Settings.AutoExposureMinBrightness);
    Before->SetNumberField(TEXT("exposure_max_ev100"), Exposure->Settings.AutoExposureMaxBrightness);
    Report->SetObjectField(TEXT("before"), Before);

    // Discover actual installed cloud controls for a later art review; do not guess their ranges.
    TArray<FMaterialParameterInfo> CloudParameters;
    TArray<FGuid> CloudParameterIds;
    CloudMaterial->GetAllScalarParameterInfo(CloudParameters, CloudParameterIds);
    TArray<TSharedPtr<FJsonValue>> CloudParameterNames;
    for (const auto& Parameter : CloudParameters) CloudParameterNames.Add(MakeShared<FJsonValueString>(Parameter.Name.ToString()));
    Report->SetArrayField(TEXT("cloud_scalar_parameter_names"), CloudParameterNames);
    Report->SetStringField(TEXT("cloud_material"), CloudMaterial->GetPathName());

    for (AActor* Actor : TArray<AActor*>{ Sun, Fill, Sky, Fog, Exposure, Clouds }) Actor->Modify();
    Sun->SetActorRotation(FRotator(Look.SunPitch, Look.SunYaw, 0));
    Sun->GetComponent()->SetIntensity(Look.SunIntensity);
    Sun->GetComponent()->SetLightColor(FLinearColor(1.f, .96f, .88f));
    Sun->GetComponent()->SetLightSourceAngle(.7f);
    Fill->SetActorRotation(FRotator(Look.FillPitch, Look.FillYaw, 0));
    Fill->GetComponent()->SetIntensity(Look.FillIntensity);
    Fill->GetComponent()->SetLightColor(FLinearColor(.92f, .94f, 1.f));
    Fill->GetComponent()->SetCastShadows(false);
    Sky->GetLightComponent()->SetIntensity(Look.SkyIntensity);

    // Close objects keep contrast; height falloff retains distant low-lying haze.
    Fog->GetComponent()->SetFogDensity(Look.FogDensity);
    Fog->GetComponent()->SetFogHeightFalloff(.20f);
    Fog->GetComponent()->SetStartDistance(2500.f);
    Fog->GetComponent()->SetVolumetricFogStartDistance(2500.f);
    Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(.38f, .43f, .48f));
    Fog->GetComponent()->SetVolumetricFog(true);

    Exposure->bUnbound = true;
    auto& Settings = Exposure->Settings;
    Settings.bOverride_AutoExposureMinBrightness = true;
    Settings.bOverride_AutoExposureMaxBrightness = true;
    Settings.AutoExposureMinBrightness = Look.ExposureMin;
    Settings.AutoExposureMaxBrightness = Look.ExposureMax;
    Settings.bOverride_AutoExposureBias = true;
    Settings.AutoExposureBias = 0.f;
    Settings.bOverride_AutoExposureSpeedUp = true;
    Settings.bOverride_AutoExposureSpeedDown = true;
    Settings.AutoExposureSpeedUp = 2.f;
    Settings.AutoExposureSpeedDown = 1.f;
    Settings.bOverride_BloomIntensity = true;
    Settings.BloomIntensity = .35f;

    // Preserve the cloud material and its coverage. Layer changes and a small sample increase
    // need actual skyline/performance review; they are not a promise of visible cumulus.
    CloudComponent->SetLayerBottomAltitude(Look.CloudBottomKm);
    CloudComponent->SetLayerHeight(Look.CloudHeightKm);
    CloudComponent->SetSkyLightCloudBottomOcclusion(.25f);
    CloudComponent->SetViewSampleCountScale(.8f);
    CloudComponent->SetReflectionViewSampleCountScale(.2f);
    CloudComponent->SetVisibility(true);

    const TSharedRef<FJsonObject> Applied = MakeShared<FJsonObject>();
    Applied->SetNumberField(TEXT("sun_pitch_degrees"), Look.SunPitch);
    Applied->SetNumberField(TEXT("sun_yaw_degrees"), Look.SunYaw);
    Applied->SetNumberField(TEXT("sun_lux"), Look.SunIntensity);
    Applied->SetStringField(TEXT("sun_linear_rgb"), TEXT("1.0, 0.96, 0.88"));
    Applied->SetNumberField(TEXT("sun_source_angle_degrees"), .7);
    Applied->SetNumberField(TEXT("fill_lux"), Look.FillIntensity);
    Applied->SetNumberField(TEXT("fill_pitch_degrees"), Look.FillPitch);
    Applied->SetNumberField(TEXT("fill_yaw_degrees"), Look.FillYaw);
    Applied->SetStringField(TEXT("fill_linear_rgb"), TEXT("0.92, 0.94, 1.0"));
    Applied->SetNumberField(TEXT("sky_intensity"), Look.SkyIntensity);
    Applied->SetNumberField(TEXT("fog_density"), Look.FogDensity);
    Applied->SetNumberField(TEXT("fog_height_falloff"), .20);
    Applied->SetNumberField(TEXT("fog_start_cm"), 2500);
    Applied->SetNumberField(TEXT("volumetric_fog_start_cm"), 2500);
    Applied->SetStringField(TEXT("fog_inscattering_linear_rgb"), TEXT("0.38, 0.43, 0.48"));
    Applied->SetNumberField(TEXT("exposure_min_ev100"), Look.ExposureMin);
    Applied->SetNumberField(TEXT("exposure_max_ev100"), Look.ExposureMax);
    Applied->SetNumberField(TEXT("exposure_bias"), 0);
    Applied->SetNumberField(TEXT("exposure_speed_up"), 2);
    Applied->SetNumberField(TEXT("exposure_speed_down"), 1);
    Applied->SetNumberField(TEXT("bloom_intensity"), .35);
    Applied->SetNumberField(TEXT("cloud_layer_bottom_km"), Look.CloudBottomKm);
    Applied->SetNumberField(TEXT("cloud_layer_height_km"), Look.CloudHeightKm);
    Applied->SetNumberField(TEXT("cloud_bottom_occlusion"), .25);
    Applied->SetNumberField(TEXT("cloud_view_sample_scale"), .8);
    Applied->SetNumberField(TEXT("cloud_reflection_sample_scale"), .2);
    Report->SetObjectField(TEXT("applied"), Applied);
    Report->SetStringField(TEXT("scope"), TEXT("Existing six named daylight actors only. Geometry, materials, gameplay, atmosphere model and cloud coverage are preserved."));
    World->UpdateWorldComponents(false, false);
    int32 ActorCountAfter = 0;
    for (TActorIterator<AActor> It(World); It; ++It) ++ActorCountAfter;
    Report->SetNumberField(TEXT("actor_count_after"), ActorCountAfter);
    if (ActorCountAfter != ActorCount) return Fail(5, TEXT("Actor count changed unexpectedly; map was not saved."));
    World->GetOutermost()->MarkPackageDirty();
    if (!UEditorLoadingAndSavingUtils::SaveMap(World, MapPath) || IFileManager::Get().FileSize(*MapFile) <= 0)
        return Fail(6, TEXT("Unreal did not confirm the map save. Inspect the package and editor log before retrying."));

    Report->SetBoolField(TEXT("saved"), true);
    Report->SetBoolField(TEXT("success"), true);
    Report->SetStringField(TEXT("status"), TEXT("saved_pending_visual_review"));
    Report->SetNumberField(TEXT("exit_code"), 0);
    Report->SetStringField(TEXT("finished_utc"), FDateTime::UtcNow().ToIso8601());
    if (!WriteReport(Report))
    {
        UE_LOG(LogBSStartingAreaLook, Error, TEXT("Map saved, but report write failed. Do not interpret the older report as this run's result."));
        return 7;
    }
    UE_LOG(LogBSStartingAreaLook, Display, TEXT("Starting-area look saved to the existing main map. %d actors preserved; rendered art and performance review remain required."), ActorCount);
    return 0;
}
