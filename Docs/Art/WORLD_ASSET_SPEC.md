# Borrowed Sun worldbuilding and asset specification

Revision 1 · 13 October 2026 · Solo production with AI and purchased assets · Unreal Engine 5.8.2

Build Orison as a place whose architecture, tools, streets and repairs reveal how people live under rationed daylight. The production strategy is a hybrid: original silhouettes and cultural details establish the identity; purchased and open assets supply useful construction and everyday objects; reusable assemblies turn that library into neighborhoods. Every foreground asset must work at a walking camera distance and support its intended gameplay.

The long-term library target is **at least 4,000 accepted, distinct geometric assets**. The accompanying catalog defines **4,200 planned geometry briefs across 235 families**. These are authoring and sourcing requirements, not a claim that 4,200 models exist. The first production wave contains 160 planned records; the next executable milestone selects a much smaller assortment from the library. Production proceeds by a playable street, its interiors and its interactions, rather than by completing every variant in a category.

The approved Brass Sanctuary direction and existing First Light mission remain the foundation. New neighborhood names, manufacturing profiles and destruction tiers below are the proposed worldbuilding baseline. The full city library does not enlarge the first demo or require all 4,200 assets before a release.

## 1 What the asset target measures

An accepted asset is a useful, distinct geometric design with editable source, valid scale and pivots, suitable collision, integrated materials, provenance and evidence from the game. A purchased object may satisfy a catalog requirement after adaptation and qualification. It should not also be counted as a second original asset merely because its folder or material changed.

| Domain | Families | Forms per family | Construction profiles | Planned briefs |
|---|---:|---:|---:|---:|
| Architecture | 40 | 8 | 5 | 1,600 |
| Everyday props and furniture | 100 | 8 | 2 | 1,600 |
| Infrastructure and machinery | 30 | 8 | 2 | 480 |
| Landscape, planting and food | 40 | 8 | 1 | 320 |
| Signature components and vehicle shells | 25 | 8 | 1 | 200 |
| Total | 235 | | | 4,200 |

Architecture profiles require different construction geometry: thick load-bearing masonry, framed infill, hanging timber, civic castwork and pressure shells. Prop profiles contrast workshop-built objects with separately designed standardized manufactured objects. Infrastructure has accessible outdoor and pressure-sealed designs. These are briefs for genuine geometric differences, not automatic permission to count the same mesh twice.

The count excludes material swaps, texture resolutions, LODs, collision meshes, mirrored duplicates, placed instances, debris pieces, damaged-state exports and assembly recipes. A left and right part counts twice only if both need different geometry beyond mirroring. A vendor's advertised model count does not automatically map one-to-one to accepted project assets. If two proposed records resolve to the same useful design, merge them and replace the released slot with an actual unmet world need. Preserve aliases for retired IDs.

Ubisoft described more than 4,000 unique **props** for Assassin's Creed Odyssey and reusable stocked arrangements called Lego Kits. That is evidence for investing in variety and reusable arrangements, not a universal minimum for quality. Our target includes architectural modules, landscape and vehicle components as well as props, so it uses a different counting basis. [Ubisoft account of Odyssey worldbuilding](https://news.ubisoft.com/en-us/article/22vdPhUVHV68b2FEjcotkW/assassins-creed-odyssey-the-game-that-greece-built)

The machine-readable catalog contains stable definition IDs, family, form, construction brief, district, family size envelope, materials, source route, production wave, interaction and destruction profiles, dependencies and acceptance requirements. A family's size envelope is deliberately broad. Before a record enters production, its blockout must lock width, depth, height, contact footprint, clearance and sockets. The catalog's **Planned** status cannot be promoted by merely generating an FBX.

## 2 The world that produces the objects

Orison is a crater-wall port on a stylized tidally locked moon. Settlers built terraces around water storage, protected transport routes and mirror-light access. A controlled orbital mirror network redirects daylight into the inhabited crater. Light allocation changes when a street can grow food, open a business or maintain public spaces. Electrical storage, heating and life support remain separate systems; the mirror is not an unexplained source of every utility.

The city has an old foundation, inhabited additions and recent service infrastructure. The oldest structures retain water and hold up terraces. Later residents inserted timber rooms, courtyards, workshop fronts and shade structures. The Concession standardized meters, permits, public terminals and collector equipment. Today's city contains all three layers, maintained by people who replace parts instead of abandoning entire buildings.

Rook is a pirate courier whose ship, Kestrel, connects ground-level relationships with access to the sky. The sunseed carries authorization, scarce stored charge and custody significance. It must remain visually distinct from ordinary lamp cells. The First Light mission's clinic and evacuation choices make the allocation system visible through people and places.

### Geography and causal rules

- Low Tides occupies the sheltered docks and lower terraces. Markets, workshops, shared kitchens, reservoirs and freight handling dominate its props. The name is local history; it does not require an Earth-like tide simulation.
- The Ladder climbs through residential terraces, tram routes, gardens and suspended additions. Its geometry shows how goods, water and people move vertically.
- The Crown houses allocation offices, archival facilities, collectors and launch infrastructure. Cleaner surfaces reflect maintenance resources, not a separate science-fiction genre.
- The mirror ring provides distant purpose and later orbital play. Its pressure systems, servicing equipment and large structures grow from the city's utility language.

A market needs deliveries, stock storage, waste collection, cooking water and a closing routine. A garden needs water access and maintenance reach. A collector needs a drive, a brake, a service path and somewhere to dissipate heat. A roof needs drainage. A hanging room needs a visible support path. These relationships decide what belongs in the asset library.

