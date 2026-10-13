#include "Objects/BSPhysicalObject.h"
#include "Objects/BSCarryComponent.h"
#include "Objects/BSHandProps.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Demo/BSExplorerCharacter.h"
#include "Demo/BSDemoPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/AutomationTest.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"

namespace BSPhysicalTests
{
    FBSStableId Id(const TCHAR* Space, const TCHAR* Key)
    { FBSStableId Result; Result.Namespace = Space; Result.Key = Key; return Result; }

    struct FStorage final : IBSObjectStorage
    {
        TMap<int32, FBSObjectSnapshot> Slots;
        bool bFailWrite = false, bFailReadBack = false, bPendingReadFailure = false;
        bool Read(int32 Slot, FBSObjectSnapshot& Out, bool& Exists, FString& Error) override
        {
            Error.Reset(); Exists = Slots.Contains(Slot);
            if (bPendingReadFailure) { bPendingReadFailure = false; Error = TEXT("Injected readback fault"); return false; }
            if (Exists) Out = Slots[Slot]; return true;
        }
        bool Write(int32 Slot, const FBSObjectSnapshot& Value, FString& Error) override
        {
            if (bFailWrite) { Error = TEXT("Injected write fault"); return false; }
            Slots.Add(Slot, Value); bPendingReadFailure = bFailReadBack; return true;
        }
    };

