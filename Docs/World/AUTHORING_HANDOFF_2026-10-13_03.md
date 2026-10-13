# Planet authoring handoff — edition 0.3

**13 October 2026. The full-world goal remains active.** The already-started mechanics have their Development handoff. This pass changes original world documentation, maps, data and authoring tools; it adds no Unreal levels, meshes, creatures or runtime systems. The complete canon and lore still precede planet construction.

## Current reading edition

The [living planet reader](Living_Planet_Reader.html) now contains **131,350 source words in fifteen chapters**, including research and review notes. These chapters develop parts of the fifteen planned volumes; they are not fifteen completed volumes. The [reproducible counts](AUTHORING_COUNTS.json) exclude the historical game specification and are neither finished-page counts nor quality certification. [Edition 0.2's handoff](AUTHORING_HANDOFF_2026-10-13.md) preserves the previous state.

This edition adds four substantial chapters to the existing physical, historical, social, geographic and Orison foundations:

- **Sere and Pale ecosystems:** 24 species dossiers, ECO-049 through ECO-072, with six regional food webs and four exploratory journeys. The three ecology chapters now contain 72 distinct species dossiers. The cold gardens, storm coast, fault country and polar interior use different energy sources, lifecycles and dispersal constraints. Velis exports qualified living stocks and cultivation expertise; it is not an unexplained planetary grain surplus. Dry seeds, living cultures and cryogenic holdings have different maintenance requirements. Native food fractions still require individual qualification.
- **Languages, names and belonging:** four related public varieties, a bounded shared grammar, an 80-root starter lexicon, twelve glossed sample sentences, naming practices, heritage networks and sign-language communities. Existing character and place names retain their history instead of being forcibly derived from the new vocabulary. These are authoring foundations, not complete spoken or signed languages, finished localization or recorded performances.
- **Power and exchange:** regional institutions, plural currencies, household accounts, labor, advanced industry, energy, communications, trade and illicit practices. Eighteen selected intercity dependencies connect the existing city anchors. Eight encounter seeds and five event cases show consequences at human scale; they are not additional completed POI dossiers or runtime events.
- **Talweir:** a second mapped city with nine district allocations, 780,000 residents as a planning estimate and 24 individually developed places. It has low, extended rooflines and freshwater approaches distinct from Orison's vertical crater city. The plan retains Namar at 180 m, Iren at 176 m and the existing river's westward dogleg. A flowing bypass and a separate navigation cut prevent the entire river from being treated as lock traffic. Two 1.8 m lock steps account for 3.6 m of the descent; the remaining channel grades close the four-meter relationship.

Together, the two city dossiers contain **48 detailed places and twenty district allocations**. They do not satisfy the requested hundreds of places or twelve complete major cities. Talweir's additional local dates remain explicit proposals within the established chronology; 563 AL's *Six Returns* and the 641 AL Lake Articles remain unchanged.

## Reconciliations that matter

Orison's food account now uses one **700 t per 24-hour civil turn** demand allowance for its 420,000-person core. Its six origins and five product categories describe the same mass. The urban gardens supply 24 t of the 300 t fresh-produce category, included inside the existing Saffron source row: 8%, within Orison's earlier 5–12% target. They are not extra food added to the total. The 6,900 t dry reserve covers thirty turns of full 230 t dry demand before losses and obligations. Losing only the named 170 t dry inflow produces a different upper-bound interval; neither number demonstrates sustainable regional output or nutritional adequacy.

The worked three-person household receives and spends 720 M-OR over 25 civil turns. This is an internally reconciled example, not a median income, universal price list or proven standard of living. Regional monetary issuers remain distinct. There is no universal planetary central bank, uniform law, single utility grid or fixed exchange rate.

Ordinary connected communications are fast. Geographic distance alone does not make radio take days. Relay outages, policy, verification, human response and the need to transport physical samples explain delays where they occur. Physical courier work remains useful without suppressing normal communications everywhere.

Power sources are mixed and geographically qualified. Licensed fission is an established regional option; observed geothermal water does not establish an electrical resource. Talweir's illustrative four-meter-head, 40 m³/s hydropower example yields only about 1.105 MW, far below the city's 620 MW average demand. Its longest nominal natural night is approximately 74.2 hours. Storage and supply remain design obligations rather than unaccounted free power.

Talweir's raw, potable, reclaimed and final residual flows are explicitly distinguished. The Iren reserve intake receives no credited yield before downstream-plume and treatment assessment. The mapped outfall enters the flowing bypass. Supported overwater places carry access requirements; terrestrial transit and inhabited district relationships were checked independently. These corrections improve the authored proposal without proving an engineering design.

## Verification and limits

The existing atlas, Orison city and calendar checks were rerun. They passed, including thirteen embedded Orison negative controls and 37,558 calendar/idealized-sky assertions. Repository preflight passed configuration, engine-pin, script and local asset-pointer checks. No Unreal build or play test was required or claimed because executable content did not change.

Independent physical review checked Talweir's lake projections against the source geography, nonrising channel levels, lock arithmetic, population and area totals, placement, transport connections, pumped elevation changes, and housing, food, water and energy arithmetic. Its findings were reconciled in the authored files. The review establishes consistency under the stated assumptions, not hydraulic reliability, flood protection, agricultural capacity or a playable city.

The Talweir checker passed its baseline, 24 negative controls, a redundant-route positive control and cardinal/dateline projection controls. The 1560 × 1560 city plate was rendered in Edge and visually inspected: all 24 place markers and nine district envelopes are present, with no label-box overlaps or text outside the canvas. The whole-lake inset and local channel plan show different scales explicitly.

The exchange checker passed twenty negative controls and three positive controls. Review corrected rendering that had depended on row order, a zero-deficit division case, an urban-producer mismatch and nonfinite household totals. Source IDs and category keys now determine labels and values together; reordered input produces identical artifacts, while valid revised household and food assumptions propagate into their summaries. These checks protect the authored accounts against misleading edits; they do not prove a functioning economy.

The Orison food plate was rendered in Edge and visually inspected. Both panels identify one common total, all eleven bars are present, and no text extends outside the canvas. The compiled reader passed at 1000 px and 390 px: all fifteen chapters, local links, images and contents anchors resolved, navigation to every chapter worked, and no page errors or body overflow were reported. The narrow contents and desktop Talweir screenshots were also visually inspected. Separate records are under `Artifacts/world/`; see the generated plan and exchange registers for their exact check scope.

Local rendered PNGs and inspection JSON stay in ignored `Artifacts/world/`. Portable SVGs, authoring JSON, source Markdown and generators are versioned. External scientific sources support stated principles; the planet, institutions, species and language forms remain original fiction.

## Next work, in dependency order

1. Extend the physical atlas below coarse controls: catchment boundaries, seasonal water budgets, mineral and soil resources, ocean circulation and passages, regional weather transitions and qualified energy sites. Distinguish a planning assumption from a modeled or sourced constraint.
2. Deepen the other ten major cities and their rural/secondary settlement networks. Give each a distinct reason to exist, architecture, services, institutions, approaches and relationships. Existing Orison and Talweir still need finer street, property, family and service detail.
3. Extend island, open-ocean, deep-water and remaining terrestrial ecosystems. Connect migrations, parasites, disease, decomposition, domesticated life, introduced crops and qualified native processing to actual regional ranges and productive conditions.
4. Develop cultural differences within and between settlements: faith, philosophy, literature, humor, education, kinship, domestic design, performance, heritage and signed languages. The starter lexicon is not a substitute for these lives.
5. Build checked route experiences, many more individually authored POIs, named social graphs and dynamic event chains. The existing 48 city places, twenty earlier social/leisure seeds and eight exchange seeds are different forms of material; do not combine them into a misleading completed-POI count.
6. Derive region-to-asset-family mappings and construction briefs from settled geography and material culture. Preserve the existing 4,200 stable asset-brief IDs and their production progress. Shared manufacture, local adaptations and truly distinct new requirements should be explicit. The initial budget remains under EUR 500; no new purchase was made in this pass.
7. Compile and cross-review the complete multi-volume bible and atlas before planet construction. Then prove the required Unreal capabilities and construct reviewable portions from the canon. A map or large prose collection does not establish a living, traversable planet.

## Reproduction

Use the configured Python runtime for `Tools/World/build_atlas.py --check`, `build_city_plan.py --check`, `check_calendar.py`, `build_exchange.py` and `build_talweir.py`. The exchange and Talweir tools generate their portable plates/registers unless invoked with `--check`. The geography builder from edition 0.2 regenerates its own relief plate/register when needed; its source controls were unchanged this pass.

`Tools/World/build_reader.cjs` uses bundled `marked` 17.0.5. Browser inspection uses bundled Playwright with the Edge executable path; pass `food` or `talweir` as the second argument to `inspect_world_plate.cjs`. `inspect_reader.cjs` checks both desktop and narrow layouts and navigation to every compiled chapter. Supply the bundled module directory through `NODE_PATH`. No package download or engine upgrade is needed.
