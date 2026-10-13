# Volume 02 — The hours the sky does not keep

Working canon, edition 0.1. This volume establishes Nacre's shared civil calendar, the experience of its natural light cycle, and the weather relationships that distinguish its regions. The calendar arithmetic and idealized sky geometry are checked. The regional climates are authored hypotheses supported by physical reasoning and Earth analogues; they are not outputs of a Nacre circulation model. No Unreal weather, calendar or planetary simulation is implemented by this document.

The [physical foundation review](Drafts/PHYSICAL_FOUNDATION_REVIEW.md) supplies the selected astronomical parameters. The [regional atlas](07_REGIONAL_ATLAS.md) and [coordinate data](Atlas/nacre_atlas.json) supply the place identities used here. Relief and catchment specifications determine where the proposed weather processes actually operate. A forecast, a season, a civil appointment and a neighborhood's purchased light window are different kinds of information.

## A morning that outlasts your sleep

An Orison resident can finish work, sleep, visit a friend and return to work while the natural star is still above the same half of the sky. A child who says the morning has been going on forever is making a reasonable observation. The city answers with shutters, clocks, meals, bells, light leases and customs that give smaller shapes to a much larger day.

The long cycle is not simply an Earth afternoon stretched into slow motion. The sea retains heat while an exposed roof cools. A slope enters shadow before the waterfront does. Cloud grows in one catchment while an adjacent basin remains dry. In a market, a useful patch of light can move because a collector is tracking its source, a sunshade is adjusted, or a passing orbital service begins its allocation. These causes should be discoverable. They are not interchangeable lighting presets.

Most residents understand only the parts they need. A baker knows the loading court's light and temperature. A school publishes ordinary clock hours. A navigator wants a dated visibility forecast and the phase of the destination's sky. A gardener knows when a particular planted bed receives real direct light and when its apparent brightness comes from lamps. Such knowledge gives people competence without making every conversation an astronomy lecture.

## 1. The common calendar

The shared calendar is called the **Landing Reckoning** in official translation. Its year label is After Landing, or **AL**. The reckoning is a practical agreement, not a claim that all people celebrate the same origin story. Local calendars, work cycles and religious observances coexist with it. Cross-border contracts, historical records and rescue traffic use the common conversion because an argument about which clock applies should not strand a ship.

The **Landing epoch**, elapsed second zero, is the adopted timestamp of the first major arrival's recognized establishment of continuous civilian habitation. Survey visits before it are not erased. Competing institutions dispute the ceremonial description of the event; the agreed timestamp remains useful. It is written `0.01.01 00:00:00 AL, Common Time`. Year zero exists. Dates earlier than it may be expressed with negative AL years in scholarly records; ordinary historical prose says “before Landing” where that is clearer.

One **civil turn** is exactly 86,400 standard seconds: twenty-four hours, each with sixty minutes of sixty seconds. A common watch is eight hours. First watch is 00:00–07:59:59, second is 08:00–15:59:59, and third is 16:00–23:59:59. These are clock divisions. A nurse may work across a watch boundary; a parent may sleep in second watch; a festival may use a different pattern. No labor law is implied by the arithmetic.

One regular civil year contains **thirteen spans of twenty-five turns**, totaling 325 turns. Official spans are numbered, allowing local names to vary without translation errors. An ordinary date reads **12 Fourth Span, 742 AL**. Compact records write `742.04.12`. The hour follows separately: `742.04.12 17:20 CT`. That example is in third watch; it says nothing by itself about whether Orison is in sunlight.

Some years add one **Balance turn** after Thirteenth Span. It is written `744.B`, never “the twenty-sixth turn” of every span. It lasts an ordinary twenty-four hours. Utilities, food and wages continue to exist during it. Traditions about settling accounts do not suspend debts automatically, and a clinic does not gain an unpaid day of labor because a festival uses a poetic description of time outside the year.

### The Measure cycle

The civil calendar uses twenty-two Balance turns in each 115-year Measure cycle. Its mean length is therefore `37397 / 115 = 325.191304347826` turns. This closely follows the reference orbital year of `325.191323114858` turns without pretending they are identical.

The authoritative integer rule is:

`B(Y) = 325 × Y + floor((22 × Y + 57) / 115)`

Here `B(Y)` is the whole elapsed turn on which year `Y` begins. A year has a Balance turn when `B(Y+1) − B(Y) = 326`; otherwise its length is 325. The floor operation is mathematical floor, including for negative years. An implementation that truncates negative fractions toward zero will corrupt dates before Landing.

Within the first cycle, Balance years are **2, 7, 13, 18, 23, 28, 33, 39, 44, 49, 54, 60, 65, 70, 75, 81, 86, 91, 96, 101, 107 and 112**. The pattern repeats every 115 years. Around the present, they are **734, 739, 744 and 750 AL**. Year **742 has 325 turns**. People ordinarily consult a calendar; they do not calculate this list each time they arrange a wedding.

The offset of 57 in the formula keeps civil year boundaries near the nearest full turn to the rational orbital return. The calendar's mean year is about **1.62 seconds shorter** than the chosen astronomical reference. Across years 0–1000, the checked maximum difference between a civil year boundary and the corresponding reference return is about **0.513 turn**. This is sufficient for a stable civil calendar, not sufficient for spacecraft navigation. Observatories publish astronomical times independently and never move a paid appointment silently because a new orbit solution changes a predicted equinox.

### The present and readable ages

