# Local operations

Windows PowerShell 5.1 and PowerShell 7 are supported. Run from the workspace root. The engine and compiler are installed by their official installers, outside this repository. None of these scripts install tools, create a remote, fetch assets, publish a build or delete a directory.

1. Copy `Tools/Environment.example.json` to ignored `Tools/Environment.local.json`. Set `EngineRoot` to the installed engine directory, for example `C:\Program Files\Epic Games\UE_5.8`. This is local configuration, never credentials. Engine patch compatibility is recorded by the project documentation; this script does not silently upgrade it.
2. Run `powershell -NoProfile -File Tools/Doctor.ps1`, then `powershell -NoProfile -File Tools/Preflight.ps1`.
3. Preview `powershell -NoProfile -File Tools/Build.ps1`. Execute with `-Execute`. Both editor and game targets compile. Use `-UE_ROOT <path>` as a one-run override. Close the editor first. Configure `MaxParallelActions` to suit memory; two is a conservative starting point on this machine.
4. Preview or execute `Tools/Test.ps1` in the same way. A nonzero process exit, missing JSON, zero tests, insufficient test count, missing expected tests or any nonsuccess state fails the run. Set `ExpectedTests` to the exact approved smoke suite as it grows. `-NullRHI` is only for tests known not to need rendering; it is not visual or performance QA.
5. First Light already has a real authored map, `MapsToCook` and matching `GameDefaultMap`. Run `Tools/Preflight.ps1 -ForPackage` and `Tools/Build.ps1 -Mode Package -Execute`. For future maps, save them through Unreal and update these settings deliberately. Packaging is blocked until the saved map and default are valid. Success additionally requires the expected project executable. A package must still be launched and played separately.

Dry runs of Build and Test validate the project and show the plan, but do not write artifacts or run the engine. Successful real runs create fresh directories beneath `Artifacts/`. The scripts throw on failure; `powershell -File` or `pwsh -File` returns a nonzero exit. Logs can contain local paths and should remain private. Compact JSON summaries exclude environment dumps and remote URLs. Do not use shell metacharacters such as `&` or `%` in engine or workspace paths: the batch runner rejects them.

## Backup and restore

The First Light delivery has a verified local snapshot, with current LFS payloads. See [demo validation](../Docs/Operations/DEMO_VALIDATION.md) for the observed restoration result and remaining off-device requirement.

After a reviewed first Git commit, stop editing and choose an existing backup directory outside this workspace, preferably another physical device. Preview `Tools/Backup.ps1 -BackupRoot <directory>`; add `-Execute` for a fresh snapshot. It copies tracked and nonignored untracked working files, creates and verifies a full Git bundle, and copies locally available LFS objects. Ignored local settings, secrets, caches and build artifacts are omitted. Keep purchased original downloads/receipts separately under the same rights and backup policy. No historical LFS completeness is claimed: intentionally fetch all retained historical objects before a history-complete backup only after checking bandwidth and space.

Run `Tools/VerifyBackup.ps1 -SnapshotPath <snapshot>`. It checks every manifest file and rejects escaping paths and reparse points. It does not authenticate malicious changes. Incomplete runs have no valid completed manifest and remain for inspection. There is no pruning or automatic deletion.

A monthly restore drill is a separate requirement: create a new empty directory on a spare disk, temporarily set `GIT_LFS_SKIP_SMUDGE=1` for a local `git clone <snapshot>/repository.bundle <new-directory>`, restore `LfsObjects/` into the new clone's `.git/lfs/objects/`, then materialize the current working snapshot from `WorkingTree/` and check its hashes. Restore local environment configuration manually. Note that the working snapshot is authoritative for uncommitted additions/edits/deletions; a naive overlay on a clone can resurrect deleted files, so compare the manifest and remove only individually reviewed extra files. Run preflight, editor/game compile, tests and a playable package. Disable network while checking that the current snapshot works. Historical commits require their own LFS completeness check. Record date, snapshot ID, restore duration and failures. Do not call a checksum pass a restore drill.

## Tests and CI

`Tools/Demo.ps1 -Mode Launch -Execute` starts the compiled demo without imposing a session timeout. `-Mode Validate -Screenshots -Execute` runs both mission routes with rendering, isolated QA saves and strict report validation. For an existing package, supply `-PackageExecutable` and its actual `-ReportDirectory`; see `Get-Help ./Tools/Demo.ps1 -Full`. `-Mode AuthorMap -Execute` runs the editor commandlet; replacing existing owned map/material files also requires `-Force`. These operations never build automatically. Keep asset authoring and builds serial.

`Tools/Tests/Run-DemoTests.ps1` checks the demo launcher and report validator without Unreal. Its fixtures do not replace the actual rendered acceptance run or a human playthrough.

`powershell -NoProfile -File Tools/Tests/Run-Tests.ps1` runs isolated tooling regression tests without Unreal, network access or Pester. Fixtures are kept under `Artifacts/tooling-tests/` for inspection. They cover batch quoting, failure/timeout enforcement, report gates and backup corruption/path rejection.

`.github/manual-windows.yml.example` is disabled. Start with local scripts. A future owner-reviewed private self-hosted runner may use the template only after its trust boundaries, dedicated account, repository access and manual enable variable are configured. Never execute untrusted PRs on a personal workstation. Do not connect repository events to an unattended agent with credentials. No paid service is needed for this workflow.