Artificial gravity, atmospheric retention and the stylized orbital layout remain setting assumptions. They do not justify random construction: objects still need believable weight, joints and use. Do not promise a physically simulated planet to explain an art decision.

## 3 Culture and visual identity

Brass Sanctuary combines warm mineral surfaces, indigo textile shade, copper service systems and the original Lantern Keeper. The Keeper is a moth guardian holding an artificial sun; its wing collectors connect civic symbolism to useful technology. Its final form must be developed as an original sculpture with readable anatomy, construction and maintenance access.

The city values care, practical competence and the ability to keep others supplied. Ownership marks, repair dates, inherited tools, cooperative notices and personally arranged rooms reveal that culture. Ornament derives from folding wings, collector geometry, woven shade patterns, flow channels and the repeated curves of vessels and light reflectors.

Real architectural and craft references should supply knowledge of climate adaptation, joinery, ventilation, masonry and daily use. Their mixture needs a fictional history and consistent construction logic. Foreground religious symbols, sacred text and recognizable monuments should not be used as miscellaneous decoration. Signs use reviewed, readable text; invented writing requires a consistent alphabet and meaning, not image-generated pseudo-script.

| Group | Visual expression | Objects that express it |
|---|---|---|
| Common Table | Shared upkeep, repaired open construction, public access | Tool lending, shared ovens, refill stations, seed exchanges, notice frames |
| Concession | Standard interfaces, tidy service surfaces, controlled access | Meter cabinets, permit cases, inspection gates, sealed machine housings |
| Open Wake | Durable portable gear, standardized docking connections, personal voyages | Chart cases, cargo restraints, shared winches, worn ship service tools |
| Night Choir | Adapted communication equipment and places to listen | Recorders, hidden aerials, projection carts, testimony archives |

Factions are not palette swaps or moral uniforms. A Concession-made bottle can be repaired in Low Tides. A Common Table clinic can contain a well-maintained industrial diagnostic cart. Iona, Venn and Seli should eventually have tools and storage arrangements that express their work, but their character assets are a separate character-production specification.

## 4 District and neighborhood grammar

These neighborhoods are content families and expansion options, not a commitment to build every area for the first release.

| Place | Everyday purpose | Dominant construction | Distinctive silhouette | Story evidence |
|---|---|---|---|---|
| First Light Quay | Arrivals, market trade, courier embarkation | Sandstone arches and framed shop infill | Long shade line, stepped quay, Keeper view | Deliveries, revised light schedules, repairs in progress |
| Second Dawn Lane | Lamp, cell and optical repair | Deep workshops under an arcade | Hanging light rail and dense workbench frontage | Returned goods, test lamps, sorted recoverable parts |
| Three Drops Court | Water refill and washing | Cistern masonry and shaded courtyard rooms | Basin edges, pipes and drying levels | Allocation counters, reusable vessels, maintained drains |
| Copper Thread Steps | Cloth, upholstery and shade making | Timber additions and narrow stair landings | Rolled fabric, suspended patterns, stretched awnings | Repaired sailcloth and orders for different districts |
| Common Table Yard | Food preparation and mutual aid | Broad masonry with open shelter frames | Communal oven and long tables | Public tools, queued meals, seed exchange |
| Coil Yard | Mechanical servicing and salvage | Heavy footings with open metal frameworks | Hoists, motor benches and stacked service frames | Inspected salvage and labeled replacement parts |
| Hanging Gardens | Food cultivation and domestic life | Light timber balconies and terraced retaining walls | Deep vegetation, irrigation lines, open views | Shared work, growing experiments, water repair |
| High Step Junction | People and goods moving uphill | Civic supports with inhabited infill | Transit platforms and crossing levels | Delivery transfers, route boards, waiting places |
| Civic Shade | Allocation, records and permits | Castwork, stone and controlled glazing | Broad calm masses and deep formal entries | Queues, exceptions, maintenance priority |
| Collector Ridge | Mirror-light receiving and servicing | Strong anchorage and exposed mechanisms | Large articulated reflectors and service paths | Calibration work, worn access rails, reserved parts |
| Open Wake Dock | Ship maintenance and departure | Service shells over old quay foundations | Cradles, docking arms and vessel silhouettes | Crew habits, rescue equipment, negotiated access |
| Mirror Ring | Orbital maintenance and transfer | Pressure shells and open orbital frames | Large repeated collector segments | Isolation, repair logistics, custody controls |

Each neighborhood needs one main economic function, a second function that complicates it, and evidence of the people who maintain it. Complete at least one doorway, service path and backstage space for every hero frontage. Avoid a street made entirely of facades with no plausible rooms behind them.

## 5 Palette and material system

At the first market's daytime review, aim for roughly 60 percent warm mineral and plaster surfaces, 20 percent dark timber and recesses, 10 percent indigo textiles, 7 percent copper and brass accents, and 3 percent emissive or vivid focal detail. These are composition targets, not asset-by-asset quotas. The Crown can be lighter; service yards can contain more metal.

