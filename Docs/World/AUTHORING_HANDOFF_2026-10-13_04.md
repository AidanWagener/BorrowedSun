# Planet authoring handoff — edition 0.4

**13 October 2026. Full-world authorship remains active.** This pass expands the canonical-world proposals and travel design. The earlier mechanics Development handoff remains complete; no new Unreal code, map, asset import, package or gameplay system was changed. Planet construction still follows the complete, reconciled bible.

## What this edition adds

The [reader](Living_Planet_Reader.html) now contains nineteen chapters and **177,409 source words**. [Exact source counts](AUTHORING_COUNTS.json) include research and review notes and exclude the historical game specification. They are not finished pages, a completion percentage or evidence of AAA visual quality. The [previous handoff](AUTHORING_HANDOFF_2026-10-13_03.md) preserves edition 0.3.

Three major cities receive substantial original dossiers, each with nine district allocations, twenty-four individually developed places, named overlapping lives, local history, distinct architecture, transport dependencies and service accounts:

- **Marroway:** 670,000 core residents, 110 km² gross allocation, an inland Neris confluence, decorated household boats, raised dry ground, cooking, textiles, performance and agricultural connections. The Neris retains its 18 m city control and northwest-to-southeast relationship. The sea is about 437 km away even by direct spherical comparison. The 360 t selected food commitment to Orison is additional to Marroway's own resident demand.
- **Aven:** 450,000 core residents, 99 km² gross allocation, broad occupied upland benches around the southern spill at 1,650 m. Manufacture, design exhibitions, education, ceramic work, walks and family relationships give the city an identity beyond dramatic mountain views. The local plan keeps the established river center and a supported overwater deck with dry access. An external freight gateway sits outside the district envelopes while remaining connected to the city.
- **Sola:** 290,000 core residents, five authored islands, nine districts and a 90 km² gross allocation. The three 147 AL founding landing grounds remain on Orrel; later expansion adds other islands rather than rewriting that history. Temporary residents, guest rooms, public ferries, Frame Sign, Leri heritage, music, ceramics and Porcelain Weather architecture create a distinctive city. Resident, guest, ship and regional-transfer demands remain separate.

The world now has **five mapped major-city dossiers, 47 district allocations and 120 detailed city places**, alongside 72 species dossiers. These are authored proposals, not constructed cities, completed assets or simulated populations. Seven major-city anchors still lack equivalent dossiers. Secondary settlements, countryside and oceans need extensive further work.

[Roads, skies and exploration](09_ROADS_SKIES_AND_EXPLORATION.md) adds a substantive travel volume: all thirteen existing intercity connection narratives; local movement and climbing requirements; public transport and vehicle roles; map knowledge and information age; civil versus natural time; continuous, assisted and deliberately advanced passage; rescue and recovery; and the separate physical requirements of orbit. Eight exploratory journey briefs and five pirate-adventure operation patterns broaden the experience. They are not counted as additional geolocated POIs or implemented quests.

The adventure patterns include a disputed berth release, contested courier cargo, obsolete optical receiving works, a hazardous alternate approach and access to moving orbital infrastructure. They preserve the original physical-cargo, crew, freedom and consequential-return premise. Everyday life supplies people and places worth returning to; it does not replace danger, audacity or antagonists.

## Decisions and reconciliations

The local settlement format distinguishes treated **network input** from raw abstraction and desalination intake. Distribution losses are applied to the network account. Source treatment, desalination reject flow, reuse and waste remain separate streams. This prevented the same word “raw” from meaning incompatible stages in different cities.

Sola's 156,000 m³ treated reserve corresponds to **2.4 ordinary turns** at 65,000 m³ network input, rather than three turns obtained by omitting its already declared 20% distribution loss. Under an essential 32,000 m³ delivered case with the same loss fraction, the stock represents **3.9 turns**, before further inaccessible storage or handling losses. The dossier now states both cases explicitly.

Aven's regional freight approach may have no district assignment because it is an external gateway. Ordinary interchanges still require a known district, every district must have connected service, and all nodes remain finite, within the planning frame and on authored land. This supports the actual geography without manufacturing an extra inhabited district.

Marroway's moored Blue Latch Boatroom was moved toward its tributary's northern margin. The final prose and JSON agree at (-2,100, -210, 22) m, with a separate dry access point and local water surface. The plan does not place the household in the through lane or claim that a moving vessel has already been simulated.

Travel descriptions preserve watershed differences. Aven is an inland upland destination, not a sea-ferry terminal. Its southern spill does not flow into Talweir. Talweir's eastern outflow is distinct from Dhal's drainage. Dhal remains a river port inland of its mapped mouth. The longitude seam between Merrow and Tern is a drawing convention, not a physical discontinuity.

