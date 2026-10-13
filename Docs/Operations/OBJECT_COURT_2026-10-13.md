# Physical objects and Second Dawn object court

Status: Development implementation built and launched; city placement and packaged/rendered interaction qualification remain in progress. This extends the [object ledger](OBJECT_LEDGER_2026-10-13.md). It does not complete BS-027 or establish AAA visual quality.

## Player behavior

The separate `/Game/BorrowedSun/Maps/L_ObjectLab` map contains ten original hand props, a table, shelves, stairs and a narrow doorway. F picks up/drops; left mouse throws; right mouse plus mouse movement rotates; Z collects into a ten-place test bin; F5/F9 save/load; Escape pauses and releases the grip. WASD, Shift and Space provide movement. The expedition's mission checkpoint remains separate.

The hand uses an actual physics handle and a visibility trace limited to 225 cm. Held bodies continue simulating. A conservative sphere sweep restricts the target against obstructions; excessive grip stretch releases the object. Throw velocity is bounded. Pawn collision is ignored for these small hand props to avoid launching or elevating the character. Carry eligibility uses fixed mesh dimensions, not rotation-dependent world bounds. A socket or center of mass provides the grip independently of the render pivot.

Stable placed IDs reconstruct actors from the ledger. Settled, collision-checked poses save; moving or held objects retain their previous safe pose until they settle. Collection hides/disables the world representation only after durable commit and read-back. An ambiguous storage result requires reload reconciliation. Possession changes, menu, focus flush and teardown release constraints. Sleeping props have no actor Tick; the hand ticks only when occupied. A moving-prop timer runs at 4 Hz. Out-of-bounds recovery uses a small explicit anchor set and reports failure if none is clear.

## Authored content

`Tools/Art/Build-InteractionProps.py` creates the original Blender source, ten FBX files and their manifest in `SourceArt/InteractionProps`. No vendor geometry is an input. The meshes reuse the existing workshop material palette. The bundle is a rigid prop, not a cloth simulation.

| Prop | Mass kg |
|---|---:|
| Enamel tea cup | 0.28 |
| Glazed sharing bowl | 0.65 |
| Caged service lantern | 1.10 |
| Insulated service spanner | 0.55 |
| Ordinary workshop power cell | 0.80 |
| Strapped parts crate | 3.20 |
| Woven parts basket | 0.45 |
| Stoppered ceramic flask | 0.90 |
| Repair ledger | 0.70 |
| Tied cloth parcel | 1.40 |

The source manifest records 41,280 visual triangles and 61 authored UCX hulls. The import report verifies all ten meshes, their hull counts, dimensions and material references. Unit scale is required for physical placements. Cup/bowl/basket use sector hulls to retain an interior cavity. The power cell is an ordinary prop; the mission sunseed cannot enter this ledger.

`Tools/Author-InteractionArt.ps1 -Execute` verifies the FBX hashes, then invokes an Unreal editor commandlet to create ten real mesh packages and the lab map. Replacing existing owned packages requires deliberate `-Force`. Close any process using source assets before imports/builds. Keep the separate player-data root used for the object court separate from normal expedition saves.

## Evidence

- Development Editor and Game build passed: `Artifacts/build/20261013T072952Z-f0dfabe1`.
- All 19 expected smoke tests passed, with zero warnings: `Artifacts/tests/20261013T073201Z-1f9f3aea`.
- Two new tests run actual physics worlds. They check falling/resting poses, overlap rejection, wall occlusion, physical carrying/rotation/throw velocity, release on possession changes, failed collection while held, successful suppression and reconciliation after ambiguous read-back. These fixtures use an engine cube; they do not qualify all ten final prop meshes.
- Ten mesh packages and the map imported successfully: `Artifacts/interaction-art/20261013T073257Z-da779561`.
- The development object court was launched for the user. Its map and game mode loaded, the process responded, and isolated object slots were written. Launch is not a complete playtest.
- The main development city was subsequently launched from the same source build: `Artifacts/demo/20261013T073702Z-3d576e33`. The new props are not yet placed or bound to controls in that expedition.

The first compilation caught an incorrect spawn overload and an ambiguous fixture class name. The first physics test exposed an invalid test-floor setup and the rotation-dependent carry-size defect. Both were fixed before the passing run; failed evidence is retained locally.

## Remaining acceptance

Exercise all ten actual imported meshes through carry, rotation, drop, throw, settlement, save/reload and collection in a rendered runtime. Inspect collision cavities, contact tolerance and recovery under obstructed anchors. Review screenshots and physical input feel. Place suitable props in the main workshop with mission-safe controls, qualify both expedition routes, then build and validate the combined Shipping package. Check frame pacing separately from screenshot capture. The existing delivered Shipping folders have not been replaced by this Development work.
