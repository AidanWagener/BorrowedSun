# First Light — first playable demo

The delivered milestone is a reviewable Windows build of one small expedition. This is a prototype of Borrowed Sun's central promise: walking through a place, departing in a ship, making a consequential decision, and returning to see it persist. Actual qualification and remaining limits are recorded in [DEMO_VALIDATION.md](DEMO_VALIDATION.md).

## Observable acceptance

The packaged executable opens the authored Lower Quay map without Unreal Editor. A player can collect one sunseed, board Kestrel, fly to the sky relay, select a clinic or evacuation power lease, return, disembark and install the seed at the selected service. Save/reload preserves the accepted stage, unique custody and exclusive lease. A safe recovery action cannot duplicate or discard cargo. Restart is an explicit menu action. The controls and current objective are visible in the build.

The first demo uses a continuous, bounded map. The sky relay represents the outbound flight destination; it is not a physically simulated planet or a proof of interplanetary streaming. The district, ship and sound are original geometric/procedural prototype content. No marketplace entitlement or framework installation is required to build this slice.

## Implemented contracts

- A single progress subsystem owns stage, revision, custody and power choice. Five pure/memory regression tests supplement the five existing foundation tests.
- Mission events are rejected out of order. State advances only after checkpoint persistence succeeds.
- A rolling previous checkpoint is retained; invalid/unsupported saves are reported explicitly. Reload restores a safe pose for the saved stage rather than arbitrary live actor pointers.
- The character and ship use independent pawns with explicit possession. Flight uses bounded swept movement and collision sliding. Optional flight assistance follows the same movement controller.
- One authored map references the project-owned shared material. The original district recipe uses grouped mesh instances; service effects are toggled rather than regenerated on each load.
- Opt-in validation runs use separate save namespaces. Normal player checkpoints must not be modified by automated route tests.

## Scope deliberately deferred

Full NPC schedules, combat, stealth, animation polish, vendor city kits, large-scale World Partition streaming, planet generation, sophisticated narrative tooling, voice acting, broad remapping/settings menus and commercial release qualification remain later work. The demo should establish whether the central expedition is worth expanding.

## Verification order

1. Rebuild from the recovery bundle in a separate local checkout; record that this does not protect against failure of the same physical disk.
2. Compile Editor and Game targets, create genuine map/material packages through Unreal and run all ten expected foundation/domain tests.
3. Launch with rendering, inspect UI and art, exercise real controls and fix failures.
4. Run both mission outcomes through an opt-in bounded runtime acceptance harness, using actual character movement, flight assistance and game interaction methods. This supplements human play; it does not prove the feel of every input device.
5. Cook/package, launch the staged executable, repeat the acceptance route, inspect logs and preserve compact results.
6. Commit source and authored assets, publish the local review guide and playable package, and distinguish observed results from remaining limits.