The travel study retains physical scale and separates minimum geometric distance, assumed groundspeed, handling and civil elapsed time. It does not establish a route or available transport service. A hypothetical 300 km circular orbit gives about **6.961 km/s**, **93.274 minutes** and **26.690 MJ/kg** ideal mechanical energy change from a stationary surface reference. The 10 t comparison is about **74.139 MWh** mechanical change, not fuel or electrical input. This is not Mirror Ring's adopted orbit or a Kestrel ascent design.

## Verification scope

`build_settlements.py` checks parent city identity/frame/population; closed, disjoint local surface polygons; district population and area totals; finite bounded place coordinates; district envelopes/heights; dry or explicitly supported water placement; nonrising channel controls and the inherited river-center elevation; land interchanges, gateway connectivity and separate-island transport modes; and selected water, food and power arithmetic. Each of the three city records passes fourteen embedded negative controls. These checks do not establish street alignments, bathymetry, flood protection, crop yields, drinking-water efficacy, pressure, structural support or operational capacity.

City authors independently reviewed their coordinates, named place references and resource arithmetic. A separate ecology review found Marroway's species ranges, native-food limits, the Orison supply commitment and water/reuse accounting consistent. Final river geometry and supported placements remain authored controls, not a surveyed environment.

`build_mobility.py` checks all thirteen route IDs, eight timing cases, declared civil/solar references and finite input and derived quantities. Eleven negative controls cover invalid speeds, handling, endpoints, river indices, units, route identities and orbital inputs, including finite inputs that would overflow calculations. Positive checks cover the longitude seam, kilometer/hour conversion and an independently recomposed circular-period relation. A separate physical review recalculated the orbital result and timing cases.

All three 1560 × 1570 city plates passed Edge inspection: twenty-four place markers and nine district envelopes each, no label-box overlaps and no text outside the canvas. Rendered maps were visually inspected, including the final Marroway river junction and Sola island presentation. The nineteen-chapter reader passed at 1000 px and 390 px, with every contents destination working, all local files/images/anchors present, no page errors and no body overflow. The desktop Sola reading view was also inspected visually.

Repository preflight passed configuration, engine-pin, script-syntax and local asset-pointer checks. Local evidence is under ignored `Artifacts/world/`; portable SVGs, source JSON, Markdown and generators are versioned. No Unreal build or gameplay claim follows from this documentation pass.

## What remains before construction

1. Author the remaining seven major cities: Hesh, Serein, Dhal, Merrow, Tern, Velis and Brin. Give them equally specific local geography, independent lives, infrastructure, visual grammar and places worth finding. The five existing dossiers also need finer streets, property relationships and qualified service/source models.
2. Locate secondary ports, towns, villages, refuges, mobile communities and orbital installations. The Sola–Aven sea/land connection particularly needs a real mapped Morrow gateway. Major-city centers and straight dependency lines cannot substitute for those places.
3. Develop continuous relief, catchment boundaries, water budgets, resource locations, circulation, seasonal weather and actual travel courses. Existing controls and demand accounts expose obligations rather than proving sustainable productive capacity.
4. Complete island, pelagic, deep-water and remaining terrestrial ecosystems, then ecological ranges, migrations, disease, cultivation and qualified native materials/food. Sola deliberately does not borrow distant endemic species merely to fill its gardens.
5. Expand history, faith, philosophy, art, households, social graphs, conflict and private ambitions across regions. Preserve pleasurable ordinary life while building the danger, spectacle, secrecy and consequential freedom of the pirate adventure.
6. Develop hundreds of geolocated places beyond the present 120 city dossiers; attach route briefs and operation patterns to actual terrain, named people, conditions and persistent aftermath. Do not count abstract encounter patterns as completed locations.
7. Derive regional asset families, soundscapes, construction recipes, technical proof tasks and kit requirements from the reconciled world. Preserve the original 4,200 stable asset briefs and recorded production progress. Initial asset/tool budget remains under EUR 500; this pass made no purchases.
8. Compile and review the complete multi-volume canon before building the planet. Then prove the required Unreal capabilities and construct from that canon, preserving geographic and persistent-state authority across scale.

## Reproduction

Run the configured Python runtime on `Tools/World/build_settlements.py marroway aven sola` and `Tools/World/build_mobility.py`; `--check` avoids output rewriting. The settlement format is documented in [SETTLEMENT_PLAN_FORMAT.md](Cities/SETTLEMENT_PLAN_FORMAT.md). Existing Orison and Talweir retain their specialized formats and tools.

Compile the reader with `Tools/World/build_reader.cjs` and bundled `marked`. Use `inspect_world_plate.cjs <Edge path> <marroway|aven|sola>` and `inspect_reader.cjs <Edge path>` with bundled Playwright through `NODE_PATH`. No package installation, engine upgrade or cloud service is required.
