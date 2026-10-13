# Second Dawn workshop art pass

13 October 2026

The workshop has passed source-mesh checks, Unreal import, a Win64 Shipping build and packaged rendered route validation on Unreal 5.8.2. A separate packaged run records application frame pacing. The reviewed frontage has legible signage and three readable hanging lamps. These results qualify the current art iteration, automated routes and sampled pacing; a new human playtest, full release qualification and acceptance against the [approved visual benchmark](../Art/STARTING_MARKET_BENCHMARK.md) remain outstanding.

## Current source and presentation

The [source manifest](../../SourceArt/LanternWorkshop/workshop-manifest.json) records 12 original meshes and seven materials. The [Second Dawn recipe](../../SourceArt/LanternWorkshop/SecondDawn.assembly.json) contains 39 parts. Unreal authored 20 packages: those meshes and materials plus the assembly data asset. Editable Blender source and FBX exports remain in `SourceArt/LanternWorkshop`.

The pass adds supported hanging lamps, rack brackets, a projecting trade sign and revised counter materials. The review harness retains its four existing camera transforms and adds workshop-front and workshop-bench views. The assembly remains decorative and static: carrying, throwing, persistent object state and destruction are not implemented by this pass.

## Executed evidence

| Check | Observed result | Report |
|---|---|---|
| Editor and Game Development compile | Passed on the pinned engine | [Build](../../Artifacts/build/20261012T231332Z-a79a2f1e/build-result.json) |
| Unreal smoke automation | All 14 expected tests passed with NullRHI | [Tests](../../Artifacts/tests/20261012T231718Z-99383ddc/test-result.json) |
| Final workshop import | 20 packages saved; no degenerate-tangent or nearly-zero-binormal warnings | [Import](../../Artifacts/market-art/20261012T233011Z-20a7a103/market-art-result.json) and [log](../../Artifacts/market-art/20261012T233011Z-20a7a103/engine.log) |
| Unpackaged rendered routes | 40 checks passed at 1920 x 1080; 12 PNGs including six required route captures; both mission outcomes and disk checkpoint reloads | [Run](../../Artifacts/demo/20261012T233201Z-039a6da2/demo-result.json) and [runtime checks](../../Artifacts/demo/20261012T233201Z-039a6da2/runtime-result.json) |
| Final FBX round-trip | All 12 hashes match the source manifest; one active UV0 per mesh; zero degenerate geometry/UV triangles or invalid tangent bases | [Geometry and tangent report](../../Artifacts/workshop-review/fbx-geometry-tangents.json) |
| Win64 Shipping package | Build and package passed for `L_BorrowedSunDemo` | [Shipping build](../../Artifacts/build/20261012T234007Z-0a359faa/build-result.json) |
| Packaged rendered routes | All 40 checks passed at 1920 x 1080; 12 PNGs including six required route captures; both mission outcomes and disk checkpoint reloads | [Shipping run](../../Artifacts/demo/20261013T051425Z-6e05acce/demo-result.json) and [runtime checks](../../Artifacts/demo/20261013T051425Z-6e05acce/runtime-result.json) |
| Packaged pacing run | All 38 checks passed; 4,552 application frame intervals recorded without screenshots | [Measured run](../../Artifacts/demo/20261013T051813Z-ef0ed8e7/demo-result.json) and [interval results](../../Artifacts/demo/20261013T051813Z-ef0ed8e7/runtime-result.json) |

The 14-test run preceded the final mesh regeneration. The final import, rendered routes, Shipping package and FBX inspection cover the regenerated exports. Each screenshot run produced six route-milestone captures and six art-review views; the summary reports' `ScreenshotCount: 6` counts only the required route captures. The separate measured run excluded screenshots so image readback would not distort its interval sample.

The source fix clears inherited UV layers before generating UV0, removes zero-area triangles with a stable cross-product check, and reduces the trade-sign bevel to avoid collapsed faces. Read-only Blender 4.5.14 inspection found no near-zero tangents or bitangents, or nonorthogonal tangents. Both desk-lamp and petal-lamp meshes have zero boundary and nonmanifold edges. Other decorative meshes retain recorded open/nonmanifold detail; this report does not qualify every mesh for physics or fracture.

## Application frame pacing

The separate Shipping run sampled 75.871 seconds after a five-second warmup, at 1920 x 1080 with a 60 FPS cap. These are wall-clock intervals between rendered game ticks, including scheduling and frame pacing. They do not isolate CPU or GPU cost or establish cold-start, input-latency or sustained-play performance.

| Mode | Samples | p99 ms | Maximum ms |
|---|---:|---:|---:|
| Overall | 4,552 | 16.987 | 22.491 |
| On foot | 2,470 | 16.987 | 17.222 |
| Flight | 2,082 | 16.990 | 22.491 |

The report recorded no intervals above 33 ms and no sample overflow. This is application-pacing evidence for the tested routes, not full release qualification.

## Delivery copy

The qualified Shipping package was copied to the sibling `BorrowedSunWorkshop/Windows` directory. All 30 files, totaling 991,578,141 bytes, matched the source package by size and SHA-256. The [delivery manifest](../../../BorrowedSunWorkshop/delivery-manifest.json) records each file and qualification report. The build used a dirty working tree based on commit `dffaad03c95e00c44dacd0a411fb2428f2db9d60`; that base commit alone does not reproduce the qualified source. Existing packages and player data were preserved.

## Visual review and remaining work

The Shipping front, bench and market-arcade images below were visually inspected. In the front view, the sign is fully legible without column-capital occlusion and all three hanging lamps read clearly. Counter surfaces appear smoother, and the repair cell is visible on the right of the bench. No obvious tangent-shading artifact is visible in these views. The market-arcade view places the workshop in the surrounding street; its foreground canopy pole partly obscures the shop name. These three still views do not establish appearance from every angle or during motion.

The bench view still shows grain running across long counter and shelf members, with conspicuous repetition. Localized wear also needs further artwork. The [earlier unpackaged frontage](../Art/Reviews/workshop-2026-10-13/front.png) and [bench](../Art/Reviews/workshop-2026-10-13/bench.png) remain available for comparison.

![Reviewed Shipping workshop frontage](../Art/Reviews/workshop-2026-10-13/shipping-front.png)

![Reviewed Shipping workshop bench](../Art/Reviews/workshop-2026-10-13/shipping-bench.png)

![Reviewed Shipping market arcade](../Art/Reviews/workshop-2026-10-13/shipping-market-arcade.png)

Next, correct the grain direction and localized wear, then review the same camera views and record a human playtest. The source geometry, import, package, automated routes and sampled pacing do not establish AAA visual acceptance or physical-object gameplay.
