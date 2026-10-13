# Planet authoring handoff — 13 October 2026

The full-world goal remains active. The existing mechanics Development handoff is complete; no runtime, asset import, map construction or package change occurred during this authoring pass.

## What now exists

Eleven substantive chapters are available in the [edition 0.2 reader](Living_Planet_Reader.html), currently 85,014 source words. They establish Nacre/Rauth/Ilex, a checked physical candidate, an approximately 660.6-Earth-year settlement history, overlapping social traditions, ecological anatomy and food webs, everyday pleasures, planetary geography, sky/calendar, and Orison's city plan. The [source counts](AUTHORING_COUNTS.json) are reproducible and exclude the old specification; they include technical research and review notes. They are not finished-page or quality claims.

The geographic concept has four continental masses, two island arcs, 24 regional anchors, twelve city centers and thirteen intended intercity connections. Edition 0.2 adds ten relief provinces, three lake surfaces and eight total water trunks. Coastal outlines were refined around unchanged port coordinates; their earlier broad outlines left several ports hundreds of kilometers inland. Talweir now has explicit level reaches through Namar at 180 m and Iren at 176 m. The route table uses great-circle lower bounds on the 5,900 km reference sphere. It exposes distance without claiming navigable roads or sea passages along those mathematical lines.

The calendar fixes the Landing epoch, year zero, thirteen 25-turn spans and 22 Balance turns per 115 years. Its small reference-orbit drift is measured. Regional sky/weather descriptions distinguish civil scheduling, natural sunlight, terrain shade and electrical service. They do not amount to a climate simulation or exact eclipse ephemeris.

Morrow has 24 additional species dossiers, ECO-025 through ECO-048, and six distinct regional ecological networks. Together with the initial foundation this makes 48 authored species dossiers. These are proposals with habitat, processing and dispersal limits, not a complete biosphere or implemented creature library.

Orison has eleven district envelopes, 24 individually written places, mapped coordinates, twenty transport dependencies and fourteen water dependencies. Its proposal checks population and service allowances while leaving sustainable yields and final engineering open. The [city plate](Cities/orison_city_plan.svg) separates actual point positions from labels and shows the whole breached crater in an inset. The [network register](Cities/ORISON_NETWORK_CHECKS.md) records connection geometry without presenting straight lines as usable streets or pipes.

Independent reviews checked the core chronology against physics, society and ecology. Corrections include separating the Pale Reach coastline from Kesh, distinguishing early household practices from their later formal institutions, preserving inherited technical standards, identifying Mirror Ring as a moving orbital annex, clarifying age/calendar units and keeping low-dispersal native species from becoming universal planetary decoration.

Working canon adopts limited native biochemical overlap with explicit processing, active ventilation/circulation for large ribbon movers and a marine/estuarine identity for pale hinge. Detailed ranges remain conditional on habitat and dispersal. The leisure draft's bell oil and fold syrup remain proposed food ingredients until their production/qualification dossiers are written; introduced substitutes are available.

The second review clarified the 36-million-standard-year Orison crater, observed thermal water versus unproven electrical yield, the west-southwest position of Rauth, a nominal longest night of about 71.7 hours, regulated water supply from Three Drops down to Common Table, and a separately pumped two-meter treatment transfer. Ferry interchanges are not vessel positions. Three waterfront POIs require supported structures and shore access. The old ecology draft's superseded food and species-range wording was reconciled.

## Verification performed

