# First Light v0.4.0 — starting market detail pass

This is a playable step toward the [approved generated benchmark](../Art/STARTING_MARKET_BENCHMARK.md). The target has **not** been matched. Review the actual Shipping captures in the adjacent `Market_Previews` folder. The starting street now contains deeper architecture, scanned props, cloth and foliage; repeated compositions, a simplified Keeper and spacecraft, sparse city life and unfinished harbor art remain visible. Asset/tool purchases remain EUR 0.

## Implemented

- Twenty-one authored Blender meshes (503,216 source triangles before instancing): individual arch stones, recessed doors, carved balconies, draped cloth, lanterns, drains, relief paving, planters and an original Keeper study. Editable source and FBX manifests are retained.
- Nine reviewed Poly Haven model assets and four surface sets, plus separately documented generated fabric albedo. The Market directory contains 122 real Unreal packages. The intake register records original authors, URLs, licensing and source hashes.
- A narrower market aisle with varied stock, supported lanterns and street-end trees; the clinic passage and original mission anchors remain reachable. Collision remains on substantial props and building masses; decorative detail is not a per-triangle collision obstacle.
- DX12 SM6 and selective Nanite, with foliage shape preservation on the tree. Lighting, compact HUD and a framed sunseed have been updated. The new scene uses real runtime geometry and materials.
- Isolated rendered QA verifies actual resolution and the idle start pose. Automated runs ignore physical desktop/controller input and drive actual gameplay methods. Ordinary play keeps input enabled. New expeditions clear residual character velocity.

## Qualification

| Check | Observed result | Run |
|---|---|---|
| Market authoring | 102 source hashes verified; 122 packages authored | `20261012T213029Z-7cd5876e` |
| Map authoring | Map and surface saved through Unreal | `20261012T213820Z-d9a4125a` |
| Editor / Shipping compile and cook | Passed | `20261012T215439Z-885bc2b1` |
| Unreal automation | 13 expected tests passed | `20261012T214903Z-8dbe0c90` |
| Packaged visual routes | 39 checks; both outcomes and disk checkpoints | `20261012T215853Z-b86b1284` |
| Separate frame run | 37 checks; both routes, no screenshots | `20261012T220204Z-d0b895f1` |
| Launcher/report regression | 35 passed | `demo-20261012T212835Z-72348546` |

Ten actual packaged captures were inspected at 1920 x 1080, including the starting view, shopfront, aerial view, Keeper, dock and route outcomes. Image capture is separate from timing. These checks do not establish physical gamepad behavior, extended manual flight feel, subjective audio quality or equivalence to a AAA environment team.

## Application frame intervals

Ryzen 7 7435HS / RTX 4070 Laptop / 15.7 GiB usable RAM, 1920 x 1080, 60 FPS cap. This measures wall-clock intervals between game ticks after the initial five seconds, including scheduling and pacing. It does not isolate GPU cost, CPU cost, cold startup or input latency.

| Mode | Samples | Mean ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|
| Overall | 4,548 | 16.667 | 16.781 | 16.946 | 21.872 |
| On foot | 2,466 | 16.667 | 16.773 | 16.922 | 17.647 |
| Flight | 2,082 | 16.667 | 16.794 | 16.982 | 21.872 |

## Delivery and reproduction

`Play_Borrowed_Sun.cmd` uses the new `BorrowedSunMarket/Windows` package and the existing `BorrowedSunPlayer` data root. The separate market-preview launcher uses its own player-data root so the starting area can be reviewed without changing ordinary mission saves. Previous packages remain preserved. Input/audio preferences and saved mission identities are retained.

Use the pinned Blender to run `Tools/Art/Build-MarketKit.py`, then build the editor, author Market assets, author the map, run the smoke suite, and make a Shipping package using the documented tools. Run visual QA and frame QA separately with `-Width 1920 -Height 1080` and fresh isolated user directories. See [local operations](../../Tools/README.md).

The build report records the dirty working tree used during qualification. The delivery manifest records the final source commit and hashes. Detailed logs remain in ignored `Artifacts`; compact evidence is retained here. A verified local backup is a same-device recovery copy, not an off-device restore test.

## Remaining visual work

Localized weathering, more varied facade profiles and shop identities, hero sculpture refinement, bespoke spacecraft art, harbor water/terrain, inhabitants and animation need further production. Placements currently come from a deterministic C++ scene recipe; moving them into editable authored level data would allow layout changes without recompiling game code. The present asset pipeline and successful route tests are evidence of working implementation, not proof that the approved visual target is finished.
