# City and region accounts — what each promise actually supplies

**Edition 0.7 · present 742 AL · 13 October 2026.** This is an accounting reconciliation of the twelve city dossiers, the [exchange ledger](Exchange/nacre_exchange.json) and the [productive-landscape study](Resources/nacre_production.json). It preserves their authored allocations. It establishes neither surveyed productive capacity nor signed contracts. Its machine-readable companion is [nacre_city_region_accounts.json](Resources/nacre_city_region_accounts.json); the read-only arithmetic and relationship checker is [check_city_region_accounts.py](../../Tools/World/check_city_region_accounts.py).

The twelve cores contain **4.88 million habitual residents**, about **14.02% of the planet's 34.8 million surface and marine residents**. Their food allowance totals **8,062.5 tonnes of edible product per civil turn**. Their municipal delivered-water cases total **960,000 m³ per turn**, and their stated mean electrical service demands total **4,475 MW**. These are useful scales, with different boundaries. None is the total need of Nacre. Regional towns, farming settlements, mobile communities, extraction sites and much transport remain outside these city service accounts.

Of the core food total, **2,542 t/turn has a complete mass-by-source allocation** in existing documents. Another **5,520.5 t/turn** has no complete comparable product-mass origin table: some dossiers supply calorie shares, some describe regional networks without assigning quantities. The earlier agricultural study sizes **620 t/turn of existing Orison food**, not 620 tonnes of new surplus. Keeping these boundaries visible lets the world grow without quietly selling the same harvest to several cities.

## 1. A meal, a cargo movement and a harvest are different entries

An Orison household eating food from Nine River creates an Orison consumption requirement. The region supplying it has a corresponding delivery obligation. Passing through a Marroway store creates handling work. Those are three views of one flow. Adding all three to planetary food consumption would count the same product repeatedly; ignoring the latter two would erase the agricultural landscape and the transport system that make the meal possible.

The ledger therefore keeps recipient demand and origin commitments in separate tables. A city can receive its whole basket without producing its whole basket. A producing region can meet a city's order while still needing to supply other residents. A trading network named after a city is not automatically a farm in that city, and a region named on a delivery is not proof of available surplus. Every capacity field remains unverified or null. **Null means unestablished**, not zero production and not unlimited supply.

All flow rates here use a **24-hour civil turn**. A natural day lasts about 136.755 hours; food and water rates must not be multiplied by that number because a writer called a turn a day. Annual comparisons use the existing production model's mean calendar year of approximately 325.191304 turns. At those assumptions, the twelve core food allowances amount to about **2.622 million tonnes per mean civil year**. This is edible product, including its water content, rather than dry primary biomass or agricultural gross harvest.

The common food categories are dry product, fresh product, cultured product, individually qualified native fractions, and other preserved product. The labels reconcile vocabulary across dossiers; they do not establish identical recipes or nutritional values. A tonne of fresh food is not interchangeable with a tonne of dry grain. A cultured food's wet mass is not its protein content. Feedstock consumed in making cultured food must appear upstream; it is not another human meal to add downstream.

Temporary occupants remain people with usual homes for the population census. Their destination services still need an occupancy allowance. The home-city baseline may be an annualized resident screen that has not deducted their absence, so adding every visitor case to every home population is not a precise planetwide dietary total. Departing ship stores are a separate stock movement whose eventual consumers must also have a location and time boundary. Nothing in this chapter allocates the remaining regional population or adds the new settlement anchors to it.

## 2. The twelve baseline accounts

Water input means treated water entering municipal distribution, after source treatment and before distribution loss. Talweir has not separated that boundary: its source account gives combined raw-to-delivery losses. The dash below is deliberate. Power means end-use/service demand at each dossier's stated boundary, not installed generation. The peak column is a local study case, not a forecast that all peaks occur together.

