# Orison terrain and starting-market art checkpoint

13 October 2026. The user explicitly authorized building the planet now, with the starting-area art pass continuing in parallel. This checkpoint connects the existing main game to a real regional Landscape and improves the market's geometry, materials and daylight. **The approved generated screenshot has not been matched.** The complete living planet remains the active goal.

The observable acceptance condition for this increment is readable architecture and shop surfaces at the real walking camera, the same functioning workshop and expedition, and a geographically anchored regional surface with verified collision. Source validation, Unreal smoke tests and both rendered mission routes passed. The Shipping package from source commit `f21433a396c33189025bb3fbc8e1d20d10eb5412` also passed rendered routes and a separate normal-window pacing measurement; details and limitations follow below.

## What changed in the main game

The market kit retains its 21 stable mesh names and pivots. Balcony screens now have real apertures and connected stone webs, with side balusters and drip grooves. Canopies have deeper asymmetric folds, hems, seams, ties and tassels. Stone frames have restrained edge wear. Drain grilles are recessed between dressed stone curbs. Their runtime height remains 8 cm so the recessed bed clears the displaced paving. The three 400 cm paving patches use a 200 cm scanned surface period in both displaced geometry and the Unreal material. The kit totals 733,276 source triangles; that count is production metadata, not a quality or performance claim.

The stone and plaster palette is less uniformly orange. The cloth material uses authored panel masks to separate subdued indigo panels from patterned edges. The masks are exported as linear numeric values. Unreal's intermediate MeshDescription sRGB-decodes their bytes and the mesh builder re-encodes them; the regression therefore checks the actual built color buffer consumed by the material. The quiet canopy mask is 30/255, with full-strength border values of 255/255. Changing the FBX exporter to SRGB would double-transform this contract.

The final daylight uses a 35,000-lux sun at pitch −42°, yaw −18°, which reaches the route between the tall street walls. An existing 3,500-lux fill faces the workshop frontage; skylight intensity is 1.8. Exposure remains EV100 11–11.6. Fog density is 0.004 with height falloff 0.20. These are authored art settings, not a functioning planetary solar or weather simulation. The first rendered look was rejected for an excessively dark street; the second still obscured workshop materials. The third makes the sign, timber and teal enamel readable without globally lifting exposure.

The Keeper sits farther down the view corridor, at runtime X=250 m, on a terrain-supported foundation. Background buildings also sample their footprints and extend foundations below the lowest sampled ground. Selected central background blocks were removed from the view corridor. The current city remains a compressed prototype composition; these cosmetic buildings are not completed canonical districts.

## Regional construction

The main map now contains a native **27 × 24 km** Landscape: 1009 × 1009 height samples and 256 components, imported from the canonical Orison city plan. The [source contract](../../SourceArt/World/Orison/README.md) records axes, byte order, sea datum, control residuals and the preservation pad. R16 SHA-256: `08b8506576e172427b2efd4fa5d5274c693cb24517e33066c9eb00f9014a2005`.

The coordinate authority maps the existing workshop anchor to city-local (−4780, −1370, 92) metres without moving its playable objects. A flat 100 m preservation pad remains below the quay, blending back to the region by 280 m. Canonical rim and dry POI elevations constrain the macro shape; supported structures above water do not become artificial islands. An opaque sea plane identifies the planning datum.

The old recovery plane at runtime Z=−500 cm would have respawned a player legitimately walking downhill. With the regional terrain loaded, recovery instead begins three metres below the sea datum. A rendered check places the player on actual downhill Landscape collision, waits for grounded movement and verifies that recovery does not occur before resuming the expedition.

The terrain importer loads the existing main map, replaces only its two tagged terrain/sea actors when explicitly requested, and preserves the other actors. It clears read-only flags only on its map and two exact material outputs. Repeat import was tested with those material files deliberately made read-only to reproduce an LFS checkout. The lighting commandlet edits six existing named actors and records before/applied settings; it does not rebuild the map. The runtime module does not gain an editor dependency.

## Executed verification

| Verification | Result | Local evidence |
|---|---|---|
| Editor and Game Development compile | Passed on UE 5.8.2 CL 56702186 | `Artifacts/build/20261013T151857Z-df0cd970` |
| Source terrain regeneration and negative controls | Byte/hash match; all eight intentional corruptions rejected | `Tools/World/build_orison_heightfield.py --check`; source JSON |
| Full market import | Verified source hashes and saved actual Unreal assets | `Artifacts/market-art/20261013T145648Z-556fdb90` |
| Material-only iteration | Exactly nine architectural materials saved after matching full import | `Artifacts/market-art/20261013T145908Z-8c7a57ce` |
| Repeat terrain import, read-only material regression | Passed; 256 native components | `Artifacts/orison-terrain/20261013T152100Z-fc2d4133` |
| Final authored look | Passed; existing actors preserved | `Artifacts/starting-area-look/20261013T152121Z-09f31e36` |
| Unreal smoke suite | All 21 expected tests passed, including built cloth masks and coordinate authority | `Artifacts/tests/20261013T152139Z-5944ca02` |
| Final Development rendered routes | 63 checks passed across Clinic and Evacuation, including physical props, checkpoints and regional collision/recovery | `Artifacts/demo/20261013T152204Z-be8f4c91` |
| Script/report regressions | 27 tooling checks and 39 demo checks passed | `Artifacts/tooling-tests/20261013T151756Z-ba3e06f9`; `demo-20261013T151756Z-68b2f75e` |
| Package preflight | Passed | Executed after final source changes; does not prove cooking or performance |