| Material family | Surface construction | Wear logic | First application |
|---|---|---|---|
| Sandstone | Visible bedding, restrained granular variation, separate mortar | Step noses round; water stains descend from outlets | Quay and arcade bases |
| Lime plaster | Layered coverage with shallow waviness | Damage exposes coherent substrate at impacts and wet zones | Shop infill and domestic walls |
| Oiled timber | Grain follows the member, joinery remains legible | Contact edges polish; end grain absorbs more | Benches, posts and shutters |
| Copper | Seams, folded edges and fixings | Oxidation follows exposure; handling polishes selected areas | Roof details and machinery |
| Brass | Cast or machined sections, distinct finish from copper | Grip wear, recess dirt, readable machining | Tools, lamps and fittings |
| Painted steel | Coherent coating thickness and welded construction | Edge chips expose metal; leaks have a source | Industrial cabinets |
| Ceramic | Glaze over a plausible body with visible rim thickness | Foot wear, glaze variation, localized chips | Household vessels |
| Woven fiber | Warp and weft at usable scale | Tension, seams, hems, patches and fading | Shade sails and upholstery |
| Glass | Consistent thickness and restrained tint | Dust and handling at sensible locations | Lamp diffusers and shop windows |
| Rubber and seals | Matte deformation-friendly profiles | Compression and edge abrasion | Pressure systems and grips |

Use a small set of project master materials: opaque hard surface; layered architectural surface; cloth; foliage; glass; emissive light housing; decal; and specialist water. Add a new master only when behavior requires it. Material instances supply color and surface values. Shared tiling surfaces and trim sheets handle repeated detail; unique textures belong to unique storytelling or hero shapes.

Starting texture-density targets are 256 pixels per meter for ordinary architecture, 512 for inspected foreground surfaces and up to 1,024 for selected small hero details. Inspect these targets against actual camera distance. Start with 1K small props and 2K general textures; reserve 4K for justified hero surfaces. Never load 8K vendor maps by default. Keep source maps outside the cooked build and generate reviewed working resolutions reproducibly.

Trim sheets cover masonry borders, timber moldings, industrial panel edges, brass fitting bands, embroidered textile borders and civic motifs. They reduce unique texture work without eliminating mesh silhouette detail. A bevel that affects the outline is geometry; tiny repeated surface relief can be a normal map. Close-up damage needs a coherent substrate, not a generic noise mask spread equally over everything.

Indigo Monsoon remains the later rain/night reference. Author roughness and sheltered/exposed surfaces so they can support a wetness layer, but do not add real-time weather, puddle accumulation or cloth simulation to every object now.

## 6 Architectural modules and assembly interfaces

The starting module grid is 10 cm, with a 50 cm planning subdivision. Common bay widths are 200, 300 and 400 cm; normal story heights begin at 300 or 400 cm. These are new content conventions to validate against the existing player capsule and current map. They are not instructions to rescale the existing city blindly.

An architecture module uses a lower corner or documented central axis as its pivot. The pivot, forward face, up axis and snap plane must be consistent across the family. Every exception includes a preview and socket metadata. Straight modules meet on exact planes; uneven stone and plaster cover the joint visually without creating traversal snags.

| Interface | Contract |
|---|---|
| Wall to wall | Matching end planes, correct thickness, explicit inside/outside corner pieces |
| Wall to floor | Floor elevation socket and a substrate intersection that cannot reveal daylight |
| Arch to pier | Defined spring height and bearing depth; span changes use authored geometry |
| Door to opening | Frame envelope, hinge socket, sweep clearance, latch socket and threshold collision |
| Window to wall | Separate reveal, closure and external shade; interior thickness remains visible |
| Roof to wall | Bearing surface, eave line, drainage direction and access route |
| Stairs to landing | Exact riser accumulation, continuous walking collision and rail termination |
| Bridge to abutment | Bearing sockets, guard continuity, expansion detail and clearance volume |
| Utility to structure | Mount points, maintenance envelope and valid cable or pipe route |
| Destructible insert to fixed frame | Intact seam, thickness, interior cut surface and stable damaged-state footprint |

Use deliberate module families for curves, slopes and irregular corners. Arbitrary nonuniform scale can stretch stone, ruin snap dimensions and distort collision; validate it only where the family explicitly supports deformation. Apply authoring transforms before export. Unreal units are centimeters; use a known one-meter reference and an orientation test at every new import route.

The generator must keep structural, facade, ornament, service and interaction layers separable. A wall's fixed frame can share an instance batch while its door is a separate interactive actor. A collection of small decorations can use a static arrangement only when none of its components needs independent state.

## 7 Assemblies that build a lived city

An assembly is a reusable arrangement of existing assets, not an additional unique mesh for the 4,200 target. Store an assembly definition, child keys, local transforms, source references, collision policy, snap sockets, clearances, placement tags and independent object spawn markers. Preserve authored child keys across edits. A placement key plus a child key produces stable instance identity; an array index does not.

| Assembly family | Required layers | Controlled variation |
|---|---|---|
| Shopfront | Structural bay, shutters, counter, sign, task light, stock, waste path | Trade, frontage width, opening state, stock arrangement |
| Workshop | Work surface, storage, active task, tools, power, service access | Occupation, repair stage, owner history |
| Shared kitchen | Preparation, heat, water, serving, seating, cleaning | Meal scale, queue direction, shaded occupancy |
| Domestic balcony | Access, guard, shade, storage, plant or seating | Household use and exposure |
| Garden terrace | Soil depth, containment, irrigation, access, shade | Crop stage and water allocation |
| Dock service point | Mooring, power, cargo handling, safety clearance | Vessel size and service purpose |
| Civic queue | Entrance, schedule, access controls, shelter, waiting | Public capacity and allocation status |
| Utility repair | Open service point, tools, replacement part, barriers | Work type and completion stage |
| Quiet story space | Personal object, evidence of recent use, sightline | Specific occupant or event |
| Vista frame | Foreground scale cue, midground activity, distant landmark | Camera height and approach direction |

