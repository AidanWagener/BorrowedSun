# Borrowed Sun workspace

A local-first Unreal project for a solo city-to-orbit adventure. The first demo, **First Light**, implements a compact walking/flight expedition with two persistent outcomes. See [demo scope](Docs/Operations/DEMO_SCOPE.md) for acceptance and limits; qualification results belong in the validation records. Initial tool/asset spending ceiling: under EUR 500; setup purchases: EUR 0.

## Start here

1. Read [the current demo validation](Docs/Operations/DEMO_VALIDATION.md) for the playable milestone and [the earlier foundation validation](Docs/Operations/VALIDATION.md) for setup history.
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
| Config | Reviewed engine/plugin pin |
| Tools | Build, test, doctor, preflight, backup and tooling regression commands |
| Docs/Registers | Backlog, decisions, asset candidates, budget and machine observation |
| Docs/Templates | Task, asset intake, dependency qualification, playtest and decision templates |
| Docs/Research | Development handbook, additional framework research and source registers |
| Artifacts | Ignored logs, test fixtures and build output |
| Vault | Optional ignored private license evidence; requires independent backup |

The full design remains in the accompanying Borrowed Sun game specification. This workspace contains the demo source and development workflow. The first demo uses one continuous map and original procedural prototype content; broader streaming, combat, NPCs and vendor-kit production remain later work. Remote CI is not enabled.

## Play and validate First Light

Compile first, then use `Tools/Demo.ps1 -Mode Launch -Execute` to launch the current source project. The script defaults to a dry run. `-Mode Validate -Screenshots -Execute` runs both mission outcomes with rendering and fresh isolated save slots, preserving JSON and screenshots. It exercises gameplay methods and movement; physical keyboard/controller feel still requires playtesting. See the script's help and [demo scope](Docs/Operations/DEMO_SCOPE.md).

The loop is: take the sunseed, board Kestrel, fly to the sky relay, choose clinic or evacuation power, return, disembark and install it. **E** interacts, **G** toggles optional flight assist, **R** recovers a safe position, **F5/F9** save/load, and **ESC** opens the menu. A new expedition requires an explicit menu confirmation.

## Daily use

Pick one acceptance condition from [the backlog](Docs/Registers/backlog.json). Work on that outcome, run relevant checks, play the affected route, review the diff, then commit a coherent state. Keep one active gameplay item and one independent research/review item. Follow [AGENTS.md](AGENTS.md) for assisted development and [daily operations](Docs/Operations/DAILY.md) for integration.

The recommended free tools and assets are recorded in [the asset candidate register](Docs/Registers/asset-candidates.json). Candidate status does not mean installed, licensed to this account, or qualified for this engine. Do not import the entire list. Use [asset intake](Docs/Templates/asset-intake.json) for one selected candidate at a time.

The additional [framework research](Docs/Research/Framework_Research.md) assesses gameplay, editor/content, DevOps and AI repositories. Its [candidate register](Docs/Registers/repository-candidates.json) preserves exact revisions and qualifications; the [experiment queue](Docs/Registers/framework-spikes.json) defines bounded next trials. None of those third-party candidates is installed by this delivery. The native Unreal MCP plugin was inspected but remains disabled.

## Source control and portability

This delivered workspace is locally initialized with Git and repository-local LFS configuration when indicated by the validation record. No remote is created. The portable ZIP excludes `.git`, local environment settings, artifacts, engine binaries and third-party downloads. After unpacking, run `git init -b main` then `git lfs install --local`; create your first reviewed commit. Fill local engine configuration from the example before execution.

For sustained production prefer a short path such as `C:\Dev\BorrowedSun` on a suitable disk. Clone/copy deliberately; do not run the same game project in two editors. Do not put the active source checkout inside consumer file sync. Keep off-device backups and practice restoring the actual LFS payloads, not just Git pointers.

`.github/manual-windows.yml.example` is intentionally inactive. Local automation is the baseline; remote runners and paid services require deliberate account setup and a trusted execution environment.

No third-party game assets are redistributed in this workspace. Licenses of future imports and dependencies remain separate. Keep the initial budget gross of tax, currency conversion and recurring costs.
