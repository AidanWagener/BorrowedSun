# Delivery validation - 12 October 2026

Historical foundation record. The later authored map, packaged playable demo, persistence and rendered checks are recorded in [DEMO_VALIDATION.md](DEMO_VALIDATION.md). Statements below describe the earlier foundation milestone only.

This record describes the delivered foundation, not the full game. No third-party assets were acquired, no paid service was enabled and no remote repository was created.

## Completed

- Launcher Unreal 5.8.2 / changelist 56702186 identified locally and pinned. Microsoft C++/SDK selections recorded in [engine-validation.json](engine-validation.json).
- Runtime and editor C++ modules compiled with the real installed engine. Initial Editor build took 185.58 seconds; initial Development Game build took 69.56 seconds. These are single local observations, not performance guarantees.
- Five real Unreal automation tests passed: canonical/distinct identity, invalid identity rejection, rigid transform motion conservation, transform round-trip, and failure without output mutation for scaled anchors.
- Final PowerShell `Tools/Build.ps1 -Execute` compiled Editor and Game successfully after AndroidFileServer was explicitly disabled. Evidence is in ignored `Artifacts/build/20261012T114024Z-9348e396/`.
- Final `Tools/Test.ps1 -Execute -NullRHI` passed all five expected tests after that change; its report validator enforced test presence and success. Evidence is in ignored `Artifacts/tests/20261012T114048Z-942bdb04/`.
- Doctor and preflight passed against the installed engine. Source control was initialized locally and Git LFS installed for this repository only; no global Git identity was changed.
- Twenty-seven operational regression checks passed under Windows PowerShell 5.1 and PowerShell 7.6.5. These used mock engine commands and real temporary Git fixtures. They cover native command quoting, error propagation, bounded timeout and log preservation, missing/failed/zero test reports, expected test membership, map/default-map validation, and backup manifest integrity/path containment.
- The source scaffold deliberately contains no fabricated Unreal binary assets. Packaging requires an existing project-owned map and a matching startup-map configuration.

## Not yet established

- No map has been authored, cooked or packaged for the game. No staged game executable has been launched. The first remaining editor milestone is [FIRST_MAP.md](FIRST_MAP.md).
- The foundation tests do not implement or validate inventory transactions, saves, actual streaming, input, camera, flight handling, NPCs, UI, GPU rendering or performance. Those are backlog tasks.
- Candidate asset rights have been researched but acquisition, local entitlement, import and engine qualification are pending.
- The mock backup tests are not an external-device restore drill of this game. A first independent backup and a clean restoration remain BS-002.
- A local repository is not an off-device backup or a private GitHub service. No remote, runner, cloud budget, access control or collaboration lock service is configured.

## Reproduce

Use the pinned engine and local environment configuration. Run Doctor, Preflight, Build with `-Execute`, then Test with `-Execute -NullRHI`. Run `Tools/Tests/Run-Tests.ps1` for the standalone operations suite. Follow the first-map runbook before attempting a real package. Do not count a dry run or a mock UAT success as a real cook.

Logs and engine binaries are excluded from the portable delivery archive. Compact evidence stays in the repository; detailed local artifacts remain available in this working copy. New changes require their relevant checks again.