| Core and source | Residents | Edible product t/turn | Delivered water m³/turn | Treated input m³/turn | Mean / peak electrical MW |
|---|---:|---:|---:|---:|---:|
| [Orison](Cities/ORISON.md), REG-001 | 420,000 | 700 | 60,000 | 66,666.667 | 600 / 1,050 |
| [Marroway](Cities/MARROWAY.md), REG-002 | 670,000 | 1,139 | 140,000 | 175,000 | 460 / 840 |
| [Hesh](Cities/HESH.md), REG-004 | 310,000 | 496 | 58,000 | 68,235.294 | 215 / 430 |
| [Serein](Cities/SEREIN.md), REG-006 | 530,000 | 874.5 | 100,000 | 125,000 | 450 / 820 |
| [Talweir](Cities/TALWEIR.md), REG-009 | 780,000 | 1,248 | 180,000 | Unsplit | 620 / 1,100 |
| [Aven](Cities/AVEN.md), REG-010 | 450,000 | 742.5 | 90,000 | 105,882.353 | 420 / 740 |
| [Dhal](Cities/DHAL.md), REG-011 | 160,000 | 264 | 30,000 | 34,090.909 | 150 / 270 |
| [Merrow](Cities/MERROW.md), REG-012 | 610,000 | 1,037 | 115,000 | 130,681.818 | 410 / 760 |
| [Tern](Cities/TERN.md), REG-015 | 360,000 | 594 | 72,000 | 83,720.930 | 440 / 790 |
| [Velis](Cities/VELIS.md), REG-016 | 120,000 | 192 | 30,000 | 37,500 | 110 / 220 |
| [Brin](Cities/BRIN.md), REG-018 | 180,000 | 297 | 33,000 | 37,500 | 240 / 440 |
| [Sola](Cities/SOLA.md), REG-021 | 290,000 | 478.5 | 52,000 | 65,000 | 360 / 620 |

Orison's 700-tonne central case is the exchange ledger's adopted point within its older 600–800-tonne range. The range is not an additional food customer. Its 1.2-million-person functional service region includes the 420,000 core; it is not another population to add to the 4.88 million. Regional allocation and nonresident demand require their own rows.

The eleven stated treated-input cases sum to **929,277.971 m³/turn**, excluding Talweir. Talweir's 225,000 m³ raw abstraction delivers 180,000 under a combined 20% loss assumption. Under that simple loss-only boundary, its intermediate treated input lies between those two quantities, depending on how the losses are divided. The twelve-city treated-input sum is therefore not one established number. Its corresponding accounting interval is approximately **1,109,278–1,154,278 m³/turn** until the stages are separated; this interval is not a source-yield estimate.

The sum of local peak cases is **8,080 MW**, which is an arithmetic sum only. Timing, local storage, transmission constraints, industrial schedules and correlated weather determine an actual interconnected-system peak. Likewise, 4,475 MW of means implies about **107,400 MWh per turn**, or **34.926 TWh per mean civil year**, at the current mixed city-service boundaries. It does not include a secretly complete regional agricultural, industrial or propulsion system.

## 3. Water: preserve every boundary before drawing another reservoir

The first necessary distinction is between fresh source water and treated water. The second is between treated water and delivered water. The third is between new supply and the internal reuse loop. Municipal delivery is also distinct from irrigation over the regional production landscape. A city cannot claim its own taps are supplied because a neighboring river looks large, and a farm cannot withdraw the same municipal allocation again merely because it lies outside the city map.

The following figures are the dossiers' illustrative source-process assumptions, rounded for reading. They are not licensed withdrawals or measured seasonal yields. Source residual includes the modeled water difference at treatment; its chemistry and destination still need design.

| City | Source-stage quantities per turn | Boundary still needing closure |
|---|---|---|
| Orison | About 66,667 m³ treated network input; raw abstraction and recovery unsplit | Source-specific raw recovery, usable storage classes and actual sewer return flows |
| Marroway | 184,211 m³ river raw at 95% recovery → 175,000 product; 9,211 process residual | Seasonal flow, abstraction rights, intake/outfall influence and treatment peaks |
| Hesh | 54,737 upland raw → 52,000 product; 23,193 brackish-lake raw → 16,235 product | Separate fresh/brackish source limits, concentrate salt inventory and disposal |
| Serein | 100,000 fresh raw → 95,000 product; 66,667 seawater → 30,000 product | Catchment/groundwater/roof independence, coastal intake and outfall, storage operations |
| Talweir | 225,000 raw → 180,000 delivered under combined losses | Treated input and separate treatment/distribution losses; no credited Iren reserve yield |
| Aven | 110,294 upland raw → 105,882 product at 96% recovery | Catchment seasonality, storage and pumped pressure service at the upland city |
| Dhal | 35,885 river raw → 34,091 product at 95% recovery | Source chemistry, flood operations, plume separation and catchment limits |
| Merrow | 136,127 river raw → 130,682 product at 96% recovery | Salinity influence, actual intake/outfall conditions and regional competition |
| Tern | 61,047 fresh raw → 58,605 product; 55,814 seawater → 25,116 product | Regional freshwater location and sustainable yield; separate desalination ecology |
| Velis | 39,474 combined river/ground/roof raw → 37,500 product under a shared 95% screen | Whether groundwater and river supply overlap; distinct actual process recoveries |
| Brin | 31,579 fresh raw → 30,000 product; 16,667 seawater → 7,500 product | Seasonal fresh source, winter operating conditions and coastal discharge |
| Sola | 84,444 seawater → 38,000 product; additional 20,000 runoff and 7,000 groundwater product | Raw recoveries for the latter two; island-specific freshwater yield and storage |

