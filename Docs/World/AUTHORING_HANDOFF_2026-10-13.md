# Planet authoring handoff — 13 October 2026

The full-world goal remains active. The existing mechanics Development handoff is complete; no runtime, asset import, map construction or package change occurred during this authoring pass.

## What now exists

Seven substantive foundations are available in the [reader](Living_Planet_Reader.html). They establish Nacre/Rauth/Ilex, a checked physical candidate, an approximately 660.6-Earth-year settlement history, overlapping social traditions, ecological anatomy and food webs, everyday pleasures and the first planetary geography. The [source counts](AUTHORING_COUNTS.json) are reproducible and exclude the old specification; they include technical research and review notes. They are not finished-page or quality claims.

The geographic concept has four continental masses, two island arcs, 24 regional anchors, twelve city centers, six downhill trunk-river concepts and thirteen intended connections. The route table uses great-circle lower bounds on the 5,900 km reference sphere. It exposes distance without claiming navigable roads or sea passages along those mathematical lines.

Independent reviews checked the core chronology against physics, society and ecology. Corrections include separating the Pale Reach coastline from Kesh, distinguishing early household practices from their later formal institutions, preserving inherited technical standards, identifying Mirror Ring as a moving orbital annex, clarifying age/calendar units and keeping low-dispersal native species from becoming universal planetary decoration.

Working canon adopts limited native biochemical overlap with explicit processing, active ventilation/circulation for large ribbon movers and a marine/estuarine identity for pale hinge. Detailed ranges remain conditional on habitat and dispersal. The leisure draft's bell oil and fold syrup remain proposed food ingredients until their production/qualification dossiers are written; introduced substitutes are available.

## Verification performed

- `Tools/World/build_atlas.py`: unique IDs, coordinate bounds, closed/non-self-intersecting concept coastlines, non-overlap, city containment, descending trunk elevations, route endpoints and known geodesic invariants. These are authoring consistency checks, not a terrain simulation.
- `Tools/World/inspect_atlas.cjs`: local headless-browser rendering, twelve visible city markers and no text outside the authored canvas. The initial Chrome direct-SVG screenshot timed out; the successful run used Edge with an HTML wrapper and GPU disabled. The final screenshot was visually inspected, and two label contrast/overlap issues were corrected.
- `Tools/Preflight.ps1`: repository configuration, engine pin, script syntax and asset pointer checks passed. No Unreal build was necessary because executable game content did not change.
- `Tools/World/inspect_reader.cjs`: seven chapters, all local links/images and contents anchors resolved, navigation to the atlas worked, and no page errors or body overflow at 1000 px and 390 px. Contents and atlas screenshots were visually inspected.
- `git diff --check`: whitespace review. Final generated counts were refreshed after prose reconciliation.

Local visual evidence is under `Artifacts/world/`; the portable SVG, source data and generator are versioned. The map intentionally remains a coarse geographic concept, visibly unlike finished terrain cartography.

## Next authoring work

1. Qualify the named culinary fractions and deepen regional language and aesthetics beyond the shared institutional traditions. The four leisure milestones are now integrated into the historical spine.
2. Develop a geographic/physical model beneath the concept: relief, catchments, lakes, ocean passages, circulation assumptions, seasonal sky and regional weather. Fix a calendar with explicit intercalation and human-readable scheduling conventions.
3. Complete distinct Morrow, Sere, island and polar ecosystems. The first 24 dossiers mostly support Kesh and selected marine/cold habitats; they are not a complete planetary biosphere.
4. Write full city dossiers and maps, beginning with Orison's actual crater horizon, drainage, food, freight, local ecology and district relationships. Preserve distant settlements and wilderness at their established scale.
5. Author hundreds of individual POIs, named people/social networks, route experiences and dynamic events from the resulting places. The twenty social/leisure encounter seeds are not the requested hundreds of completed POIs.
6. Derive regional asset families, kit requirements, sound, construction recipes and production proofs from that canon. Keep shortlist, purchase, rights, import and verified asset quality separate. The initial budget remains under EUR 500.
7. Compile and review the complete multi-volume edition and atlas before beginning planet construction. Do not silently convert a useful foundation into a completed bible.

## Reproduction

Use the workspace's Python runtime for `Tools/World/build_atlas.py`; `--check` validates without rewriting. `Tools/World/build_reader.cjs` uses the bundled `marked` package (17.0.5 in this environment). `Tools/World/inspect_atlas.cjs` uses the bundled Playwright package and takes a browser executable path. Supply the bundled Node module directory through `NODE_PATH`; no package download or engine upgrade is needed.
