# Borrowed Sun workspace

A local-first Unreal project for a solo city-to-orbit adventure. The first demo, **First Light**, implements a compact walking/flight expedition with two persistent outcomes. See [demo scope](Docs/Operations/DEMO_SCOPE.md) for acceptance and limits; qualification results belong in the validation records. Initial tool/asset spending ceiling: under EUR 500; setup purchases: EUR 0.

## Start here

The new [worldbuilding and asset specification](Docs/Art/WORLD_ASSET_SPEC.md) defines Orison's construction language, 4,200 planned geometry briefs, physical-object contracts and staged destruction. Use the [kit shortlist](Docs/Art/WORLD_KIT_SHORTLIST.md), [complete family index](Docs/Art/WORLD_ASSET_FAMILIES.md) and [catalog register](Docs/Registers/world-asset-catalog.json) for production planning. The adjacent `Borrowed_Sun_Asset_Catalog.html` is a searchable offline review artifact. Planned entries are not completed models.

1. Read [the current market build validation](Docs/Operations/DEMO_V04_VALIDATION.md) and [approved art direction](Docs/Art/ART_DIRECTION.md). Earlier [v0.3](Docs/Operations/DEMO_V03_VALIDATION.md), [v0.2](Docs/Operations/DEMO_V02_VALIDATION.md), [first playable](Docs/Operations/DEMO_VALIDATION.md), and [foundation](Docs/Operations/VALIDATION.md) records remain historical evidence.
2. Read [local operations](Tools/README.md). The installed engine is pinned in [engine-lock.json](Config/engine-lock.json). Personal engine paths belong in ignored `Tools/Environment.local.json`.
3. Run the doctor and preflight from this directory:

```powershell
powershell -NoProfile -File Tools/Doctor.ps1 -RequireEngine
powershell -NoProfile -File Tools/Preflight.ps1
```

4. Preview build/test commands, then execute deliberately with `-Execute`. The test suite requires expected tests to actually run. Close Unreal before source builds.

```powershell
powershell -NoProfile -File Tools/Build.ps1
powershell -NoProfile -File Tools/Build.ps1 -Execute
powershell -NoProfile -File Tools/Test.ps1 -Execute -NullRHI
```

5. The authored demo map is now `/Game/BorrowedSun/Maps/L_BorrowedSunDemo`; its map and material were created and saved through Unreal's commandlet APIs. `MapsToCook` and the default map are configured. See [first-map runbook](Docs/Operations/FIRST_MAP.md) for provenance and regeneration. A bundle restore was rebuilt in a separate local checkout; independent-device recovery remains an operational requirement.

```powershell
powershell -NoProfile -File Tools/Preflight.ps1 -ForPackage
powershell -NoProfile -File Tools/Build.ps1 -Mode Package -Execute
```

Do not mistake a headless smoke test for a rendering, gameplay, cook or Shipping validation. See the validation record for the exact current result.

## Where things live

| Path | Purpose |
|---|---|
| Game | Unreal project, source modules, owned config and future content |
| SourceArt | Reviewed CC0 source maps and props with hashes in the intake registers |
| Config | Reviewed engine/plugin pin |
| Tools | Build, test, doctor, preflight, backup and tooling regression commands |
| Docs/Registers | Backlog, decisions, asset candidates, budget and machine observation |
| Docs/Templates | Task, asset intake, dependency qualification, playtest and decision templates |
| Docs/Research | Development handbook, additional framework research and source registers |
| Artifacts | Ignored logs, test fixtures and build output |
| Vault | Optional ignored private license evidence; requires independent backup |

The full design remains in the accompanying Borrowed Sun game specification. This workspace contains the demo source and development workflow. The first city block combines original modular geometry with reviewed CC0 textures and scanned market props in one continuous map. Broader streaming, combat, NPCs and paid vendor-kit production remain later work. Remote CI is not enabled.

## Play and validate First Light