Sola's 65,000 m³ input is emphatically not 65,000 m³ of seawater. Its desalination case alone takes about 84,444 m³ raw, while two other sources supply treated product. Hesh's combined raw flow is similarly not all freshwater: the lake stream has a separate dissolved-load problem. Tern and Brin must retain both their freshwater and seawater infrastructure. A drawing of one attractive municipal works cannot satisfy both source paths without showing their actual treatment and connection arrangements.

The reuse targets sum to **256,000 m³/turn** across the twelve cities. Adding reuse to potable delivery gives **1,216,000 m³/turn of gross end-use throughput** under the adopted municipal screens. It does not create 256,000 m³ of extra external source. The same water can have several uses before final discharge or consumption. Basin depletion also differs from network loss: some leaked or uncollected water returns to a catchment, with different quality and timing.

For example, Brin delivers 33,000 potable plus 9,000 reclaimed. Its collection screen receives 29,000; 9,000 returns to reuse and 20,000 remains for final management. The 13,000 not collected is consumption, evaporation or other uncollected flow. The distribution boundary closes as **37,500 input = 4,500 network loss + 13,000 uncollected + 20,000 final residual**. Treatment residual from the freshwater and seawater works is upstream and additional. This example describes a book balance, not harmless discharge or a proven operating plant.

Orison's 70,000 m³ wastewater figure is **design capacity**, not an observed collection rate. This ledger leaves its collection and final-residual quantities null. Dividing or subtracting that design capacity as though it were actual sewage would manufacture a return balance. Talweir's 155,000 collection and 50,000 reuse have explicit flow meanings and can be reconciled; its 190,000 nominal sewage-treatment capacity remains a separate rating.

Storage also requires the right denominator. Serein's 300,000 m³ treated target represents **2.4 ordinary input turns at 125,000**, not three loss-bearing turns. The source dossier has been clarified accordingly. Sola's 156,000 m³ gives 2.4 at 65,000; Velis's 90,000 gives 2.4 at 37,500. Orison's older forty-turn illustration is a delivered-load equivalent before losses and unavailable volume. If its entire 2.4 million m³ were usable treated network supply, the corresponding loss-bearing input duration would be **36 turns**. The document describes treated or protected reserves of several classes, so that conditional arithmetic must not silently certify all storage as interchangeable potable water.

These distinctions affect architecture and exploration. Raw reservoirs need catchments and sediment access. Treated stores need protected maintenance. Reuse pipes need distinguishable connections. A wastewater rating needs receiving works, not a picturesque drain into the nearest native habitat. They also allow ordinary competence: a city can maintain a comfortable public bath because its water and heat have specific budgets, while an isolated pressure-zone outage remains a bounded local problem.

## 4. Food commitments already made

Twenty-six canonical procurement rows supply the five cities with complete origin-by-product tables: Orison, Serein, Sola, Brin and Velis. They total the **2,542 t/turn already inside those recipients' core demand**. The JSON preserves every source row, category and source location. Five duplicated JSON mentions of those same commitments are retained as aliases with zero additional demand. The eighteen qualitative exchange dependencies remain relationships with unassigned tonnage.

### Nine River, Marroway and three distant plates

Nine River's named external allocations are:

| Recipient | Dry | Fresh | Culture | Conditional native | Other | Total t/turn |
|---|---:|---:|---:|---:|---:|---:|
| Orison, existing SUP-002 | 170 | 145 | 35 | 0 | 10 | 360 |
| Serein source table | 110 | 60 | 35 | 0 | 15 | 220 |
| Sola source table | 90 | 60 | 20 | 8.5 | 10 | 188.5 |
| **Named outward commitments** | **370** | **265** | **90** | **8.5** | **35** | **768.5** |

The Marroway dossier repeats the Orison row to explain consolidation. That repetition does not add another 360 tonnes. EX-001's Marroway-to-Orison relationship also adds no tonnage. The regional obligation is already represented by the destination allocation.

Marroway's **1,139 resident tonnes** are additional consumers, but its own origin split remains unspecified. Its text names Nine-River production supplemented by wider Kesh trade. Thus **1,907.5 t/turn** is a combined demand envelope consisting of Marroway residents plus those named outward commitments. It is **not** a claim that Nine River grows all 1,907.5 tonnes. Its exact regional share depends on Marroway's eventual import allocation; other regional residents and customers add further obligations. Nor must all outward freight pass one Marroway quay: neighboring ports and regional collection routes are part of the existing description.