The start of **742 AL** is elapsed civil turn **241,292**. It occurs about **55.08 minutes after** the model's 742nd mean orbital return to the Landing phase. Both represent approximately **660.621 standard years** since Landing. The common rounded statement “about 660.6 Earth years” remains correct. The precision difference between the civil boundary and idealized orbital anniversary must not become a fictional historical event.

Reader-facing biographies give **standard age**, measured in Julian years of 365.25 civil turns. Birth dates carry AL. Someone described as thirty is thirty standard years old unless a passage explicitly says otherwise. Eighteen orbital years correspond to about sixteen standard years; legal adulthood cannot be imported from an Earth number without stating the jurisdiction's actual rule. A full biography stores its birth timestamp and the date on which an age was calculated.

For durations, use hours and civil turns when precision matters. “Three turns until the ferry” is unambiguous. “Three days” may mean three work cycles in dialogue, but the writer's record must resolve it. **Long day** means the natural solar cycle, approximately **5.698125 civil turns** or **136.755 hours**. **Year** in AL history means the local orbital/civil year convention above. Avoid an unexplained “month”; use a named span or a specified number of turns.

Common Time is a shared reference clock. Cities may publish convenient local clock offsets, but those offsets are administrative and must be labeled. They are not inferred from fifteen degrees of longitude. Nacre rotates much more slowly than Earth: one degree of longitude corresponds to approximately **22.79 minutes of mean solar phase**, not four. The reference date used by shipping remains common even when a local household clock displays another hour. A vessel crossing the map seam does not gain an extra turn or reset an obligation.

## 2. The calendar does not command the seasons

This edition fixes an additional phase convention without changing the selected orbit: in the reference almanac, the first northward crossing of Ilex over Nacre's equatorial plane occurs **twenty civil turns after the Landing epoch**. Rauth's stellar orbit has eccentricity 0.010, with periapsis aligned to the reference northern solstice. This makes the northern half of the seasonal year slightly shorter and gives it modestly greater stellar flux at its high point. It does not make every northern climate warmer than every southern one.

The phase is an authorial astronomical datum. It is not evidence that the first settlers selected their landing for seasonal symbolism, and it is not a requirement that new year occur at natural dawn. The reference calculation uses Kepler's relation between true and mean anomaly. It omits coupled precession, lunar parallax and perturbations. Historical and future precise almanacs need the later numerical model; regional authors can already use the order and approximate civil placement of the seasons.

| Reference event in 742 AL | Approximate common date | Geographic meaning |
|---|---|---|
| Northward crossing | 20 First Span, late third watch | Ilex's declination crosses from south to north |
| Northern solstice | 1 Fifth Span, first watch | Greatest northward declination, about +14.07° |
| Southward crossing | 6 Eighth Span, second watch | Declination crosses from north to south |
| Southern solstice | 13 Eleventh Span, third watch | Greatest southward declination, about −14.07° |

These are approximate dates from a reference orbit, not implemented eclipse or weather forecasts. Their machine-readable companion includes rounded clock values so calculations can be reproduced, but the prose deliberately avoids pretending that the physical world has been solved to the minute.

The atlas uses **northward season**, **northern high season**, **southward season** and **southern high season** when a global comparison is necessary. “Summer” and “winter” remain local words. At a given latitude, temperature and rain can lag stellar forcing by substantial amounts because the sea, soil, snow and atmospheric circulation retain memory. An equatorial wet season is not defined by the same experience as a northern thaw. A sailing season may be best during neither summer nor winter.

Climate authors must distinguish a repeating seasonal tendency from an annual guarantee. A wet-season onset can arrive early, stall or return after an apparent break. A quiet storm season can still contain one damaging event. Those deviations should arise from coherent regional conditions, not from a storyteller deciding that every calendar anniversary must deliver identical weather.

### Two kinds of holiday

**Landing observances** occupy 1 First Span. Some municipalities mark arrival; others commemorate the labor omitted from the oldest manifests. The date is civil and remains fixed. Rain can move a procession indoors without moving the anniversary. Nobody needs to stop the natural sun to preserve the program.

**Balance Tables**, where practiced, take place on the intercalary turn. Neighborhoods inspect shared stores, return borrowed tools, perform old arguments and eat together. Their rarity gives them a different social weight from annual new year. A child may remember one vividly and be much older at the next. These practices remain local; an accounting office's holiday is not automatically an ecological event.

**First Open Water** is an observed local occasion at cold harbors. It follows a navigable route verified by working crews, not a universal date. Celebrating it does not certify every inlet or ice shelf safe. A small town can hold its gathering later than the regional capital without being behind the times.

**The Returning Bowl** is a proposed seasonal celebration for river communities after the first useful recharge has been confirmed. The cue is water entering usable storage without unacceptable contamination, not simply the first spectacular storm. Some years its preparations outlast the first rains. The choice links pleasure to actual local knowledge rather than turning flood damage into picturesque decoration.

**Dark-sky gatherings** on Morrow are scheduled against the long night, cloud prospects and local obligations. They need not always celebrate an equinox. The absence of Rauth is ordinary to residents; the occasion can be music, storytelling or looking at stars with friends. A forecast that postpones a gathering creates anticipation rather than automatic crisis.

## 3. Daylight by city

The following table uses the current city coordinates without moving them to fit a preferred climate. It gives the duration for which the **center of Ilex** is geometrically above an unobstructed horizon, with its declination held at the stated seasonal value for one idealized rotation. It excludes refraction, the stellar disc's finite size, terrain, clouds, eclipses and changing declination during the long day. It is a comparative geometry table, not a city's measured number of usable sunshine hours.