Compile first, then use `Tools/Demo.ps1 -Mode Launch -Execute` to launch the current source project. The script defaults to a dry run. `-Mode Validate -Screenshots -Execute` runs both mission outcomes with rendering and fresh isolated save slots, preserving JSON and screenshots. It exercises gameplay methods and movement; physical keyboard/controller feel still requires playtesting. See the script's help and [demo scope](Docs/Operations/DEMO_SCOPE.md).

The loop is: take the sunseed, board Kestrel, fly to the sky relay, choose clinic or evacuation power, return, disembark and install it. **E** interacts, **G** toggles optional flight assist, **R** recovers a safe position, **F5/F9** save/load, and **ESC** opens the menu. A new expedition requires an explicit menu confirmation.

The current verified player build is **v0.4.0 Shipping**, in the adjacent `BorrowedSunMarket` folder. `Play_Borrowed_Sun.cmd` starts it with persistent player data in `../BorrowedSunPlayer/`, outside the packaged build folder. **O** from the menu opens saved mouse sensitivity, vertical-look inversion and audio volume. Earlier packages remain beside it for historical comparison. The newer Second Dawn assembly is source work and has not replaced the verified package.

The first Orison block implements the approved Brass Sanctuary direction: warm masonry, indigo awnings, copper roofs, market props and the Lantern Keeper silhouette. [Asset provenance and authoring instructions](Docs/Art/ART_DIRECTION.md) distinguish imported scans from original meshes. Night and rain are future Indigo Monsoon studies. Screenshot validation also captures four clean city review views after both mission routes.

Use `Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute` to reproduce a player package. `Tools/Demo.ps1 -Mode Validate -MeasureFrames -UserDirectory <isolated-root> -PackageExecutable <game-exe> -Execute` collects frame intervals at the required 60 FPS cap. Run screenshot validation separately. Frame intervals describe application pacing, not isolated CPU/GPU costs.

## Daily use

Pick one acceptance condition from [the backlog](Docs/Registers/backlog.json). Work on that outcome, run relevant checks, play the affected route, review the diff, then commit a coherent state. Keep one active gameplay item and one independent research/review item. Follow [AGENTS.md](AGENTS.md) for assisted development and [daily operations](Docs/Operations/DAILY.md) for integration.

The recommended free tools and assets are recorded in [the asset candidate register](Docs/Registers/asset-candidates.json). Candidate status does not mean installed, licensed to this account, or qualified for this engine. Do not import the entire list. Use [asset intake](Docs/Templates/asset-intake.json) for one selected candidate at a time.

The additional [framework research](Docs/Research/Framework_Research.md) assesses gameplay, editor/content, DevOps and AI repositories. Its [candidate register](Docs/Registers/repository-candidates.json) preserves exact revisions and qualifications; the [experiment queue](Docs/Registers/framework-spikes.json) defines bounded next trials. None of those third-party candidates is installed by this delivery. The native Unreal MCP plugin was inspected but remains disabled.

## Source control and portability

This delivered workspace is locally initialized with Git and repository-local LFS configuration when indicated by the validation record. No remote is created. The portable ZIP includes materialized game assets and the reviewed CC0 source files; it excludes `.git`, local environment settings, artifacts and engine binaries. After unpacking, run `git init -b main` then `git lfs install --local`; create your first reviewed commit. Fill local engine configuration from the example before execution.

For sustained production prefer a short path such as `C:\Dev\BorrowedSun` on a suitable disk. Clone/copy deliberately; do not run the same game project in two editors. Do not put the active source checkout inside consumer file sync. Keep off-device backups and practice restoring the actual LFS payloads, not just Git pointers.

`.github/manual-windows.yml.example` is intentionally inactive. Local automation is the baseline; remote runners and paid services require deliberate account setup and a trusted execution environment.

Poly Haven CC0 assets are redistributed with their provenance in the [material](Docs/Registers/city-material-intake.json) and [prop](Docs/Registers/city-prop-intake.json) intake registers. No paid kit or third-party framework is included. Licenses of future imports and dependencies remain separate. Keep the initial budget gross of tax, currency conversion and recurring costs.
