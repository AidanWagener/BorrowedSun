# Local operations

Windows PowerShell 5.1 and PowerShell 7 are supported. Run from the workspace root. The engine and compiler are installed by their official installers, outside this repository. None of these scripts install tools, create a remote, fetch assets, publish a build or delete a directory.

1. Copy `Tools/Environment.example.json` to ignored `Tools/Environment.local.json`. Set `EngineRoot` to the installed engine directory, for example `C:\Program Files\Epic Games\UE_5.8`. This is local configuration, never credentials. Engine patch compatibility is recorded by the project documentation; this script does not silently upgrade it.
2. Run `powershell -NoProfile -File Tools/Doctor.ps1`, then `powershell -NoProfile -File Tools/Preflight.ps1`.
3. Preview `powershell -NoProfile -File Tools/Build.ps1`. Execute with `-Execute`. Both editor and game targets compile. Use `-UE_ROOT <path>` as a one-run override. Close the editor first. Configure `MaxParallelActions` to suit memory; two is a conservative starting point on this machine.
4. Preview or execute `Tools/Test.ps1` in the same way. A nonzero process exit, missing JSON, zero tests, insufficient test count, missing expected tests or any nonsuccess state fails the run. Set `ExpectedTests` to the exact approved smoke suite as it grows. `-NullRHI` is only for tests known not to need rendering; it is not visual or performance QA.
5. First Light already has a real authored map, `MapsToCook` and matching `GameDefaultMap`. Run `Tools/Preflight.ps1 -ForPackage` and `Tools/Build.ps1 -Mode Package -Execute`. For future maps, save them through Unreal and update these settings deliberately. Packaging is blocked until the saved map and default are valid. Success additionally requires the expected project executable. A package must still be launched and played separately.

Dry runs of Build and Test validate the project and show the plan, but do not write artifacts or run the engine. Successful real runs create fresh directories beneath `Artifacts/`. The scripts throw on failure; `powershell -File` or `pwsh -File` returns a nonzero exit. Logs can contain local paths and should remain private. Compact JSON summaries exclude environment dumps and remote URLs. Do not use shell metacharacters such as `&` or `%` in engine or workspace paths: the batch runner rejects them.

## City art authoring

The approved art direction is in `Docs/Art/ART_DIRECTION.md`. Reviewed CC0 source maps and FBX files are in `SourceArt/PolyHaven`, tracked with LFS; their hashes are in the two city intake registers. Fetch scripts use the public Poly Haven API with an identifying user agent and verify source checksums. No live asset API is used by the game.

```powershell
python Tools/Fetch-CityMaterials.py
python Tools/Fetch-CityProps.py
powershell -NoProfile -File Tools/Build.ps1 -Execute
powershell -NoProfile -File Tools/Author-CityArt.ps1 -Execute
```

`Author-CityArt` is a dry run without `-Execute` and refuses existing output unless `-Force` is deliberate. It verifies all 23 source files, then creates 42 real Unreal packages through editor APIs. Keep Unreal closed. The two scanned meshes retain native UVs; the modular architecture uses world-scale tiling. LFS lockable files can be read-only; make only the exact owned packages being deliberately regenerated writable. Do not recursively clear asset permissions.

Rendered screenshot validation now also captures six clean art review views after both mission routes finish: four city views and the workshop frontage and counter. Frame measurement remains a separate run without screenshots or camera tours.

## Starting market authoring

The approved generated target and visual acceptance criteria are in `Docs/Art/STARTING_MARKET_BENCHMARK.md`. Use the portable Blender version and checksum in `Config/blender-lock.json`; record its executable path locally. Run Blender with `--background --factory-startup --python Tools/Art/Build-MarketKit.py` from the repository root. It creates 21 FBX meshes: original architecture, textiles, lantern and Keeper sculpture, plus paving relief derived from a reviewed CC0 height scan. The editable `.blend` source and manifest retain dimensions, material slots, triangle counts and file hashes. Keep exports at centimetre scale. The FBX importer flips Y to Unreal's handedness: authored front faces become +Y; the scan relief compensates for this conversion.

