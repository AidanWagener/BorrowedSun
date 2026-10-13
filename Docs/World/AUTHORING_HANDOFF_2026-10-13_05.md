# Planet authoring handoff — edition 0.5

**13 October 2026. The full-world goal remains active.** The already-started ordinary-prop mechanics passed their earlier Development handoff. This edition continues world authorship only: no Unreal runtime, map construction, new mechanics, asset purchase or asset import. Complete and reconcile the planet bible before beginning planet construction.

## Material added

The [living planet reader](Living_Planet_Reader.html) contains **23 chapters and 217,861 source words**. [Exact counts](AUTHORING_COUNTS.json) use the reader's consistent counting method and include research/review notes; the historical game specification is excluded. These are source words, not completed pages or a measure of visual quality. The [previous handoff](AUTHORING_HANDOFF_2026-10-13_04.md) preserves edition 0.4.

Three substantial city dossiers add nine districts and twenty-four individually developed, mapped places each:

- **Hesh:** 310,000 core residents and 70 km² gross district allocation. A dry closed-basin city of mineral rooms, thin shade structures, intimate ornament, independent crews, sectional routes, saline processes and industrial remains. Its 270 m lake outline is projected directly from LAK-003 and clipped to the city frame. The RIV-003 terminal remains approximately 62.3 km southwest, not at the city center. Lake-supported placement uses the lake surface, not an assumed zero sea level.
- **Serein:** 530,000 core residents and 114 km² gross allocation. A western fog-coast city of sheltered courts, enclosure manufacture, distinctive windows, public rooms, ship fabrication and exposed approaches. Marine layer and cliff behavior use bounded primary-source analogies; weather is not permanently foggy. Its twelve linked biographies preserve Tam Arel and Neris Vale. Rauth is low toward inland eastern sky rather than over the western sea.
- **Dhal:** 160,000 core residents and 74 km² gross allocation. A lowland forest river city with humid-service construction, publishing, river yards, protected domestic interiors and layered native habitat. RIV-005 retains the [166°, 4°, 8 m] city control and its separate coastal mouth. The city is inland; Rauth is below its ordinary horizon. Vault vanes and associated species retain their actual ecological dossiers rather than borrowing Kesh vegetation.

The full library now contains **eight mapped major-city dossiers, 74 district allocations and 192 detailed city POIs**, alongside 72 species dossiers. Counts refer to authored records, not built settlements, completed models, actual simulated populations or implemented quests. Four major cities still require equivalent dossiers: Merrow, Tern, Velis and Brin.

[Productive landscapes](06b_PRODUCTIVE_LANDSCAPES.md) adds 6,479 source words on settlement beyond the major cores, cultivation, rural life, water, light, nutrient/material flows, six regional production landscape families, seven scenario families and asset requirements. The twelve core-city estimates total 4.88 million of the 34.8 million planetary population; the remainder requires much more settlement geography than twelve isolated capitals.

The seven new scenario families are not counted as additional geolocated POIs. They include contested consignee claims, salvage access through occupied ground, a still-running crop room, a safe but unpopular product batch, residue custody, a returning work crew and a protected living-stock shipment. They need concrete sites, people and persistent aftermath before becoming finished encounter dossiers.

## Resource model and decisions

Selected introduced staple lineages adapted to Nacre's long natural cycle are now an explicit fictional biological premise. This develops existing canon about natural-cycle crops; it is not an empirical prediction for present Earth grain. Sensitive crops and propagation stocks retain controlled-light requirements. The world does not assume that a few greenhouse roofs supply all city food or that every field receives free artificial noon.

The first [production study](Resources/PRODUCTION_ARITHMETIC.md) reads the existing Orison food ledger. It sizes only the **620 t/turn dry, fresh and cultured subset** through thirteen allocated crop cases. The 30 t conditional native and 50 t other-preserved categories still need product-specific source and processing accounts. Regional residents and other export customers remain additional obligations.

At its openly hypothetical yields and retained fractions, the study requires **24,402.25 ha productive canopy** and **347.21 km² farm-land allowance plus unstacked urban canopy**. Culture products consume upstream crop substrate; their wet output is not free additional primary production. The urban 24 t fresh subset is deducted from Saffron's 90 t row before the rural 66 t is sized.

The selected annual evapotranspiration is **168.777 million m³**. After assumed effective rain, urban condensate recovery, leaching allowance and irrigation delivery fractions, source irrigation is **47.852 million m³/year**, averaging **147,149 m³ per civil turn**. Seasonal peaks, catchment rights and source capacity are not established. Agricultural flow is separate from municipal drinking-water service. Assumed-head hydraulic pumping is about **11,951 MWh/year**, or **1.531 MW** mean, excluding treatment, cooling and other services.

The alternative lighting comparison explicitly measures useful photons delivered at canopy per joule, including optical losses. Lighting all modeled canopy under its chosen schedule would imply about **6.78 GW mean** supplementation. This is a consequence comparison, not adopted regional demand. For the 24 t urban fresh target, its daylit-canopy case gives **90.33 MW mean** fixture load; four layers with three shaded layers give **158.08 MW**. Orison's 600 MW end-use budget has not yet been reconciled with a selected farm system. The city and exchange chapters now point to that unresolved obligation rather than silently treating the target as supplied.