Sola's 8.5-tonne native fraction remains conditional upon a qualified Kesh processing chain. It is not proof that a Nine River wetland organism is safely edible. If that line cannot be qualified, Sola's source dossier calls for a compatible introduced replacement and a revised basket. That revision would preserve recipient food mass while changing product chemistry, crop/feedstock needs, supplier commitments and potentially energy. It cannot be treated as a free substitution in an otherwise unchanged farm model.

### Red Ledger and Hesh

| Recipient | Dry | Fresh | Culture | Other | Total t/turn |
|---|---:|---:|---:|---:|---:|
| Orison, SUP-004 | 15 | 25 | 15 | 10 | 65 |
| Sola | 15 | 0 | 10 | 0 | 25 |
| Serein | 60 | 10 | 10 | 4.5 | 84.5 |
| **Named regional deliveries** | **90** | **35** | **35** | **14.5** | **174.5** |

The first two entries appear in Hesh's JSON export notes; Serein's later 84.5-tonne line appears in the city prose and its own recipient table. This ledger includes all three once. Hesh's own **496 tonnes** remain a different recipient basket. The resulting **670.5 tonnes of named destination demand** agrees with Hesh's prose, while leaving the city's imports unresolved. It must not be renamed total Red Ledger harvest.

This distinction is especially consequential in a closed basin. Additional product can require additional crop water, culture feedstock and managed residuals. Hesh's municipal raw water does not include all water used to grow exports. Expanding cultivated area may also increase leaching and concentrate-management obligations. A splendid new greenhouse is therefore a demand proposal requiring a source and disposal account, not a technical trick that makes dry geography irrelevant.

### Serein, Talweir and Brin's network labels

Serein allocates **35 t** to facilities within the city and **410 t** to wider REG-006 coastal production. Those **445 t** have an explicit regional location. It imports the remaining 429.5 through North Gate, Nine River, Red Ledger and the Talweir regional culture line. Brin additionally requests **90 t from a Serein-serving coastal supply network**. That language identifies a procurement network, not necessarily 90 tonnes grown exclusively in REG-006 or consumed by Serein. The ledger keeps it separate until producer locations are assigned. A combined 535-tonne network obligation is useful for arranging suppliers; only 445 currently has that explicit REG-006 location.

Talweir's regional network is explicitly allocated **125 t to Sola** and **25 t of stabilized culture product to Serein**: **150 t** with stated REG-009 provenance. Brin assigns another **45 t to Talweir-serving production and processors**, consisting of 25 dry, 15 culture and 5 other. Across the named network the three recipient obligations total **195 t**, but the final 45 retains its broader network qualifier. Talweir's 1,248-tonne own diet is separate and currently apportioned by calories, not food mass.

Brin's whole 297-tonne basket remains intact: 75 local/regional, 90 Serein-serving, 60 northern Kesh network, 45 Talweir-serving, and 27 other Pale settlements. Its northern Kesh row cannot be arbitrarily assigned to Nine River or North Gate just to make a regional table complete. Its other-Pale row cannot be assigned entirely to one unbuilt town. Producers, stores and routes should be selected together in the next pass. Stable dry and culture cargo creates different travel demands from fresh produce, and a northern route must account for workable seasonal access and storage.

### Saffron, Aster, Velis and the distant pool

Orison's existing REG-001 rows total **188 t**: 140 from Saffron's urban/rural belt and 48 from qualified Saffron/coastal processing. The **24-tonne urban fresh target is inside the first row's 90 tonnes of fresh product**, and inside the city's 700 total. It is not a seventh source. Glassback's 75-tonne row remains separate. The final **12 tonnes in SUP-006** combines REG-011, REG-012 and REG-021: 5 conditional native and 7 other preserved product. It remains one unsplit pool, not twelve tonnes each from Dhal, Merrow and Sola.

Sola's own local commitments are **95 t from controlled production within the city** plus **45 t from Aster growers outside the city frame**. Together these 140 tonnes comprise 10 dry, 75 fresh, 40 culture and 15 other. They do not establish extra capacity to cover Sola's unassigned portion of SUP-006, visiting crews or ship stores. Some material in distant specialties may also pass through Sola without having been grown there; processing and carriage need separate records when the pool is resolved.

Velis allocates **52 t within the city**, **110 t from its wider catchment/hinterland**, **24 t from other Sere networks**, and **6 t of selected cross-sea ingredients**. Its 162 locally located tonnes are a food target, not evidence of unused regional surplus. Its separate viable-stock export case is **0.8 t net biological material plus 1.2 t containment and conditioning**, about 2 t gross dispatched. Those tiny valuable shipments can enable future harvest elsewhere; they are not additional edible bulk or immediate future output. Seed, clonal material and partner cultures have distinct qualification and recovery needs.