The Second Dawn workshop is the first assembly prototype: an authored data asset places 46 parts using 11 new mesh designs. The source build and 14 automation tests pass after repairing a test setup error. It is not yet evidence of final visual quality, and its decorative props do not yet implement carry or inventory behavior.

Generate arrangements from a deterministic seed and tagged valid combinations. Keep the authored layout after acceptance; regenerate only selected children or a new reviewed version. Artist overrides must survive a rerun. Define exclusions: door sweeps, stair treads, drainage channels, player clearances, quest access and workshop reach zones.

For a market, alternate the spatial rhythm: broad active workshop, narrow quiet frontage, open passage, crowded food counter, planted recess. Randomly selecting a different pot does not solve repetitive frontage widths, identical canopies or identical lighting. Review repetition at three scales: silhouette from the opposite quay, shop rhythm down the street, and hand-level detail at a counter.

## 8 The first block as a quality proof

Concentrate the next art pass on a connected street of approximately 60–90 meters, subject to the existing mission route. It needs a strong approach view, one excellent workshop, an adjacent contrasting trade, a courtyard or side passage and an unobstructed route to Kestrel. Finish one 10–15 meter section to the target before extending the treatment down the street.

The initial production assortment should include:

1. One deep, finished arcade bay with coherent pier, arch, cornice, wall thickness, drain and floor edges.
2. The Second Dawn bench, cabinet, lamp family, optical parts, trade sign and coherent work-in-progress arrangement.
3. A contrasting food frontage using existing scanned bowls, cookware, produce and custom storage arrangements.
4. One domestic upper level with plausible room depth, shutters, laundry or plants and maintenance access.
5. A completed quay edge, mooring point, public light and view of the Keeper.
6. Ten independently movable test objects, including a cup, bowl, lantern, hand tool, cell, small crate, basket, bottle, book and soft bundle represented initially as a rigid object.

The catalog wave labels organize library expansion; this assortment overrides numerical order. Do not model eight crate designs before the street has a convincing doorway, counter and lamp. Map existing suitable assets into the catalog first, with a record of what still needs improvement.

Acceptance requires actual game captures: approach, walking eye level, counter inspection, alley reverse view, interior threshold and elevated view. Use the same camera positions and lighting for comparisons. Inspect silhouette, material response, contact, believable construction, repetition, collision and performance. A generated benchmark image remains a target, never a claim of gameplay footage.

## 9 Interaction capabilities

Interaction is a combination of capabilities, not a single all-purpose label. Inspection, use, physical movement, collection, containment, damage and mission custody can vary independently. A cup may be carried and collected. A valve can be used but stays attached. A crate may move physically and contain items while remaining too large for inventory. A mission core can be carried only through its defined custody transitions.

| Profile | Physical behavior | Collection behavior | Authoring requirements |
|---|---|---|---|
| FIXED | Static geometry | None | Snaps, traversal collision, correct support |
| FIXED_USE | Fixed actor with an explicit action | None by default | Reach point, use socket, state display |
| HINGED_USE | Constrained movement or controlled animation | None by default | Hinge axis, limits, sweep and latch clearance |
| CARRY_COLLECT | Independent rigid object; carry and throw when allowed | Eligible through a later item definition | Complete surfaces, convex collision, mass, grips, persistent ID |
| HEAVY_MOVE | Push, drag or constrained tool-assisted move | Not pocket inventory by default | Stable mass, broad contact, recovery bounds |
| MISSION_CUSTODY | Explicit mission state owns it | Mission-specific transfer | Atomic custody, recovery rules, no decorative duplicate |
| CONTAINER | Container capability layered over another profile | Transactional contents | Closure, access socket, persistent content identities |

The catalog profiles are defaults. They require per-object tuning before implementation. A heavy tank vessel must not be throwable merely because its family is eligible for collection; size and mass gates override the default. Large props that fail the carry envelope become heavy-move or fixed-use objects while retaining their definition IDs.

Inventory layout, slot rules, weight limits, stacking, economy, theft response and item rarity are deferred. Create clean item-definition references and transfer interfaces now so those decisions do not force a mesh or identity rebuild later.

## 10 Physical object authoring contract

Every movable object needs a finished underside, back, rim, interior where visible, and contact surface. Players will rotate it and put it on a shelf. A scan with a missing underside, a paper-thin vessel or a fused collection of merchandise must be repaired or classified as static scenery.

The render pivot should make placement predictable, usually at the base center for standing props. Store grip sockets separately. Physics center of mass is a physical property and need not equal the render pivot. Document default upright orientation, rest faces and intended hand position. A long tool needs a grip location and rotational limit that keep it out of the camera.

Use simple or convex collision for simulated objects. Choose a small number of hulls that preserve useful contact and cavities without excessive cost. A bowl may need a small convex decomposition to support stacking; its outer bounding box alone is insufficient. Unreal's Use Complex Collision As Simple setting does not allow that object itself to simulate physics. [Epic collision reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine)

| Metadata | Required meaning |
|---|---|
| AssetDefinitionId | Stable design identity; independent of filename and display name |
| InstanceId | Stable identity of one placed or spawned object |
| Capabilities | Inspect, use, carry, throw, collect, contain, damage |
| MassKg and mass policy | Authored mass or validated density-derived mass; no blanket engine default |
| Grip sockets | Position and orientation for holding; optional second hand |
| Physics profile | Collision responses, damping, sleep, CCD eligibility, maximum velocities |
| Surface profile | Impact sound, sliding sound, friction and restitution intent |
| Item definition reference | Optional bridge into future inventory |
| Ownership and custody | World, held, inventory, container, socket, destroyed or recovery |
| Save policy | Durable object, lightweight moved-object delta, disposable ambience or mission-critical |
| Recovery bounds | Allowed region, fallback anchor and quest recovery behavior |
| Destruction profile | Durable, state swap, breakable, separable assembly or selected structure |
| Source and revision | Provenance, geometry hash and definition version |

