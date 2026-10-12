# First Light playable milestone — 12 October 2026

Historical v0.1 Development milestone. The current Shipping build, persistent preferences and measured frame intervals are recorded in [DEMO_V02_VALIDATION.md](DEMO_V02_VALIDATION.md).

First Light is a Windows Development demo built with installed Unreal 5.8.2, changelist 56702186. It uses an original, stylized prototype city and ship in one continuous map. Spending remains EUR 0. This record qualifies the first playable loop, not the complete game specification.

## Delivered behavior

Walk through Lower Quay, acquire the Sunseed, board Kestrel, fly to the sky relay, authorize clinic or evacuation power, return, disembark and install the seed. The chosen service lights up and the other remains unavailable. Cargo custody and the exclusive choice survive save/load. Optional flight assist uses the ship movement component. The menu supports continuation, checkpoint restoration, a confirmed fresh expedition and quitting. Recovery restores a safe checkpoint pose.

The package is delivered beside the workspace at `../BorrowedSunDemo/Windows/BorrowedSun.exe`; `../Play_Borrowed_Sun.cmd` launches it. See the accompanying `../Borrowed_Sun_Demo_Review.md` for controls and the review route.

## Actual evidence

| Qualification | Observed result | Local evidence relative to workspace |
|---|---|---|
| Editor and Game C++ compile | Passed on the pinned engine | `Artifacts/build/20261012T125944Z-a83e53e2/` |
| Real map/material authoring | Unreal commandlet saved both assets; zero errors/warnings | `Artifacts/demo-map-qualified.log` |
| Cook, stage, package and archive | Passed; Win64 Development; 48 original package files, 944,510,508 bytes | Same build directory, `package.log` and `Package/Windows/` |
| Unreal automation | All 10 expected tests passed | `Artifacts/tests/20261012T125552Z-a9c73b2a/` |
| Editor rendered loop | 32 checks passed across both outcomes, 82.298 seconds | `Artifacts/demo/20261012T125643Z-e2552571/` |
| Packaged rendered loop | 32 checks passed across both outcomes, 82.188 seconds | `Artifacts/demo/20261012T130311Z-375e0164/` |
| Packaged visual inspection | All six actual 1280 x 720 screenshots inspected; readable mission/choice/completion UI, city colors and relay geometry | `../BorrowedSunDemo/Windows/BorrowedSun/Saved/DemoValidation/` |
| Operational regressions | 27 checks passed on Windows PowerShell 5.1; foundation suite also passed PowerShell 7 | `Artifacts/tooling-tests/20261012T125437Z-93012521/` and foundation validation |
| Demo launcher/report regressions | 22 checks passed on Windows PowerShell 5.1 and PowerShell 7 | `Tools/Tests/Run-DemoTests.ps1`; retained fixtures under `Artifacts/tooling-tests/` |
| Corrupt-primary recovery | Packaged game loaded backup as Returning / Evacuation / revision 4 / custody true | Compact evidence below; full log in task `work/recovery/packaged-save-recovery.log` |

The rendered harness requires a fresh isolated QA save namespace, actual render capability, both complete routes, real movement, normal interaction methods, duplicate-interaction rejection and checkpoint reads from disk. Missing, stale, incomplete or failed reports fail the launcher. Requested screenshots must exist and have PNG signatures; their visual quality was inspected separately. It does not simulate physical keyboard/gamepad input or establish performance targets. Runtime duration is an automation observation, not an expected human play time.

The backup fault test used namespace `QA_Recovery_20261012T1310`, a copied known-valid QA backup and a zero-byte primary. Ordinary player slots and original test fixtures were untouched. After BeginPlay, the packaged Development console reported:

```text
Progress = (SchemaVersion=1,Stage=Returning,Choice=Evacuation,Revision=4,bSunseedOwned=True)
LastError = Recovered the previous checkpoint: Checkpoint could not be read or has the wrong save class.
```

Manual native input previously verified Enter continuation, Escape menu and Q exit in the editor game. The package ran its routes through a real rendered viewport. Listening, extended manual flight handling and a physical gamepad pass remain unverified.