## 5. What calorie shares do — and do not — tell us

Four cities describe regional origins as shares of annual calories. Their product-mass baskets cannot be split using the same percentages without choosing the foods. The following are average calorie-equivalent allocations at each dossier's stated dietary screen, not new food tonnes:

| City | Total million kcal/turn | Existing origin shares expressed as million kcal/turn |
|---|---:|---|
| Talweir | 1,950 | Lake Country regional 1,462.5; other connected Morrow 429; city primary production 58.5 |
| Aven | 1,080 | Wider Aven basins 669.6; other connected Morrow 388.8; core primary production 21.6 |
| Dhal | 384 | Regional agricultural/culture belt 261.12; other Morrow 111.36; urban growing/culture 11.52 |
| Tern | 864 | Nearby Sere 190.08; other Sere 241.92; other continents 414.72; urban 17.28 |

Talweir's 75% regional calorie share does **not** establish 936 tonnes of Lake Country product. It might carry much of its energy as relatively dry staples while other sources provide a higher mass of fresh food. The conversion requires product water content and usable energy, not another percentage copied into a spreadsheet. Culture feedstocks must also be reconciled so crop energy consumed upstream is not counted again as additional human dietary energy downstream.

Marroway, Hesh and Merrow provide product-category baskets and calorie/protein screens but no complete mass-origin table. Across the seven cities with explicit calorie targets, the target subtotal is **8.253 billion kcal per turn**. The other five cities' absence of a calorie target is a gap, not a zero-calorie diet. Their total product mass cannot fill it automatically.

Reserve definitions differ as well. Orison's 6,900 tonnes covers thirty turns of its actual 230-tonne dry-product row. Brin's 9,450 covers ninety turns of its 105-tonne dry row. Sola, Serein and Velis similarly use dry-category stores. Other cities express reserves as mixed dry calorie equivalents, often at 3,200 kcal/kg. A calorie-equivalent reserve is not necessarily a physical store of precisely that mass and composition. Summing all the reserves into a universal emergency-food duration would conceal both definition differences and the need to supply fat, protein, micronutrients, preparation energy and particular diets.

The next closure should choose a manageable number of food families with wet mass, edible energy, protein, storage behavior and processing yield. Each city can then keep its culinary identity while procuring intelligible quantities. Pepperprint in Hesh, a Talweir communal meal and a Sola guesthouse breakfast need not become the same universal ration. The model should support pleasure and local skill: one grain can need a different soaking schedule without becoming nutritionally useless, and a popular fresh ingredient can be seasonal without making the city fail.

## 6. Guests and ship stores already outside the core food rows

| Place and case | Temporary people | Extra food t/turn | Guest domestic water | Separate departing ship stores t/turn |
|---|---:|---:|---|---:|
| Serein ordinary | 9,600 | 15.84 | 1,200 m³ already inside baseline other uses | 20 |
| Velis ordinary | 1,500 | 2.4 | 202.5 m³ already inside accommodation allowance | 4 |
| Sola ordinary | 18,000 | 29.7 | 2,160 m³ already inside baseline other uses | 35 |
| Sola event ceiling, replacing ordinary occupancy | 36,000 | 59.4 | 4,320 m³ domestic component, replacing 2,160 | Same separate case, unless revised |
| Brin seasonal peak | 12,000 | 19.8 | 1,500 m³ additional domestic delivery | 12 |

Sola's event case is 36,000 guests **instead of**, not alongside, the ordinary 18,000. Holding other uses fixed, it adds 2,160 m³ to the existing water case, producing 54,160 m³ delivered and **67,700 m³ treated input** at the same 20% distribution loss. Public services and unusually large crews may add further demand. The corresponding guest-food increase above the ordinary case is 29.7 tonnes, not 59.4 additional to the already counted ordinary guests.

Brin's peak domestic addition would raise its 33,000 delivered to 34,500. At 12% loss that needs **39,204.545 m³ input**, an increase of 1,704.545. This does not automatically establish enough treatment or accommodation, nor does it include extra public/commercial water. Those are operating cases to build against the already defined local infrastructure.

The four explicit ship-store lines total **71 t/turn**. They need origins and ownership separate from the core tables. The three ordinary guest-food lines total 47.94 tonnes. They are destination facility allowances, not proof of 47.94 tonnes of additional planetary eating after their home populations have already been counted. Brin's seasonal maximum should not be added to every ordinary case and then advertised as a normal day. The remaining cities still have unquantified guest/crew obligations; a null allowance is not a prohibition on visitors and not unlimited free provisioning.

