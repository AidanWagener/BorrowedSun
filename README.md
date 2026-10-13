# Borrowed Sun workspace

A local-first Unreal project for a solo city-to-orbit adventure. The first demo, **First Light**, implements a compact walking/flight expedition with two persistent outcomes. See [demo scope](Docs/Operations/DEMO_SCOPE.md) for acceptance and limits; qualification results belong in the validation records. Initial tool/asset spending ceiling: under EUR 500; setup purchases: EUR 0.

## Start here

**Current priority, 13 October:** build Nacre from the [living planet bible](Docs/World/README.md), while continuing the starting area's [approved visual benchmark](Docs/Art/STARTING_MARKET_BENCHMARK.md). The user's latest instruction explicitly starts construction before the remaining lore volumes are complete. The first construction step places the existing playable Orison block into a canonical regional terrain frame; architecture, materials and lighting improve in parallel. Preserve the verified [city prop integration](Docs/Operations/CITY_OBJECTS_2026-10-13.md). Authored plans, imported content and tested runtime behavior remain separate claims.

The [Orison terrain and market checkpoint](Docs/Operations/ORISON_TERRAIN_AND_MARKET_2026-10-13.md) records the first native regional Landscape, revised architecture and daylight, actual gameplay captures and remaining visual gaps. It is not a completed planet or a claim that the reference image has been matched.

The new [worldbuilding and asset specification](Docs/Art/WORLD_ASSET_SPEC.md) defines Orison's construction language, 4,200 planned geometry briefs, physical-object contracts and staged destruction. Use the [kit shortlist](Docs/Art/WORLD_KIT_SHORTLIST.md), [complete family index](Docs/Art/WORLD_ASSET_FAMILIES.md) and [catalog register](Docs/Registers/world-asset-catalog.json) for production planning. The adjacent `Borrowed_Sun_Asset_Catalog.html` is a searchable offline review artifact. Planned entries are not completed models.

1. Read [the current workshop build validation](Docs/Operations/WORKSHOP_ART_PASS_2026-10-13.md) and [approved art direction](Docs/Art/ART_DIRECTION.md). Earlier [v0.4](Docs/Operations/DEMO_V04_VALIDATION.md), [v0.3](Docs/Operations/DEMO_V03_VALIDATION.md), [v0.2](Docs/Operations/DEMO_V02_VALIDATION.md), [first playable](Docs/Operations/DEMO_VALIDATION.md), and [foundation](Docs/Operations/VALIDATION.md) records remain historical evidence.
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
| SourceArt | Original Blender/FBX work, Tripo briefs and reviewed CC0 sources with intake records |
| Config | Reviewed engine/plugin pin |
| Tools | Build, test, doctor, preflight, backup and tooling regression commands |
| Docs/Registers | Backlog, decisions, asset candidates, budget and machine observation |
| Docs/Templates | Task, asset intake, dependency qualification, playtest and decision templates |
| Docs/Research | Development handbook, additional framework research and source registers |
| Artifacts | Ignored logs, test fixtures and build output |
| Vault | Optional ignored private license evidence; requires independent backup |

The [living planet bible](Docs/World/README.md) is the current authority for expanding the world; the original game specification remains a preserved historical source. This workspace also contains the demo source and development workflow. The first city block combines original modular geometry with reviewed CC0 textures and scanned market props in one continuous map. Broader streaming, combat, NPCs and paid vendor-kit production remain later work. Remote CI is not enabled.

## Play and validate First Light

Compile first, then use `Tools/Demo.ps1 -Mode Launch -Execute` to launch the current source project. The script defaults to a dry run. `-Mode Validate -Screenshots -Execute` runs both mission outcomes with rendering and fresh isolated save slots, preserving JSON and screenshots. It exercises gameplay methods and movement; physical keyboard/controller feel still requires playtesting. See the script's help and [demo scope](Docs/Operations/DEMO_SCOPE.md).

The loop is: take the sunseed, board Kestrel, fly to the sky relay, choose clinic or evacuation power, return, disembark and install it. **E** interacts, **G** toggles optional flight assist, **R** recovers a safe position, **F5/F9** save/load, and **ESC** opens the menu. A new expedition requires an explicit menu confirmation.