For latitude `φ` and stellar declination `δ`, the horizon hour angle satisfies `cos H = −tan φ tan δ`; duration is `P_solar × H/π`. Values outside the crossing range mean continuous light or darkness for that idealized cycle. NOAA's solar-geometry references distinguish geometric crossing from apparent sunrise and explain why atmospheric refraction cannot be represented by one perfectly accurate universal correction. We adapt the geometry to Nacre's clock and do not import Earth's timing constants. [NOAA solar geometry](https://www.gml.noaa.gov/grad/solcalc/solareqns.PDF), [NOAA glossary](https://www.gml.noaa.gov/grad/solcalc/glossary.html)

| City | Latitude | Northern-solstice light | Equinox light | Southern-solstice light |
|---|---:|---:|---:|---:|
| Orison | 17° N | 71.7 h | 68.4 h | 65.0 h |
| Marroway | 4° N | 69.1 h | 68.4 h | 67.6 h |
| Hesh | 18° N | 71.9 h | 68.4 h | 64.8 h |
| Serein | 36° N | 76.3 h | 68.4 h | 60.4 h |
| Talweir | 28° N | 74.2 h | 68.4 h | 62.6 h |
| Aven | 12° S | 66.1 h | 68.4 h | 70.7 h |
| Dhal | 4° N | 69.1 h | 68.4 h | 67.6 h |
| Merrow | 30° S | 62.1 h | 68.4 h | 74.7 h |
| Tern | 49° S | 55.6 h | 68.4 h | 81.1 h |
| Velis | 38° S | 59.8 h | 68.4 h | 77.0 h |
| Brin | 61° N | 88.8 h | 68.4 h | 48.0 h |
| Sola | 11° N | 70.5 h | 68.4 h | 66.3 h |

Brin is northern and cold, but **61° N does not place it inside Nacre's nominal polar circle near 75.93°**. Its winter does not automatically contain months without any sunrise. The Pale Interior's present anchor at 72° N is also outside that circle; higher parts of the region may extend beyond it. Southern Blue's 82° S anchor does experience geometric polar light and darkness. Latitude names borrowed from Earth must not overrule Nacre's smaller seasonal tilt.

Orison's open-sky midday stellar elevation is high even in its shorter-light season. A crater approximately twenty-four by twenty-two kilometers across cannot be declared wholly sunless merely because its walls reach above the quay. The current city concept places terraces between roughly eight and 1,350 meters elevation. Local horizon profiles, terrace orientation, neighboring buildings and overhangs decide which rooms remain shaded. The western terraces can face a relatively open dawn approach while losing parts of the later light; other courts behave differently. Every important collector target needs its own exposure record.

Twilight is long enough to become a place in the social imagination, but its duration changes with latitude and season. “Twilight” also names several physical thresholds and many ordinary impressions. The beginning of usable light in a misty courtyard is not the same event as the star's center crossing the open horizon. We will retain geometric thresholds for calculation and allow residents to name the experiences they actually use.

## 4. Rauth's uneven night

Rauth does not rise and set each natural day. Its center remains near one direction in a settled observer's sky, with small librations and longer-term corrections. Its bands rotate, its clouds change, and its phase follows the illumination geometry. The near side of Nacre receives powerful parent-light during favorable phases; the far side does not receive that direct glow. Neither side lacks natural stellar daylight.

| City group | Reference view of Rauth | Environmental implication |
|---|---|---|
| Orison, Marroway, Hesh | Center roughly 57°, 61° and 65° high | Strong parent-light can reach open near-side spaces; streets still depend on orientation and cloud |
| Sola | Center about 31° high | Ridges and island relief strongly shape which places receive it |
| Serein | Center about 13° high | Coastal haze and local relief can remove a large part of the direct view |
| Talweir, Aven, Dhal, Merrow, Velis, Brin | Entire disc below the ordinary reference horizon | Direct parent-light is absent at the city centers |
| Tern | Center about 5.66° below; upper limb about 0.69° below | A marginal case: height, refraction and libration can reveal a thin edge from selected viewpoints |

These values include the observer's displacement from Nacre's center on the reference sphere. They do not include local elevation or refraction. Tern must not be described as having a broad, permanently visible giant above its harbor. A special high-cliff view of a distorted edge could instead become a precise local pleasure, known to residents and dependent on conditions.

Clear near-side nights can retain colored directionality, while far-side nights make artificial lighting more prominent. A canopy, gorge or heavy cloud can create darkness on either hemisphere. Animals respond to light level, spectrum and concealment, rather than to a binary near-side flag. A courier who crosses from Orison to Morrow should notice the change without every resident explaining it.

Eclipse seasons are separate from storms. Near the equatorial-plane crossings, Nacre can pass through Rauth's shadow. The current first-order example gives central shadow passages lasting a few hours and a small annual shadow fraction. It does not establish exact eclipse dates for 742 AL. Local observers need the star above their horizon and the relevant shadow geometry to see daytime disappear; residents already in night do not witness another sunset. A cloudy city can miss the spectacle while its optical power receivers still record the loss.

The Concession can schedule a maintenance outage during a predicted eclipse. It cannot cancel the eclipse, sell an unshadowed orbit that geometry does not permit, or power the deep night from a low reflector inside Nacre's shadow. Moving orbital mirrors, ground collectors, stored energy and regional generation retain their distinct roles. Weather can interrupt optical supply while an electrical reserve continues; a failed switch can interrupt lamps while the sky remains perfectly clear.

