# Second Dawn joinery and surface pass

13 October 2026

This pass replaces the workshop's crosswise, repeating plank texture with grain aligned to individual construction pieces. Authored contact polish and dirt follow the bench work zones, drawer handles, shelf lips, joints and feet. The observable acceptance condition is a convincing improvement in the same actual Unreal front and bench views, with both packaged mission routes and saved checkpoints still working. The larger [starting-market benchmark](../Art/STARTING_MARKET_BENCHMARK.md) remains unmet.

Source and imported content are committed at `ec513de3c595633abe6c3bd767846268b900fd16`. A Shipping package built from that clean checkpoint and its delivery copy passed integrity checks; final packaged review is pending. The existing verified player launcher remains on the earlier workshop package until the new package is qualified.

## Construction and materials

The [Blender generator](../../Tools/Art/Build-LanternWorkshop.py) retains all 12 mesh names, material-slot order, dimensions and the same 39-part assembly recipe. Each timber component has a full UV0 chart at 60 cm per tile, with V following its longitudinal faces. Stable offsets prevent adjacent boards from sharing identical grain. Transverse end faces use separate projections; this is not a separately scanned end-grain material.

The [surface intake](../Registers/workshop-surface-intake.json) records Poly Haven's `fine_grained_wood` by Rob Tuytel, CC0, with original 2K colour, OpenGL normal and roughness maps. The importer flips the normal green channel once and uses tangent-space normal sampling. Original files, publisher sizes/MD5 and local SHA-256 hashes are retained. No asset purchase was made.

Vertex colour stores unsigned angular variation in R, authored contact polish in G and localized dirt in B. It is exported as linear numeric data. The masks are not ambient occlusion or signed curvature. Extra geometry is confined to components needing spatial interpolation; it prevents dirt at a shelf joint from spreading across a whole cabinet panel.

| Mesh | Previous triangles | Current triangles |
|---|---:|---:|
| Repair bench | 12,728 | 32,528 |
| Stock cabinet | 12,528 | 33,512 |
| Tool tray | 2,644 | 5,644 |

The nine other mesh triangle counts are unchanged. The assembly remains static decoration; this pass does not implement collecting, carrying, throwing or destruction.

## Executed evidence

| Check | Observed result | Evidence |
|---|---|---|
| Editor and Game Development compile | Passed on Unreal 5.8.2 CL 56702186 | [Build](../../Artifacts/build/20261013T054928Z-336c0e24/build-result.json) |
| Final FBX round-trip | All 12 meshes have finite UV0, zero degenerate geometry/UV triangles and zero invalid tangent bases; bounds and material slots match the preceding version | [Report](../../Artifacts/workshop-wear-review/20261013T075116/roundtrip.json) |
| Final Unreal import | 119 source hashes verified; 23 packages saved; no warnings or errors in the import log | [Result](../../Artifacts/market-art/20261013T060816Z-18e3246b/market-art-result.json), [log](../../Artifacts/market-art/20261013T060816Z-18e3246b/engine.log) |
| Intermediate rendered routes | 40 checks passed at 1920 x 1080; front and bench images reviewed for grain, tone and scale | [Run](../../Artifacts/demo/20261013T060140Z-35b80ca7/demo-result.json) |
| Win64 Shipping build | Passed from the clean source checkpoint; no warnings or errors in the package log | [Build](../../Artifacts/build/20261013T060953Z-2d99bdec/build-result.json), [log](../../Artifacts/build/20261013T060953Z-2d99bdec/package.log) |
| Candidate delivery copy | All 30 files and their sizes/SHA-256 match the source package; 1,002,357,471 bytes total | [Manifest](../../../BorrowedSunWorkshopSurfaces/delivery-manifest.json) |

The intermediate render predates the final cabinet-mask density correction and does not qualify the final assets. The final round-trip report identifies source-manifest SHA-256 `6cf1f2ec57027eabff6d899fc5a86d51119a12668ce86ffa4e7a8fd0f79420e9`. The assembly recipe is byte-identical to its baseline. Existing open/nonmanifold decorative detail remains; this report does not establish that every mesh is suitable for physics or fracture.

The earlier 14-test Unreal smoke run is historical evidence recorded in the [previous workshop pass](WORKSHOP_ART_PASS_2026-10-13.md). Runtime gameplay code is unchanged here. Source inspection, import and rendered/package checks are the relevant new verification; no new 14-test run is claimed.

## Outstanding qualification

The candidate is in the sibling `BorrowedSunWorkshopSurfaces/Windows` folder. Its manifest status is `CandidateAwaitingPackagedValidation`; copy integrity is not gameplay qualification. The player was still running the previous workshop build at the last observation. The next action, after that session closes, is to run final screenshot validation on this candidate and inspect the front, bench and market-arcade views, then run frame measurement separately:

```powershell
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -Screenshots -Width 1920 -Height 1080 -PackageExecutable '..\BorrowedSunWorkshopSurfaces\Windows\BorrowedSun.exe' -UserDirectory '..\..\work\qa-workshop-surfaces-final-01' -Execute
powershell -NoProfile -File Tools/Demo.ps1 -Mode Validate -MeasureFrames -Width 1920 -Height 1080 -PackageExecutable '..\BorrowedSunWorkshopSurfaces\Windows\BorrowedSun.exe' -UserDirectory '..\..\work\qa-workshop-surfaces-frames-01' -Execute
```

Only after those pass should the delivery manifest and launchers be promoted. Preserve `BorrowedSunPlayer`, the existing preview data and previous packages. Human playtesting and visual acceptance against the approved benchmark remain outstanding even when these checks pass.