Start tuning ordinary handheld props around 0.1–8 kg and a nominal 2 m reach. These are design starting points, not final eligibility rules. Long, heavy or dangerous-looking objects may have distinct handling even within that mass range. Use mass and extent together. Set throw impulse and angular velocity limits from playtesting; do not multiply force indefinitely by how long a button is held.

Use a physical handle or constrained target so held objects still encounter geometry. Epic's Physics Handle supports grabbing and moving a simulating object; Physics Constraints support joints such as a swinging sign. They supply mechanics, not a complete interaction or persistence system. [Epic physics components](https://dev.epicgames.com/documentation/en-us/unreal-engine/physics-components-in-unreal-engine)

Holding must handle obstruction, release, player death or reset, menu pause, save/load and scene transitions. A held object should not pull the player through geometry or launch the player by intersecting the capsule. Disable or constrain the object-to-player response during holding while preserving world collision. Define whether standing on held objects is allowed; the initial slice should prevent that exploit.

Small fast-moving objects may need CCD after measurement. Avoid enabling expensive collision and continuous ticking on every prop. Sleep stationary objects, use event-driven interaction queries and limit active simulation by interest and relevance. Do not silently remove an object the player is holding, watching or using for a mission to satisfy a budget.

## 11 Identity and persistence before inventory

A proposed object record contains definition ID, instance ID, definition revision, custody state, owning sector or container, transform, relevant velocity, interaction state, damage state and a monotonically increasing transaction revision. Serialize content identifiers and values, not raw actor pointers. Current mission custody must remain the authority for the sunseed until a deliberate migration unifies the systems.

Exactly one location owns an object at a time. Collection follows validate, reserve, commit, then update presentation. If the later inventory refuses the transfer, the world actor remains. After a successful commit, the world representation is suppressed and the save records that absence. On reload, a collected object must not respawn from its original arrangement.

Containers follow the same rule. Moving a container does not create copies of its contents. Opening a crate and spawning visible items is a representation change, not a new ownership event. A spawn request includes a unique operation ID so repeating it after a load cannot duplicate loot.

For large repeated scenes, decorative instances may be promoted to actors on interaction only if the placed item has stable identity and authored interaction metadata. Promotion removes or hides precisely that instance, creates the actor and transfers state atomically. Demotion reverses the representation only when safe. Never use a HISM array position as persistent identity because removal and regeneration can reorder it.

Saving motion requires an explicit policy. Ordinary objects can restore at a validated resting transform with zero velocity. A held object can restore in a designated safe hold or drop state. Mission objects need guaranteed recovery anchors. Do not promise deterministic replay of arbitrary rigid-body motion.

Sector unload must refuse to discard held or transaction-reserved objects. Move custody to a persistent owner or complete a safe transfer first. Store ordinary moved-object deltas and removal tombstones by stable ID. Regenerating the static street must reapply those deltas rather than reset the player's changes.

## 12 Destruction as a staged vision

The recommended long-term scope is rich local destruction and selected structural changes. Universal building collapse would change the game's foundations: mission access, NPC navigation, shelter interiors, utilities, streaming, saves and performance all need to survive arbitrary topology changes. That is a separate production commitment, not a switch to enable late in development.

Chaos supplies pre-fractured Geometry Collections, clustering and constraints for controlled breakage. It can integrate effects and audio, and cached simulations suit authored set pieces. Those facilities do not automatically preserve quests, rebuild navigation or save a ruined city. [Epic destruction overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/destruction-overview)

| Tier | Content scope | Recommended mechanism | Persistence |
|---|---|---|---|
| D0 | Durable tools, protected landmarks, fixed foundations | Impact response or cosmetic marks only | Normal object state |
| D1 | Cups, bottles, crates, small lamps, selected screens | Intact-to-broken state swap; bounded fragments or small Geometry Collection | Broken state plus persistent gameplay result |
| D2 | Stalls, shutters, railings, hanging signs, machinery covers | Authored separable parts, constraints and selected fracture | Named component states and final blockers |
| D3 | Chosen wall panels, bridge sections, special structures | Fractured insert within a designed structural boundary | Topology state, access state, utility state, stable rubble collision |
| D4 research | Arbitrary structural collapse throughout the city | Support graph, systemic damage, broader simulation and extensive world logic | Persistent structural graph with migrations |

Catalog values such as D1_BREAKABLE and D3_SELECTED express eligibility, not a promise that every entry receives that implementation. Most buildings remain fixed. The first destruction proof should use six objects with different materials and one bounded wall insert, in an isolated map after carry/save behavior is reliable.

For a solo developer with the current foundation, a small breakage prototype is plausibly several focused days; a polished limited prop set with effects, audio, persistence and tests is more plausibly several weeks. Selected structural destruction can add multiple months once navigation, mission recovery, art and save migration are included. Citywide collapse is an open research and production program with no credible delivery date yet. These are planning ranges, not estimates derived from measured project throughput.

### Prepare geometry now

Keep load-bearing structures, nonstructural infill, doors, windows, attachments and loose contents separate in source files. Add thickness and believable interior material where a break may expose it. Avoid fusing a roof, hanging sign, lamp and cabinet into a single irreversible mesh. Use common attachment sockets and document what should detach from what.