`Tools/Fetch-MarketAssets.py` downloads the reviewed Poly Haven CC0 source set. `Docs/Registers/market-asset-intake.json` retains authors, URLs, license decisions and original file hashes. The generated cloth albedo has separate prompt provenance. After building the editor, run `Tools/Author-MarketArt.ps1 -Execute`; use `-Force` only when deliberately replacing this owned Market asset directory. The wrapper verifies source hashes and a fresh complete authoring report. The commandlet imports real assets, matches whole material-slot suffixes, handles the plant's separate masked leaf material and flips its OpenGL normal map. Opaque detail meshes and the tree use Nanite (tree shape preservation enabled); cloth and potted foliage retain ordinary mesh rendering. Windows explicitly targets DX12 Shader Model 6. Actual rendering and performance still require separate qualification.

For a 1080p comparison, use `Tools/Demo.ps1 -Mode Validate -Screenshots -Width 1920 -Height 1080 -UserDirectory <fresh-root> -Execute`. The harness verifies actual viewport dimensions, waits for editor asset/shader compilation and lets the opening dialogue fade before recording the idle starting pose. DPI-aware rendering and the validation-only `-ForceRes` flag prevent desktop scaling from silently reducing the requested resolution. Explicit automated runs ignore physical input; ordinary play retains it. The harness still exercises actual character movement and interactions, and does not certify physical keyboard or gamepad behavior.

## Backup and restore

The First Light delivery has a verified local snapshot, with current LFS payloads. See [demo validation](../Docs/Operations/DEMO_VALIDATION.md) for the observed restoration result and remaining off-device requirement.

After a reviewed first Git commit, stop editing and choose an existing backup directory outside this workspace, preferably another physical device. Preview `Tools/Backup.ps1 -BackupRoot <directory>`; add `-Execute` for a fresh snapshot. It copies tracked and nonignored untracked working files, creates and verifies a full Git bundle, and copies locally available LFS objects. Ignored local settings, secrets, caches and build artifacts are omitted. Keep purchased original downloads/receipts separately under the same rights and backup policy. No historical LFS completeness is claimed: intentionally fetch all retained historical objects before a history-complete backup only after checking bandwidth and space.

Run `Tools/VerifyBackup.ps1 -SnapshotPath <snapshot>`. It checks every manifest file and rejects escaping paths and reparse points. It does not authenticate malicious changes. Incomplete runs have no valid completed manifest and remain for inspection. There is no pruning or automatic deletion.

A monthly restore drill is a separate requirement: create a new empty directory on a spare disk, temporarily set `GIT_LFS_SKIP_SMUDGE=1` for a local `git clone <snapshot>/repository.bundle <new-directory>`, restore `LfsObjects/` into the new clone's `.git/lfs/objects/`, then materialize the current working snapshot from `WorkingTree/` and check its hashes. Restore local environment configuration manually. Note that the working snapshot is authoritative for uncommitted additions/edits/deletions; a naive overlay on a clone can resurrect deleted files, so compare the manifest and remove only individually reviewed extra files. Run preflight, editor/game compile, tests and a playable package. Disable network while checking that the current snapshot works. Historical commits require their own LFS completeness check. Record date, snapshot ID, restore duration and failures. Do not call a checksum pass a restore drill.

## World library planning tools

`Tools/Art/Build-WorldCatalog.py` generates 4,200 planned geometry briefs, `Docs/Registers/world-asset-catalog.json`, the family index and the adjacent offline catalog. It generates planning data, not 3D models. Definition IDs derive from family, form and construction keys. The tool refuses to regenerate a register containing mapped or accepted production progress so that qualification work cannot be silently erased. Reconcile progress and ID migrations before changing an active register.

`Tools/Art/Render-WorldSpec.mjs <node_modules-directory>` renders the two Markdown documents in `Docs/Art` using the existing `marked` module. It copies readable Markdown and standalone HTML deliverables beside the workspace. Use the configured Node runtime; no hosted service or network is required. The offline catalog reads embedded data and exports filtered JSON locally.