## 5. Weather with causes and memory

Regional weather is authored around interacting reservoirs and flows. Ocean temperature, wet soil, snow, groundwater, vegetation and built storage remember previous conditions. Air pressure, wind, humidity and cloud connect neighboring places. A dry spell does not empty every aquifer at once; a single shower does not erase a drought. USGS distinguishes streamflow drought and groundwater contributions, which supports keeping these responses separate in the fiction. [USGS: drought and groundwater](https://www.usgs.gov/centers/sawsc/science/south-atlantic-water-science-center-drought-monitoring)

The long solar day gives heating and cooling time to develop, but does not guarantee one storm per afternoon. Large-scale flow can reinforce, displace or suppress a local sea breeze. Terrain channels air, sheltered water buffers temperature, and clouds can prevent an exposed surface from following the same thermal pattern as a nearby clear slope. NOAA's sea-breeze account supplies an Earth analogue for the coupled circulation; Nacre's strength, reach and timing remain design hypotheses. [NOAA: the sea breeze](https://www.noaa.gov/jetstream/ocean/sea-breeze)

The main authoring states are **settled**, **gathering**, **active**, **clearing** and **recovery**. These are production descriptions, not words every resident must use. Settled conditions can be sunny, overcast or steadily misty. Gathering shows the change: pressure trend, rising humidity, altered distant cloud, a change in swell. Active weather has a footprint and duration. Clearing describes the atmosphere. Recovery describes what happened to land, water, infrastructure and organisms. These last two are never automatically the same.

An ordinary event should have a credible path back to ordinary life. People dry laundry, inspect a roof, reopen a path, enjoy the clean air or complain that visitors overreacted. Severe damage belongs to specific events and vulnerabilities. Constant disaster would erase the livelihoods, festivals and comfortable routines that make the world worth inhabiting.

### Working timescales

| Process | Appropriate initial authoring scale | What must persist afterward |
|---|---|---|
| Passing shower or gust front | Tens of minutes to several hours | Wet surfaces, limited runoff, changed visibility or disturbed activity |
| Fog bank or local inversion | Several hours to civil turns, depending on supply and mixing | Condensation, damp material and any interrupted schedule |
| Regional rain episode or maritime front | Civil turns; sometimes a sequence lasting a long day or longer | Catchment storage, delayed flow, erosion and repair needs where justified |
| Snow accumulation and thaw | Multiple events and seasonal history | Snow water, refreezing, sheltered remnants and downstream lag |
| Drought or unusually wet season | Spans to multiple local years | Groundwater, vegetation, crop decisions, prices and habitat recovery |
| River flood wave | Basin-specific delay after contributing weather | Sediment, contamination, inspections, changed channels and human obligations |
| Ecological recovery | Species-specific seasons or years | Recruitment, lost cohorts, altered habitat and successful restoration |

These ranges are scenario guidance rather than a calibrated distribution. A future simulation should not choose a duration merely because it fits a play session. Time compression, if adopted, changes presentation and must preserve the ordering of causes, forecasts, travel and aftermath.

## 6. The twenty-four regional weather relationships

Each brief below states a intended causal pattern, a meaningful transition and a reason to enjoy the region. None supplies an unsupported annual rainfall total or claims that the current concept coastlines generate a validated climate. Seasonal phase labels refer to the shared astronomical frame; local onset and lag remain adjustable through the future circulation review.

### REG-001 — Saffron Escarpment

Warm eastern water and seasonally stronger moisture transport support Orison's intended wet interval, most plausibly from the approach to northern high season into its aftermath. Uplift on the escarpment favors cloud and rainfall on some exposures. The city experiences this as a sequence of humid gathering, substantial rain episodes, intervening light and eventual retreat—not a monsoon switch producing uninterrupted downpour.

An active episode can cover the rim while leaving the breached harbor in lighter rain. Drainage paths then matter: roof water, hillside runoff and industrial contamination arrive by different routes. Clearing brings bright wet copper, conversations moved outside and urgent but ordinary drain work. Recovery can keep a low passage closed after the collector ridge is dry. The approved Indigo Monsoon belongs to these particular conditions; the same city also has clear workdays and pleasant evenings.

### REG-002 — Nine-River Lowlands

Marroway receives both local weather and upstream consequences. Rain in Glassback can raise a river after the city has cleared. A wet plain with already saturated soil can respond differently from the same storm after a dry interval. Flood announcements therefore report contributing basins and expected arrivals, rather than announcing that all water rises when rain begins on screen.

Normal high-water periods support transport and native nurseries. Residents shift landings and use raised courts without treating each seasonal rise as catastrophe. Exceptional levels threaten particular levees, treatment sites or crops. After recession, usable depth, salinity, contamination and the condition of a bank recover at different rates. A fine post-flood turn can be full of repairs, cooking and visiting. Its pleasures do not require pretending every temporarily inundated place was ruined.

### REG-003 — Glassback Range

The range divides moist eastern approaches from more sheltered interior country. Altitude, aspect and pass geometry matter more than a single regional temperature. A journey can leave warm rain, cross a cloud-bound saddle and descend into dry brightness. Snow occupies the appropriate heights and exposures; it is not painted onto every summit because the region is called mountainous.

Forecasts distinguish slope cloud from a larger frontal passage. Long daytime heating can strengthen upslope movement; clear-night cooling can send dense air into valleys. A storm's end does not remove loose debris or an unstable snow layer. Refuges display the observations of people who actually crossed recently. Good weather offers broad views between basins, still mornings above cloud and sociable arrivals at maintained shelters. Travel difficulty should preserve these intervals of competence and reward.

### REG-004 — Red Ledger Basin

Hesh's dryness depends on the interior position and shelter of its basin, with mountain-fed water arriving from elsewhere. Clear intervals bring broad temperature differences between exposed ground and protected interiors. Dust requires both a source of loose material and enough wind; a dry landscape does not emit a perpetual orange haze.

Occasional intense rain can produce fast runoff on compacted or poorly absorbing surfaces. It may cause local flooding without substantially restoring the basin's long-term water reserve. Mountain recharge and managed storage remain distinct from rain in the city square. The best cool periods draw people into courtyards and public roofs. After a dust event, cleaning collectors and filters has a familiar rhythm; after useful recharge, celebrations acknowledge the measured water rather than the spectacle of cloud alone.

### REG-005 — Foldwood Marches

The southern Kesh mosaic is provisionally wetter around southern warming and high season, with river corridors retaining moisture into drier intervals. Native canopy movement responds to light, water stress and species behavior. Adjacent stands need not open or fold at the same hour. A traveler learns that a darkened path may indicate closed producer fans, approaching weather or simply a different community of organisms.

Show the transition into dry conditions through soils, plant posture, water use and animal distribution before using severe fire as an event. Fire requires suitable fuel and ignition; some native tissues may resist it, while imported plantings behave differently. Rain after a long dry interval can expose scent and new activity without instantly restoring depleted reserves. Comfortable night travel, sheltered gatherings and the return of a familiar canopy form belong beside the hazards.

### REG-006 — Serein Fog Coast

Serein's intended cool water and marine inversion can hold low cloud against the coast while inland heights remain clearer. This is a circulation hypothesis that must survive the later ocean model. Fog collection supplements water; it does not remove the need for catchments, storage and treatment. NOAA's marine-layer description provides the analogue for a coast and nearby uplands experiencing sharply different conditions. [NOAA: marine layers](https://www.noaa.gov/jetstream/ocean/marine-layer)

Fog can thicken during otherwise settled weather. A changing wind or disturbed inversion can lift it, revealing an entire harbor without a dramatic storm. Wet façades and sound remain different after visibility improves. Some residents choose a fog walk for pleasure, others climb to a sunlit roof gathering above the layer. Forecasts distinguish a low ceiling that matters to flight from ground visibility that matters to a pedestrian or ferry pilot.

### REG-007 — Cairn Steppe

The cooler interior has broad exposed ground, seasonal growth and substantial nighttime cooling where sky and soil permit it. Its year should show a changing resource map: sheltered water, producer growth, drying cover and possible seasonal snow. It is not empty simply because the observer sees few tall structures.

Wind fronts can be visible at great distance through moving vegetation or dust from genuinely disturbed ground. A clear forecast for a town does not guarantee identical wind across an exposed ridge. After rain, some tracks become soft while stable stony routes remain useful. Good travel periods encourage gatherings, races and long visits between settlements. The region's strongest dramatic image may be quiet: a migration progressing beneath a distant cloud line with enough room to understand its scale.

### REG-008 — North Gate Sounds

The northern channels combine maritime buffering, seasonal light changes and headland exposure. Weather differs across the mouth of an inlet. A sheltered anchorage can be comfortable while an outside crossing remains rough from a storm that has already moved away. Swell, local wind and tidal current need separate descriptions.

The useful seasonal contrast is between windows of easier access and periods demanding more conservative routes, not a universal closed winter. Some upper inlets or freshwater margins may freeze before open salt water does. Maintenance crews travel during weather breaks; performers and shared instructors often follow the same service boats. Clearing weather can start a busy social interval. A forecast should identify which passage has improved instead of turning every harbor from red to green at once.

### REG-009 — Talweir Lake Country

The lakes moderate nearby temperatures while uplands feed delayed inflow. Sheltered bays can grow fog as different air and water temperatures meet. Exposed reaches support wind-driven waves that surprise visitors who hear “lake” and expect quiet water. The same regional pressure pattern can make one crossing easy and another uncomfortable because their directions differ.

Northern seasonal warming affects mixing, managed cultivation and shoreline activity, but exact lake overturn schedules need depth and water-temperature specifications. Flooding may be an upstream event; low water can reveal structures without making their surfaces stable to walk on. Festivals choose alternate stages and landing places as conditions change. A beautiful clear long night over the lakes can still carry a cold drainage wind. Residents prepare for it without treating the evening as an expedition.

### REG-010 — Aven Uplands

Aven's high basins are cooler than nearby lowlands and can collect cold air beneath clear skies. The plateau city, a shaded hollow and a pass may therefore report different conditions within a short distance. Solar warmth on an exposed wall does not establish the temperature of an unlit floor or stream crossing.

Southern seasonal wetting can bring cloud to ridges and rain to lower routes; the exact moisture source and precipitation distribution remain a circulation obligation. Frost or hail events require the appropriate air and surface conditions. Walking clubs value the clear intervals between them, and public roof edges make ordinary movement comfortable in light rain. A refuge's report can be more relevant than the city's headline forecast. Recovery focuses on the actual route: loose slope material, refrozen seepage or an inspected bridge.

### REG-011 — Green Vault

The low-latitude eastern canopy retains moisture and shades the ground. Its wetness is maintained by an atmospheric supply and local recycling, not by trees creating water from nothing. Long bright phases can alter canopy convection, while rain and interception create different timing beneath the leaves. A forest floor may continue dripping after the sky has cleared.

Seasonal changes can be expressed through storm frequency, river level, producer behavior and reproductive cues rather than a winter coat of snow. Dhal's workers read ventilation and decay as carefully as temperature. A dry anomaly raises fire and water stress only where the tissues and conditions support those outcomes. Comfortable river travel, canopy openings after rain and concentrated animal activity provide pleasures without turning the whole forest into an ordeal. Ground, canopy and open-water forecasts have different uses.

### REG-012 — Merrow Estuaries

Merrow receives freshwater, sediment, marine water and weather along overlapping routes. A surge driven by offshore wind can oppose river discharge. Heavy upstream rain can shift salinity when there is little rain over the coast. Native nurseries occupy particular ranges and cannot all respond as one biological meter.

Seasonal runoff changes food and transport conditions, while local sea breezes alter work along exposed quays. Clearing from a wet episode opens fine public dining spaces, but treatment crews may still sample water and repair damaged screening. The Blue Sill legacy makes such work visible and sometimes contentious. Recovery should include thriving habitat where stewardship succeeded. A return visit can reveal more life and easier movement, not only a new prohibition or another contamination crisis.

### REG-013 — White Reed Savannas

The open southwestern Morrow mosaic depends on seasonal moisture and flood-fed corridors. Southern warming and the later response of surrounding water provide the proposed broad wet-season frame, subject to circulation review. Pale structural fans expose different surfaces as they grow, dry or turn with wind; their appearance is a biological response, not an independent seasonal color slider.

Storms can arrive in separated tracks, leaving nearby grazing or material-gathering areas with unequal resources. Mobile workers and wildlife adjust to that patchiness. In dry periods a dependable refuge becomes more valuable without every household being in immediate danger. The return of useful rain brings transport problems and welcome gatherings together. Good forecast information may persuade a traveler to wait for the soil to drain before driving, then reward that patience with clear air and flourishing country.

### REG-014 — Morrow Crown

The northern uplands store snow and supply lower basins where altitude and precipitation support it. Sunlit and shaded slopes lose snow differently. A warm spell can increase downstream water before a pass has lost its drifts. The absence of Rauth from most local skies leaves clear long nights darker, but terrain, cloud and artificial light still shape each site.

Observatories seek particular visibility rather than a generic sunny state. Mine transport, agricultural valleys and protected headwaters have different weather vulnerabilities. Wind can keep a ridge exposed while filling a sheltered route. Good conditions offer long views and gatherings at inhabited high places. A snow-related closure should retain its physical cause until conditions or work remove it; a scene transition must not reopen the route merely because the player returned after an arbitrary timer.

### REG-015 — Tern Storm Coast

Tern's exposed southern coast receives repeated maritime disturbances, with the more demanding interval provisionally weighted toward southern cooling and winter. Its protected inlet is valuable precisely because offshore conditions can differ. A storm coast also has calm spells, productive weather windows and days when the sea is the reason people are happy to live there.

Forecasts separate swell arrival, harbor wind, visibility and conditions at the rescue destination. Regattas use contingency courses rather than requiring impossible perfect weather. When an active storm passes, returning crews may find the city busy, convivial and tired. Damaged breakwaters need inspection; minor deck repairs need ordinary work. The marginal possible glimpse of Rauth from selected high places is a local sky event, not a second guaranteed source of clear-night illumination over the harbor.

### REG-016 — Velis Cold Gardens

Velis combines a cooler estuary, sheltered cultivation and a stronger seasonal light range than tropical cities. A garden's glazing, substrate, ventilation and heat reserve matter more than its picturesque roof shape. The city trades plant and microbial stocks, so weather disruption affects transport timing and biological quality as well as supply volume.

Southern bright season permits different outdoor pleasures without making every planted enclosure redundant. Cool wet episodes bring condensation problems; clear nights can demand careful heat management. Residents distinguish a beautiful frost on an unused railing from one damaging a particular crop. After a storm, reopened paths and shared meals can make the gardens feel generous. A forecast is often read beside a cultivation log, connecting present conditions to decisions made several spans earlier.

### REG-017 — Ember Fault

The geothermal province sits amid cold southern country. Local heat produces steam, warm runoff and small refuges from the regional climate. It does not lift every valley into permanent summer. A calm clear period can collect gases in a hollow; a windy cold period can disperse them while making exposure more difficult. Both observations matter without inventing arbitrary poison weather.

Snow, meltwater and mineral deposition interact around maintained works. A warm-looking crust may cover unstable ground, while a plain engineered channel can be the safe and useful route. Bathing places and public refuges can be comfortable institutions with attentive operators. Recovery after a disrupted well or watercourse involves chemistry and inspection, not merely a change in particle effects. Any eruption or major seismic event requires its own geological cause and authored aftermath.

### REG-018 — Brin Long Shore

Brin's sixty-one-degree latitude brings strong differences in long-day duration, while its maritime position buffers some temperature changes. It remains outside the nominal polar circle. Winter still has daylight; summer still has a substantial dark interval. Ice, fog and storms affect access through water temperature and exposure rather than an assumed Earth Arctic calendar.

Specialist services plan around delivery windows, but the city is not constantly waiting for rescue. Clear periods support sport, work outside and visits along the shore. A forecast can distinguish harbor operations from an exposed approach and identify an alternative port before a departure fails. When a supply interruption ends, unloading is a social event with practical limits: biological cargo, medicine, replacement parts and people do not all receive the same handling priority.

### REG-019 — Pale Interior

Cold uplands, sheltered basins and ice margins form a varied interior. The regional anchor at seventy-two degrees north still receives some direct stellar light in the reference winter geometry, though local terrain may block it. Areas extending farther poleward can experience true polar night. Distinguish these cases in future site dossiers.

Blowing snow can destroy visibility without fresh snowfall; a clear atmosphere can coexist with hazardous surface drift. Wind shelter and dependable markers make maintained routes valuable. Native life uses short productive windows and protected microhabitats that fast travelers can miss. Fine conditions offer extraordinary quiet, companionship and a sense of distance. A closed station may simply be between work seasons. Its orderly stores should not be rewritten as a catastrophe because a lone traveler arrives there at night.

### REG-020 — Split Cape

Rocky coasts and islands divide exposure into many local cases. A headland can take heavy sea spray while a nearby inlet is calm enough for ordinary work. Breeding or nursery activity follows food and conditions, so the most convenient landing season for people may overlap a sensitive biological interval.

Forecasts matter alongside ecological notices. A route that is physically accessible may be avoided to protect a gathering, while another is closed because its surf is dangerous regardless of wildlife. Storm recovery includes inspecting landing structures and observing displaced colonies. Good periods bring craft exchanges, visiting families and simple pleasure beside the sea. The cape's atmosphere should include sun between clouds, drying walls and laughter carrying across sheltered water, rather than one continuous grey gale.

### REG-021 — Aster Chain

The low-latitude island arc separates wet windward slopes from sheltered lee settlements over short distances. Seasonal changes in the prevailing moisture supply can reverse which harbor is most convenient. Those changes require a coherent regional wind model; an island must not become windward merely because the camera moved.

Freshwater storage is a serious concern even amid lush views and surrounding sea. An active rain episode replenishes some stores and carries sediment into others. Ferry schedules distinguish protected inter-island movement from exposed crossings. Sola's celebrations can move between harbors without vanishing whenever one route is uncomfortable. Clear weather after rain reveals the island chain's layered relief and makes travel itself a pleasure. Recovery includes useful cleaning and repairs, not automatic destruction of every lightweight awning.

### REG-022 — Neral Arc

Remote islands near the map seam experience real ocean weather rather than a special boundary climate. Seasonal winds, marine fronts and island relief shape access. Small catchments can react quickly to rain and then return to low flow, while endemic life follows its own storage and dispersal strategies.

Observations are sparser, so forecasts should admit greater local uncertainty without declaring all events unpredictable. A returning research crew can update the record with a failed anchorage, a reliable shelter or an ordinary successful crossing. Good periods allow careful visits and recovery of knowledge that was never globally catalogued. After a damaging event, assistance takes actual travel and supplies. Quiet intervals let residents and native communities resume their lives; remoteness is not permission for permanent disaster or compulsory horror.

### REG-023 — Pelagic Roads

Open-water weather crosses administrative regions and joins distant journeys. Swell generated elsewhere can arrive under clear local skies. Productive fronts, wind, surface temperature and circulation create shifting routes for wildlife and ships. The region's anchor is not the center of all ocean activity.

Voyages compare several possible paths with changing fuel, time, comfort and refuge implications. A storm need not chase the player; its movement follows the weather field. Floating communities experience domestic life while this unfolds: cooking, maintenance, games, fatigue and the pleasure of a clear horizon. Recovery may involve a delayed rendezvous rather than physical damage. Forecast updates must preserve what a vessel could receive at its location, and an old bulletin must remain visibly old rather than becoming a misleading promise.

### REG-024 — Southern Blue

At the eighty-two-degree southern anchor, the chosen obliquity creates true seasonal polar light and darkness. Continuous light is not continuous warmth, and darkness does not make every sea surface freeze instantly. Ocean transport, ice, cloud and wind control the actual environment. Precise ice extent remains unresolved until the physical map and climate model are available.

Stations organize work around useful access, visibility and biology. Some occupation is seasonal by design. A return to a carefully closed site can be a successful resumption rather than a mystery. Long views, low light, ice movement and native seasonal gatherings offer wonder. Severe weather has clear causes and refuge consequences, while fine periods allow competent exploration and companionship. Navigation must handle the pole and longitude convergence honestly; crossing a meridian there does not leap hours on the civil clock.

## 7. Forecasts are a public craft

Nacre's forecasts come from orbital observation, surface stations, instrumented vessels, local observers and models of unequal quality. A sophisticated city can still have poor information about a remote basin. A fisher's report can correct a stale instrument; a fashionable institution can issue an excellent forecast and communicate it badly. None of these sources is an excuse for omniscience.

A useful public bulletin states its **place, issue time, valid interval, expected conditions, uncertainty and practical consequence**. A harbor notice might say: “Outer crossing: freshening wind expected during second watch. Earlier departure offers a wider margin. Inner ferries continue. Update after the ridge report.” It should not use an unexplained planetary percentage or hide a confidence range behind one reassuring icon.

Weather probability describes a defined event in a defined interval. If rain probability is shown, the record needs an intensity threshold and geographic meaning. It is not automatically the percentage of the sky covered by cloud or the fraction of the interval spent raining. NWS guidance makes that distinction for Earth forecasts; Nacre's public products should be equally clear about what their numbers mean. [NWS forecast terms](https://www.weather.gov/ppg/forecast_terms), [NWS probabilistic weather education](https://www.weather.gov/mrx/probeducation)

Forecast skill should decline with horizon and poor observation, not with the player's story progress. Astronomical phase can be predicted farther ahead than a particular shower. A seasonal outlook can help growers plan reserves without promising the hour of the first storm. A navigation service can predict an orbital passage accurately while remaining uncertain whether cloud will permit the intended optical service below it.

Local habits make these distinctions visible. Tern displays outside-swell observations beside its inner-harbor notice. Marroway reads upstream gauge reports even under blue sky. Hesh posts recharge information separately from the rain outlook. Serein identifies the expected height of the marine layer. Brin lists which route observations have arrived recently. These practices distinguish places through useful knowledge and create believable opportunities to talk, trade information or change a journey.

The default tone is competent public service. False reports, withheld observations and corruption can occur, but making every forecast deceptive would destroy the very institution the fiction needs residents to use. Most bulletins should be worth reading. When one is wrong, the world should distinguish uncertainty, equipment failure, a mistake and deliberate manipulation.

## 8. Water after the weather

A flood carries several clocks. Rain may stop; a tributary may peak; the main river may continue rising; an estuary may remain backed up by marine conditions. A treatment plant can restart before all household supplies are cleared. A bridge can appear intact while its foundation needs inspection. NOAA's flood guidance identifies persistent contamination and infrastructure hazards after water recedes, supporting a distinct recovery phase in the fiction. [NWS: after a flood](https://www.weather.gov/safety/flood-after)

The world should give residents useful decisions within those clocks. A school changes its entrance. A workshop lends drying racks. A ferry serves a temporary landing. A community kitchen feeds a repair crew and keeps its ordinary customers. These are dynamic consequences with names and routes, not solely damage statistics. They allow a player to help without making the same flood destroy the same bridge on every visit.

Drought is equally specific. Meteorological shortfall, dry soils, reduced river flow and depleted groundwater are related but distinct. A stored reserve can soften one problem while a native wetland suffers another. Water restrictions can be justified, unjustly distributed or both. Recovery may take several good episodes and a change in withdrawal, not just one cinematic storm. A household celebrating rain can still know that the basin is not replenished.

Tides, seasonal flow, storm surge and engineered releases must remain separate contributors to water level. Rauth's large permanent tidal figure is not a daily wall of water. Local tide tables require basin geometry and variable forcing; the history and atlas must not borrow an Earth twelve-hour tidal cycle without checking it. A release from a reservoir has an operator and schedule, while a surge has a wind and pressure history. Their overlap can create a compelling event precisely because each has a different cause.

## 9. Translating this volume into future world behavior

Every regional weather record should eventually identify a spatial footprint, current regime, recent history, expected transition and affected reservoirs. A region identifier is useful for authorship but does not make a wall in the atmosphere. Neighboring areas blend and exchange weather; a mountain barrier or sea front explains a sharp change where one exists.

Each important place needs a smaller exposure record: horizon profile, elevation, shelter, water connection, surface type and infrastructure dependencies. Weather and sky then affect it through those properties. A covered market receives sound, runoff and changes in customers without rain falling through its roof. An indoor clinic responds to supply and ventilation rather than taking the outdoor temperature instantly. A distant forest does not grow back because its actors unloaded.

Future event specifications must retain an aftermath until an actual process removes it. Drying, cleaning, repair, erosion, growth and social decisions take different times. Some outcomes can be represented cheaply at a distance, but the represented state must agree when the player returns. Weather should also create positive events: comfortable gatherings, reliable passage, renewed growth, clear views, restored supply and the enjoyment of a well-prepared home.

The future game may accelerate time or offer travel transitions. Its authority must remain one coherent chronology: departures precede arrivals, forecasts retain issue times, a night journey ends in the correct solar phase, and a promised visit is not repeated by a reload. This volume specifies the world requirement; it does not claim those implementation choices have been made or verified.

## Arithmetic evidence and remaining physical work

[The calendar checker](../../Tools/World/check_calendar.py) reads the canonical city coordinates and checks civil boundaries, intercalation, negative dates, watch transitions, conversion round trips and geometric sky identities. Run it with `--json` for the full numerical report. It makes no file changes and uses no Unreal runtime. The compact example `742.04.12 17:20 CT` converts to elapsed second **20,855,121,600** after the Landing epoch.

The successful verification run covered years −1000 through 2000 at their boundaries, detailed turn/watch cases in selected years, the epoch and present year, invalid dates, the 115-year cycle, polar/equinox identities and all twelve city coordinates. That establishes the calendar's arithmetic and the stated idealized table. It does not certify the regional climates, exact stellar rise times, an eclipse almanac or future simulation behavior.

The remaining physical work is specific: coupled orbital and spin evolution; a full observer ephemeris including the moon's displacement; atmospheric refraction appropriate to the selected air; detailed terrain horizons; ocean and atmospheric circulation; drainage and lake storage; snow and ice response; and region-specific weather distributions. Those investigations may refine forecast ranges and seasonal lag. They must not silently change the existing calendar, coordinates or historical dates. If a physical revision requires a new almanac, record which model changed and preserve the distinction between civil memory and the sky it describes.
