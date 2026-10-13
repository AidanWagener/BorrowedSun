# Borrowed Sun approved art direction

The [worldbuilding and asset specification](WORLD_ASSET_SPEC.md) expands this direction into district grammar, a 4,200-entry planned geometry catalog, collection and physical-object contracts, and future destruction tiers. The [kit shortlist](WORLD_KIT_SHORTLIST.md) retains technically and artistically relevant candidates regardless of AI labels, while recording published terms separately. These planning records do not certify finished models or implement the proposed interactions.

The user approved **Brass Sanctuary** as the architectural and material direction, with **Indigo Monsoon** as the reference for rain and night lighting. This decision governs new asset selection and the first Orison city block.

![Approved daytime reference](References/BrassSanctuary.png)

Warm plaster, sandstone, indigo shade cloth and oxidized copper establish the everyday city. Deep arcades, perforated screens and stepped waterfront construction provide a coherent architectural vocabulary. Machinery should look maintained and repaired: useful service access, cables, collectors and docking equipment give ornament a purpose.

The original Lantern Keeper is the city's landmark: a moth guardian holding its artificial sun, framed by wing-shaped solar collectors. The first in-game version is a silhouette study, to be refined as a hero asset. The city and guardian are original designs, not licensed BGE2 content.

![Approved rain and night reference](References/IndigoMonsoon.png)

The first implementation is a golden daytime block. Indigo Monsoon defines future lighting and weather studies; it does not introduce a weather or time-of-day system in this pass.

## First asset intake

Four Poly Haven sets provide 2K base color, DirectX normal and roughness maps. Original source bytes, authors, download URLs and hashes are in [the intake register](../Registers/city-material-intake.json). Poly Haven publishes its assets under [CC0](https://polyhaven.com/license), including permission for commercial use and redistribution.

| Source | Use |
|---|---|
| [Plastered Wall 02](https://polyhaven.com/a/plastered_wall_02) | Façades and a deliberately recolored copper surface study |
| [Seaworn Sandstone Brick](https://polyhaven.com/a/seaworn_sandstone_brick) | Quay foundations, steps and supporting masonry |
| [Cobblestone Floor 08](https://polyhaven.com/a/cobblestone_floor_08) | Promenade paving |
| [Denim Fabric 02](https://polyhaven.com/a/denmin_fabric_02) | Woven shade sails |
| [Brass Pot 01](https://polyhaven.com/a/brass_pot_01) by Rico Cilliers | Scanned market cookware, imported FBX with 1K maps |
| [Brass Diya Lantern](https://polyhaven.com/a/brass_diya_lantern) by Bhargav Kubal | Hanging market lights, imported FBX with 1K maps and masked glass |

The copper study uses recolored plaster variation and metallic response; it is not a scanned copper asset. Upgrade it when the silhouette and lighting are approved. Materials remain world-scaled so changing a building's dimensions does not stretch its texture.

Twelve original reusable meshes are authored through Unreal's editor APIs: arcade, turned column, dome, lattice screen, draped shade sail, planter, wing panel, palm frond, cornice, Keeper mask, folded robe and solar collector segment. Their generator is [BSCityArtCommandlet.cpp](../../Game/Source/BorrowedSunEditor/Private/Demo/BSCityArtCommandlet.cpp). Two scanned props bring the total to 14 meshes, seven materials and 21 imported texture maps. Repeated geometry is instanced. Detailed art is concentrated on the playable quay; distant buildings establish scale. The separate [prop intake register](../Registers/city-prop-intake.json) preserves original source hashes and provenance.

The daylight pass uses Lumen, virtual shadow maps and a bounded physical exposure range. A one-time graphics preset migration enables the intended illumination and reflection quality while retaining input, audio, display and mission preferences. Later launches retain subsequent quality choices. Night and rain remain reference directions.

## Starting market production

The v0.4 starting market adds the [approved benchmark](STARTING_MARKET_BENCHMARK.md), a pinned Blender authoring pipeline and 21 exported architectural, textile, paving and sculpture meshes. [The market intake register](../Registers/market-asset-intake.json) retains exact authors, source links and checksums for nine CC0 models (crate, basket, pomegranate, potted plant, vase, stool, bowl, pan and tree) and four surface sets. These are additions to the earlier city assets above. Generated indigo fabric has separate prompt and hash provenance in `SourceArt/Generated`.

The Blender source and exports are editable production assets; the generated benchmark remains a reference image. Source triangle counts, package counts and successful tests do not establish that its visual quality has been reached. Actual runtime captures and the corresponding validation report are the review evidence.

The original Second Dawn workshop adds 12 meshes in a 39-part assembly. Its [surface pass](../Operations/WORKSHOP_SURFACES_2026-10-13.md) uses component-aligned UV grain and authored contact/dirt masks. [The workshop intake](../Registers/workshop-surface-intake.json) records Rob Tuytel's CC0 `fine_grained_wood` colour, normal and roughness maps, original files and checksums. These joinery maps replace the shared medieval plank scan on workshop meshes; existing market doors retain their separate materials.

## Kit purchasing decision

[KitBash3D Shangri-La](https://kitbash3d.com/products/shangri-la) is a candidate for selected roof and distant monastery forms, subject to an exact asset/version and gameplay suitability review. [Neo City](https://kitbash3d.com/products/mini-kit-neo-city) is a candidate for industrial details; its broad cyberpunk façades do not establish this city's primary identity. Neither kit is purchased or imported by this pass. The first block uses the reviewed CC0 maps and original reusable geometry; asset spending remains EUR 0.

Build one representative block, inspect it at walking and flight camera distances, verify the mission route, and measure the packaged result before expanding the city. Final sculptural quality, bespoke spacecraft art and dense street life remain separate milestones.