## Recovery and provenance

The current playable source at commit `4e7accbeed2461d7b763416bcbb667187493af96` was backed up in snapshot `BorrowedSun-20261012T131114Z-b08ad644` with a verified Git bundle, 89 working-tree files and both LFS payloads. It was restored into task `work/recovery/demo-restore`, with every working-file hash checked against the manifest. A fresh Editor and Game build passed in `Artifacts/build/20261012T131223Z-4e3b1139/` inside that restored checkout; all ten Unreal tests passed in its `Artifacts/tests/20261012T131829Z-c9f2aef6/`. No restored package or separate rendered route was run there; those were qualified in the delivery checkout above.

The restore exposed a Git index/line-ending normalization issue after copying the exact snapshot bytes over a new clone. `git add --renormalize .` refreshed that disposable clone's index; `git write-tree` exactly matched `HEAD^{tree}`, and clean preflight passed before the build. The LFS copy encountered already-created directory names, so the restored payloads and working-file hashes were verified explicitly. No source content was changed to obtain a pass. The fresh Game build slowed under memory pressure, then completed without intervention.

All available recovery copies are on the same physical disk; independent-device loss protection is not established. No private remote or hosted runner has been created. The pre-demo foundation also has its earlier independently rebuilt local bundle. The portable ZIP includes materialized asset bytes, but intentionally excludes Git history, ignored configuration and generated build products.

The first package was built from the reviewed working tree based on foundation commit `6bf65c61fb68e7688d2f05830d21f94c270befa3`; its build report honestly records a dirty source tree. Final source is committed after qualification. The delivery manifest records the actual packaged file hashes and the final source revision; documentation-only commits do not imply a rebuilt binary.

## Scope limits and next review

- This is a prototype city-to-sky expedition. The relay is about 150 metres above the quay. There is no planet, orbital mechanics, interplanetary streaming or separate sector travel.
- Consequences are lights, signs, subtitle dialogue and persistent state. There are no simulated patients, evacuating crowds, combat, crew, economy or multiplayer.
- KitBash assets and candidate frameworks remain researched candidates. No paid or third-party game content was imported, and no external AI plugin was enabled.
- The visuals are original procedural blockout art. Further art direction, animation, controller feel, accessibility/rebinding, audio listening and profiling require deliberate review.
- This is a Development package on this machine, not a Shipping or clean-machine certification. Prerequisite installation, distribution signing and commercial release qualification remain future work.
- Windows displayed a network permission prompt during an interactive packaged launch. It was left untouched. Installed engine source identifies the Development Trace control listener on TCP port 1985; no supported runtime switch was found to disable it. The game has no multiplayer or network dependency. A future Shipping build must be separately qualified; no Windows firewall settings were changed.
- Checkpoints restore mission stage and safe poses, not arbitrary exact-world saves. Save-version migration, power-loss durability and every possible file-system failure are not established by these checks.

Review the human experience next: objective clarity, walking and flight feel, the sense of departure/return and whether the two promises create a meaningful choice. Only then choose the next asset intake or framework experiment.

## Reproduce

```powershell
powershell -NoProfile -File Tools/Preflight.ps1 -ForPackage -RequireClean
powershell -NoProfile -File Tools/Build.ps1 -Mode Package -Execute
powershell -NoProfile -File Tools/Test.ps1 -Execute -NullRHI
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -Screenshots -Execute
powershell -NoProfile -File Tools/Tests/Run-DemoTests.ps1
```

For the delivered package, use `Tools/Demo.ps1 -Mode Validate -Screenshots -PackageExecutable '../BorrowedSunDemo/Windows/BorrowedSun/Binaries/Win64/BorrowedSun.exe' -ReportDirectory '../BorrowedSunDemo/Windows/BorrowedSun/Saved/DemoValidation' -Execute`. Close other instances first. Package validation requires the installed engine lock through the workspace tools; ordinary play via the launcher does not require an editor session.