The current verified player build is the **Orison terrain and market Shipping checkpoint, 13 October 2026**, retaining the v0.4.0 gameplay version, in the adjacent `BorrowedSunOrison` folder. `Play_Borrowed_Sun.cmd` opens the main expedition with persistent data in `../BorrowedSunPlayer/`. `Preview_Orison_Market.cmd` uses separate `BorrowedSunOrisonPreviewPlayer` data. Both packaged mission routes, physical props, saves and regional collision passed. A separate 1080p pacing run stayed near 60 FPS for most samples, with one 127 ms flight hitch; see the [full evidence and remaining art work](Docs/Operations/ORISON_TERRAIN_AND_MARKET_2026-10-13.md). **O** from the menu opens saved mouse sensitivity, vertical-look inversion and audio volume. Earlier packages remain for comparison. Four [Tripo Studio candidates](Docs/Operations/TRIPO_STUDIO_INTAKE.md) still await local export and Unreal preparation; they are not in this package.

The earlier [workshop surface pass](Docs/Operations/WORKSHOP_SURFACES_2026-10-13.md) has a committed source checkpoint and a hash-verified Shipping candidate in `../BorrowedSunWorkshopSurfaces`. It corrects wood grain and adds authored contact polish and dirt. Both mission routes and checkpoints passed 40 offscreen rendered checks; four of 12 captured views have been visually reviewed. Normal-window presentation and a new frame-pacing run remain pending. [Preview Workshop Surfaces](../Preview_Workshop_Surfaces.cmd) opens this candidate with separate player data; the ordinary launcher now selects the newer Orison checkpoint above.

Source development now includes the [main-city prop integration](Docs/Operations/CITY_OBJECTS_2026-10-13.md), with 20 smoke tests and 57 rendered checks passing. Ten original props are available in Second Dawn and the separate object court, with pickup, rotation, throwing, persistent collection and actor reconstruction. F grabs/drops, left mouse throws, right mouse plus movement rotates, and Z collects. Inventory browsing/retrieval remains deferred. The rendered check covers actual workshop interaction and both mission routes; human input-feel review and combined Shipping delivery remain separate.

The first Orison block implements the approved Brass Sanctuary direction: warm masonry, indigo awnings, copper roofs, market props and the Lantern Keeper silhouette. [Asset provenance and authoring instructions](Docs/Art/ART_DIRECTION.md) distinguish imported scans from original meshes. Night and rain are future Indigo Monsoon studies. Screenshot validation also captures six art review views after both mission routes, including the workshop frontage and counter.

Use `Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute` to reproduce a player package. `Tools/Demo.ps1 -Mode Validate -MeasureFrames -UserDirectory <isolated-root> -PackageExecutable <game-exe> -Execute` collects frame intervals at the required 60 FPS cap. Run screenshot validation separately. Frame intervals describe application pacing, not isolated CPU/GPU costs.

## Daily use

Pick one acceptance condition from [the backlog](Docs/Registers/backlog.json). Work on that outcome, run relevant checks, play the affected route, review the diff, then commit a coherent state. Keep one active gameplay item and one independent research/review item. Follow [AGENTS.md](AGENTS.md) for assisted development and [daily operations](Docs/Operations/DAILY.md) for integration.

The recommended free tools and assets are recorded in [the asset candidate register](Docs/Registers/asset-candidates.json). Candidate status does not mean installed, licensed to this account, or qualified for this engine. Do not import the entire list. Use [asset intake](Docs/Templates/asset-intake.json) for one selected candidate at a time.

The additional [framework research](Docs/Research/Framework_Research.md) assesses gameplay, editor/content, DevOps and AI repositories. Its [candidate register](Docs/Registers/repository-candidates.json) preserves exact revisions and qualifications; the [experiment queue](Docs/Registers/framework-spikes.json) defines bounded next trials. None of those third-party candidates is installed by this delivery. The native Unreal MCP plugin was inspected but remains disabled.

## Source control and portability

The GitHub repository is [AidanWagener/BorrowedSun](https://github.com/AidanWagener/BorrowedSun). Authored Unreal assets, Blender/FBX source and source images use Git LFS. Clone with Git LFS installed and run `git lfs pull` before building. Fill ignored local engine configuration from the example before execution. Engine binaries, generated caches, local player saves and credentials are excluded. Historical portable ZIPs are separate snapshots and do not track subsequent repository changes.

For sustained production prefer a short path such as `C:\Dev\BorrowedSun` on a suitable disk. Clone/copy deliberately; do not run the same game project in two editors. Do not put the active source checkout inside consumer file sync. Keep off-device backups and practice restoring the actual LFS payloads, not just Git pointers.

`.github/manual-windows.yml.example` is intentionally inactive. Local automation is the baseline; remote runners and paid services require deliberate account setup and a trusted execution environment.

Poly Haven CC0 assets are redistributed with their provenance in the [material](Docs/Registers/city-material-intake.json) and [prop](Docs/Registers/city-prop-intake.json) intake registers. No paid kit or third-party framework is included. Licenses of future imports and dependencies remain separate. Keep the initial budget gross of tax, currency conversion and recurring costs.
