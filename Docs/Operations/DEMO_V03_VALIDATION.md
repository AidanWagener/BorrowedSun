# First Light v0.3.0 — first Brass Sanctuary city block

The user-approved direction is **Brass Sanctuary**, with **Indigo Monsoon** reserved for future rain and night lighting. The first Orison Lower Quay art pass is packaged in Win64 Shipping on Unreal 5.8.2 CL 56702186. Both existing mission outcomes still complete and survive disk save/load. This is an early modular art pass, not final concept-art fidelity or a claim of AAA quality. Asset spending remains **EUR 0**.

## What changed

The 84 by 64 metre playable quay now has twelve building bays, arcades, columns, lattice screens, roof pavilions, copper domes, five market shelters, overhead shade cloth, brass cookware and lanterns. Repeated parts use instanced meshes. The original Lantern Keeper has a pleated robe, angular mask, articulated silhouette and radial solar halo; it remains a sculptural study. Walking, docking, service anchors and saved mission IDs retain their established coordinates and behavior.

Four Poly Haven CC0 material sets supply 2K maps. Two scanned props supply 1K maps and FBX geometry. In total, the city folder contains **14 meshes (12 original, two scans), seven materials and 21 textures**, saved as 42 real Unreal asset packages. The 23 raw source files retain provider metadata and SHA-256 hashes. See [art direction and credits](../Art/ART_DIRECTION.md), [material intake](../Registers/city-material-intake.json) and [prop intake](../Registers/city-prop-intake.json). No KitBash3D kit was purchased or imported.

Daylight uses Lumen, virtual shadow maps, bounded exposure and a shadowless cool directional fill for readable shaded spaces. The first launch upgrades older illumination/reflection quality settings once, retaining input, audio, display and mission preferences. Subsequent launches retain later quality choices.

## Qualification

| Check | Result | Local run |
|---|---|---|
| Asset intake and authoring | Raw hashes verified; 42 packages saved through Unreal APIs | `Artifacts/city-art/20261012T191046Z-091f255f` |
| Editor and Shipping compile, cook, stage and archive | Passed | `Artifacts/build/20261012T192727Z-66efb125` |
| Unreal automation | 13 expected tests passed, including graphics migration and preference preservation | `Artifacts/tests/20261012T192359Z-868194de` |
| Packaged rendered routes | 34 checks passed across Clinic and Evacuation | `Artifacts/demo/20261012T193130Z-2fb61824` |
| Separate frame measurement | 34 checks passed, without screenshots | `Artifacts/demo/20261012T193336Z-adf25801` |
| Launcher/report regressions | 35 passed on Windows PowerShell 5.1 | `Artifacts/tooling-tests/demo-20261012T192835Z-3e710c1c` |
| Preflight and diff whitespace | Passed | Run against current source |

All **ten actual packaged screenshots** were visually inspected: six gameplay checkpoints and four unobstructed art views. The launcher gate reports six required gameplay screenshots; the four extra art captures are reviewed separately. Textures, imported props, skyline geometry, route prompts and both completion screens appeared. Deep canopy undersides, simple silhouette buildings, flat water and sparse street dressing remain visible quality limits. FBX intake reported missing smoothing-group information; both props imported and rendered, but close-up hero use needs further mesh review.

Compact evidence is committed under `Evidence/v03-*`. Detailed logs and isolated QA data remain local. The tests exercise gameplay methods and actual movement, but do not establish extended manual flight feel, physical gamepad behavior or subjective audio quality.

## Measured application pacing

Measured on the Ryzen 7 7435HS / RTX 4070 Laptop / 15.7 GiB usable RAM machine at **1280 x 720, 60 FPS cap**. The first five seconds were excluded. Both complete routes were sampled for 77.120 seconds without screenshot capture.

| Mode | Intervals | Mean | 95th percentile | 99th percentile | Maximum |
|---|---:|---:|---:|---:|---:|
| Overall | 4,627 | 16.667 ms | 16.786 ms | 16.917 ms | 22.905 ms |
| On foot | 2,545 | 16.667 ms | 16.774 ms | 16.882 ms | 17.131 ms |
| Flight | 2,082 | 16.667 ms | 16.798 ms | 16.985 ms | 22.905 ms |

No sampled interval exceeded 33.333 ms. These are wall-clock intervals between rendered game ticks, including pacing and scheduling. They are not isolated GPU/CPU costs, cold-start measurements, input latency or certification for another machine or resolution.

## Delivery and reproduction

The adjacent `Play_Borrowed_Sun.cmd` opens `../BorrowedSunCity/Windows/BorrowedSun.exe` and retains `../BorrowedSunPlayer/Saved` for player data. v0.1 Development and v0.2 Shipping remain preserved. Preview images under the adjacent `City_Previews` folder are unmodified Shipping captures.

```powershell
powershell -NoProfile -File Tools/Build.ps1 -Execute
powershell -NoProfile -File Tools/Author-CityArt.ps1 -Force -Execute
powershell -NoProfile -File Tools/Demo.ps1 -Mode AuthorMap -Force -Execute
powershell -NoProfile -File Tools/Test.ps1 -Execute -NullRHI
powershell -NoProfile -File Tools/Build.ps1 -Mode Package -Configuration Shipping -Execute
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -Screenshots -PackageExecutable <shipping-game-exe> -UserDirectory <fresh-visual-root> -Execute
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -MeasureFrames -PackageExecutable <shipping-game-exe> -UserDirectory <fresh-frame-root> -Execute
```

Use the actual `BorrowedSun-Win64-Shipping.exe` under `BorrowedSun/Binaries/Win64` for automated runs. Keep Unreal executions serial. Existing LFS-lockable map, material or mesh packages must be writable for deliberate regeneration; see `Tools/README.md`. The source ZIP includes the reviewed CC0 source files, so refetching them is optional.

The qualified package was built from a working tree based on `a9aaafb7b4323b659dd9c82e62a25037a3f7a591`; the build report correctly records dirty source. The delivery manifest records the final committed revision and verified package/source hashes. Runtime code, config and assets remained unchanged after qualification. The new local recovery snapshot is hash verified; it is not a fresh restored build and is on the same physical disk.

## Next quality gate

Finish one representative facade and market arrangement before multiplying the kit. Review proportions, silhouette, construction logic, join seams, material scale, weathering and street composition at walking and flight distances. The current block establishes a usable layout and repeatable asset pipeline; it does not demonstrate equivalence to a AAA environment team.

Night/rain, interiors, inhabitants, sculpted hero detail, finished spacecraft, planetary streaming and multiplayer are not implemented in this pass. Separate GPU/CPU profiling and startup analysis remain outstanding.