Geometry intended for fracture should be watertight and avoid problematic intersecting source pieces. Epic identifies open and intersecting geometry as causes of poor or unpredictable simulation. Hollow vessels require closed wall volume, not filling the entire vessel with solid collision. [Epic Geometry Collection authoring guidance](https://dev.epicgames.com/documentation/en-us/unreal-engine/geometry-collections-user-guide)

Fracture should follow material: ceramic produces sharp shell fragments, wood follows member joints and grain, metal bends or detaches through authored states, cloth tears through a later specialist treatment. Uniformly applying a rock-like fracture to every object destroys material credibility. Interior cut surfaces need their own material response.

### Preserve useful results rather than every shard

Separate gameplay debris from cosmetic debris. A broken crate's released item has a persistent ID and custody transaction. Most splinters and dust expire after a measured lifetime. A wall opening and its blocking rubble persist as named states. For settled structural debris, an authored stable damaged mesh and collision can replace transient simulation once the transition is no longer visible.

Never claim deterministic physics reconstruction from a seed alone. Preserve important final transforms or switch to a defined aftermath state. Keep source fracture revisions compatible with saved damage states; provide migration when chunk layouts change. Do not let rebuilding fracture geometry resurrect contents or remove a mission route.

A destructible wall requires both sides to exist, a meaningful destination behind it, valid navigation updates or authored alternate routes, audio/lighting behavior, and a rule for what remains after reload. Destruction that opens an unbuilt void is not finished. Structural support, collapse propagation and ship impacts remain later design decisions.

Streaming and distant representations must also respect destruction. An intact packed assembly or HLOD must not reappear at distance after its wall was removed nearby. Keep selected mutable sections outside immutable merged geometry, or provide state-aware replacements. Test visibility, shadows, occlusion and indirect lighting in both intact and damaged states. Reapply saved damage before revealing a streamed section; a visible intact-to-ruined pop on every return is not acceptable.

Begin with strict per-event fragment budgets, short-lived cosmetic dust and a cap on active nearby debris bodies. Initial laboratory budgets might be 10–20 meaningful chunks for a small prop and 30–80 for a selected panel, then measured down or up on the target hardware. These are project experiment settings, not engine limits or a performance guarantee. Disable chain reactions until a bounded scene proves they are affordable and coherent.

## 13 Mesh and rendering budgets

The current development machine has approximately 16 GB RAM and an RTX 4070 Laptop GPU. Intake must remain selective. A vendor package can overwhelm memory through texture resolution and dependencies even when its geometry renders well. Keep an empty import project or isolated intake area on the pinned engine and bring across only qualified dependencies.

| Asset role | Initial working triangle range | Initial material-slot target | Review distance |
|---|---:|---:|---|
| Small repeated prop | 500–8,000 | 1–2 | 0.5–3 m |
| Inspected tool or vessel | 3,000–25,000 | 1–3 | 0.2–1.5 m |
| Furniture | 5,000–40,000 | 1–3 | 0.5–5 m |
| Architectural module | 2,000–60,000 | 1–4 | 1–20 m |
| Hero component | Determined by silhouette and measured cost | Reviewed individually | Ground and flight |
| Distant mass | Simplified specifically for its screen size | 1–2 | Skyline |

These are review bands, not hard triangle caps or proof of quality. Nanite may suit many rigid detailed meshes, but it does not eliminate material, shadow, overdraw, collision, texture or actor costs. Use a measured configuration appropriate to the pinned engine. Keep foliage, transparency, moving constraints and fracture under their own qualification tests.

Repeated static geometry should use compatible instancing or packed static assemblies. Interactive objects remain separately addressable. Collision and save metadata must not vanish when optimizing a street. Avoid a Blueprint actor per decorative screw, persistent Tick on idle props, and a unique dynamic material instance on every repeated object.

The playable target remains a measured 1080p/60 FPS experience at the defined quality preset. Record frame pacing, CPU and GPU timings separately where available, memory pressure, shader compilation state and traversal hitches. A 60 FPS cap alone can hide spare capacity or a bottleneck. The older packaged benchmark applies to that build only; new dense art and physics require a new packaged measurement.

## 14 Asset production pipeline

| Stage | Required output | Exit condition |
|---|---|---|
| World brief | Purpose, district, construction and reference | Object belongs in the city and fills a catalog need |
| Reuse check | Existing-library search and donor candidates | No unnecessary duplicate production |
| Blockout | Dimensions, silhouette, pivots and use points | Looks correct beside player and neighboring modules |
| Exemplar | One polished representative of the family | Approved in neutral light and target scene |
| Author or adapt | Editable source, structured materials, complete surfaces | Geometry and finish satisfy the brief |
| Gameplay preparation | Collision, sockets, capabilities, mass, damage boundaries | Interaction test pad passes relevant checks |
| Import | Engine-authored asset packages and deterministic manifest | References survive editor restart and cook |
| Assembly | Clearances, child keys, variation and placement | Route remains navigable and legible |
| Runtime review | Matched captures and measured play | Visual, interaction and performance criteria pass |
| Register | Source, hashes, decisions, costs and evidence | Record marked accepted with traceable proof |

Choose the smallest capable tool chain: Blender for geometry, the existing Python authoring scripts for repetitive work, Unreal commandlets and editor APIs for import and assembly, and Git plus the current backup process for source control. The Blender source is the authoritative editable geometry; the importer owns generated runtime packages. Avoid adding Houdini, a new material system or an asset-management service until a measured repetitive task justifies its integration cost.

AI is useful for research, brief expansion, script generation, naming checks, geometry automation, material variants and systematic review. It is not the final authority on scale, construction, collision or artistic quality. Render each family exemplar in the game, compare it with the benchmark and correct it before producing its variants. A generated image can guide a motif; it is not a collision-ready model or evidence of a completed environment.

For procedural meshes, store parameters, script revision, source seed and output hashes. Use deterministic export names. Generate into staging, validate all outputs and then import; a partial generator failure should not overwrite the last accepted family. Every new import path gets a one-object orientation and scale trial before batch use.

For purchased assets, retain the original source path and version. Adapt through a project-owned wrapper, material conversion or an editable derivative whose lineage is recorded. Preserve the original separately where storage permits. Avoid editing the only copy of a vendor package. Unreal package moves must use Unreal's tools so references and redirectors can be handled correctly.

## 15 Source control and workspace layout

The existing workspace already has build, test, preflight and recovery scripts. Extend it with content contracts rather than replacing it with a new framework. Use `Tools/Preflight.ps1`, `Tools/Build.ps1` and `Tools/Test.ps1` for the relevant checks, and preserve engine pinning.

| Location | Responsibility |
|---|---|
| SourceArt/Original | Editable geometry, textures and generator sources for new owned families |
| SourceArt/PolyHaven | Existing open-source model and material intake with hashes |
| SourceArt/Vendor | Selected licensed source assets where the agreement and repository distribution permit |
| Game/Content/BorrowedSun/World | Project-owned runtime asset organization as the library grows |
| Game/Content/BorrowedSun/Market | Existing market assets; migrate only through a reviewed Unreal operation |
| Docs/Art | World grammar, family briefs and visual acceptance |
| Docs/Registers | Catalog, source candidates, intake, costs and decisions |
| Tools/Art | Reproducible generators and import support |
| Artifacts | Ignored runs, temporary exports, captures and validation logs |
| Vault | Private receipts and agreements; independently backed up |

Large binary source and runtime files follow the repository's LFS policy. Do not add DerivedDataCache, Intermediate, Saved or generated packaged builds to source control. Keep dependencies and purchased source out of public distribution when their terms require it. The catalog and original tooling can be reviewed without uploading vendor geometry.

One Unreal process owns binary content during writes. Separate code work may be isolated, but concurrent asset writers are not a speed improvement. Review and commit one coherent content outcome. Verify recovery hashes and periodically restore onto independent storage; another folder on the same disk does not protect against disk loss.

Keep machine paths in local configuration. A build manifest records source revision, asset-generator version, engine version, import manifest and package evidence. Preview images link to the exact build that produced them. Do not replace the verified player build with an unqualified source change merely to make a new asset immediately visible.

## 16 Validation that protects quality

Automate objective failures: missing files, invalid dimensions, duplicate IDs, broken references, malformed transforms, unsupported materials, missing collision and changed source hashes. Human visual review remains necessary for repetition, construction plausibility, composition and style. The user reviews the strongest current evidence; the process should not repeatedly ask them to approve trivial export settings.

| Test | Required result |
|---|---|
| Snap matrix | All intended module adjacencies fit with no visible gap or traversal snag |
| Surface turntable | Front, back, underside, interior and contact surfaces hold up at interaction distance |
| Scale and grip | Object reads correctly beside a person and does not obscure the camera when held |
| Collision drop | Representative objects settle without tunneling, jitter or unnatural support |
| Stack and contact | Vessels and crates rest plausibly; inaccessible cavities are intentional |
| Carry obstruction | Object stops or safely releases against walls and doorways |
| Save and reload | Moved, collected and contained objects retain exactly one identity and location |
| Placement regeneration | Existing saved changes survive assembly edits and deterministic regeneration |
| Destruction reload | Broken state, contents and route access persist without duplicated rewards |
| Editor restart and cook | Assets and material references load from disk and survive packaging |
| Walking and flight route | First Light remains traversable and legible from both modes |
| Performance traversal | Dense street and object pile meet measured budgets without transient overload |

The first physical-object test map contains ten props, a table, shelves, stairs, a narrow doorway, a recovery volume, a container stub and one save boundary. Test pickup, rotation, drop, throw, obstruction, stacking, sleep, collect refusal, collect success, reload, reset and mission-core protection. The inventory stub accepts or rejects transfers without choosing the final inventory UI or economy.

The later destruction test map adds six material types and one bounded wall insert. Verify break thresholds, audio, fragments, repeat impacts, collection during damage, destruction during holding, content release, save/reload and a future streaming boundary. Test disabling the feature without invalidating saved object definitions.

## 17 Kit selection and procurement

The companion shortlist contains direct download or purchase pages, current public price observations, intended catalog coverage and qualification steps. Selection is based on artistic fit, useful components, editability, import cost and measured runtime suitability. **AI-related restrictions do not automatically disqualify a candidate**, following the user's direction on 13 October 2026. Record the published terms separately; that preference does not itself change a vendor agreement.

The first comparison should place a representative item from each serious candidate beside the same custom shopfront and existing CC0 props. Compare material finish, scale, collision, separability and the effort to make it belong in Orison. Do not acquire several overlapping market packs before making this comparison.

KitBash's earlier Kits 4.0 release and its current Gameplay Ready products are different milestones. Confirm the actual package/version. Gameplay Ready listings describe Nanite, custom collisions and packed level instancing, but those do not supply our carrying, collection, persistence or destruction design. [Kits 4.0 announcement](https://kitbash3d.com/a/blog/kits-4-0-now-available-change-the-game) · [Gameplay Ready announcement](https://kitbash3d.com/a/blog/introducing-gameplay-ready-kits-built-for-real-time-performance-in-unreal-engine)

The initial under-€500 ceiling remains. The existing €480 planning envelope reserves €240 for memory, storage and independent backup, €120 for qualified assets or tools, €40 for operations and €80 as buffer. These are allowances, not spending commitments. Current recorded spending is €0. A kit's USD display price is not its final EUR cost with tax. No subscription or bulk purchase is assumed.

The preferred immediate stack is the existing open assets and custom kit, selected free Minerva/Neo City/Luminara components for evaluation, and **one** paid market-kit candidate after price and component qualification. Medieval Market is the first paid benchmark candidate because its listing exposes useful model information and a Gameplay Ready package. Market Street and Desert Bazaar remain direct competitors; Shangri-La remains a selected roof and skyline donor, not the city template.

## 18 Additional asset systems outside the mesh count

The city also requires non-mesh work. Track it explicitly rather than hiding it inside a model total.

| System | Initial library direction | Acceptance |
|---|---|---|
| Surface materials | Ten core families with district variants | Consistent scale, lighting response and texture memory |
| Trim sheets | Six initial construction/ornament sets | Reusable UV bands without visible repetitive artifacts |
| Decals | Repair marks, labels, water trails, public notices, wear | Meaningful placement, readable text, limited overdraw |
| Signs and graphics | Eight first-block trades plus civic allocation system | Coherent typography, spelling, hierarchy and original motifs |
| Assembly recipes | Workshop, food stall, domestic balcony, quay service | Stable children, clearances and controlled variation |
| Sound | Material impacts, handling, mechanisms and neighborhood ambience | Distinct weight/material response and bounded voice counts |
| Effects | Dust, warm light, steam, water, sparks and later fracture | Good readability and measured transparency cost |
| Animation | Door, shutter, fan, valve, collector and later carry presentation | Correct pivots, event timing and interaction state |
| Lighting | Daytime reference rig and later wet/night rig | Stable exposure and reproducible comparison captures |
| UI support | Interaction labels and later item icons | Readable affordances without committing inventory design |

Characters, creatures, complex vehicles, combat, dialogue and full animation systems need dedicated production specifications. This worldbuilding catalog includes vehicle shells and signature components, not a claim of 200 fully operational vehicles or characters. No NPC crowds or simulated economy are implied by a detailed marketplace.

## 19 Production waves and effort control

The generated catalog currently contains 160 W0 records, 1,000 W1 records, 2,400 W2 records and 640 W3 records. These waves are a library planning aid, not a schedule. The production assortment in section 8 takes priority over family ordering.

- W0 First block: establish exemplars and a varied market assortment, qualify object movement and produce matched in-game evidence.
- W1 Neighborhood: build coherent side streets, rooms and service spaces using proven families; expand only the trades the playable area needs.
- W2 District library: extend civic, garden, dock and utility families after a second neighborhood proves reuse without visual sameness.
- W3 Extended library: add remaining construction profiles, orbital environments and specialty content when the game needs them.

Track accepted assets per focused hour, rework rate, time to import, cost to place, material-slot distribution, dependency size and player-visible variety. Measure these on the first 20–30 representative assets. Any completion estimate before that is speculative.

For illustration only, an average of two focused hours per accepted asset would mean 8,400 hours for 4,200 assets; at 20 hours per week that is 420 weeks before unrelated game work. Heavy reuse and purchased assets can reduce some of that effort, while hero assets can take much longer. The practical speed strategy is to satisfy gameplay spaces with a small, excellent library first and grow it from measured needs.

Keep one active content outcome and one independent research task. Batch repetitive exports after the exemplar is approved. Avoid generating hundreds of models that share an uncorrected shape or material defect. A library entry can be fulfilled by a qualified donor, an original mesh or a new construction variant; the source route is not the quality measure.

## 20 Decisions that are fixed now and decisions kept open

The project proceeds with original Brass Sanctuary identity, the existing Orison premise, a hybrid source strategy, stable object identities, separable interactive props and a long-term target of at least 4,000 accepted designs. The library specification deliberately includes future breakage boundaries.

Inventory UI and economy, systemic theft reactions, combat damage, arbitrary building collapse, cooperative multiplayer and full-city streaming remain separate decisions. Preserve clean interfaces without implementing speculative frameworks for all of them. A capability flag is cheaper and more useful now than a half-finished inventory or destruction system attached to every mesh.

The next implementation outcome is concrete: **a finished Second Dawn frontage next to a contrasting trade, with ten representative props that can be carried, dropped, thrown and saved without duplication**. Collection can be proven through a transfer stub. After that, prototype bounded breakage on the same objects and compare the effort and performance before committing to larger destruction.

## 21 Delivery and evidence

The offline catalog and its JSON companion are generated from `Tools/Art/Build-WorldCatalog.py`. The complete form index is `Docs/Art/WORLD_ASSET_FAMILIES.md`. The kit register and shortlist preserve source links and distinguish prices from assumptions. No paid kit was purchased or imported for this specification.

Existing market assets remain existing assets until explicitly mapped and qualified against catalog definitions. The catalog starts with zero accepted entries to avoid silently certifying old meshes against new interaction and destruction requirements. This does not mean the playable project has no models.

The latest workshop source build and 14-test automation pass establish compilation, persisted assembly references and data validation. A new packaged visual review and physical-object implementation remain the next production work. The last verified v0.4 player package is retained for comparison.