Selected regional commitments are now visible together: Nine River's Orison/Sola/Serein allocations total **768.5 t/turn**; Red Ledger's total **174.5 t/turn**; Talweir's selected Sola/Serein rows total **150 t/turn**. Their nearby-city resident demand is separate and may include imports. Hesh's 496 resident tonnes plus its three named regional delivery destinations give 670.5 across destination accounts, not 670.5 proven Red Ledger production.

## Continuity corrections

The shared history previously described Dhal and Merrow as forming along different coasts. It now locates them within different coastal drainage systems and distinguishes inland Dhal from estuarine Merrow. Their atlas positions and rivers were not moved.

Hesh's shore historian is **Oran Keff**, avoiding an unintended identity collision with Oran in Dhal's Doss household. Dhal's 23 stated standard-age conversions were checked independently and one rounded age corrected during authoring.

Serein's Counterweight Quay and unfinished-boat yard anchors were moved closer to their authored shore, approximately 108 m and 232 m away respectively. They remain on dry ground within their district envelopes. The supported Closewater Gallery has a separate dry shore approach. These distances do not establish actual berth design or navigation.

## Actual verification

The shared settlement checker passes all six records it supports. **Hesh passes 19 negative controls; Serein and Dhal pass 15 each.** Existing Marroway/Aven pass 15 each and Sola 14. The new cases protect inherited lake geometry, spherical projection, clipped shoreline and the nearby river terminal. A wrong local water surface is rejected where supported water placement exists. District and place coordinates, population/area totals, transport connectivity and selected service arithmetic also remain checked.

Hesh's lake vertices are independently reprojected from the parent geography, then clipped against its frame; cyclic vertex order and winding do not affect comparison. A 0.02 m numerical tolerance applies to stored projection/clip coordinates, not to real shoreline accuracy. Water placement falls back to the containing water polygon's level when there is no flowing channel. River-city controls still preserve their exact atlas center height.

The production checker passes **20 negative and four positive controls**. An independent review recomputed the crop areas, irrigation, pumping, urban subset and stacked-light cases. It found two bounded edge cases: alternative reports retained hardcoded scenario labels, and excluded-food sums could overflow while individual inputs remained finite. Both are fixed and covered. Reports now use current scenario parameters; declared and derived food totals must be finite.

City authors checked their prose against local IDs, coordinates and service accounts. Cross-reviews found Hesh's food/native-biology/salt accounting and Dhal's species, food limits, sky and age relationships consistent with the current foundations. Hesh's finite residue-cell example has no assumed salt-sales outlet. These are authored process studies, not operating permits or engineering validation.

All three new SVG city plates were rendered in Edge, inspected visually and checked for twenty-four place markers, nine district envelopes, zero label-box overlap and no text outside the canvas. The productive-landscape plate passes its thirteen-allocation and canvas checks and was visually inspected. The **23-chapter reader passes at 1000 px and 390 px**, with every chapter destination, local file/image and internal anchor present, no page errors and no body overflow. Desktop Hesh and mobile productive-landscape views were inspected visually.

Repository preflight passed configuration, engine pin, script syntax and local asset-pointer checks. No Unreal build, cook or gameplay verification was required or performed for this documentation edition. Generated evidence is in ignored `Artifacts/world/`; portable sources, SVGs and tools are versioned.

## Next work

1. Author Merrow, Tern, Velis and Brin with equally specific settlement plans, architecture, people, supplies and meaningful places. Preserve their distinct watersheds, latitudes and social histories.
2. Reconcile all city-local dates into the master chronology and their named people into a deliberate identity/social register. Local city proposals are not yet a complete canonical historical integration.
3. Allocate regional populations and build secondary settlements, ports, rural communities, island/pelagic life and orbital installations. The Sola–Aven connection still needs a mapped Morrow coastal gateway and actual inland continuation.
4. Complete productive source and catchment accounts: actual working landscape allocations, remaining food categories, seasonal storage, salinity and residue destinations, materials, nutrients, power and freight. Resolve Orison's controlled-crop choices within a coherent end-use budget. Do not count a world climate simulator as a prerequisite for every bounded fictional assumption.
5. Expand the remaining ecosystems, cultural histories, faith, art, institutions and personal networks. Preserve adventure, danger, secrecy and freedom alongside ordinary pleasure and competent work.
6. Geolocate hundreds of further places beyond the 192 city dossiers. Bind travel briefs, production scenario families and pirate operations to specific terrain, people, conditions and saved aftermath; abstract patterns are not completed POIs.
7. Derive regional assets, sound, construction recipes and suitable pack requirements from the reconciled world, retaining the existing 4,200 stable briefs. Budget remains under EUR 500 initially; this edition made no purchases.
8. Complete and review the multi-volume canon, then begin engine proof and planet construction. Do not resume unrelated mechanics while the world-first phase is active.

## Reproduction

Use the configured Python runtime with `Tools/World/build_settlements.py marroway aven sola hesh serein dhal` and `Tools/World/build_production.py`; their `--check` switches verify without rewriting output. The [settlement format](Cities/SETTLEMENT_PLAN_FORMAT.md) now documents the optional lake and nearby-river provenance records. Orison and Talweir retain their specialized tools.

Compile with `Tools/World/build_reader.cjs` and bundled `marked`. Inspect with `inspect_world_plate.cjs <Edge path> <hesh|serein|dhal|production>` and `inspect_reader.cjs <Edge path>`, using bundled Playwright through `NODE_PATH`. No engine upgrade, external package installation or cloud service is necessary.