Physical receiving and retail operations must distinguish resident purchases, guesthouse supplies, ship stores, and cargo merely transferred through a port. Sola's large unique-inbound handling range is not its own dinner. Merrow and Serein likewise distinguish distinct consignments from repeated handling. Orison and Talweir's older gross-freight ranges use broader working descriptions and should receive aligned definitions before a planetwide tonnage total is attempted. No such total is claimed here.

## 7. Productive landscape coverage and the lighting decision

The existing [productive-landscape volume](06b_PRODUCTIVE_LANDSCAPES.md) covers Orison's **230 dry + 300 fresh + 90 cultured = 620 t/turn**. It leaves 30 native and 50 other product tonnes outside its crop model. The same 620 is already present in the city's 700-tonne basket. Applying that study to another recipient requires another allocation, actual geography and appropriately reviewed assumptions; it does not reveal hidden excess capacity.

The study's crop and substrate cases require about **24,402 hectares of productive canopy** and **34,721 hectares of gross farm land plus unstacked urban canopy** under its area convention. Its source irrigation screen is about **147,149 m³/turn**. These are existing hypothetical sizing results for selected Orison flows, not a complete Nine River irrigation budget. The municipal water total in this chapter excludes most regional agricultural abstraction, while a particular urban growing allocation may overlap a city's process-water allowance. They cannot simply be added without tracing the actual connection.

Whole-canopy artificial supplementation under that study would imply roughly **6,778 MW mean lamp power**, before thermal and other services. This counterfactual exceeds the combined 4,475 MW city-service mean. The point is not that the setting must abandon large farms. Its selected introduced staple lines are explicitly fictional products of long adaptation and breeding for Nacre's natural cycle. Many must exploit natural light. Sensitive stocks and selected crops can have controlled schedules, but every such choice has to identify its canopy, useful photons and electrical boundary. Universal night lighting cannot appear in environmental art without appearing in the energy account.

For a transparent comparison, retain the existing generic fresh-crop assumptions: 30 gross tonnes per canopy hectare per mean civil year, 80% retention, 250 µmol/m²/s canopy light target, effective canopy efficacy 3 µmol/J, sixteen scheduled light hours per civil turn, and half those hours requiring electric supplementation in the daylit case. These are fictional study assumptions, not measured yields. Four-layer cultivation gives three lower layers full electric light and the top layer the stated supplement; stacking reduces footprint while raising mean lamp load per unit total output.

| Existing fresh target used in comparison | Product t/turn | Canopy ha under shared crop case | Daylit supplement mean lamp MW | Four-layer mean lamp MW |
|---|---:|---:|---:|---:|
| Orison nested urban target | 24 | 325.19 | 90.33 | 158.08 |
| Serein within-city fresh row | 30 | 406.49 | 112.91 | 197.60 |
| Sola within-city fresh row | 60 | 812.98 | 225.83 | 395.20 |
| Brin local/regional fresh row | 55 | 745.23 | 207.01 | 362.26 |

**These are comparisons, not adopted loads or newly granted capacity.** They exclude cooling, dehumidification, pumping, processing, maintenance, storage and distribution losses. Crop type, natural-light use, seasonal timing and actual productivity can change the result. A writer cannot invoke that possibility as proof of whichever smaller electrical number would be convenient.

Serein currently allocates 40 MW mean to controlled agriculture and cultures together. Its full 30-tonne city fresh target cannot all use this shared lighting case within that allowance, even before the other services. The concrete choices are to reduce the explicitly lit fraction, specify substantial natural-cycle production with storage/import cover, develop a different crop-specific throughput model, or revise and supply the electrical budget. The present ledger chooses none silently. At the shared daylit case's roughly 3.764 MW per daily tonne, 40 MW would cover only about **10.63 t/turn of lamps-only output**. The practical limit would be lower when the same allowance also serves cultures and climate control.

Sola's local 95-tonne product target includes 60 fresh, 30 culture and 5 other. The fresh comparison alone demands 225.83 MW mean daylit supplemental lamps, or 395.20 MW in the four-layer case. The latter already exceeds the city's entire stated 360 MW mean. Sola needs a specific cultivation program and load placement rather than a claim that all attractive glass rooms together fulfill the row. Moving some production outside the city would change a procurement allocation and its region's requirements; it cannot be treated as an invisible geometry edit.

