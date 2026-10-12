# World library planning and workshop source validation

13 October 2026

The new worldbuilding specification establishes the long-term asset library, sourcing strategy, physical-object authoring requirements and staged destruction. It contains 4,200 planned geometry briefs in 235 families, with an offline searchable catalog and direct kit links. No paid vendor kit was purchased or imported by this work.

The user asked that AI-related restrictions not remove otherwise useful kits from the shortlist. Selection records preserve that direction and retain published terms separately. The catalog starts with zero accepted entries; existing meshes require explicit mapping and review against the new contracts.

## Workshop source work

Before the planning request, a Second Dawn lamp-repair workshop was authored with 11 original meshes and a 46-part data-driven assembly. Its editable Blender source, exports, recipe and manifest are in `SourceArt/LanternWorkshop`. Unreal's commandlet created 16 content packages: meshes, four new materials and the assembly data asset.

The source market now uses the assembly for one stall. The assembly validates stable child keys, meshes, transforms and positive scales. Substantial furniture has authored collision. These are decorative/static parts; carrying, collection and destruction remain proposed work.

## Executed evidence

- Catalog generation validated 4,200 distinct definition IDs and the expected category totals. Every record is Planned and no record claims a finished model.
- Local HTML passed 17 browser checks in headless Edge: search, filters, details, pagination, filtered JSON export, mobile overflow, document navigation and script errors. Desktop catalog, detail dialog and specification previews were visually inspected. Evidence is `Docs/Registers/world-catalog-validation.json`.
- Repository preflight and `git diff --check` passed after the source and planning changes. Preflight does not establish visual or gameplay quality.

- Initial Editor and Game Development build passed: `Artifacts/build/20261012T221809Z-72e1acc9`.
- Workshop-only Unreal import passed with 16 saved packages: `Artifacts/market-art/20261012T222134Z-afdaeefa`.
- The first automation run crashed because its negative-test fixture appended an element directly from the same TArray being modified. The fixture now makes an independent copy before appending; the failure was not a gameplay or mesh-load failure.
- Editor and Game Development rebuild passed: `Artifacts/build/20261012T222722Z-3013906a`.
- All 14 required automation tests passed after editor restart: `Artifacts/tests/20261012T222819Z-7ba22715`. The new test checks the saved assembly, material references, furniture clearance and rejection of invalid data.
- The existing v0.4 recovery snapshot was hash-verified for 737 files. It remains same-device recovery evidence, not an independent-device backup.

The source workshop has not yet received a new packaged visual/performance qualification. The verified v0.4 player package and its ordinary save data remain the delivery baseline. No claim of matching the visual benchmark is made by a successful import or test run.

## Next implementation outcome

Review the Second Dawn frontage in the game, refine its materials and construction, then implement ten representative movable props with safe carrying, throwing and save/reload. Prove collection through a transfer stub before choosing the inventory design. Add bounded prop breakage only after that interaction and identity foundation passes.