The render run used Unreal's real RHI at 1920 × 1080, offscreen, with isolated QA saves. Fourteen screenshots were captured. Street-start, workshop-front, market-arcade and region were directly reviewed from the final run; they are retained unmodified with hashes in [the review directory](../Art/Reviews/orison-market-2026-10-13/). A final source correction restores the drain placement from Z=3 to Z=8 cm after review found the lowered grates hidden under displaced paving; these Development images precede that correction. The subsequent Shipping review must verify it. Other captures are available in the local run but are not all claimed as visually reviewed. Offscreen rendering is not normal-window presentation or frame-pacing qualification.

![Actual Shipping street gameplay](../Art/Reviews/orison-market-2026-10-13/shipping-street-start.png)

![Actual Shipping workshop](../Art/Reviews/orison-market-2026-10-13/shipping-workshop-front.png)

## Remaining visual and world work

The market still repeats closed doors and similar stalls. The Shipping capture confirms visible drain grates after the height correction, but their curbs still read as overly straight bright lines. The distant city lacks the reference's dense, layered buildings and connective infrastructure. The Keeper's sculpture and surface detail remain simplified; its smaller silhouette also overlaps the objective panel from the starting camera. Vegetation, hanging services, localized construction wear and variations in shop occupation need substantially more authored work. The workshop is more readable, but its stock is still arranged too uniformly. Neither automation nor the triangle count resolves these art findings.

The region capture shows the existing city as a very small patch inside a larger basin. Its terrain is smooth and visibly banded, with a schematic shoreline and plain sea. It has no finished roads, settlements, vegetation, erosion, drainage, coastal defenses, caves or wildlife. Ground samples are about 24–27 m apart: foot-scale surface detail requires a finer production layer. The patch is flat in its coordinate convention; planetary curvature, World Partition streaming and inter-region handoff are not implemented. Flight still has the original ±450 m horizontal bounds and 280 m altitude ceiling. The new terrain does not imply region-wide playable flight.

The next art increment should break shopfront repetition and compose the route's middle distance, with the same walking-camera comparison. The parallel world increment should establish an explicit route and traversal transition from the preserved quay into the canonical region, then add finer terrain and coast geometry. Keep stable object identities, both mission outcomes and existing saves intact. No new assets were purchased in this checkpoint.

## Shipping delivery

The package was built from the clean source checkpoint above, using the pinned engine. Both the main expedition map and object court cooked, but the default player launcher opens the main expedition. The new delivery is `../BorrowedSunOrison/Windows`; all 30 delivered files, totaling 1,011,172,522 bytes, were SHA-256 compared with the source package and rechecked after runtime qualification. [Build evidence](Evidence/orison-market-shipping-build.json) and [delivery manifest](Evidence/orison-market-delivery-manifest.json) are retained in the repository. No warnings or errors were found in the package log.

The packaged screenshot run, `Artifacts/demo/20261013T153553Z-0b11269d`, passed **63 checks** across both mission outcomes, physical workshop props, persisted collection/checkpoints and real Landscape collision. It captured 14 views at 1920 × 1080 with a real offscreen RHI. Six final views were directly reviewed and retained: street-start, workshop-front, workshop-bench, city-overview, lantern-keeper and orison-region. The [capture manifest](../Art/Reviews/orison-market-2026-10-13/shipping-capture-manifest.json) identifies their original files and hashes. The overview and regional views expose the unfinished outer city and coarse terrain; they are part of the evidence, not hidden by the street composition. [Runtime evidence](Evidence/orison-market-shipping-runtime.json).

The separate normal-window run, `Artifacts/demo/20261013T154004Z-8ee47193`, passed **61 gameplay/report checks**, with screenshot capture disabled. On this RTX 4070 Laptop GPU machine at 1920 × 1080 with the 60 FPS cap, it recorded 4,762 application frame intervals over 79.479 seconds after a five-second warmup:

| Route mode | Samples | Median | P95 | P99 | Maximum | Over 33 ms |
|---|---:|---:|---:|---:|---:|---:|
| Walking | 2,687 | 16.668 ms | 16.801 ms | 16.961 ms | 17.821 ms | 0 |
| Flight | 2,075 | 16.669 ms | 16.781 ms | 16.934 ms | 127.214 ms | 1 |
| Overall | 4,762 | 16.668 ms | 16.794 ms | 16.951 ms | 127.214 ms | 1 |

Most intervals are close to the 60 FPS target; **one 127.214 ms flight hitch remains unexplained**. This is application pacing including scheduling and transitions, not isolated CPU/GPU profiling, a whole-region flight test or a guarantee on other hardware. The actual 1080p viewport and configured cap were checked by the harness. No concurrent game/editor process was present. Human evaluation of keyboard/controller feel and the final visual target remains outstanding. [Frame evidence](Evidence/orison-market-shipping-frames.json).

`../Play_Borrowed_Sun.cmd` now selects this main-game delivery, preserving the existing `BorrowedSunPlayer` data root. `../Preview_Orison_Market.cmd` opens the same main game with its own `BorrowedSunOrisonPreviewPlayer` root. Previous packages and player data remain available. The game was not left running after automated qualification. The source/art checkpoint is pushed to GitHub and all 22 source-art LFS locks from this pass were released.