- `Tools/World/build_atlas.py`: unique IDs, coordinate bounds, closed/non-self-intersecting concept coastlines, non-overlap, city containment, port proximity, descending or explicitly level lake reaches, route endpoints and known geodesic invariants. These are authoring consistency checks, not a terrain simulation.
- `Tools/World/build_geography.py`: relief/lake references, level lake surfaces, ocean mouths near the authored coast, closed-basin terminal water, basin focus membership and downstream lake connection. Independent review exercised sixteen one-off negative controls and two positive controls. Catchment polygons and hydraulic behavior remain unvalidated.
- `Tools/World/check_calendar.py`: 37,558 arithmetic assertions, including date round trips, year boundaries, negative years and idealized sky geometry. No climate or long-term orbital proof is claimed.
- `Tools/World/build_city_plan.py`: unique IDs, finite/bounded local positions, population, POI district envelopes/heights, explicit supported overwater access, every district's connection to a declared regional gateway, structured uphill pumping/lift qualifications, potable-source compatibility, storm descent and orbital/ground separation. Thirteen embedded negative controls plus an alternate-gateway positive control passed. These are dependency checks, not service simulation.
- `Tools/World/inspect_atlas.cjs`: local headless-browser rendering, twelve visible city markers and no text outside the authored canvas. The initial Chrome direct-SVG screenshot timed out in edition 0.1; the successful method uses Edge with an HTML wrapper and GPU disabled. The edition 0.2 relief screenshot was visually inspected.
- `Tools/World/inspect_city_plan.cjs`: 24 place markers, eleven district envelopes, no text outside the canvas and no numbered-label box overlaps. The screenshot was visually inspected and an inset caption was moved clear of the rim.
- `Tools/Preflight.ps1`: repository configuration, engine pin, script syntax and asset pointer checks passed. No Unreal build was necessary because executable game content did not change.
- `Tools/World/inspect_reader.cjs`: eleven chapters, all local links/images and contents anchors resolved, navigation to the atlas and Orison worked, and no page errors or body overflow at 1000 px and 390 px. Narrow contents and desktop Orison screenshots were visually inspected.
- `git diff --check`: whitespace review. Final generated counts were refreshed after prose reconciliation.

Local visual evidence is under `Artifacts/world/`; the portable SVG, source data and generator are versioned. The map intentionally remains a coarse geographic concept, visibly unlike finished terrain cartography.

## Next authoring work

1. Qualify the named culinary fractions and deepen regional language and aesthetics beyond the shared institutional traditions. The four leisure milestones are now integrated into the historical spine.
2. Continue the geographic model beneath the authored relief/lake controls: actual catchment boundaries, regional resource yields, ocean passages and circulation, finer weather transitions and regional seasonal maps. Calendar arithmetic is established; full ephemerides and climate are not.
3. Complete distinct Sere, island and polar ecosystems, then deepen ecology's lifecycles, disease/parasites, migratory networks and carrying capacities. Morrow's new networks need continued cross-region review; 48 dossiers are not a complete planetary biosphere.
4. Write the other eleven major city dossiers and maps, then secondary settlements and rural communities. Orison still needs more local history, named social networks, streets, property-level relationships and construction-facing architecture. Preserve the established planetary distances.
5. Author hundreds of further individual POIs, named people/social networks, route experiences and dynamic events from the resulting places. Orison's first 24 are substantive dossiers; twenty older social/leisure encounter seeds are not substitutes for the requested hundreds of places.
6. Derive regional asset families, kit requirements, sound, construction recipes and production proofs from that canon. Keep shortlist, purchase, rights, import and verified asset quality separate. The initial budget remains under EUR 500.
7. Compile and review the complete multi-volume edition and atlas before beginning planet construction. Do not silently convert a useful foundation into a completed bible.

## Reproduction

Use the workspace's Python runtime for `Tools/World/build_atlas.py`, then `build_geography.py`, then `build_city_plan.py`. The atlas and city builders accept `--check` without rewriting; geography regenerates its plate/register. `check_calendar.py` is read-only, and `--json` prints its arithmetic report. `Tools/World/build_reader.cjs` uses the bundled `marked` package (17.0.5 in this environment). The three visual inspection scripts use bundled Playwright and take a browser executable path; the atlas script optionally takes `nacre_relief_and_water.svg` as a second argument. Supply the bundled Node module directory through `NODE_PATH`; no package download or engine upgrade is needed.