The Second Dawn source generator is `Tools/Art/Build-LanternWorkshop.py`, run with the pinned Blender executable and `--background --factory-startup --python Tools/Art/Build-LanternWorkshop.py` from the repository root. It creates 12 meshes, the editable source, assembly recipe and hashes. After compiling the editor, `Tools/Author-MarketArt.ps1 -WorkshopOnly -Force -Execute` imports the 23 workshop packages: 12 meshes, seven dedicated materials, three wood texture maps and the assembly. It retains existing market content and reuses the shared shadow material. The manifest records mesh/material counts and the UV/vertex-colour contract. A full market import also includes the workshop. Source-only checks do not replace a rendered and packaged review.

`Tools/Fetch-WorkshopWood.py` retrieves the reviewed Poly Haven `fine_grained_wood` 2K colour, OpenGL normal and roughness maps. It checks pinned publisher sizes/MD5 before saving original bytes and records SHA-256 in `Docs/Registers/workshop-surface-intake.json`. The importer flips the normal map's green channel once. Each wooden component maps V along its length at 60 cm per tile, with deterministic offsets between boards; transverse end faces use separate charts. Workshop vertex colour R is unsigned angular variation, G is authored contact polish and B is localized dirt. These masks are neither ambient occlusion nor uniformly applied wear. FBX export uses `colors_type='LINEAR'` to retain numeric weights. Extra surface density is limited to the bench, cabinet and tool tray where the masks need interpolation.

## Tripo API adapter