Brin's 55 tonnes is explicitly **local and regional**, not a statement that it all grows in the city's lit rooms. Its 20 MW cultivation allowance cannot validate the entire row by itself. Under the shared daylit comparison, that power buys at most about **5.31 t/turn of lamp-only output**, before services. A credible next pass can partition the 55 among suitable naturally lit seasonal production, stored suitable crops, qualified controlled growing and other sources. If imports increase, revise the 75-tonne local/regional row and corresponding external commitments visibly.

Velis already has a different explicit screen: **180,000 m² of active lit canopy**, 150 W/m² when on, sixteen hours per turn, yielding **18 MW mean and 27 MW instantaneous lamps** within its 30 MW cultivation/stock allowance. The associated 8-tonne fresh-output target implies about **144.53 retained t/ha per mean civil year** if supplied continuously from those 18 hectares. That is about **6.02 times** the generic study's 24 retained t/ha/year. This is not automatically a contradiction: crop families and cultivation systems can differ. It is a precise throughput claim needing crop cycles, harvest mass, retained fractions, downtime and actual canopy occupancy. If the generic retained yield were used instead, the same 18 hectares would supply only about **1.33 t/turn**. The current document does not declare either case achieved.

The strongest next decision is therefore a **small set of explicit cultivation archetypes**, with one testable budget each: naturally lit long-cycle staples; daylit fresh crops with bounded supplementation; fully lit high-value fresh crops; propagation and stock recovery; and cultured foods with real substrate conversion. Each has its own environmental architecture. The result can be visually abundant while remaining materially legible: open growing mosaics, screened nurseries, dense clean culture rooms, storage yards and seasonal covered courts, rather than one repeated glowing greenhouse representing every process.

## 8. Electrical origin allocations and what still supplies them

Some cities give average source portfolios; others only candidate connection and plant ratings. They must not be mixed into a claimed generation balance. The existing average allocations with explicit amounts are:

| City | Existing mean-source allocation MW | Limitation |
|---|---|---|
| Orison | Regional 360; local dispatchable 120; variable annual mean 120 | Source and fuel obligations, gross losses and actual hourly availability remain unclosed |
| Serein | Regional 225; local dispatchable 160; wind 45; solar 20 | The sum meets the mean only; imports can share weather/failure dependencies |
| Sola | Regional 170; local dispatchable 100; wind 55; solar 35 | No selected island reactor, fuel contract or inexhaustible cable |
| Brin | Dispatchable 190; wind 35; solar 15 | Explicit net contributions; gross auxiliaries, network and storage losses additional |
| Velis | Imports 40; local dispatchable 45; wind 20; solar 5 | Explicit net contributions; recovery heat still depends on the operating source |

For Marroway, Hesh, Talweir, Aven, Dhal, Merrow and Tern, candidate import connections, dispatchable nameplates, solar/wind ratings and storage cases do not by themselves establish corresponding average energy deliveries. Their mean demand remains a real planning obligation, but its complete source allocation is unfinished. A connection's rating does not say which regional plant produced the energy, what else that plant serves, or whether the connection is available during the local worst case.

Power exported by a regional producer is not a second end use to add to the receiving city. It is a matching source obligation, enlarged by actual transmission and conversion losses where applicable. Generation can be located outside the city's map, but then needs land, access, cooling, fuel or renewable resource, maintenance and network links there. Moving a symbol beyond the frame does not complete those dependencies.

The next energy pass should compare source systems against the same explicit load chronology. One option is dispatchable regional production with qualified fuel and local reserve. Another is more distributed dispatchable service with useful heat recovery. Another combines larger renewable generation with actual storage, interconnection and flexible demand. Licensed fission exists as a world option; that does not select a reactor design or erase its material supply chain. Observed thermal features do not establish a geothermal plant. Neither sunlight leases nor the Sunseed supply limitless electrical energy.

The calculation needs ordinary turns, long natural darkness, seasonal heating/cooling, planned maintenance and at least one correlated disruption. City deficits already reveal why stored MWh alone are insufficient: a store may have enough energy and inadequate MW discharge. This chapter neither repairs those shortfalls by inventing nameplate capacity nor portrays the cities as permanently failing. Their present fiction is competent operation; the remaining authoring obligation is to specify the sources and operating practices that earn it.

## 9. Closure choices that preserve scale and make geography useful

**First, finish recipient baskets before expanding source capacity.** Retain the 8,062.5-tonne core product screen while choosing complete nutritional compositions. Give the five presently unquantified calorie screens actual menu assumptions. Resolve Talweir, Aven, Dhal and Tern's mass-by-origin tables from foods, not calorie percentages. Give Marroway, Hesh and Merrow explicit local/import shares. Identify ordinary guest and ship provisioning suppliers. These choices determine the agricultural obligation without assigning the remaining planetary population here.

