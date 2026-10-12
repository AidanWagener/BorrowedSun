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

Rendered screenshot validation now also captures four clean city review views after both mission routes finish. Frame measurement remains a separate run without screenshots or camera tours.

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

The Second Dawn source generator is `Tools/Art/Build-LanternWorkshop.py`, run with the pinned Blender executable in background mode from the repository root. It creates 11 meshes, the editable source, assembly recipe and hashes. After compiling the editor, `Tools/Author-MarketArt.ps1 -WorkshopOnly -Force -Execute` imports the 16 workshop packages while retaining the existing market content. A full market import now includes the workshop. Source-only checks do not replace a rendered and packaged review.

## Gameplay and tooling tests

Player packages use `Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute`; the Editor target remains Development. `Tools/Demo.ps1 -UserDirectory <isolated-root>` passes Unreal's player-data override and derives the correct validation report path for that root. This keeps QA preferences and checkpoints separate from normal play. For pacing measurement, use `-Mode Validate -MeasureFrames`; it requires the 60 FPS target and both routes, and rejects combining measurement with `-Screenshots`. The generated JSON includes sample counts and percentiles; it does not measure GPU work separately.

`Tools/Demo.ps1 -Mode Launch -Execute` starts the compiled demo without imposing a session timeout. `-Mode Validate -Screenshots -Execute` runs both mission routes with rendering, isolated QA saves and strict report validation. For an existing package, supply `-PackageExecutable` and its actual `-ReportDirectory`; see `Get-Help ./Tools/Demo.ps1 -Full`. `-Mode AuthorMap -Execute` runs the editor commandlet; replacing existing owned map/material files also requires `-Force`. These operations never build automatically. Keep asset authoring and builds serial.

`Tools/Tests/Run-DemoTests.ps1` checks the demo launcher and report validator without Unreal. Its fixtures do not replace the actual rendered acceptance run or a human playthrough.

`powershell -NoProfile -File Tools/Tests/Run-Tests.ps1` runs isolated tooling regression tests without Unreal, network access or Pester. Fixtures are kept under `Artifacts/tooling-tests/` for inspection. They cover batch quoting, failure/timeout enforcement, report gates and backup corruption/path rejection.

`.github/manual-windows.yml.example` is disabled. Start with local scripts. A future owner-reviewed private self-hosted runner may use the template only after its trust boundaries, dedicated account, repository access and manual enable variable are configured. Never execute untrusted PRs on a personal workstation. Do not connect repository events to an unattended agent with credentials. No paid service is needed for this workflow.