`Tools/Tripo.ps1` wraps the official API CLI pinned to version 0.6.1 in `Config/tripo-lock.json`. It creates one text-to-model task per authored brief. **This is an API workflow:** Tripo Studio credits cannot fund API requests; the two products have separate credit pools. See the [official Studio/API explanation](https://www.tripo3d.ai/help/api-plugins/tripo-studiotripo-api). The user's authorization covers spending existing credits for game assets; it does not purchase credits or enable auto-recharge. This adapter has no purchase, top-up or batch operation.

The first generated candidates use Studio separately; see [Studio intake](../Docs/Operations/TRIPO_STUDIO_INTAKE.md). API job state and Resume commands below do not recover Studio generations. The register records actual Studio settings separately from the brief's proposed API model version and seeds.

Run from the repository root. `Doctor` checks the installed CLI and version locally; it does not check authentication, balance or connectivity. `Plan` needs neither authentication nor the CLI and sends nothing. `Generate` without `-Execute` also prints a plan. The official CLI manages authentication internally; this adapter never reads its credential store. It invokes the npm shim's installed Node entrypoint directly so authored quotes, Unicode and shell characters remain prompt data.

```powershell
powershell -NoProfile -File Tools/Tripo.ps1 -Mode Doctor
powershell -NoProfile -File Tools/Tripo.ps1 -Mode Plan -BriefFile SourceArt/Tripo/Briefs/moth-service-lantern-v1.json
powershell -NoProfile -File Tools/Tripo.ps1 -Mode Generate -BriefFile SourceArt/Tripo/Briefs/moth-service-lantern-v1.json -Execute
powershell -NoProfile -File Tools/Tripo.ps1 -Mode Resume -BriefFile SourceArt/Tripo/Briefs/moth-service-lantern-v1.json
```

The JSON brief has `schemaVersion: 1`, a stable lowercase `id`, a `prompt` of at most 1,024 characters, a reviewed `model` alias, an integer `seed`, and a `params` object. `seed` supplies `model_seed`, `image_seed` and `texture_seed`; explicit seed parameters can override individual stages. Optional `artReview` metadata stays local. For example:

```json
{
  "schemaVersion": 1,
  "id": "workshop-lamp-candidate-v1",
  "prompt": "A maintained brass repair lamp with an indigo enamel shade and visible fastened joints.",
  "model": "tripo-v3.1",
  "seed": 41001,
  "params": {
    "face_limit": 30000,
    "texture": true,
    "pbr": true,
    "texture_quality": "detailed",
    "geometry_quality": "detailed",
    "auto_size": true
  }
}
```

Each job owns ignored `Artifacts/tripo/<id>/request.json`, `state.json` and `downloads/`. Generation records an `unknown` transaction before contacting the API, then immediately saves the actual task ID as `submitted`. It returns without waiting. Repeating `Generate` for any recorded job is refused, including failed or ambiguous jobs. Keep the exact brief bytes for that job; author a new ID deliberately for a different candidate.

`Resume` makes one query against the saved task ID and downloads only when that same task succeeds. Repeat it later while the state remains `submitted`. Completion requires downloaded model files, nonzero sizes and SHA-256 hashes. Query or download errors retain the task ID for another `Resume`; terminal server failures are recorded as `fail`. Inspect the returned `status` rather than treating process exit alone as evidence of a completed model. Requests are bounded to 300 seconds by default (`-TimeoutSeconds`, 10..600).

If submission returns no confirmed ID, the state remains `unknown`; never resubmit to resolve a timeout. Verify the matching task in Tripo first, then recover that existing task with `-Mode Resume -BriefFile <original-brief> -TaskId <verified-task-id>`. Raw CLI diagnostics and signed download URLs are excluded from the durable state. A completed candidate is still outside Unreal: review provenance, dimensions, materials, topology, collision and actual runtime appearance before importing it.

`powershell -NoProfile -File Tools/Tests/Run-TripoTests.ps1` runs offline mock-CLI regression tests for safe prompt arguments, version enforcement, duplicate prevention, ambiguous submission recovery, durable task IDs, failed downloads, artifact hashes and path containment. Fixtures remain under `Artifacts/tripo-tests/`; these tests do not generate models or verify the API account.

## Gameplay and tooling tests

The expected smoke suite now contains 17 tests. Three object-ledger tests cover real isolated SaveGame slots, collection custody, repeated placement/reload, failed writes, stale sessions and malformed checkpoint recovery. Run `Tools/Test.ps1 -Execute -NullRHI` after compiling. They qualify the [persistence foundation](../Docs/Operations/OBJECT_LEDGER_2026-10-13.md), not physical carrying or rendered object behavior.

Player packages use `Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute`; the Editor target remains Development. `Tools/Demo.ps1 -UserDirectory <isolated-root>` passes Unreal's player-data override and derives the correct validation report path for that root. This keeps QA preferences and checkpoints separate from normal play. For pacing measurement, use `-Mode Validate -MeasureFrames`; it requires the 60 FPS target and both routes, and rejects combining measurement with `-Screenshots`. The generated JSON includes sample counts and percentiles; it does not measure GPU work separately.

`Tools/Demo.ps1 -Mode Launch -Execute` starts the compiled demo without imposing a session timeout. `-Mode Validate -Screenshots -Execute` runs both mission routes with rendering, isolated QA saves and strict report validation. For an existing package, supply `-PackageExecutable` and its actual `-ReportDirectory`; see `Get-Help ./Tools/Demo.ps1 -Full`. `-Mode AuthorMap -Execute` runs the editor commandlet; replacing existing owned map/material files also requires `-Force`. These operations never build automatically. Keep asset authoring and builds serial.

Add `-Offscreen` to `-Mode Validate -Screenshots` to use a real RHI viewport without a visible game window. Supply an explicit isolated `-UserDirectory`; the script requires it to separate QA preferences and saves. This was exercised against the workshop surface Shipping candidate at 1920 x 1080: both routes, disk checkpoints and 12 captures completed. The engine's pinned `FSlateApplication::MakeWindow` creates a generic window for this mode and `UGameEngine::CreateGameWindow` does not show it. A separate immutable player package can remain open, but concurrent GPU load and absent display presentation make this unsuitable for performance qualification. The script rejects `-Offscreen -MeasureFrames` and offscreen interactive launch; do not substitute NullRHI for rendered review.

`Tools/Tests/Run-DemoTests.ps1` checks the demo launcher and report validator without Unreal. Its fixtures do not replace the actual rendered acceptance run or a human playthrough.

`powershell -NoProfile -File Tools/Tests/Run-Tests.ps1` runs isolated tooling regression tests without Unreal, network access or Pester. Fixtures are kept under `Artifacts/tooling-tests/` for inspection. They cover batch quoting, failure/timeout enforcement, report gates and backup corruption/path rejection.

`.github/manual-windows.yml.example` is disabled. Start with local scripts. A future owner-reviewed private self-hosted runner may use the template only after its trust boundaries, dedicated account, repository access and manual enable variable are configured. Never execute untrusted PRs on a personal workstation. Do not connect repository events to an unattended agent with credentials. No paid service is needed for this workflow.
