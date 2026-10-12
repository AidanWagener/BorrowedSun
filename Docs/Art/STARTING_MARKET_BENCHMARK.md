# Starting market visual benchmark

The user explicitly approved this generated gameplay mockup on 12 October 2026 and asked for the starting area to reach its visual quality. It is a target image, not an actual render of project geometry, a performance claim, or evidence that the target has been achieved.

![Approved target](References/StartingMarketTarget.png)

The bounded production task is the starting market seen from the walking camera. The v0.3 package and source commit `5ab555d325eb3ed873d347a58b8fec2e120e7c16` preserve the working baseline. Keep the established expedition completable while refining the starting area.

## Acceptance through actual gameplay

- Convincing architectural depth: separate masonry arch stones, recessed doors, bevels, carved borders, supported balconies and believable construction joints.
- Warm worn plaster and stone, precise texture scale, distinct timber and aged metal responses, localized weathering that follows construction.
- Indigo and gold cloth with proper drape, seams, attachments and visible transmitted/bounced light underneath.
- Composed, varied market clusters using appropriate vessels, produce, baskets, crates, textiles and plants, with clear player passage.
- Worn flagstone paving, drains, restrained guide inlays and a focal sunseed reliquary.
- Readable sunlit and shaded areas, controlled bloom and an uncluttered mission HUD.
- Repeated visual review at the actual starting camera, close range, oblique angles and flight height; identify remaining mismatches honestly.
- Both mission outcomes and saved-state behavior still pass in the packaged build. Measure new performance separately from screenshot capture.

Do not label this task AAA-complete based on asset counts, polygon density or automation passes. The user judges visual quality against the captured result. Concentrate work in the playable starting area before expanding the city. The larger city, detailed landmark sculpture and inhabitants in earlier references remain separate scope unless explicitly brought into this benchmark.

## Production choices

Add a pinned portable Blender tool for bevelled mesh authoring and repeatable FBX export. Unreal remains the importer, material author and final renderer. Selective Poly Haven CC0 intake supplies scanned props and surface maps with source hashes. Original procedural geometry supplies the project's distinctive architecture. Generated fabric albedo has its own prompt/provenance record and still requires seam and material review.