**Second, map producer cohorts to the commitments already made.** Nine River must show who covers its 768.5 named outward tonnes before promising extra surplus. Red Ledger must separately cover its 174.5 outward tonnes and its share of Hesh residents without borrowing municipal water. Talweir's processors must explain the 150 explicitly regional tonnes, with Brin's 45 placed in actual source geography when known. Serein-serving and northern Kesh network labels should remain broad until their producers are selected. An origin cohort may contain many farms and communities; it need not become a single megafactory or an owned resource of the destination city.

**Third, assign landscape and seasonal calendars to those cohorts.** The Nine River lowlands, upland Aven basins, western coastal growing, closed Red Ledger basin, Sere managed lands and Brin's northern production cannot all use the same rainfall, crop or light schedule. Use the existing regional climate hypotheses and terrain, then state the actual farming assumptions that need testing. Route distance matters most for the relevant product: fresh produce has a different storage and timing problem from dry staples or stabilized cultures. The current atlas gives locations, not navigable canals or unlimited cheap daily airlift.

**Fourth, complete source-water accounts at the farm and city boundaries.** Assign actual catchments and storage paths, record competing demands, and distinguish total withdrawal from net depletion. Split Talweir's intermediate treatment boundary. Split Sola runoff and groundwater raw recovery. Resolve Orison's source and sewer flows. Keep Brin and Tern's freshwater and seawater portfolios distinct. Demonstration fog capture remains supplementary where the Serein dossier gives it no firm bulk credit.

**Fifth, choose the controlled-growing fraction and fund its whole service load.** Use the comparison table to make a deliberate decision. Reducing lit output is a production choice, not failure of the world's ambition. Expanding it is also legitimate if the source plants, fuel, grid, heat rejection, capital and operating staff are authored. A world with hundreds of detailed settlements gains depth from different solutions rather than every municipality owning an unexplained vertical farm of equal productivity.

**Sixth, write a bounded industrial material ledger.** Existing city specialisms identify useful work but rarely quantify it. Prioritize agricultural nutrients and media, culture substrate, membranes and treatment chemicals, electrical fuel and replacement parts, structural metal, ceramics, insulation and critical electronics. For each, distinguish recovered stock from new extraction and imported specialist material. Repair reduces replacement demand; it does not create all seals, coatings, alloying constituents and electronics indefinitely. Give rejected material and contaminated streams destinations. No mining or fuel output is assigned by this chapter.

**Seventh, connect stores and journeys to actual public life.** Allocate reserve ownership and accessible release conditions before calling stock a civic safety net. Make the feeder road, drying hall, receiving yard, nursery, kitchen and ordinary shop parts of the same chain. Allow useful redundancy and beautiful places that operate well. Travel becomes more meaningful when a ship carries a product someone actually needs, a detour follows a geographic constraint, and returning later reveals a successful repair or a seasonal change rather than a universal crisis reset.

## 10. Verification boundary and maintenance

The checker reads the twelve current source JSONs, all twenty-six canonical procurement rows from JSON or prose tables, the existing exchange dependencies, and the production model. It checks core population and service fields, water-stage and reuse arithmetic, product/category sums, calorie-share preservation, duplicate aliases, nested urban production, guest inclusion boundaries, stock-versus-food separation, lighting calculations and finite numeric values. It derives aggregate summaries from IDs rather than relying on list order.

The current verification rejects **21 deliberate invalid cases**, including duplicate consumption, conversion of Talweir raw water into treated input, reuse credited as fresh source, a split distant-source pool, calorie shares treated as tonnes, guests charged twice for baseline water, nonfinite values, and lower stacked growing layers treated as naturally lit. A positive control reverses city, source, alias and category ordering and obtains the same results. These tests establish the authored accounting relationships. They do not independently establish source yields, process performance, nutritional adequacy, weather, engineering reliability, ecological permissions or playable runtime behavior.

Prose-only source-process assumptions retain their dossier attribution and are checked for arithmetic; the script is not a general natural-language fact extractor. When a source city changes a table, core service value or procurement commitment, update this ledger visibly and rerun the checker. When a relationship becomes a quantity, add a canonical recipient-linked entry and identify which older qualitative relationship it resolves. When a new community receives an allocation, use the separate regional population boundary rather than increasing the twelve-city census by accident.

The outstanding work is a concrete set of decisions: product recipes and origin quantities, producer cohorts and their land, catchments and seasonal stores, controlled-light fractions, regional electrical sources and fuel, and industrial replacement flows. None requires reducing the planet to a few city blocks. They give the larger world more places with a reason to exist, and give those places ordinary lives beyond serving as scenery for a shortage.
