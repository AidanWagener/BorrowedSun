# Ordinary prop persistence foundation

13 October 2026

The [runtime object ledger](../../Game/Source/BorrowedSun/Public/Objects/BSObjectLedger.h) now persists ordinary prop identity, resting pose and collection custody through Unreal SaveGame slots. Its source checkpoint is `32b43651888f3a171f4a82b404d90f8ab817853a`. Ten distinct **data records**, not ten finished interactive models, have been exercised through actual disk save/reload. The ledger is not yet connected to actors, player controls or the delivered game. This is the first implementation stage of BS-027; carrying, throwing and physical reconstruction remain outstanding.

The acceptance condition for this stage is that repeated placement cannot resurrect a collected identity or reset a moved pose, refused/failed transfers preserve current state, successful transfers survive reload, and stale writers cannot overwrite later state.

## Implemented contract

- Each record stores canonical instance and definition IDs, definition revision, a resting transform, collectibility, one custody location, an object revision and the latest transaction receipt. No actor pointers or live rigid-body velocity are serialized.
- World and collection membership are mutually exclusive. The collection stub derives its contents from these same records; it does not create another inventory copy. Its caller-supplied capacity is a test mechanism, not a final inventory design.
- Re-registering an authored placement retains the saved record. Changing its definition, definition revision or collectibility requires an explicit migration. Mission-owned records are rejected; the existing sunseed authority is unchanged and is not integrated with this ledger.
- Move and collect require the expected object revision and a transaction GUID. Retrying the latest accepted operation returns the durable result without another write. Reusing that receipt with a different payload or replaying an older revision is rejected.
- Two separate `BorrowedSun_Objects_<namespace>_0/1` slots alternate complete generations. The namespace permits only 1–48 ASCII letters, digits, underscores and hyphens. These slots are separate from mission checkpoints.
- The next snapshot is written and read back before in-memory custody changes. An ambiguous read-back requires reload before further mutation. The previous slot remains available if a later generation is invalid.
- Loading rejects duplicate identities, invalid transforms, contradictory custody, unsupported schema, inconsistent transaction revisions and conflicting snapshots at the same generation. A slot belonging to a different world is not overwritten.

This is a synchronous API intended for serialized use on the game thread. It detects sequential stale sessions; it is not a cross-process transaction lock or multiplayer authority. A checkpoint is bounded to 4,096 records. Corruption recovery can restore the preceding generation and lose the latest action; `GetRecoveryWarning()` exposes that event and must be surfaced by the eventual player-facing integration. The validator checks record invariants, not arbitrary file tampering or every possible corruption pattern.

## Actor integration still required

Open the ledger before reconstructing interactive placements. Register each authored stable identity, then use its saved record to decide whether to spawn a world actor and where. `ShouldSpawnInWorld()` is currently a data predicate; no actor factory has been wired to it yet.

The physical layer must qualify render meshes and convex collision, enforce reach/mass/size gates, use a physics handle with world obstruction, and prevent held props from moving the player's capsule. It must supply a collision-validated resting or recovery transform when saving. The ledger currently validates mathematical transforms only; it does not sweep geometry or prevent an authored pose from intersecting a wall.

After collection succeeds, the actor can be suppressed. A refused transfer must retain the actor. Menu, unpossess, reset, load and scene transitions must release or safely preserve a held object before changing its representation. Streaming, explicit collection release, containers, inventory UI and destruction are later integration work; no such behavior is claimed by these tests.

The next concrete step is the ten-prop test court from the [world asset specification](../Art/WORLD_ASSET_SPEC.md): qualified props, table, shelves, stairs, narrow doorway, recovery volume and collection stub, with actual pickup/rotation/drop/throw, obstruction, settling, reload and mission-route regression checks.

## Executed evidence

| Check | Observed result | Evidence |
|---|---|---|
| Development Editor and Game compile | Passed on Unreal 5.8.2 CL 56702186 | [Build](../../Artifacts/build/20261013T063308Z-51d81eea/build-result.json) |
| Shipping Game compile | Passed from the clean source checkpoint; compilation only, not a newly qualified player package | [Build](../../Artifacts/build/20261013T063600Z-ce13a949/build-result.json) |
| Full expected smoke suite | All 17 tests succeeded; zero warnings, failures or unrun tests; NullRHI | [Result](../../Artifacts/tests/20261013T063409Z-a466bf70/test-result.json), [automation detail](../../Artifacts/tests/20261013T063409Z-a466bf70/Automation/index.json) |
| Real disk custody/reconstruction | Ten IDs; moved transform; collection refusal/success; retry after reload; repeated recipe registration; definition migration refusal; mission-owned record rejection | `BorrowedSun.Smoke.Objects.DiskCustodyAndReconstruction` |
| Failure and stale-write behavior | Injected write/read-back failures; no premature in-memory transfer; stale session and revision rejection; reconciliation on reload | `BorrowedSun.Smoke.Objects.FailureAtomicityAndStaleWrites` |
| Recovery and malformed records | Corrupted latest disk generation; previous snapshot recovery with warning; both-invalid rejection; future schema, wrong world, conflicting generation and unsafe namespace rejection | `BorrowedSun.Smoke.Objects.RecoveryAndValidation` |

Disk tests use fresh random QA namespaces and delete only their two own slots on normal completion. Failure injection uses the storage interface. These tests do not drive physics, physical input devices, rendered actors, shipping gameplay or the final inventory.

The first build exposed an existing unity-build name collision among city/HUD colour constants. The city palette identifiers now have a `City` prefix; their values and string literals were verified unchanged. The first test run stopped on a test-fixture error while duplicating an element from the same Unreal array. Copying the fixture record before adding it fixed that setup error; the full successful rerun above is the qualifying result. Earlier failures remain in `Artifacts/build/20261013T062519Z-e55b0bf3` and `Artifacts/tests/20261013T063018Z-cb4c6855`.

The separate [workshop surface candidate](WORKSHOP_SURFACES_2026-10-13.md) retains its earlier packaged source checkpoint. Its offscreen rendered routes and visual review passed; normal-window presentation and frame-pacing qualification remain pending. It does not contain this new ledger.