    struct FPhysicalTestScene
    {
        UWorld* World;
        UStaticMesh* Cube;
        FStorage* Disk;
        UBSObjectWorldSubsystem* Store;
        FString Error;
        FPhysicalTestScene()
        {
            const auto Init = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
                .RequiresHitProxies(false).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(true);
            World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Init);
            GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            World->GetWorldSettings()->NotifyBeginPlay();
            World->GetWorldSettings()->NotifyMatchStarted();
            Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
            Store = World->GetSubsystem<UBSObjectWorldSubsystem>();
            auto Storage = MakeUnique<FStorage>(); Disk = Storage.Get();
            Store->OpenWithStorage(Id(TEXT("qa"), TEXT("physical_world")), MoveTemp(Storage),
                FBox(FVector(-2000, -2000, -300), FVector(2000, 2000, 2000)), Error);
            Solid(FVector(0,0,-25), FVector(20,20,.5));
        }
        ~FPhysicalTestScene()
        {
            World->BeginTearingDown();
            World->EndPlay(EEndPlayReason::Quit);
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
        }
        void Tick(int32 Frames)
        {
            for (int32 I = 0; I < Frames; ++I) { World->Tick(LEVELTICK_All, 1.f / 60.f); ++GFrameCounter; }
        }
        AStaticMeshActor* Solid(FVector Location, FVector Scale)
        {
            auto* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
            Actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
            Actor->GetStaticMeshComponent()->SetStaticMesh(Cube);
            Actor->SetActorScale3D(Scale);
            Actor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
            Actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
            return Actor;
        }
        FBSPhysicalObjectPlacement Placement(const TCHAR* Name, FVector Location)
        {
            FBSPhysicalObjectPlacement P; P.InstanceId = Id(TEXT("lab"), Name); P.DefinitionId = Id(TEXT("prop"), TEXT("qa_cube"));
            P.DisplayName = Name; P.Mesh = Cube; P.MassKg = 1; P.Transform.SetLocation(Location); return P;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSPhysicalReconstructionTest, "BorrowedSun.Smoke.Objects.PhysicalReconstructionAndFailures",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSPhysicalReconstructionTest::RunTest(const FString& Parameters)
{
    using namespace BSPhysicalTests;
    FPhysicalTestScene S;
    if (!TestTrue(TEXT("Physical world opens"), S.Store->GetLedger() != nullptr)) return false;
    const auto P = S.Placement(TEXT("cup"), FVector(0,0,200));
    auto* Object = S.Store->RegisterPlacement(P, S.Error);
    if (!TestNotNull(TEXT("Placement creates a simulated world actor"), Object)) { AddError(S.Error); return false; }
    TestTrue(TEXT("Simple collision has a real physics body"), Object->GetBody()->IsSimulatingPhysics());
    TestNull(TEXT("Duplicate live identity cannot create a second actor"), S.Store->RegisterPlacement(P, S.Error));
    TestFalse(TEXT("Floor intersection is rejected"), Object->IsPoseClear(FTransform(FVector(0,0,10))));
    TestFalse(TEXT("Recovery region bounds reject the whole object, not just its pivot"), Object->IsPoseClear(FTransform(FVector(1990,0,100))));
    S.Tick(360);
    TestTrue(TEXT("Gravity makes the body land on the real floor"), Object->GetActorLocation().Z > 48 && Object->GetActorLocation().Z < 53);
    TestFalse(TEXT("The settled prop sleeps"), Object->GetBody()->IsAnyRigidBodyAwake());
    TestTrue(TEXT("Settled collision-checked pose persists"), S.Store->SaveSettledObjects(S.Error));
    const FTransform Saved = S.Store->GetLedger()->GetSnapshot().Find(P.InstanceId)->RestTransform;
    TestTrue(TEXT("Saved pose matches simulated landing"), Saved.Equals(Object->GetActorTransform(), .1));
    S.Disk->bFailWrite = true;
    TestFalse(TEXT("A failed save refuses collection"), Object->CommitCollection(Id(TEXT("collection"), TEXT("bin")), 10, S.Error));
    TestFalse(TEXT("Failed collection keeps the prop visible"), Object->IsHidden());
    TestTrue(TEXT("Failed collection keeps simulation"), Object->GetBody()->IsSimulatingPhysics());
    S.Disk->bFailWrite = false; S.Disk->bFailReadBack = true;
    TestFalse(TEXT("Ambiguous write is not reported successful"), Object->CommitCollection(Id(TEXT("collection"), TEXT("bin")), 10, S.Error));
    TestFalse(TEXT("Unconfirmed transfer keeps its representation until reconciliation"), Object->IsHidden());
    S.Disk->bFailReadBack = false;
    TestTrue(TEXT("Reload reconciles the accepted disk transfer"), S.Store->Reload(S.Error));
    TestTrue(TEXT("Previous world actor was removed"), Object->IsActorBeingDestroyed());
    TestNull(TEXT("Saved collection prevents reconstruction"), S.Store->RegisterPlacement(P, S.Error));
    TestTrue(TEXT("Collected absence is successful, not a spawn error"), S.Error.IsEmpty());
    TestEqual(TEXT("Identity remains singular"), S.Store->GetLedger()->GetSnapshot().Objects.Num(), 1);
    auto Unsafe = S.Placement(TEXT("unsafe"), FVector(100,0,100)); Unsafe.bMissionCustody = true;
    TestNull(TEXT("Mission custody cannot be moved into ordinary collection"), S.Store->RegisterPlacement(Unsafe, S.Error));
    TestEqual(TEXT("Refused mission object adds no record"), S.Store->GetLedger()->GetSnapshot().Objects.Num(), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSPhysicalCarryTest, "BorrowedSun.Smoke.Objects.PhysicalCarryAndRelease",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSPhysicalCarryTest::RunTest(const FString& Parameters)
{
    using namespace BSPhysicalTests;
    FPhysicalTestScene S;
    FActorSpawnParameters Always; Always.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Pawn = S.World->SpawnActor<ABSExplorerCharacter>(FVector(-170,0,100), FRotator::ZeroRotator, Always);
    auto* PC = S.World->SpawnActor<APlayerController>(); PC->Possess(Pawn); PC->SetControlRotation(FRotator::ZeroRotator);
    auto* Hand = Pawn->GetCarryComponent();
    const auto P = S.Placement(TEXT("carried"), FVector(0,0,172));
    auto* Object = S.Store->RegisterPlacement(P, S.Error);
    if (!TestNotNull(TEXT("Carry test prop spawns"), Object)) { AddError(S.Error); return false; }
    TestFalse(TEXT("Empty hand does not tick"), Hand->IsComponentTickEnabled());
    TestEqual(TEXT("Eye ray reaches the real prop"), Hand->TraceObject(), Object);
    auto* Wall = S.Solid(FVector(-90,0,180), FVector(.1,2,3));
    TestNull(TEXT("Opaque wall blocks reaching the prop"), Hand->TraceObject()); Wall->Destroy();
    if (!TestTrue(TEXT("A simulated grip is acquired"), Hand->TryGrab(S.Error))) { AddError(S.Error); return false; }
    TestTrue(TEXT("Held object still simulates"), Object->GetBody()->IsSimulatingPhysics());
    TestTrue(TEXT("Hand and actor agree on ownership"), Object->IsHeld() && Hand->GetHeldObject() == Object);
    TestFalse(TEXT("Occupied hand cannot grab twice"), Hand->TryGrab(S.Error));
    PC->SetControlRotation(FRotator(0,18,0)); S.Tick(60);
    TestTrue(TEXT("Physics handle moves the body toward the view"), Object->GetActorLocation().Y > 15);
    TestEqual(TEXT("Handle remains attached during normal movement"), Hand->GetHeldObject(), Object);
    const FQuat Before = Object->GetActorQuat(); Hand->RotateHeld(12,0); S.Tick(30);
    TestFalse(TEXT("Rotation input physically turns the held body"), Object->GetActorQuat().Equals(Before,.01));
    S.Disk->bFailWrite = true;
    TestFalse(TEXT("Collection failure while held is reported"), Hand->Collect(Id(TEXT("collection"),TEXT("bin")), 10, S.Error));
    TestEqual(TEXT("Failed collection retains the physical grip"), Hand->GetHeldObject(), Object);
    S.Disk->bFailWrite = false;
    Hand->Release(true); S.Tick(1);
    TestFalse(TEXT("Throw clears actor ownership"), Object->IsHeld());
    TestNull(TEXT("Throw clears hand ownership"), Hand->GetHeldObject());
    TestFalse(TEXT("Empty hand stops ticking after throw"), Hand->IsComponentTickEnabled());
    TestTrue(TEXT("Throw adds real bounded velocity"), Object->GetBody()->GetPhysicsLinearVelocity().Size() > 350
        && Object->GetBody()->GetPhysicsLinearVelocity().Size() < 820);
    // Reposition only the fixture; the gameplay carry path never teleports held bodies.
    Object->SetActorLocation(FVector(0,0,172),false,nullptr,ETeleportType::TeleportPhysics);
    Object->GetBody()->SetPhysicsLinearVelocity(FVector::ZeroVector); PC->SetControlRotation(FRotator::ZeroRotator);
    TestTrue(TEXT("Released prop can be grabbed again"), Hand->TryGrab(S.Error));
    PC->UnPossess();
    TestNull(TEXT("Possession changes release stale hand references"), Hand->GetHeldObject());
    TestFalse(TEXT("Unpossessed prop is no longer held"), Object->IsHeld());
    PC->Possess(Pawn); PC->SetControlRotation(FRotator::ZeroRotator);
    TestTrue(TEXT("Can grip after repossession"), Hand->TryGrab(S.Error));
    TestTrue(TEXT("Successful collection while held commits"), Hand->Collect(Id(TEXT("collection"),TEXT("bin")), 10, S.Error));
    TestNull(TEXT("Collection releases the physical constraint"), Hand->GetHeldObject());
    TestTrue(TEXT("Committed collection hides the world prop"), Object->IsHidden());
    TestFalse(TEXT("Collected body cannot collide or simulate"), Object->GetBody()->IsSimulatingPhysics());
    TestEqual(TEXT("Collected collision disabled"), Object->GetBody()->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBSImportedHandPropTest, "BorrowedSun.Smoke.Objects.ImportedHandPropsAndCityController",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBSImportedHandPropTest::RunTest(const FString& Parameters)
{
    using namespace BSPhysicalTests;
    FPhysicalTestScene S;
    TArray<ABSPhysicalObject*> Props;
    for(int32 I=0;I<BSHandProps::Count;++I)
    {
        FBSPhysicalObjectPlacement P;
        if(!TestTrue(TEXT("Authored model loads"),BSHandProps::MakePlacement(I,TEXT("import_qa"),
            FTransform(FVector(-400+(I%5)*200,(I/5)*250,180)),P,S.Error))) return false;
        auto* Object=S.Store->RegisterPlacement(P,S.Error);
        if(!TestNotNull(*P.DisplayName,Object)) {AddError(S.Error);return false;}
        TestTrue(TEXT("Imported body can be carried"),Object->CanCarry()); Props.Add(Object);
    }
    S.Tick(1200);
    for(auto* Object:Props)
    {
        const FString Name=Object->GetPlacement().DisplayName;
        double Lowest=DBL_MAX;
        const FMeshDescription* Mesh=Object->GetPlacement().Mesh->GetMeshDescription(0);
        if(!TestNotNull(TEXT("Imported source geometry is available for contact measurement"),Mesh))return false;
        const auto Positions=FStaticMeshConstAttributes(*Mesh).GetVertexPositions();
        for(const FVertexID Vertex:Mesh->Vertices().GetElementIDs())
            Lowest=FMath::Min(Lowest,Object->GetActorTransform().TransformPosition(FVector(Positions[Vertex])).Z);
        AddInfo(FString::Printf(TEXT("%s: min vertex Z %.3f cm, speed %.3f cm/s, angular speed %.3f deg/s"),*Name,Lowest,
            Object->GetBody()->GetPhysicsLinearVelocity().Size(),Object->GetBody()->GetPhysicsAngularVelocityInDegrees().Size()));
        TestFalse(*(Name+TEXT(" sleeps after landing")),Object->GetBody()->IsAnyRigidBodyAwake());
        TestTrue(*(Name+TEXT(" clears actual floor collision")),Object->IsPoseClear(Object->GetActorTransform()));
        TestTrue(*(Name+TEXT(" remains on the floor")),Lowest> -1 && Lowest<2);
        TestTrue(*(Name+TEXT(" saves its settled pose")),Object->PersistRestingPose(S.Error));
    }
    FActorSpawnParameters Always; Always.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Pawn=S.World->SpawnActor<ABSExplorerCharacter>(FVector(-170,-400,100),FRotator::ZeroRotator,Always);
    auto* PC=S.World->SpawnActor<ABSDemoPlayerController>(); PC->Possess(Pawn); PC->Confirm();
    auto* Hand=Pawn->GetCarryComponent();
    for(auto* Object:Props)
    {
        // Only the fixture is teleported. The player path uses the same physical grip as the city.
        Object->SetActorLocation(FVector(0,-400,165),false,nullptr,ETeleportType::TeleportPhysics);
        Object->GetBody()->SetPhysicsLinearVelocity(FVector::ZeroVector);
        FVector Eye; FRotator View; Pawn->GetActorEyesViewPoint(Eye,View);
        PC->SetControlRotation((Object->GetBody()->Bounds.Origin-Eye).Rotation());
        if(!TestEqual(TEXT("Ray reaches imported model"),Hand->TraceObject(),Object)) return false;
        PC->GrabProp();
        if(!TestEqual(TEXT("Main city controller grips imported prop"),Hand->GetHeldObject(),Object))return false;
        Hand->RotateHeld(12,8); S.Tick(15);
        TestEqual(TEXT("Imported prop stays held during rotation"),Hand->GetHeldObject(),Object);
        PC->ToggleMenu(); TestNull(TEXT("Menu releases imported body"),Hand->GetHeldObject()); PC->Confirm();
        Object->SetActorLocation(FVector(0,-400,165),false,nullptr,ETeleportType::TeleportPhysics);
        Object->GetBody()->SetPhysicsLinearVelocity(FVector::ZeroVector);
        PC->SetControlRotation((Object->GetBody()->Bounds.Origin-Eye).Rotation());
        PC->GrabProp(); TestEqual(TEXT("Prop can be gripped again"),Hand->GetHeldObject(),Object);
        PC->ThrowProp(); S.Tick(1); // Chaos applies queued impulses on the simulation step.
        TestNull(TEXT("Throw clears main city grip"),Hand->GetHeldObject());
        TestTrue(*(Object->GetPlacement().DisplayName+TEXT(" receives bounded throw velocity")),Object->GetBody()->GetPhysicsLinearVelocity().Size()>500
            && Object->GetBody()->GetPhysicsLinearVelocity().Size()<820);
        Object->Destroy();
    }
    return true;
}
#endif
