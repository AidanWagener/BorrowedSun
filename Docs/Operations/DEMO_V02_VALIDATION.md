# First Light v0.2.0 — Shipping and preferences

The current player build uses Win64 Shipping on Unreal 5.8.2 CL 56702186. The existing walking/flight expedition and both persistent outcomes remain intact. This iteration adds a preferences screen, independent player-data storage, a Shipping build option and reproducible frame-interval measurement. Spending remains EUR 0.

## Player-facing changes

Launch the adjacent `Play_Borrowed_Sun.cmd`. It starts `../BorrowedSunShipping/Windows/BorrowedSun.exe` with player data in `../BorrowedSunPlayer/Saved/`. Keep the complete package together and retain the player-data directory when updating builds. The previous Development package remains in `../BorrowedSunDemo/`.

From the welcome or pause menu, press **O** for preferences. **[ / ]** changes mouse sensitivity, **I** toggles vertical-look inversion, and **- / =** adjusts audio volume. Preferences save automatically, independently of mission checkpoints. Defaults are sensitivity 0.65, inversion off and volume 80%. Enter or Escape returns to the menu.

The Shipping welcome screen opened without the earlier network-permission prompt during interactive verification. A read-only socket observation of the running Shipping process found no TCP connections/listeners or UDP endpoints. No firewall settings were changed. This is a local observation, not a universal clean-machine certification.

## Qualification

| Check | Result | Local evidence |
|---|---|---|
| Editor and Shipping compile, cook, stage and archive | Passed | `Artifacts/build/20261012T175341Z-0324955c/` |
| Unreal automation | 12 expected tests passed, including invalid preference values and isolated disk configuration round-trip | `Artifacts/tests/20261012T175608Z-eeb0dbb5/` |
| Shipping gameplay with screenshots | 32 checks passed across Clinic and Evacuation; all six actual screenshots visually inspected | `Artifacts/demo/20261012T175702Z-6dad28d4/` |
| Separate Shipping measurement run | 33 checks passed, including screenshot exclusion; both complete routes and disk checkpoint loads | `Artifacts/demo/20261012T175828Z-6d5f89ff/` |
| Launcher/report regressions | 35 passed on PowerShell 5.1 and PowerShell 7 | `Artifacts/tooling-tests/demo-20261012T175453Z-70878381/` and `demo-20261012T175458Z-430598cb/` |
| Operations regressions | 27 passed on PowerShell 5.1 | `Artifacts/tooling-tests/20261012T174733Z-8ffe8f29/` |
| Native keyboard and preferences | O opened preferences; sensitivity changed to 0.75, inversion to on, volume to 70%; all returned after process exit and relaunch. Enter, Escape and Q worked. | Isolated task `work/qa-v02/preferences/`; original screenshot `../Demo_Previews/Preferences.jpg` |

Compact build, test, runtime and socket evidence is committed under `Evidence/v02-*`. Detailed logs and QA fixtures remain local. Tests use isolated player-data roots and namespaces. No ordinary player checkpoint was present in the previous package or default local save directory when the launcher was updated; no test progress was copied into normal play.

## Measured application pacing

Observed on the recorded Ryzen 7 7435HS / RTX 4070 Laptop / 15.7 GiB usable RAM machine at a **1280 x 720 viewport and 60 FPS cap**. Five seconds of startup were excluded. Screenshot capture was disabled for measurement. The sampled period covered 76.903 seconds of both mission routes.

| Samples | Count | Mean interval | 95th percentile | 99th percentile | Maximum |
|---|---:|---:|---:|---:|---:|
| Overall | 4,614 | 16.667 ms | 16.794 ms | 16.951 ms | 20.905 ms |
| On foot | 2,532 | 16.667 ms | 16.786 ms | 16.943 ms | 17.412 ms |
| Flight | 2,082 | 16.667 ms | 16.805 ms | 16.955 ms | 20.905 ms |

No sampled interval exceeded 33.333 ms. These are wall-clock intervals between rendered game ticks, including pacing and scheduling. They are **not isolated GPU time, CPU work time, input latency, startup measurements or a hardware minimum specification**. This is one local run with ordinary desktop background activity, not a controlled benchmark across machines or resolutions.

The first measurement exposed a missing cap: the new settings subclass required its own default INI section. The section and reset defaults were corrected, the package rebuilt, all Unreal tests rerun and both packaged routes requalified. The measurement validator now rejects a missing 60 FPS cap. The earlier uncapped run remains a diagnostic fixture, not the current performance qualification.

## Reproduction

```powershell
powershell -NoProfile -File Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute
powershell -NoProfile -File Tools/Test.ps1 -Execute -NullRHI
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -Screenshots -PackageExecutable <shipping-game-exe> -UserDirectory <fresh-visual-qa-root> -Execute
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -MeasureFrames -PackageExecutable <shipping-game-exe> -UserDirectory <fresh-frame-qa-root> -Execute
```

Use the real `BorrowedSun-Win64-Shipping.exe` under `BorrowedSun/Binaries/Win64` for automated qualification. `UserDirectory` maps both the runtime saved-data location and the expected report location; an inconsistent explicit report path is rejected. Do not run multiple editors/builds or qualification instances together. A fresh player-data root prevents older preferences from contaminating the intended test conditions.

## Limits and continuity

The demo remains one prototype map, with original geometric content and a sky relay rather than planetary/orbital streaming. This pass does not add missions, final art, combat, NPC simulation or multiplayer. Extended manual flight handling, physical gamepad use and an audio listening pass remain unverified. Volume persistence and application to the game's audio components do not establish subjective audio quality. Separate CPU/GPU profiling, cold-start/PSO analysis, clean-machine prerequisites, signing and distribution remain future work.

The qualified package was built from a reviewed working tree based on `40fd87333cda54c3db01215f229d9746107fa4df`; the build record correctly says dirty. The final delivery manifest records the committed source revision and actual package hashes. Runtime source and config were not changed after the final build. The original source-restoration evidence remains in the v0.1 record; v0.2 receives a fresh verified snapshot, but has not been rebuilt from that snapshot. Recovery copies remain on the same physical disk. No external account, paid asset, plugin framework or hosted service was added.
