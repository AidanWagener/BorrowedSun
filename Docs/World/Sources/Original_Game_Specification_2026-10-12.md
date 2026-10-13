# Borrowed Sun game design and production specification

Version 1.0 | 12 October 2026 | Original concept for a solo creator using Unreal Engine, AI assistance and licensed assets

## 1 The decision

Build **Borrowed Sun**, a third person science fiction pirate adventure about stealing back a city's daylight. A small crew operates a battered courier ship in a vertical port beneath an orbital mirror. Every major job begins with a human problem in the streets, becomes a physical object or person you must transport, and ends with a decision that changes the city you fly back into.

The distinguishing mechanic is the **sunseed**: a stolen authorization core that can redirect the mirror's power. It is a persistent mission object, a tool, a source of pursuit and a political choice. Plug it into a clinic to restore power and open a route; activate it aboard your ship to help your systems while emitting a detectable signal; authorize a service and the neighborhood changes. Its fiction connects street interaction, heists, flight and consequence through one readable system.

Preserve both parts of the Beyond Good and Evil 2 appeal from the first integrated prototype: intimate life in a multicultural city and piloting your own ship out of its atmosphere. A single excellent expedition must contain both. The full universe remains a funded expansion path with its own engineering gates. A solo release needs a complete, bounded story and a proven production rate.

This specification defines a creative north star, functional contracts, production sequence and research basis. All Borrowed Sun designs, staffing assumptions, costs, durations and numerical targets are proposals to validate. They are not measured Unreal benchmarks, vendor promises or a commitment that one developer can produce an AAA universe. Awards are an aspiration; the controllable goals are coherence, surprise, emotional specificity and execution.

**Recommended immediate commitment:** a twelve week engineering and design prototype, followed by a separately approved production decision. Do not buy a catalog of city kits before the representative block, flight and packaging tests pass.

## 2 The game in one page

**Pitch.** In a city that rents its daylight, a debt courier steals the key to the artificial sun. Build a crew, pull impossible jobs from market alleys to orbital cathedrals, and decide who gets to see morning.

**Player fantasy.** You know the back doors, the people who repair your ship and the patrol routes above your home. You can leave the city behind without becoming detached from its fate. Your expertise, promises and relationships matter more than collecting hundreds of weapons.

**Genre and camera.** Third person action adventure with compact immersive mission spaces, assisted arcade flight, light crew progression and authored systemic consequences. The ship uses a chase camera by default. An optional cockpit view is a later polish feature. Single player and offline for the first commercial release; Windows PC and controller plus keyboard and mouse are the initial platform contract.

**The core loop.** Hear a problem, investigate it, choose an approach, infiltrate or negotiate, extract a consequential payload, fly through a changing pursuit, allocate the reward or power, and return to see who benefited. A 10 to 25 minute job sits inside a 45 to 75 minute story chapter. Exploration supplies information, shortcuts, equipment choices and relationships that alter later jobs.

**The emotional promise.** The skyline is somebody's home. You should recognize a light coming on in a distant window because you helped the person living there.

**The signature trailer moment.** The player carries a glowing core through a dark market, leaps across an interrupted lift, boards a ship hanging beside a balcony, threads a solar collector canyon, climbs through cloud into the mirror's shadow and turns to see the powerless district below. Later, that district lights up as its residents sing over the radio. All footage must come from the actual playable build.

**What earns distinction.** One unusual mechanic that works at multiple scales; readable vertical geography; a small cast with contradictory loyalties; a tactile ship that feels like home; a recognizable musical identity; and consequences expressed through places, people and sound. Adding more square kilometers is a secondary ambition.

## 3 Creative pillars and decisions

| Pillar | Required player experience | Design test |
|---|---|---|
| One journey across scales | Street, rooftop, sky and orbit form one consequential expedition | Complete a job without losing cargo, pursuit context or motivation across a transition |
| People make the skyline matter | Named characters belong to recognizable places | Players connect at least two distant landmarks to people after the slice |
| Power is physical and political | The sunseed produces a useful immediate effect and a visible consequence | Players can explain who gains and who loses before a major allocation |
| A ship is a home and a tool | Flying and returning feel rewarding | Players voluntarily return to the ship between objectives |
| Authored problems allow improvisation | Multiple approaches interact through common rules | At least two supported solutions work without mission-specific cheats |

These pillars break ties. If a feature consumes weeks without improving one of these experiences, defer it. A new vehicle does not outrank better handling for the only vehicle. A second city does not outrank an emotionally effective first return. A cosmetic crowd does not outrank readable pursuit or a reliable save.

**Tone.** Warm, irreverent and politically alert, with danger that has consequences. Labor disputes, bureaucracy and resource control create conflicts, but ordinary people also eat, flirt, argue, celebrate and invent. Humor belongs to individuals and situations. Do not make an accent, a lineage or poverty the joke.

**Originality boundary.** Borrow the ambition of connected scales, lived-in science fiction and a pirate crew. Invent the setting, terminology, story, cast, silhouettes, factions, symbols, music and missions. Do not reproduce Ubisoft characters, hybrid designs, Ganesha architecture, religious monuments, logos, dialogue or trailer composition. Borrowed Sun is a working title; name and trademark clearance remain a release task.

## 4 World and cultural direction

### The port of Orison

Orison clings to the wall of a crater on a tidally locked moon. Its long twilight is moderated by an orbital mirror and industrial heat exchange. A concession company sells blocks of illumination, heating and power through neighborhood contracts. This is intentionally stylized science fiction: the mirror and power grid need coherent rules, not a claim of astrophysical realism. A compact glossary separates the mirror's light scheduling from the city's electricity and life support; switching one control does not magically fix every utility.

The city has three visual layers. The **Low Tides** are sheltered docks, markets, workshops and water storage. The **Ladder** holds residences, tramways, gardens and hanging construction. The **Crown** contains polished administration, collectors and the launch corridor. A large city is implied through vistas and traffic. The first release makes a small, meaningful portion navigable.

Three viewpoints carry the location: a crowded bridge under heat exchangers; a residential terrace where the mirror rises between towers; and the same crater seen from orbit. Keep the three views geometrically consistent. The player should be able to identify the port from space without a UI pin.

### Cultures have histories and work

Use maritime trade, repair economies, climate adaptation and migration as research themes. Develop a documented design reference library for real ports and diaspora communities, then transform those principles into fictional histories. Commission cultural review before production voice and hero art. References should explain how buildings are cooled, where people cook, how festivals occupy streets and why multiple languages coexist. A decorative mix of unrelated sacred imagery is not sufficient worldbuilding.

People share institutions without sharing a single culture. Neighborhood cooperatives, professional guilds, merchant families, labor movements and recent arrivals overlap. Clothes and interiors communicate profession, means, weather and personal taste. Scripts and signs require a consistent fictional writing system or reviewed translations; random glyphs and machine translated foreground signs fail the art gate.

### Engineered people

The setting includes human populations and two visibly adapted lineages: pressure workers with protective dermal plates and orbital technicians with lightweight support prosthetics. First release characters share a compatible humanoid animation skeleton wherever the silhouette permits. Lineage grants no fixed morality, nationality or intelligence. Individual beliefs follow biography. Broader biological diversity belongs to the full vision after a character pipeline has been proven.

This keeps the social theme of manufactured dependence while developing an identity distinct from BG&E2's human-animal hybrids. The fictional systems of ownership and labor can be discussed through contracts and lived consequences without using a people as a combat resource.

### Factions and conflicts

The **Concession** runs the mirror and argues that centralized control prevents collapse. Its local officials vary from sincere engineers to profiteers. The **Common Table** coordinates mutual aid but disagrees over whom to prioritize. The **Open Wake** is a network of independent captains; some help refugees, others sell freedom at extortionate prices. The **Night Choir** preserves unlicensed broadcasts and testimonies, including records that undermine the Concession's origin story.

Use three public faction relationships in the first release; the Night Choir operates as named contacts within the main story. A relationship is a small set of explicit flags and service states, not an invisible universal morality score. Never gate the only path to completion behind a faction that a valid player choice can permanently alienate.

## 5 Player and crew

The player is an established local courier using the callsign **Rook**. A fixed biography anchors dialogue and production. Appearance customization is limited to a small set of heads, skin and hair options, clothes and color accents. It should not demand arbitrary species rigs, fully reactive pronouns and dozens of cinematic body types before the story works. Voice selection and full character creation are expansion decisions.

Rook forged a service record to escape a labor contract. The forgery implicates someone who stayed behind. The inciting theft offers a way to help the city, but also tempts Rook to erase evidence of personal wrongdoing. The character can be kind, opportunistic or cautious within this established history.

| Crew member | Personal conflict | Gameplay role | Production boundary |
|---|---|---|---|
| Iona the engineer | Helped build the rationing network and wants to repair the damage | Restores one disabled mechanism or provides a bypass route | Ship station, radio and selected authored scenes |
| Venn the navigator | Keeps promising to leave but supports a family in the port | Reveals a hidden flight path or delays a scan | Route annotation and limited pursuit intervention |
| Seli the medic and broadcaster | Believes public truth matters more than the crew's safety | Negotiation evidence, survivor aid and alternate extraction | One in-person companion segment at most in the first release |

Each has a desire, a secret, a useful specialty, a boundary and a relationship to a place. Trust changes only through named decisions, promises kept or broken, and specific mission outcomes. Give the player a conversation explaining a fracture before a crew service changes. Crew never vanish permanently because of an opaque point threshold.

The ship is the recurring dramatic set. Three reusable stations, a table, a bunk and a cargo cradle support short scenes. A single interaction camera grammar and reusable gestures can carry most dialogue. Reserve bespoke animation and facial work for a few decisive moments. Avoid autonomous squad combat across every environment in the solo release; radio and station roles preserve character presence at far lower integration cost.

## 6 Story structure and mission grammar

### A complete first release

The first release targets about four hours on the main path and up to six with optional exploration. It contains an introduction folded into the opening job, six substantial story jobs, three crew episodes and optional discovery encounters. Planning arithmetic is 150 minutes of main jobs, 45 of crew scenes and play, 30 of investigation and meaningful returns, and 15 of finale and credits: approximately 240 minutes. Optional discoveries and alternate approaches add up to 120 minutes. These are pacing targets for testing, not a reason to pad travel. The jobs reuse a coherent port and orbital location through changed access, patrols, lighting and relationships. Reuse is valuable when it changes what the player knows or can do.

Act one introduces courier work and the theft of the sunseed. Act two exposes the infrastructure's dependencies and the crew's complicity; providing immediate relief creates a detectable energy signature and invites scrutiny. Act three forces a choice about who controls the mirror. Three ending configurations recombine the same final location and a small set of relationship scenes. Centralized reform restores reliable capacity quickly but retains surveillance and concentrated authority. Cooperative distribution gives neighborhoods a voice but begins with reduced output while shared maintenance is established. An independent network removes a single point of control but leaves uneven coverage and ongoing obligations for the captains who maintain it. Explain these costs through demonstrated events before the final decision. None is presented as a hidden perfect ending.

The campaign should answer its central question even if the player never buys another episode. A final distant transmission can invite more adventures without withholding the emotional resolution.

### Mission grammar

Every substantial mission uses the same contract: an intelligible need, a discoverable location, at least two approaches, one variable complication, a payload or changed person, a cross-scale consequence, and a return beat. Complications are authored choices from a bounded library rather than arbitrary objective replacement. Keep a clear critical path plus optional leverage.

| Mission | Human need and play | Reused systems | Persistent outcome |
|---|---|---|---|
| Borrowed Morning | Steal the sunseed from a relay during a shift change | Rerouting, combat or infiltration, flight | City power choice and wanted signature |
| A Light for the Clinic | Secure durable backup after the opening mission's temporary service lease | Investigation, negotiation, stealth, cargo | Clinic service and a witnessed promise |
| The Unregistered | Extract a technician before a record audit | Credentials, traversal, pursuit, ship pickup | Crew recruitment and a patrol route change |
| The Price of Quiet | Intercept a broadcast blackmail shipment | Survey tool, orbital flight, boarding at a dock | Public evidence or protection for an informant |
| A Table for Everyone | Balance aid and safe passage through a threatened district | Crew leverage, district flags, return route | Access and visible public response |
| Who Owns the Dawn | Reach the mirror control chamber and decide control | Mastery of established mechanics | Ending, crew futures and city illumination |

**Knowledge progression.** The journal connects clues, people, places and unresolved questions. A clue provides a new action, prediction or interpretation. The map records what the player has learned rather than exposing every reward at the start. Paths need visible or contextual information so that exploration is an informed choice. Mobius's account of revising Outer Wilds pathing supports this principle; the proposed implementation here is a smaller authored clue graph. [R02](https://www.mobiusdigitalgames.com/news/the-intentionality-of-wandering)

**Dialogue scope.** Use short conversations with two to four choices, explicit topic conditions and a consistent exit. Optional branches return to a shared spine unless they unlock a supported world-state change. First release planning cap: approximately 12,000 to 20,000 final spoken or displayed words, subject to reading and performance tests. Prefer high quality performances for the crew and key contacts, with text or limited barks elsewhere. Do not promise fully voiced procedural conversations.

**Narrative test before cinematics.** Run the complete story as cards or a lightweight text build with unfamiliar readers. Test motivation, clue comprehension and ending tradeoffs. Mobius documented this kind of reduced narrative prototype for Outer Wilds; use it to catch expensive structural faults before creating locations and performances. [R01](https://www.mobiusdigitalgames.com/news/demaking-outer-wilds)

## 7 The vertical slice

### Borrowed Morning

A 25 to 35 minute replayable slice opens in a market just after its allotted light has shut off. Seli needs power for a clinic. Iona identifies a relay containing the sunseed, and Venn offers an extraction route. The player learns how power, credentials and pursuit interact by solving one immediate problem.

**Minutes 0 to 4.** Walk through the market, meet one vendor and discover that the direct lift is powerless. Offer a battery, find a service stair or negotiate access through a workshop. Movement, interaction and the district's situation are taught through use. Optional guidance remains available.

**Minutes 4 to 11.** Reach the relay using an overhead maintenance path, forged permission or a direct confrontation. A readable scanner reveals cable routes and the patrol's current target. Two coherent approaches are required in the slice; the third is a release goal. Guards react to noise and broken access, but a detected player can still continue.

**Minutes 11 to 16.** Take the core. Activate the clinic's isolated backup or restore the dockworkers' evacuation lift before the relay locks. Both beneficiaries are introduced before the decision. The clinic route gives Seli immediate leverage; the lift route lets trapped workers clear the flight corridor. A later mission can repair the deferred service. This is a choice about whom to help first, with known limits and no hidden civilian death. Separately, optional interventions trade the core's capacitor charges against safer traversal.

**Minutes 16 to 23.** Carry the cargo to the ship, board and escape through collectors. Pursuit follows confirmed sightings. Ascend through cloud while maintaining steering and respond to one clear interception. The level transition preserves the ship's damage, cargo and story state. The mirror enters view and the port is identifiable below.

**Minutes 23 to 28.** Dock at a maintenance ring. Resolve a short interaction and choose a sunseed calibration. View the selected service lighting up below; dialogue and music acknowledge the choice. Keep this orbital encounter compact so the return receives its budget.

**Minutes 28 to 33.** Pilot back and walk for two to three minutes through the same market. A person, service and route respond to the decision. The clinic is operating or the dockworkers have returned; the other group explains its continuing need. This playable return is mandatory in the polished slice and reuses the opening environment.

### Slice acceptance

Use at least ten external players who did not build the game, across two iteration rounds. These small samples support qualitative decisions and obvious failure discovery; they cannot predict sales or awards. Record familiarity with action and flight games. Seek players with different control and accessibility needs.

- At least eight of ten finish without the developer taking the controls; at least seven can explain the cargo's consequence.
- At least seven can distinguish the experience from a generic city sandbox in their own words; do not prompt them with the desired hook.
- At least six choose to continue or replay when offered free time after the session, and explain what they want to do next. Record actual behavior and enjoyment comments alongside the counts; politeness and recruitment bias limit interpretation.
- Both supported approaches complete, including detection, retreat, reload and an interrupted ascent.
- Controls remain understandable when moving from walking to flight; no participant needs an unexplained mode switch to progress.
- No reproducible progress blocker, duplicated quest cargo or save corruption remains after the regression suite.
- The packaged build meets the agreed performance profile on the chosen baseline PC; cinematic footage alone does not pass.

If players love flight but find the ground loop weak, keep one ground mission space and improve it. If they love the city but dislike flying, improve assisted flight and shorten empty travel. Do not remove half of the requested fantasy silently; a change to that promise needs an explicit product decision.

## 8 Scope tiers and the full vision

| Dimension | Engineering prototype | Polished slice | Solo release | Funded full vision |
|---|---|---|---|---|
| Experience | 10 minute greybox expedition | One 25 to 35 minute mission | Complete 4 to 6 hour arc | Multiple arcs and systemic free exploration |
| City | One block and a pad | One detailed district route | Three compact connected neighborhoods | Several distinct cities and settlements |
| Flight | One ship and transition test | City escape and one orbital destination | Port airspace, ascent, orbital hub and limited exterior routes | Regional and interplanetary travel with more ships |
| Surface freedom | Bounded test space | Authored flight corridor with lateral freedom | Clearly bounded playable region | Arbitrary landing only after a planet pipeline passes |
| Crew | One placeholder radio role | Three voices with one full character scene | Three authored crew arcs | More recruits and richer station/squad simulation |
| Combat | One enemy and one weapon | Three roles including drone | Five roles and one boss built from existing rules | Larger ecology with measured encounter cost |
| Multiplayer | None | None | Offline single player | Two-player co-op if separate production gate passes |
| Content state | Disposable | Production-quality subset | Supported save and patch migrations | Expanded state, network and compatibility commitments |

**Full vision functional requirements.** A player can discover a problem on a planetary surface, investigate its actors, assemble a crew approach, acquire a ship, fly to another region or orbital destination, undertake an encounter and return to persistent consequences. Cities retain local cultural identities and commercial reasons to exist. Crew abilities affect solutions without replacing player control. Ships carry persistent damage, cargo and modules. A later second player can participate in jobs under clearly defined ownership and save rules.

**Planet technology gate.** Unrestricted land-anywhere flight requires radial gravity, terrain LOD, collision streaming, planetary navigation, weather and horizon transitions, scale-aware rendering, save coordinates, and camera behavior across poles and large distances. Demonstrate landing, walking, takeoff and return on a small test sphere before committing to a world. An attractive planet in orbit is not evidence that this gate passed.

**Full vision content gate.** Measure time per approved neighborhood, mission, character and destination. The expansion is eligible only when the budget covers the measured quantities, maintenance and contingency. Seek specialist engineering and art help when the work requires simultaneous ground, flight, network and planetary expertise. AI assistance changes throughput; it does not remove these specialties.

**Explicit deferrals.** MMO infrastructure, hundreds of interiors, fully destructible cities, arbitrary ship construction, fully simulated planetary economies, real-time generated dialogue and an unlimited universe are outside the first release. These are exclusions from that release, not promises for later. Reconsider them only if they demonstrably strengthen the core experience.



## 9 Ground movement and interaction

The target is confident, expressive movement in a readable space. Start with walk, sprint, jump, crouch, dodge and contextual mantle. Add a short assisted vault over approved obstacles. A jet-assisted jump can extend a leap between authored anchors after basic camera and collision work is stable; unrestricted personal flight is deferred because it bypasses mission boundaries and changes every encounter.

Initial tuning values are hypotheses: walk 4.5 meters per second, sprint 7.5, a 2.5 to 3 meter comfortable gap, and a mantle height range selected for the purchased animation set. Test a representative character capsule and animation against stairs, doorways, balcony rails and slopes before fixing dimensions. Keep critical paths at least two character widths unless a narrow passage has a tested special behavior. No visual ledge should suggest a supported jump that the collision forbids.

The interaction system supplies one consistent focus target, verb, range, duration, condition and result. A door, person, terminal, cargo socket and ship seat share the same interface but own different logic. The prompt states why an interaction is unavailable and never silently eats input. Hold interactions support a press toggle alternative. If an object unloads or the player moves out of range, cancel without consuming currency or changing mission state.

**Survey tool.** A short scan reveals authored traces: power paths, ownership seals, recent activity and useful landmarks. The tool supplies leads, not an all-seeing overlay of every NPC's feelings. An ordinary contact can become important because the player learns something or helps them. A discovered route appears in the journal and remains usable without rescanning.

**Cargo.** The sunseed has a dedicated secure harness or ship mount. A separate slot carries one ordinary mission payload, with one corresponding ship cradle; an extracted person uses a designated passenger seat. Heavy cargo uses a compact carried representation. Carrying restricts sprint and weapon use, but accessible assistance can remove the movement penalty. Approved temporary placement anchors let the player set a crate down without losing its ID. The sunseed never gets discarded because a new delivery arrives. Its tool abilities require it in the current character or ship mount; a core installed elsewhere grants no remote handheld power.

**Camera.** Use a stable shoulder camera with obstruction handling, adjustable distance and field of view, optional shoulder swap, independent horizontal and vertical sensitivity, invert options and reduced motion. Flight has independent settings. Mission design cannot require rapid camera rotation to understand an objective. Disable shake, bob, blur, chromatic distortion and forced recentering independently.

**Input contract.** Actions are named and rebindable rather than tied to an assumed physical button. Ground and flight use separate contexts with consistent confirm, cancel, interact and pause actions. Entering the pilot seat changes context only after possession succeeds. Menus, dialogue and cargo placement must all work with a controller and with a keyboard without a mouse. The onboarding explicitly teaches braking before the first dangerous flight.

## 10 Combat stealth and pursuit

Ground combat supports the mission rather than consuming the entire production. The release loadout is a compact pulse sidearm, a shock staff using a small reliable move set, and a disruption pulse powered by the tool. Do not add a large loot weapon catalog. Each attack needs an anticipation cue, collision rule, impact response, interrupt policy and recovery. Player control should be responsive; animation cannot silently prevent a dodge after a visually completed attack.

Enemies begin as three roles: a guard who pressures at range, a shield operator who holds a lane, and a drone that exposes the player's location. Release additions are a mobile flanker and a relay technician who restores disabled systems. A boss combines existing rules in a changing environment. Avoid a bespoke cinematic boss that introduces an entire new control scheme.

Starting encounter budget is three to six active opponents, with no more than two attackers committing to close-range attacks simultaneously. Encounters use cover, alternate elevations, escape routes and interactable power. The camera and audio must communicate off-screen threats. Enemy endurance is tuned around intended decision time, not sponge-like health. Exact damage, recovery and assist values belong in data, with a documented baseline.

**Stealth.** Sight, sound and discovered evidence feed the same alert model. Visibility comes from tested geometry and conditions, not arbitrary light color. A guard notices a disabled relay, searches the last known location, calls support if able and can lose the player. An awareness meter has clear beginning, escalation and recovery cues. Disabling a camera does not automatically tell every guard exactly where the player is.

**Failure continues the mission.** Detection opens a supported combat or escape path. Surrender, incapacitation or ship disablement return the player to a safe checkpoint with the objective recoverable. A failed optional promise can have a narrative consequence; a simulation fault cannot permanently destroy required cargo. An easy mode can slow detection and increase assistance independently of combat damage.

**Law and pursuit.** Four explicit states govern security: clear, suspicious, identified and intercepted. Reports have witnesses, timestamps and a last known position. Ground security and flight patrols exchange an event only when a communication route exists. Pursuit uses bounded reinforcements and authored spawn routes outside the player's immediate view. Leaving line of sight starts a search; changing a transponder can help only when the ship is not under direct observation.

The sunseed adds a detectable signature when activated. It does not create an invisible universal wanted level. Show the emission radius or an understandable qualitative warning before use. A quiet neighborhood cannot instantly know about a covert action elsewhere. Separate the story's long-term investigation flags from the short-term pursuit state.

**Civilian policy.** First release combat spaces allow civilians to flee to safe locations. Accidental contact with the ship in a pedestrian zone uses speed limiting and collision recovery. Do not create a detailed civilian injury simulation unless it is an intentional, funded design goal. Essential contacts have explicit recovery states. Their protection must fit the fiction and never make a mission impossible to complete.

## 11 Ship flight and space encounters

The **Kestrel** is an asymmetrical workboat with a large cargo cradle, visible repair seams and a recognizable fin arrangement. The final ship and sunseed deserve original design work: they carry the brand in every screenshot. Start with a placeholder vehicle for handling. Licensed environment pieces can dress the finished interior after the silhouette, sightlines and interaction layout are locked.

Flight is assisted and readable. The ship can hover, move laterally, climb, turn, brake and boost. Aerodynamic and orbital realism are subordinate to control. Use the same fundamental input vocabulary in the city and space. Space adds optional roll and greater drift, with stabilization on by default. Landing assistance snaps only inside a declared landing volume after the player accepts; it must not seize control unexpectedly.

| Flight state | Initial speed envelope | Purpose and constraints |
|---|---|---|
| Docking and pedestrian proximity | 0 to 15 m/s | Precision, generous collision recovery, boost disabled |
| City flight | Cruise 35 to 70 m/s, burst up to 100 | Readable turns and routes; streaming and stopping tests determine final cap |
| Ascent corridor | 120 to 250 m/s | Authored hazards and cloud handoff; no claim of physical planetary distance |
| Orbital encounter | 80 to 250 m/s locally | Tactical maneuvering around a station and debris |
| Travel between distant sites | Abstracted cruise | Guided transfer after a clear destination choice; never pretend every kilometer is simulated |

These are candidate tuning values for a stylized world, not engine capability claims. At 100 m/s, a two second load delay consumes 200 meters of distance before braking or safety margin. Derive prefetch distance from measured loading and braking, then lower maximum speed if the destination cannot be safely made ready. City streets should not be scaled up arbitrarily to hide poor handling.

**Heat and power.** Ordinary propulsion and emergency landing never consume the last progression-critical resource. Boost and disruption generate heat on a 0 to 100 scale. Crossing 85 gives a clear warning; reaching 100 disables the optional boost briefly while normal flight remains available. Cooling rates and upgrade differences are tuning hypotheses. Never strand the player because a finite fuel resource was spent exploring.

**Space combat.** The slice needs one interception with two readable enemy patterns. Release encounters add escort, cargo inspection, disable-and-escape and a station approach. Use one forward weapon, a defensive pulse and environmental cover. Target assistance and lead indicators can be adjusted. Enemy ships have obvious engines or emitters that express their role; do not require precision shots against an indistinguishable silhouette.

**Boarding and interior policy.** The player can explore the Kestrel when docked. During active flight, they remain at the controls. Boarding another vessel occurs at an authored stationary docking state in the first release. Walking around a freely accelerating ship, simultaneous ship-to-ship boarding and combat, and seamless exterior EVA are separate engineering features for the full vision. This boundary removes moving-frame physics and navigation from the critical path while preserving a useful crew home.

**Ship progression.** Two utility mounts compete among a quieter transponder, improved heat sinks and a reinforced cargo cradle. The player cannot install all three. Reinforcement protects a fragile payload during rough flight; it does not increase the one-payload mission topology. The sunseed mount is dedicated and cannot be displaced by upgrades. A module changes a decision and presents its tradeoff. Show damage through two or three authored states and audio; arbitrary mesh destruction is unnecessary.

## 12 The sunseed and persistent consequences

The sunseed is an authorization device with a small local capacitor. Its key grants access to power-routing controls; its stored energy runs short local interventions. It cannot power an entire neighborhood from its own battery. A clinic intervention unlocks and starts an existing isolated backup supply; a long-term allocation redirects existing grid capacity. Its short-term charges open a door, stabilize a lift controller or provide a defensive pulse.

The fictional security rule is a paired hardware credential whose live attestation the mirror accepts. Copying a file does not create a second valid device. Temporarily socketing it authorizes a local service, then the player removes it; that service retains its limited lease after the core leaves. Permanent mirror calibration happens only at explicit story decisions. The authority, remaining capacitor charge, active service leases and physical custodian are separate save fields. This is a consistent fiction contract, not a real-world cryptographic specification.

Start with three intervention charges, replenished at designated safe service points. Primary mission completion always has a route that does not require a charge already spent elsewhere. A critical story allocation is an explicit decision and never happens because the player used an ordinary contextual interaction. Show affected parties, expected change and whether the choice can be revisited.

The slice's clinic-versus-evacuation-lift decision is a mutually exclusive routing lease during a maintenance window. It is separate from the three refillable capacitor charges. Recharging cannot authorize a second lease in that window. The deferred service can be repaired in a later authored job, so the choice leaves a legible obligation rather than an irreversible hidden punishment.

The core has exactly one custodian: player harness, ship mount, world socket or secured quest storage. It may have visual proxies in a UI or a distant scene, but only one authoritative state. A transfer is transactional: reserve destination, verify conditions and atomically commit ownership and any charge change; only then detach and attach the presentation. Cancellation before commit preserves previous custody. After commit, recovery reconstructs the committed destination instead of awarding a second object. Restart recovery follows the newest validated save revision, not an assumption that an in-memory change reached disk. Save and travel use this same contract.

**District state.** Each neighborhood stores a bounded combination of service status, access state and public response. The first release supports three authored power configurations per relevant mission location, not arbitrary simulation of every lamp. State may alter lighting groups, a lift, a shop service, a patrol route and two or three character lines. Keep irreversible story decisions separate from reversible service interventions.

**Example.** Restoring the clinic lights its rooms, opens Seli's assistance and draws a patrol to the relay that recorded the unlicensed reroute. It does not spawn infinite security. Restoring the evacuation lift instead lets dockworkers clear the flight corridor and leaves a later obligation to help the clinic. Helping a third party can reveal a non-power route. All consequences must be playable and signposted; none should exist only as a morality label.

**Persistence boundary.** Store named decisions, completed objectives, unique cargo, crew trust events, services, modules and player transforms. Ambient pedestrians, every cup knocked over and every traffic vehicle do not need indefinite persistence. Designers identify the few physical changes that carry meaning. This provides a believable world without serializing an entire city.

## 13 Economy progression and discoveries

Use three legible progression channels: credits for ordinary services, reputation expressed through specific relationships and access, and knowledge that reveals new solutions. The sunseed's charges are a tactical resource, not a fourth grind currency. Equipment unlocks widen choices instead of making early spaces numerically obsolete.

Credits come from completing contracts, optional recovered cargo and clearly signposted salvage. They pay for modules, optional repairs, information and cosmetic personalization. Standard recovery and mandatory mission travel remain free or safely subsidized. The game cannot softlock because a player bought a coat rather than fuel. Keep vendors local and memorable; one useful specialist per neighborhood is stronger than a dozen identical menus.

Initial balance model: a normal job pays roughly 300 credits, a minor useful purchase costs 100 to 200, and a ship module costs 600 to 900. These are relative test values. A player should encounter an interesting choice after one or two jobs and see a larger goal within three. Validate with a spreadsheet or script using minimum, typical and completionist routes; no specific money economy is established until those runs work.

Allow one sale credit per unique recovered item ID. Quest cargo cannot be sold through an ordinary vendor. Prevent buying back items from producing an infinite profit loop. A fail-forward event cannot grant the same payout twice. Debt is a narrative obligation with chosen consequences, not an endlessly compounding timer that punishes accessibility settings or exploration.

Discoveries include a maintenance frequency, a forged seal, a crew contact, a hidden approach and evidence that changes dialogue. A journal entry states what the player learned and where it might matter. A secret should pay off through a view, story or option rather than only a collectible counter. Optional locations need a reason to exist in the port's infrastructure and culture.

The release world supports a limited set of ambient events: a stalled lift, a patrol inspection, an argument, a small delivery and a neighborhood celebration. These use shared actors and state patterns. Spawn only when conditions fit and cooldowns allow. Major story events take priority. Procedural selection must never overwrite an authored mission, trap a contact or repeatedly stage the same emergency beside the player.

## 14 Visual direction and asset strategy

Choose **sunlit industrial fantasy** as the visual identity: chalky plaster and dark mechanical structure, oxidized copper, woven shade, deep indigo shadow and controlled amber light. The port should look excellent at noon-equivalent mirror light, in twilight and after a power change. Neon is an accent, not the universal solution to a mismatched asset collection.

Use three scales of detail deliberately. At silhouette scale, the crater, mirror and three district shapes distinguish the world. At street scale, balconies, cranes, awnings and cable paths explain routes. At hand scale, a few repaired objects and personal possessions tell stories. The player sees high detail where they can act; distant geometry carries proportion and atmosphere.

**Kit purchasing order.** Begin with free Neo City assets to prove the import, collision and packaged performance path. For the city, evaluate one primary building collection such as CyberPunk or Cyber District and one street-prop collection such as Cyber Streets; these were explicitly included in KitBash3D's Gameplay Ready launch collection. Verify the current exact asset version and license before purchase. Consider older Neo Tokyo pieces for selected skyline forms only after inspecting their suitability. Do not buy Brooklyn or Manhattan simply because they were the first city packs researched: the final art direction should decide. [T35](https://kitbash3d.com/products/mini-kit-neo-city) [T34](https://kitbash3d.com/a/blog/introducing-gameplay-ready-kits-built-for-real-time-performance-in-unreal-engine)

**Buy versus create.** Buy generic architecture, pipes, ordinary props, compatible locomotion and supporting sound effects. Create the Kestrel silhouette, sunseed, hero faces and wardrobe accents, faction marks, foreground signs, important interiors and signature music. Adapt purchased materials to a documented palette and roughness range. Keep vendor source assets untouched and derive project variants through wrappers and instances.

**The representative block.** Approve one intersection with a traversable roof, interior entrance, power socket, cover, landing sightline, day and twilight lighting, and pedestrians. It must support a mission route. Its approved dimensions, materials and assemblies become the district recipe. A pretty street with no gameplay cannot serve as the production template.

**Character visual contract.** Prioritize readable faces and strong silhouettes over photorealistic pores. A consistent stylized-realistic finish reduces the clash between purchased environment assets and bespoke characters. Test the face under actual gameplay lighting and at conversation distance. Keep clothing and appendages inside the verified animation envelope. Reserve elaborate cloth and hair simulation for a performance budget that supports it.

**Effects and destruction.** Express power through light paths, sound and small mechanical movement. Use authored breakables and damage states for important encounters. Reserve particle density for cues the player needs; color, shape and animation together distinguish damage, interactability and route guidance. A color change alone is insufficient.

Mobius's published art history shows how the conflict between close-up navigation and space-scale vistas changed Outer Wilds' production approach. The lesson here is to validate Borrowed Sun's look at every intended camera scale before mass-producing city content. [R03](https://www.mobiusdigitalgames.com/news/our-new-style)

## 15 Audio music and performance

The city's sound is geographical. Workshop rhythms, public announcements, water machinery and domestic music identify districts even when they are off-screen. Mix wide atmosphere separately from interactable cues. The mirror's transition changes the sound of the city as people switch routines; a power restoration is heard before it becomes a distant vista.

The Kestrel has a small set of layered states: idle, hover, cruise, strained boost, damage and docking. Engine pitch communicates speed but should not fatigue the player during travel. Space exterior sound is a stylized cockpit or suit interpretation; keep that convention consistent. Important cues have subtitles or visual equivalents.

Commission one adaptable musical identity rather than unrelated tracks. A melody begins as a street performance, reappears as a crew member's quiet rendition and expands during the orbital reveal. The ending transforms it according to the city's state. Prepare stems for exploration, tension, pursuit and relief with musically valid transition points. Long quiet moments should be intentional, allowing the place and dialogue to breathe.

First release planning allowance is roughly 35 to 50 minutes of reusable music and four to six distinct ambience beds, with additional layers and transitions. This is a commissioning brief to estimate, not a requirement to fill the game with continuous score. A smaller excellent suite is acceptable if it covers the intended emotional functions.

Use an audio event naming scheme and an event table linking action, emitter, concurrency, priority, subtitle and fallback. Cap repeated barks; prioritize danger, crew dialogue and mission information over incidental speech. Conversations interrupt or queue by policy. Save the fact that a crucial line was delivered so loading does not repeatedly replay it.

Human performers are the preferred final voice for the main cast. Temporary synthetic speech can test timing only where rights and tool terms allow. Never clone a performer or use a purchased voice outside its license. Store consent, usage scope and source provenance with recordings. Budget editorial time for pronunciation, consistency, mixing and localization-friendly delivery.

## 16 User experience accessibility and quality bar

The interface exposes the current objective, nearby interaction, immediate threat, cargo custody and ship condition. It avoids filling the screen with every possible job. A compass can be optional; landmarks, signs, sound and journal clues must provide a navigable baseline. An assisted navigation mode offers stronger guidance without changing the story outcome.

The journal has four simple views: current job, people, places and discovered connections. A crew page shows available help and unresolved promises in plain language. A power allocation screen states the immediate result, cost and reversibility. Inventory is limited to useful tools, modules and significant cargo; do not build an equipment grid to support trivial pickups.

Pause is always available in the offline release, including flight and dialogue. Support adjustable subtitle size, speaker identification, contrast backgrounds, independent dialogue/music/effects levels, captions for critical sound, remapping, toggle alternatives to holds, aim assistance and separate flight assists. Avoid rapid tapping, precision timing or simultaneous inputs as the only path through the story. UI text scales without truncation and is tested at common television viewing distances as well as desktop distance.

Difficulty controls separate incoming damage, enemy aggression, detection, navigation help and flight stabilization. An exploration mode preserves plot choices. Accessibility does not reduce rewards or produce a lesser ending. Test menus and controls early with users who need the options; a settings checklist alone does not establish accessibility.

**First five minutes.** The player knows who needs help, where to go, how to interact and why the city is dark. They perform a meaningful action before a long lore explanation. **First half hour.** They meet a crew member, solve a problem in more than one possible way, fly the ship, see orbit and witness one consequence. **First return.** They recognize a place they changed.

The quality bar is observable: a new player can read traversal affordances; input feels reliable; every transition preserves intention; a choice makes sense before and after commitment; the same route remains interesting because knowledge or conditions changed; and the final scene resolves a personal relationship as well as the infrastructure conflict. A feature is complete only when its failure, reload and accessibility paths also work.



## 17 Architecture and system contracts

The following is the proposed Borrowed Sun implementation, not an existing or tested build. The first release is offline single player. Keep persistent story and inventory state independent of streamed Actors. A district can unload without erasing a promise, restoring a stolen sunseed, or duplicating a recruited crewmate.

Use a small C++ foundation with Blueprint presentation and Primary Data Assets for definitions. Unreal's Asset Manager supports primary IDs, asset bundles and asynchronous loading; use those capabilities for definitions and optional content rather than a Blueprint that references the entire game. [T25](https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine)

| Module | Owns | Public contract |
|---|---|---|
| BorrowedSunCore | Stable IDs, tags, event records, save schema | Validate IDs; resolve definitions; publish typed events |
| WorldState | Sunseed custody, promises, factions, quest facts | Apply validated commands; return immutable snapshots |
| Interaction | Requests from player, NPC and ship | Query options; validate request; commit one result |
| Travel | Sector readiness and frame conversion | Request/cancel transfer; report stage and reason |
| Population | Named NPC records and active representations | Spawn/despawn representation without changing identity |
| Presentation | Animation, sound, cameras, illumination variants | Observe committed state; never award inventory |
| EditorTools | Intake, district generation and validation | Produce manifests and reports; absent from runtime |

A module is an ownership boundary; do not create a separate plugin for every table row. Start with one runtime module and one editor module. Expose interfaces for gameplay objects so replacements do not require editing every mission.

**Authoritative command contract:**

```text
Command { RequestId, ActorId, TargetId, ExpectedRevision, Verb, Payload }
Result  { Accepted, Reason, NewRevision, Events[] }

Apply(command):
  if RequestId in RecentOutcomes: return previous result
  if Sequence <= DecidedHighWater: return ExpiredWithoutReplay
  validate permissions, current revision and preconditions
  compute next state; check invariants
  commit state, outcome and high-water mark; then notify presentation
```

One dispatcher serializes commands and issues monotonically ordered IDs `(CampaignId, Sequence)`. Persist the latest 1,024 decided outcomes and their high-water mark; older requests cannot replay after journal pruning. Store permanent unique reward/consumption keys separately. For the sunseed, exactly one custody record names a character, ship cradle, device or recovery location. Commit custody, charge and mission facts together before detaching/attaching visual representations. Precommit cancellation preserves old custody; postcommit recovery reconstructs the committed destination. Failed presentation never repeats an award.

Use GAS if the approved systems need substantial ability/effect interactions; its documented responsibilities include abilities, attributes and effects. StateTree suits readable NPC states. MassEntity is a separate data-oriented framework, justified later by measured crowd demand. [T17](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine) [T18](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine) [T19](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine) Prototype nearby characters as Actors and keep the state APIs independent of that choice. Crew station/radio roles do not require a city-wide autonomous squad simulation.

## 18 Streaming and the street-to-orbit journey

The first release uses **authored sectors**, not a fully traversable spherical moon. Orison, its ascent corridor and the mirror maintenance ring form one designed expedition. Keep a persistent Unreal world containing geographically separated city and orbital content sectors. A cloud-masked transfer maps the ship between sector anchors while preserving control and state. From orbit, an aligned moon/crater/city proxy represents the region below. Landing is supported only at authored destinations.

This is a production choice, not a claim that a real planetary distance is simulated. Avoid a blocking `OpenLevel` call on the normal flight path. The core playable requirement is an uninterrupted sense of piloting; whether the transfer achieves that quality must be tested in the prototype.

World Partition loads cells around streaming sources and exposes a streaming-completion query. Hard Actor references can bundle otherwise separate content, while Level Blueprint references can make actors always loaded. [T08](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine) The persistent ship/player and lightweight coordinators are intentional exceptions; city Actors must not be retained by mission managers. Use HLOD for unloaded architecture and separate inexpensive orbital vistas. HLOD and Nanite address different levels of representation. [T09](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine) [T14](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine)

**Transfer record:**

```text
TravelRecord {
  TransferId, SourceSector, DestinationSector, Stage,
  SourceAnchor, DestinationAnchor, CheckpointRevision,
  TimeoutPolicy, LastSafePadId
}
Stages: Idle -> Preparing -> Ready -> Committing -> Reconciling -> Complete
Failure: Preparing/Ready -> Cancelled; after commit -> Recovery
```

1. **Preparing:** Add a destination streaming source and load asset bundles. The pilot retains control; damage, cargo and pursuit can change. Prepare destination gameplay geometry with registered collision/navigation, but keep its rendering, shadows, local audio and NPC behavior suppressed. A persistent environment controller owns global sky/lights; preloading cannot change source lighting.
2. **Ready:** Validate collision sweeps, arrival clearance, required navigation paths, docking anchors and mission actors. Warm essential rendering states using verified precache/load requests or a controlled warm-up pass. Merely Loaded cells do not prove active collision or readiness.
3. **Committing:** At a movement/physics synchronization boundary, finish preceding damage/interaction commands, recheck eligibility, and stop integration for the transfer step. Capture current state, record destination ownership, and relocate craft, attachments and camera exactly once without sweeping across the sector gap. Apply the fixed rigid-anchor mapping below; reset movement and camera previous-transform/interpolation caches. Resume integration on the next update.
4. **Reconciling:** Reveal prepared destination visuals, enable destination behavior and switch environment/audio settings under the cloud mask. Restore input context and project pursuit into its supported destination form. Verify custody and craft identity before removing source streaming.
5. **Complete:** Release the old representation, mark the transaction finished, and allow a new stable checkpoint. Destination activation may be retried with the same ID without spawning duplicates.

The initial arcade craft uses one kinematic swept-movement root. Anchors are fixed rigid transforms with no scale or moving reference frame:

```text
R = DestinationRotation * inverse(SourceRotation)
position' = DestinationOrigin + R * (position - SourceOrigin)
orientation' = R * orientation
linearVelocity' = R * linearVelocity
angularVelocity' = R * angularVelocity
```

One service owns units and double-precision sector transforms. Clearance failure aborts before commit or invokes destination recovery afterward. Test maximum-speed, reverse, paused, contact and same-frame damage crossings. LWC improves precision but retains a Beta label and float-conversion risks. Sky Atmosphere supports ground-to-space rendering; neither supplies planetary gameplay. [T11](https://dev.epicgames.com/documentation/en-us/unreal-engine/large-world-coordinates-in-unreal-engine-5) [T12](https://dev.epicgames.com/documentation/en-us/unreal-engine/sky-atmosphere-component-in-unreal-engine) [T13](https://dev.epicgames.com/documentation/en-us/unreal-engine/georeferencing-a-level-in-unreal-engine)

Preload distance starts from `speed × measured load time + braking distance + margin`. Test reversals, dives and repeated crossings. Before commit, timeout can retain the ship in a readable holding route or cancel the attempt. After commit, recover to a valid destination pad with the captured state. If the pilot can reach the transfer before readiness, reduce corridor speed or provide an explicit transit/loading fallback; never cross into missing collision. The first prototype must verify both return travel and interrupted transfer recovery.

## 19 KitBash intake and procedural block production

Pin an exact supported Launcher Unreal build, Cargo connector and asset revision after a representative import/cook test. Current Cargo documentation supports Launcher Unreal 5.2–5.8 and excludes source-built engines; its newer workflow imports USD-based Blueprint/Packed Level Actor assemblies. [T03](https://help.kitbash3d.com/en/articles/7833653-using-unreal-engine-with-cargo-workflows-and-integration) [T04](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines) Do not select arbitrary source-engine upgrades to solve an unmeasured problem.

Maintain an intake project. Approved content enters the game through Unreal asset migration/tools, with vendor originals preserved and project adaptations separately named. If an engine fork becomes necessary, prove same-version content migration and packaging in an isolated source-build copy. Cargo connection to that build is unsupported. The vendor identifies Cargo as editor tooling and documents excluding it from game builds; verify the actual imported dependencies before doing so. [T05](https://help.kitbash3d.com/en/articles/12689467-exclude-kitbash3d-cargo-plugin-from-unreal-engine-builds)

**Asset manifest:** `AssetId, Vendor, Product, Revision, LicenseEvidence, SourcePath, AdaptedPath, ImportSettingsHash, Bounds, CollisionPolicy, MaterialSlots, TexturePolicy, IntendedDistance, Dependencies, ValidationResult`. Gameplay Ready branding describes collision/instancing/Nanite preparation, not a performance certification for Orison. Newer KitBash structure defaults relevant 4K textures to 2K through LOD bias. [T06](https://kitbash3d.com/pages/gameplay-ready-kits) [T07](https://help.kitbash3d.com/en/articles/11698511-structural-updates-to-unreal-engine-kits-gameplay-ready)

Run the pilot with the currently free Neo City mini-kit. After it passes, buy one primary architecture kit—CyberPunk is the initial candidate, with Cyber District as the alternative—and Cyber Streets for props. The vendor names all three in its Gameplay Ready rollout. [T34](https://kitbash3d.com/a/blog/introducing-gameplay-ready-kits-built-for-real-time-performance-in-unreal-engine) [T35](https://kitbash3d.com/products/mini-kit-neo-city) Select one architecture family after inspecting actual geometry; do not combine whole catalogs. Demo/layout scenes are not supplied for Unreal as a ready city. [T35](https://kitbash3d.com/products/mini-kit-neo-city)

Interior support varies: the vendor distinguishes dedicated furnished interiors, newer interior shells, upgraded legacy kits and older exterior-only buildings. An interior shell is not proof of a complete playable room. Inspect the exact asset in Cargo and test every intended route; do not assume all buildings are enterable or all apparent surfaces walkable. [T33](https://help.kitbash3d.com/en/articles/2445183-understanding-interiors-in-kitbash3d-kits)

Accept a building only after testing scale, pivots, stairs, doorways, camera sweeps, projectile traces, roof access and landing surfaces. Mark interiors as supported, facade-only or separately authored. Separate transparent pieces and dynamic mission objects from static assemblies. Do not destructively break every Packed Level Actor before knowing which pieces need independent control.

Build blocks in dependency order: walkable routes and landing clearances; building envelopes; approved architecture; utility paths; authored businesses/landmarks; decorative scatter; navigation and HLOD. PCG supports partitioned/hierarchical generation and runtime generation around sources. [T10](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-pcg-generation-modes-in-unreal-engine) Generate collision-bearing structure in the editor first; runtime decoration must be disposable and unable to block critical paths.

Qualify bounded static navigation for Orison's walking areas and stationary docks; flight uses a separate route/avoidance graph. World Partition Navmesh is documented as Experimental. [T22](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partitioned-navigation-mesh) If adopted, require packaged pathfinding across repeated cell unload/reload and fixed tile-pool limits. Otherwise qualify a small resident navigation set; if unreliable in the selected engine, use conventional walking maps behind explicit dock/door transfers. Walking inside a moving ship remains excluded.

```text
Recipe = { DistrictId, RecipeVersion, Seed, AssetSetVersion }
Placement = { StablePlacementKey, DefinitionId, Transform, Overrides }
ObjectId = StableId(DistrictId, StablePlacementKey)
```

Allocate and retain placement keys in the district manifest. Never use PCG output index, Actor display name or current array order as persistent identity. Changing a seed creates a reviewed revision; it does not silently rename saved objects. Keep authored overrides outside generated output. A regeneration report lists added, removed and changed placements and flags save migration needs.

Automated acceptance checks passage widths, doorway clearance, collision overlap, roof/flight exclusion zones, missing assets and duplicate IDs. Human review checks three consistent views—market, terrace and orbit—and whether lighting, signs and activity support the cultural direction. Procedural distribution supplies production speed; authorship supplies the neighborhood's meaning.

## 20 Character, animation and technical art pipeline

Begin with Rook, one crew character and one guard on the intended humanoid skeleton family. Pressure-worker plates and technician prosthetics should preserve required joint ranges. Standardize scale, reference pose, root direction, weapon sockets, contact markers, collision capsule and clothing attachment rules before producing variants.

IK Retargeting can transfer animation between different skeletons using mapped chains and contact goals; it still requires setup and validation. [T23](https://dev.epicgames.com/documentation/en-us/unreal-engine/ik-rig-animation-retargeting-in-unreal-engine) Review locomotion starts/stops, turning, stairs, crouching, carrying the sunseed, weapon aiming, boarding, seated poses and interaction reach. Foot sliding and hand contact errors should be fixed before duplicating the rig across the cast.

| Representation | Required fidelity | Technical policy |
|---|---|---|
| Rook and close dialogue | Readable face, hands and silhouette | Full tested rig; controlled facial/cloth cost |
| Near crew/enemies | Gameplay and interaction clarity | Shared animation family; budgeted update rates |
| Ambient pedestrians | Recognizable occupation and motion | Reduced rig/material cost; representation tiers |
| Distant crowds | Movement and density | Cheap animated or instanced representation; no full AI |

Character definitions carry `CharacterId, RigFamily, MeshSet, AnimationSet, VoiceSet, EquipmentSockets, RepresentationProfiles`. Named identity never depends on which mesh is active. A representation change preserves mission and interaction state; it cannot interrupt a crucial spoken line or replace a close character in view without an approved transition.

Create a compact material library for metal, cloth, skin/plates, painted panels, glass and emissive signage. Use instances and controlled parameters. Do not promise Nanite for every character: current documentation retains unsupported deformation/material cases, including Morph Targets. [T14](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine) Test the selected rig/render path independently.

The sunseed, ship silhouette, mirror and three recognizable landmarks receive original hero treatment. Maintain reviewed signage and script assets outside procedural randomization. Each hero asset passes a neutral-light turntable, gameplay-camera review, collision/contact test, distance-transition test and packaged-build check. Reuse a small dialogue camera/gesture grammar; reserve bespoke facial and cinematic work for the designated decisive scenes.

## 21 Performance targets and profiling gates

These are provisional qualification targets. No Borrowed Sun build has been benchmarked. Select and record exact CPU, GPU, RAM, storage, drivers and engine version in week one; freeze a baseline machine before setting art quantities.

| Profile | Output and frame target | Initial quality policy | Acceptance use |
|---|---|---|---|
| Baseline PC | 1080p output, 60 fps | Scalable GI/reflections, reduced crowd/shadow cost, TSR if needed | Release floor after hardware qualification |
| Recommended PC | 1440p output, 60 fps | Higher environment/lighting detail, measured internal resolution | Main visual target |
| Quality mode | 1440p output, 30 fps | Higher fidelity within 33.33 ms | Optional fallback with explicit player choice |
| Capture | Up to 4K | Offline/cinematic settings permitted | Marketing capture; never performance evidence |

For a 60 fps profile, target GPU p95 at or below 13.3 ms during the representative slice to retain approximately 20% headroom; aim for game-thread p95 below 10 ms. Whole-frame steady-state p95 should remain within 16.67 ms and p99 below 25 ms, with no unexplained sustained stall above 100 ms. CPU/GPU work overlaps; these budgets are not added. On a provisional 8 GB VRAM qualification GPU, start with a 6.5 GB application residency goal and adjust using measured platform overhead.

Epic's Lumen guide associates High/Epic with 60/30 fps console goals and roughly 4/8 ms lighting budgets at 1080p internal resolution. Those are not whole-game guarantees. Fast flight can outrun Lumen updates, and hardware ray tracing is sensitive to instance overlap. [T15](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-performance-guide-for-unreal-engine) [T16](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine) Test night signs, glass, fog, ship exhaust and crowds together. Lower expensive shadow/light coverage and representation density before expanding area.

Use packaged ten-minute routes: market circulation, relay combat, collector escape, ascent transfer, orbital docking and return. Record GPU, game/render thread, loading, memory, garbage collection and first-use spikes with Insights. [T31](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-insights-in-unreal-engine) Run a fixed warm route for comparisons and a controlled cold/first-launch scenario for PSO coverage. Precaching helps prepare rendering states; it does not replace representative traversal. [T32](https://dev.epicgames.com/documentation/en-us/unreal-engine/pso-precaching-for-unreal-engine)

A change exceeding 10% of an accepted route's CPU/GPU time or peak memory triggers review. Persistent growth after ten district round trips fails. Diagnose with an asset/feature disabled, then fix the contributor. Do not call a lower frame cap an optimization without disclosing the profile change.

## 22 Tools and AI-assisted development

Keep source, configuration, data schemas, generation recipes and editor scripts under version control. Store binary assets with a repository setup that supports their size and locking; test restoration from backup. Assign one writer per map/binary asset. Record engine/plugin versions and the last passing packaged build; perform upgrades in disposable branches/copies and require the same qualification routes before adoption.

Use AI for bounded C++ changes, validators, editor scripts, test fixtures and content metadata. Each task specifies inputs, expected outputs, invariants and a failing example. Human review owns architecture, cultural direction, play feel and acceptance. Agents may independently review or run tests, but must not simultaneously regenerate the same district or modify the same binary assets.

Unreal Python automates editor production, not packaged gameplay. Asset moves and edits must use Unreal APIs rather than filesystem renaming. [T26](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python) Build a small command surface: `ValidateIntake`, `GenerateDistrict`, `ValidateRoutes`, `BuildDerivedWorld`, `PackageCandidate`, `RunQualification`. Commands write machine-readable reports with build ID, recipe/version hashes, changed assets, warnings, timings and exit status.

The intended build chain is intake validation → deterministic generation → reviewed overrides → navigation/Smart Object/HLOD builds → cook/package → traversal tests. World Partition builders support these batch tasks, including iterative cell loading for large PCG builds. [T27](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-builder-commandlet-reference) Stop on generation errors; do not auto-submit failed output.

Use a local fast DDC and retain reusable shader/asset derivations. DDC is regenerable cache, not source backup. [T28](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-derived-data-cache-in-unreal-engine) Add shared infrastructure only when multiple machines justify maintenance. A fresh checkout must rebuild without the creator's hidden folders, desktop shortcuts or live Cargo session.

## 23 Save versioning, QA and recovery

```text
SaveHeader { SchemaVersion, BuildId, ContentManifestVersion, Checksum }
WorldSave  { Revision, Player, Ship, Crew, SunseedCustody,
             Missions, DistrictDeltas, RecipeVersions, TravelRecord,
             RecentOutcomes, DecidedHighWater, PermanentAwardKeys }
```

Save persistent changes rather than pointers or decorative instances. SaveGame stores project-defined data and supports asynchronous saving. [T24](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine) Snapshot state, journal, high-water mark and travel stage at one revision. Serialize writes; retain two validated generations. Recovery selects the newest intact compatible saved revision. An in-memory commit is not durable until writing and validation finish; discard transient commands/events on reload.

Migration functions advance one schema version at a time. Keep old fixture saves. A removed definition needs an explicit redirect, substitute or supported retirement rule. Preserve unique cargo and mission completion through migration. If a save is newer than the running build, explain incompatibility and preserve it; never overwrite it with an empty game.

Prefer stable checkpoints. Travel recovery follows the committed ownership in the selected saved snapshot: precommit returns to its source anchor; postcommit reconstructs its destination or safe pad. A crash may lose unsaved progress, never combine newer custody with older charge or objectives. Interaction recovery restores the whole saved transaction. Permanent reward keys prevent a new request ID from granting an already-paid reward again.

Use functional tests for gameplay and Gauntlet for packaged sessions where useful. [T29](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine) [T30](https://dev.epicgames.com/documentation/en-us/unreal-engine/gauntlet-automation-framework-in-unreal-engine) Mandatory regression cases include both mission approaches, detection/retreat, every custody handoff, death with core aboard, interrupted save, incompatible/corrupted save, ascent cancellation, post-commit recovery, repeated sector round trips, unavailable NPC destination and district regeneration with old saves.

Recovery rules are designed content: invalid player location moves to a validated safe anchor; a destroyed mission ship transfers recoverable cargo to its recorded salvage location; a missing named NPC is reinstantiated from authoritative state; an unavailable optional service offers a clear reason. Critical-path alternatives come from mission design, never silent inventory fabrication. Every recovery records a diagnostic reason for investigation. No release candidate passes with a reproducible progression blocker, duplicate sunseed, corrupted save, or unresolved missing-collision route.


## 24 Efficient workflow and AI review contract

One creator owns the design, integration and release decisions. Plan for **30 working hours per week: 20 focused production hours and 10 hours for administration, research, playtests, marketing and coordination**. With 46 active weeks, annual production capacity is **920 effective hours**. Holidays, recovery and unexpected interruptions are already acknowledged; overtime is not the contingency plan. Track actual focused hours by deliverable for six weeks before changing this assumption.

Keep one backlog with no more than one feature under construction and one in validation. A ticket contains a player-facing result, dependencies, exclusions, acceptance conditions, estimated hours and a rollback route. “Improve city” is not a ticket; “two approaches to the relay remain completable after detection and reload” is. Finish and validate small tickets before starting another system.

A normal week contains four four-hour implementation blocks, two hours integrating and packaging, and two hours examining performance, regressions and content. The remaining ten hours cover scheduled external playtests and business work. Start each day with a reproducible current failure or a bounded improvement; end by recording the result, unresolved problem and next action. Friday's deliverable is a packaged build, a three-minute capture and an updated risk/throughput sheet.

AI can draft original C++ helpers, editor automation, data validators, test cases, dialogue alternatives and documentation. It can examine selected logs and propose diagnoses. Assign one small contract per task; require assumptions, affected files, engine-version references and acceptance evidence. The creator reviews the diff, compiles, exercises the player path and checks the packaged build. AI-generated confidence is not test evidence. Delegate independent analysis in parallel; integrate mutations sequentially. Keep direct ownership of architecture, narrative canon, art direction, final balance and third-party rights.

The review contract is: no unexplained dependency; no unreviewed engine upgrade; no invented API accepted without compilation; no bulk replacement of stable gameplay; no credentials or restricted source in prompts. Epic restricts training on its licensed technology and prompt input to programs that train on input; MetaHuman has additional restrictions. [P04](https://www.unrealengine.com/eula/unreal) Keep KitBash source assets and renders outside generative processing unless the applicable rights expressly support that use. [P07](https://help.kitbash3d.com/en/articles/10715523-what-s-the-difference-between-purchasing-a-kit-and-a-subscription) [P08](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs) [P09](https://kitbash3d.com/pages/license-text)

Every production asset gets a ledger entry: supplier, URL, invoice, license/version/date, permitted users, attribution, engine compatibility, edits, dependencies and AI involvement. Checkpoints require source control, binary-asset coordination, off-device backup and a demonstrated restore. Engine/plugin versions remain pinned through a milestone. Buy assets only after a representative sample imports, cooks, runs and fits the visual grammar.

## 25 The first twelve weeks: engineering and design prototype

This is **240 effective hours of prototype work**, not a polished vertical slice. Use primitive environments, a placeholder humanoid, one ship and one enemy. The objective is a ten-minute expedition that carries the sunseed from ground interaction through flight into orbit and back into a saved consequence.

| Weeks / hours | Deliverable | Exit evidence |
|---|---|---|
| 1–2 / 40 | Versioned project, baseline PC, packaged greybox, input, camera and backup | Fresh checkout builds; clean install launches; restored backup reproduces the build |
| 3–4 / 40 | Walk/mantle/interact; sunseed ownership and one powered mechanism | Collect, insert, remove and reload without duplication across 20 scripted cycles |
| 5–6 / 40 | Hover/forward flight, braking, boost and assisted docking | Five unfamiliar testers complete departure, two turns and docking; record collisions and coaching |
| 7–8 / 40 | Ground/ship handoff and cloud transition to an orbital test space | Twenty round trips preserve cargo, ship damage and mission state; interrupted-transition recovery works |
| 9–10 / 40 | Relay operation with two supported approaches and recoverable detection | Both paths finish; leaving/re-entering, dying and loading during each beat do not block completion |
| 11–12 / 40 | Integrated ten-minute job, consequence and performance capture | Five new testers play without the creator taking control; at least four explain the sunseed choice |

The numbers are internal gates, not market benchmarks. Record individual observations because five people cannot validate commercial demand. Gather flight-game experience and control preferences so novice difficulty is not mistaken for a broken fantasy.

At week 12, approve further spending only if both ground play and flight show a clear enjoyable moment, transition state survives the regression route, and a representative asset block meets the chosen performance profile. Allow at most two repair cycles of 40–80 effective hours each. These consume the remaining slice allowance, including its rework reserve; they are not added after the estimated work. Where repairs exceed that reserve, reduce later slice work or explicitly revise the forecast. Two failed cycles trigger a product decision: simplify the transition or interaction model, adopt the standalone prologue, or stop this implementation. Do not build additional districts to hide an unresolved core loop.

## 26 Schedule, work breakdown and cash scenarios

The **production-quality slice requires 600–1,000 effective hours total, including the prototype**. At the assumed capacity, this is approximately eight to thirteen calendar months. It contains the 25–35 minute Borrowed Morning mission, representative final presentation, both approaches, saved consequence and packaged performance. It is the evidence for estimating the release, not an asset showcase.

| Work package | Total effective hours | Completion evidence |
|---|---:|---|
| Engineering prototype and polished slice | 600–1,000 | Section 7 acceptance, including external playtests |
| Remaining systems and integration | 700–1,000 | Final mission, economy, crew, save and settings contracts work together |
| Release content and character/audio integration | 900–1,600 | Complete four-to-six-hour arc with approved missions and three crew episodes |
| Polish, optimisation, accessibility and regression | 600–1,000 | Content lock, baseline performance, compatibility and release-critical tests |
| Release preparation and initial support engineering | 200–400 | Install/update/rollback verified; store build and support tools ready |
| **Solo release total, including slice** | **3,000–5,000** | Complete, supportable game |

The total implies **3.3–5.4 years** at 920 hours per year. It is a planning envelope, not a delivery promise; unfamiliar Unreal development adds learning work. Business/admin time sits in the separate ten-hour weekly allocation. Critical learning or external coordination exceeding that allocation reduces production capacity. Do not add the slice again to the release total.

The 5,000-hour upper estimate already contains **800 hours reserved for rework**, allocated across the five rows respectively as 100, 150, 200, 250 and 100 hours. These allowances are part of each upper row, not extra hours to add afterward. Prototype repairs draw first from the slice's 100-hour reserve and then its remaining content allowance; exhaustion requires a scope reduction or a revised forecast. The 3,000-hour lower estimate assumes an experienced creator, a clean pipeline, no additional learning period and the contractor deliverables in section 33. A zero-contractor route does not retain the same scope and hours.

Measure median focused hours for one approved encounter, one dialogue scene, one reusable interior and one district art pass. Multiply remaining quantities by those measured rates, then add integration and regression. For example, if a finished mission costs 160 hours and six remain, missions alone consume 960 hours. Replace assumptions quarterly with completed-work evidence. Additional AI tools justify optimism only after sustained throughput improves without raising defect rates.

The following is a **broad cash scenario**, not the user's budget or supplier quotations. It assumes selective specialist help and excludes the creator's living costs, tax treatment and income replacement.

| Category | Planning range EUR |
|---|---:|
| Hardware, storage and test access | 2,000–6,000 |
| Licensed assets, plugins and production services | 2,400–8,000 |
| Specialist character, animation, sound and cultural work | 8,000–30,000 |
| External QA, accessibility and localisation | 4,000–12,000 |
| Trailer, capsule art and marketing production | 4,000–10,000 |
| Business, legal, accounting and publishing administration | 2,000–6,000 |
| Subtotal | 22,400–72,000 |
| Cash contingency, 25% | 5,600–18,000 |
| **Cash scenario total** | **28,000–90,000** |

Runway adds personal monthly burn multiplied by the realistic calendar duration. Grants, projected sales and possible publisher money count only once secured. Contractor work can alter KitBash licensing needs. [P08](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs) A zero-contractor plan needs smaller character, animation, language and presentation commitments; it does not inherit this quality target at the same schedule.

**Fallback:** ship a complete 60–90 minute standalone prologue: one market/relay area, the ship, an ascent, one orbital dock, two approaches, one resolved crew conflict and two visible consequence variants. Reuse the slice foundations; remove the other districts, broader economy, extended recruitment and campaign branches. It still includes city life and playable flight. Estimate it after the slice rather than promising another arbitrary deadline.

The full vision is a staffed, funded scenario requiring a new work breakdown, specialist owners and platform/network/planet gates. It is not included in the solo hours or cash above. EVERSPACE 2's firsthand account documents a 25-person studio using separate handcrafted maps; it supports bounded-world design, not a promise of solo planetary production. [P03](https://www.unrealengine.com/developer-interviews/everspace-2-delivers-a-handcrafted-universe-brimming-with-space-combat?lang=en-US)

## 27 Risk register, dependencies and stop conditions

| Risk / dependency | Early evidence | Response / stop condition |
|---|---|---|
| Ground and flight feel like unrelated games | Testers cannot state a common objective across modes | Keep one cargo/mission state and clearer handoff; halt expansion until four of five prototype testers understand it |
| Transition corrupts state | Missing/duplicated sunseed, changed health, invalid spawn | Atomic ownership/state transfer and recovery; no slice approval with a reproducible case |
| Scenery exceeds performance budget | Representative packaged route fails agreed frame/memory targets | Reduce material/lighting/crowd cost; no new kit purchase until the route passes |
| Plugin or engine dependency fails | Packaging break, abandoned support, incompatible update | Isolate behind a small interface; maintain replacement path; reject essential dependency without a working cook |
| Content consumes runway | Measured remaining work exceeds available funded hours | Reduce to prologue or obtain funding; freeze new locations immediately |
| Narrative branches multiply | Valid choices strand essential contacts or require bespoke endings | Reconverge mission spine, retain explicit consequences; test state combinations before recording voice |
| Character/animation pipeline stalls | One approved character exceeds its allowed share of the content budget | Reuse rigs, reduce customization and stage dialogue simply before adding characters |
| Creator availability falls | Four-week rolling focused time below 15 hours/week | Reforecast dates and cash; remove parallel commitments, not recovery time |
| Asset rights or provenance unclear | Missing license, inconsistent usage terms, inaccessible supplier evidence | Quarantine the asset; substitute cleared content before distribution |
| No distinctive player response | Two slice iterations fail the comprehension/desire criteria in section 7 | Rework the sunseed and return consequence before expanding story length |

Dependencies are deliberate: movement before dressed traversal; save contracts before branching missions; transition proof before orbital art; story validation before voice recording; lighting/performance profile before city expansion; content lock before final localisation. Changes to the core fantasy, platforms, multiplayer or cash commitment belong in the decision log and require an explicit production decision.

## 28 Accessibility, localisation, QA and release

Build remapping, separate flight/ground sensitivity and inversion, hold/toggle options, subtitle sizing/backgrounds/speaker labels, non-colour warnings, independent audio channels and camera-motion controls into the slice. Offer adjustable aim/flight assistance and optional objective reminders. Menus need stable controller focus and keyboard/mouse operation. Microsoft XAGs provide useful best practices and explicitly are not a legal compliance checklist. [P05](https://learn.microsoft.com/en-us/xbox/accessibility/guidelines) Include disabled players before content lock; document tested support precisely.

Use stable string IDs and a terminology glossary from the first mission. Keep UI text out of textures, allow expansion, test pseudolocalisation and avoid sentence fragments assembled in English word order. Select release languages after audience evidence and qualified review capacity. AI translation drafts need human linguistic and in-context review; player-facing AI localisation falls within Steam's disclosure scope. [P11](https://partner.steamgames.com/doc/gettingstarted/contentsurvey?language=english)

Automate deterministic validators for duplicate IDs, missing objectives, invalid dialogue links and absent required assets. Manually test the changing player experience. Every candidate build runs the same mission route with combat/stealth, early collection, retreat, death, transition, save/load and final choice. Preserve a regression save for every shipped mission phase.

Release blockers include reproducible crashes on the supported configuration, progress locks, save corruption, duplicated quest cargo and inaccessible essential controls. Candidate acceptance requires three full campaign passes using different supported approaches, all ending configurations checked, and a clean install plus upgrade from the previous candidate. Exercise offline startup, low disk space, corrupted-save recovery, controller disconnect, missing audio device, cloud-save conflict and interrupted patching. These gates reduce known defects; they do not prove zero bugs.

Reserve three months of support availability. Keep versioned saves, migration tests, previous builds, rollback instructions and a public known-issues channel. Crash/usage collection needs a documented purpose, data minimisation and appropriate privacy handling. The game remains playable offline without analytics. Initial platform commitment is Windows PC; console ports require their own acceptance, hardware and certification work. [P18](https://learn.microsoft.com/en-us/gaming/game-publishing/onboarding/onboarding-submit-game-concept) [P19](https://sonyinteractive.com/en/news/blog/showing-your-game-to-playstation/) [P20](https://developer.nintendo.com/register)

## 29 Positioning, business and rights

Lead marketing with **“Steal back your city's daylight”**, the playable sunseed choice and the same neighbourhood seen from street and orbit. “Inspired by an unreleased AAA game” is research context, not the main commercial identity. Show actual gameplay; explain boundaries honestly. Test whether viewers remember the consequence rather than only the skyline.

After slice approval, prepare a short trailer, readable capsule, five screenshots, press kit and public demo. Compare alternative hooks using the same build and record traffic source, page conversion, demo completion and qualitative feedback. Small samples cannot predict sales. Schedule an event only when the demo represents the central loop and has support capacity; avoid a calendar-driven public launch of an unstable build.

Use a premium single-purchase model initially. Price remains a hypothesis pending comparable-release research and audience testing. Calculate break-even from **net receipts**, accounting for actual store terms, regional pricing, discounting, refunds, taxes and engine royalties. Unreal's standard royalty is generally 5% after its US$1 million lifetime exclusion, with exclusions and a qualifying 3.5% programme; check the controlling agreement and submit the revenue-bearing release notice. [P04](https://www.unrealengine.com/eula/unreal) [P06](https://www.unrealengine.com/license)

The release rights checklist has concrete owners and evidence:

- Clear the working title and original identity; maintain permission/assignment records for commissioned art, music, voices and code. Separate inspiration from copied expression. [P13](https://www.copyright.gov/register/tx-games.html)
- Record each asset's actual license. Kit purchases and subscriptions confer different continuing rights; subscription cancellation is not a perpetual raw-asset library strategy. Resolve patches/DLC and new-use assumptions before relying on them. [P07](https://help.kitbash3d.com/en/articles/10715523-what-s-the-difference-between-purchasing-a-kit-and-a-subscription) [P08](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs) [P09](https://kitbash3d.com/pages/license-text)
- Recheck entity size when hiring: current KitBash subscription guidance counts contractors and partners for Teams eligibility. [P08](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs)
- Review AI restrictions per supplier and provider. KitBash's help text describes broader AI processing limits than training alone; retain assets outside generative workflows unless cleared. Fab licenses also require per-product verification. [P07](https://help.kitbash3d.com/en/articles/10715523-what-s-the-difference-between-purchasing-a-kit-and-a-subscription) [P09](https://kitbash3d.com/pages/license-text) [P10](https://www.fab.com/eula?lang=en)
- Disclose shipped player-consumed AI content accurately to Steam; keep fixed reviewed content and avoid runtime model dependencies. Preserve human authorship records; U.S. copyright guidance does not treat prompts alone as automatic ownership of generated expression. [P11](https://partner.steamgames.com/doc/gettingstarted/contentsurvey?language=english) [P12](https://www.copyright.gov/ai/Copyright-and-Artificial-Intelligence-Part-2-Copyrightability-Report.pdf)
- Complete Steam identity/tax onboarding, US$100 application fee, store/build review, applicable first-title 30-day waiting period and two-week Coming Soon minimum. These minimums do not replace audience-building time. [P14](https://partner.steamgames.com/doc/gettingstarted/appfee) [P15](https://partner.steamgames.com/steamdirect/?l=english) [P16](https://partner.steamgames.com/doc/store/releasing)
- Complete truthful content/age-rating information, including Germany's visibility requirement, credits, music permissions and support information. [P17](https://partner.steamgames.com/doc/gettingstarted/contentsurvey/germany?language=english)

## 30 First actions and decision log

The first work session fixes the baseline PC and control devices, creates source control and backup, packages a blank project, and builds one movement pad. Then create the sunseed state test and one greybox building block. Evaluate a small licensed sample only after the measurement route exists. No catalogue purchase, public release date or full planet build is required to start.

The decision log records date, question, evidence, chosen option, rejected alternatives, cost/schedule effect, owner and revisit trigger. Seed it with: bounded single-player Windows release; 30 weekly hours/20 focused; 920 annual effective hours; persistent sunseed; three crew; one ship; selective interiors; cloud-masked transition; no runtime generated dialogue; no promised co-op; working title pending clearance.

Keep unresolved decisions visible: exact hardware tier, engine/plugin compatibility, affordable asset rights, final language set, creator runway and full release versus prologue. Resolve each at its dependency gate. The next commitment is the twelve-week prototype; the next expansion of commitment must be earned by its playable evidence.


## 31 Beyond Good and Evil 2 research

### Status and the limits of this research

Research cutoff: **12 October 2026**. Beyond Good and Evil 2 is **not confirmed canceled**. On 22 September 2026, Ubisoft writer Rik Godwin publicly said he had worked on it for several months in a support/consultant role. Creative director Fawzi Mesmar had stated in January that the project and team were unaffected by Ubisoft's recent organizational changes. The official FAQ still provides no release date. These establish continued work, not proximity to release. [B21](https://bsky.app/profile/rikgodwin.bsky.social/post/3mw4li3vxvc2a) [B20](https://www.linkedin.com/feed/update/urn:li:activity:7421227406130335744/) [B25](https://www.gamesradar.com/games/open-world/beyond-good-and-evil-2-lead-promises-to-deliver-a-remarkable-game-for-our-players-to-enjoy-as-he-confirms-the-game-is-unaffected-by-the-recent-changes-at-ubisoft/) [B19](https://www.ubisoft.com/en-gb/game/beyond-good-and-evil-2/space-monkey-program)

The reference for Borrowed Sun is the **public 2017–2019 vision**. Its current implementation and final feature set remain unknown. This review uses accessible Ubisoft descriptions, recaps, Q&As and contemporary written eyewitness reports. Official videos were located, but this document does not claim a complete viewing or frame-by-frame analysis. A cinematic communicates intention; a working prototype demonstrates a particular capability; neither establishes a finished game.

### What the public history establishes

| Date | Evidence | Interpretation |
|---|---|---|
| 2008–2009 work, discussed in 2016 | Ancel described earlier playable prototypes and technical difficulties before returning to Rayman. [B22](https://www.gamespot.com/articles/why-beyond-good-and-evil-2-has-taken-so-long/1100-6444617/) | Avoid describing every intervening year as uninterrupted production of one unchanged design. |
| June 2017 | New prequel pitch and controlled engine-scale demonstration; public engine video released 23 June. [B01](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017) [B02](https://news.ubisoft.com/en-us/article/3DumSXRBRvRiApnV2cebqy/beyond-good-and-evil-2-first-inengine-demo-revealed) | Cinematic ambition and technical demonstration are separate evidence. |
| May–June 2018 | Movement/staff-combat update, then a live co-op demonstration connecting ground action, city flight and orbit. [B03](https://news.ubisoft.com/en-us/article/7pOyDJJtQNigXHtxvifvjH/beyond-good-and-evil-2-developers-show-off-new-ships-and-staff-combat-gameplay) [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) | Substantial prototype systems existed; polish and production coverage were not established. |
| December 2018 | Public gameplay recap, with contemporary coverage identifying the earlier E3 demo. [B06](https://www.youtube.com/watch?v=Y2229DmJLIY) [B23](https://www.pcgamer.com/watch-25-minutes-of-beyond-good-and-evil-2/) | Do not count repeated footage as an independent content milestone. |
| June–October 2019 | Clone origins, character customization, worldbuilding and sound; Paris joined five other studios. [B08](https://news.ubisoft.com/en-us/article/47aHrZWvifWAQuRUl0l1ui/beyond-good-and-evil-2-space-monkey-report-5-livestream-showcases-the-journey-from-clone-to-pirate) [B09](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/2WxLDvDewvgz6Smza4jCbF/space-monkey-report-5-livestream-your-questions-answered) [B10](https://news.ubisoft.com/en-us/article/6T4sPdU8DGukHxKjZUPbti/beyond-good-and-evil-2-new-hero-design-rhino-hybrids-and-customization) | Rich design documentation, with many systems still expressed as intentions. |
| September 2020 | Ubisoft reported an internal milestone and hours of playable material, alongside Ancel's departure. [B17](https://news.ubisoft.com/en-us/article/6ptJuA0jx7qhvhTnSosl8L/an-update-on-beyond-good-evil-2) | Attributed producer statement about a non-public build. |
| 2024–2026 | Anniversary content revisited Jade's past; later staff statements confirm ongoing BGE2 work. [B26](https://www.ubisoft.com/en-au/games/beyond-good-and-evil/editions/20th-anniversary) [B20](https://www.linkedin.com/feed/update/urn:li:activity:7421227406130335744/) [B21](https://bsky.app/profile/rikgodwin.bsky.social/post/3mw4li3vxvc2a) | Continued franchise activity does not validate the old feature list. |

### Feature audit

**C** means cinematic or concept; **P** means historically demonstrated prototype, as described by contemporary sources; **I** means announced intention. Classification is about evidence, not quality. Where a row combines classes, only part was demonstrated.

| Aspect | Publicly evidenced design | Class | Unverified extent |
|---|---|---|---|
| Identity | Custom human or hybrid captain; later Q&A includes tiger and rhino choices. [B09](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/2WxLDvDewvgz6Smza4jCbF/space-monkey-report-5-livestream-your-questions-answered) | I | Finished creator and final species list |
| Combat | Sword, gun, shield, jetpack; a separate update showed staff animation. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) [B03](https://news.ubisoft.com/en-us/article/7pOyDJJtQNigXHtxvifvjH/beyond-good-and-evil-2-developers-show-off-new-ships-and-staff-combat-gameplay) | P | Final feel, weapon variety and balance |
| Augments | Slowdown and pushback effects; tactical inspection of enemies. [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) | P/I | Complete modifier catalogue |
| Observation | Spyglass reveals tactical, recruitment and distant-location information. [B23](https://www.pcgamer.com/watch-25-minutes-of-beyond-good-and-evil-2/) | P | Full investigation or photography game |
| Recruitment | Captive mechanic presented as a potential recruit. [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) | P/I | Complete recruit lifecycle |
| Crew/home | Scientists, fighters, mechanics and mothership customization discussed. [B24](https://www.gamesradar.com/new-beyond-good-and-evil-2-footage-showcases-seamless-co-op-npc-recruitment-and-ship-customization/) | I | Autonomous relationships or ship management |
| Vehicles | Hoverbike, fighter and arcade dogfighting; modular ship customization. [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) | P | Every vehicle or capital battle |
| Hijacking | Developers described taking and flying found vehicles. [B24](https://www.gamesradar.com/new-beyond-good-and-evil-2-footage-showcases-seamless-co-op-npc-recruitment-and-ship-customization/) | I | Universal implementation |
| Scale transition | Underground-to-city-to-orbit journey without visible loading; EVA near mothership. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) | P | Shipping performance or arbitrary landing |
| Celestial world | Scale/time changes and moving sunlight; mixed procedural/authored worldbuilding proposed. [B01](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017) | P/I | Fully simulated, populated solar system |
| Navigation | Galaxy Viewer progresses from local detail to System 3. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) | P | Meaningful content at every location |
| Urban society | Scheduled activity, prisoner liberation and bank raids described. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) | I | General economy or autonomous history |
| Identity response | Human/hybrid appearance changes NPC reception; subterranean flight and subcultures promised. [B07](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered) | I | Comprehensive social simulation |
| Moral choice | Cruel pirate conduct and consequences discussed. [B09](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/2WxLDvDewvgz6Smza4jCbF/space-monkey-report-5-livestream-your-questions-answered) | I | Branch count or complete morality system |
| Co-op | Shared encounters and separate travel demonstrated; unlimited player separation claimed. [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) [B07](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered) | P/I | Final player count, netcode and persistence |
| Connectivity | December 2018 Q&A requires internet even for solo play. [B07](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered) | I | Current launch requirement |
| Story cast | Trailer officers, Jade and Alpha Sections carry narrative hooks. [B11](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3DhkYd63K2mFPIFqsCIEfG/meet-the-bge2-characters-from-the-e3-trailer) [B12](https://www.ubisoft.com/en-gb/game/beyond-good-and-evil-2/news-updates/5DxAQDfi7VXh1XKfZxzdXX/jade-is-back-and-more-e3-2018-trailer-breakdown) [B13](https://www.ubisoft.com/fr-ca/game/beyond-good-and-evil-2/news-updates/3tMnrt7UfMfaPqqfKJ964M/prsentation-des-personnages-du-trailer-de-le3-2018) | C/I | Playable cinematic action or boss systems |
| Community work | Selected contributed art/music and payment arrangements documented. [B16](https://www.ubisoft.com/ja-jp/game/beyond-good-and-evil-2/news-updates/3TpQdaiIiRjTpeK5t4g5ed/showcasing-our-fans-in-beyond-good-and-evil-2) | Documented program | Replacement for professional integration |

Unknowns include final world size, mission count, explorable interiors, current platforms, budget, production throughput, save architecture, accessibility coverage, localization volume, monetization and release schedule. Old platform logos and projected beta dates are not present commitments. Likewise, a visible city does not prove every building is enterable, and a persistent world claim does not establish a simulated citizen behind every window.

### The world beneath the spectacle

The 2017 pitch connected corporate colonization, hybrid labor exploitation and piracy in System 3. Indian and Chinese migration were foundational; exploration was meant to produce crew growth and access to larger vessels. [B01](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017) Ganesha's official reference article identifies Indian-subcontinent and Southeast-Asian influences. [B15](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5pzusN60hnwscppROS1YTs/cityscapes-ganesha-inspirations) Later material situates the city on Soma, orbiting Dyaus, with protected and meteor-battered regions. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018)

One proposed origin follows a cloned miner through catastrophe, rescue and pirate initiation, introducing the AI oracle PRIMERA and Moksha Gate mystery. It was an example route, not necessarily everyone's opening. [B08](https://news.ubisoft.com/en-us/article/47aHrZWvifWAQuRUl0l1ui/beyond-good-and-evil-2-space-monkey-report-5-livestream-showcases-the-journey-from-clone-to-pirate) Programmable Diwalite links propulsion, cloning and technology. [B14](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3UG9FX9AeIl0Lpq2iWI6PA/diwalite-the-ultimate-resource-of-power) The October 2019 presentation adds a deliberate sonic identity for this material and culturally varied music. [B10](https://news.ubisoft.com/en-us/article/6T4sPdU8DGukHxKjZUPbti/beyond-good-and-evil-2-new-hero-design-rhino-hybrids-and-customization)

The cast gives the scale social meaning. Shani's trade, Knox's temperament and Dakini's command role establish individual identities. [B11](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3DhkYd63K2mFPIFqsCIEfG/meet-the-bge2-characters-from-the-e3-trailer) The Gada's doctor, cook, mechanic and expressive AI suggest domestic life, while Ubisoft describes many trailer figures as legendary officers the player encounters. [B13](https://www.ubisoft.com/fr-ca/game/beyond-good-and-evil-2/news-updates/3tMnrt7UfMfaPqqfKJ964M/prsentation-des-personnages-du-trailer-de-le3-2018) This distinction matters: a cinematic ensemble is not evidence of a fully simulated player crew.

There are also revisions or inconsistencies in public lore: the June 2018 account attributes sterility to arrival, while December's Q&A describes deliberately infertile clones as the system's original population. Do not merge these into one unquestioned canon. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) [B07](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered) Borrowed Sun needs a short authoritative world-rules document with dated changes, especially around its mirror, sunseed and energy grid.

### What comes from the first Beyond Good and Evil

Keep the original's lessons separate from the prequel's promises. Ubisoft's retrospective describes Jade as a photographer, caregiver and lighthouse keeper whose economic needs lead into investigation with a clandestine news network. Wildlife photography, companionship, stealth, staff combat and hovercraft activity support a story extending from Hillys into space. [B27](https://news.ubisoft.com/en-gb/article/4HYZp0t1d14z2afFYmzjju/beyond-good--evil---20th-anniversary-edition-launches-on-june-25) Ubisoft's brand description foregrounds exposing a government conspiracy. [B28](https://www.ubisoft.com/en-us/company/about-us/our-brands/beyond-good-evil)

Our interpretation is that care gives discovery a purpose, and evidence gives resistance a playable action. Borrowed Sun takes this relationship between everyday needs and large political stakes into its clinic, crew obligations and contested broadcasts. It does not inherit Jade, the camera economy, the conspiracy plot or any claim that a multistar sandbox is essential to that emotional effect.

## 32 Evidence and design decisions

### Traceability into Borrowed Sun

This matrix records design reasoning, not equivalence. The right-hand decisions are original proposals to validate in playtests.

| Evidence or limitation | Borrowed Sun response | Acceptance question |
|---|---|---|
| Personal action and orbital travel coexisted in the prototype. [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) | One expedition carries the sunseed from relay to ship to mirror. | Does context survive every change of scale? |
| Investigation informs action in both games. [B23](https://www.pcgamer.com/watch-25-minutes-of-beyond-good-and-evil-2/) [B27](https://news.ubisoft.com/en-gb/article/4HYZp0t1d14z2afFYmzjju/beyond-good--evil---20th-anniversary-edition-launches-on-june-25) | Scanner, testimony and credentials provide actionable information. | Does a discovered clue change a plan? |
| Recruitment promised useful growth but its full simulation was unproven. [B05](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/) [B24](https://www.gamesradar.com/new-beyond-good-and-evil-2-footage-showcases-seamless-co-op-npc-recruitment-and-ship-customization/) | Three authored crew specialists with bounded services. | Can players name each person's contribution and motive? |
| Ship life is strongly characterized in the trailer cast. [B13](https://www.ubisoft.com/fr-ca/game/beyond-good-and-evil-2/news-updates/3tMnrt7UfMfaPqqfKJ964M/prsentation-des-personnages-du-trailer-de-le3-2018) | Reusable stations, meals, radio and a cargo cradle anchor return scenes. | Does home visibly change after a job? |
| BGE2 customization expands the identity space. [B09](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/2WxLDvDewvgz6Smza4jCbF/space-monkey-report-5-livestream-your-questions-answered) | Rook has a fixed biography and limited appearance options. | Can characterization remain specific across supported choices? |
| Corporate dependence ties society to technology. [B01](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017) [B14](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3UG9FX9AeIl0Lpq2iWI6PA/diwalite-the-ultimate-resource-of-power) | Illumination contracts and the sunseed connect public systems to private obligations. | Can players explain the cost of their allocation? |
| Culture informs city identity and sound. [B15](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5pzusN60hnwscppROS1YTs/cityscapes-ganesha-inspirations) [B10](https://news.ubisoft.com/en-us/article/6T4sPdU8DGukHxKjZUPbti/beyond-good-and-evil-2-new-hero-design-rhino-hybrids-and-customization) | Research port life, migration, labor and music; commission review. | Do details follow a coherent history rather than a mood board? |
| Historical co-op promises add a separate complexity class. [B07](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered) | Offline single-player release; co-op has a funded gate. | Is the full expedition satisfying alone? |
| Planetary scale does not establish content density. [B01](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017) [B04](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018) | Three compact neighborhoods and one orbital hub before wider expansion. | Can production reuse produce another meaningful mission? |
| Community assets require selection and integration. [B16](https://www.ubisoft.com/ja-jp/game/beyond-good-and-evil-2/news-updates/3TpQdaiIiRjTpeK5t4g5ed/showcasing-our-fans-in-beyond-good-and-evil-2) | Licensed kits and AI output enter an explicit acceptance pipeline. | Is the second accepted asset faster to integrate without quality loss? |

### The co-op decision is a deliberate product difference

The historical BGE2 promise combines free player separation with shared story and online dependency. That creates design questions the public evidence does not answer: whose world records a choice, how progress merges, what happens when one player leaves with the mission object, and which encounters remain coherent when partners occupy different scales. These are inference-based questions, not claims of observed defects.

Borrowed Sun's sunseed makes ownership unusually consequential. Two people cannot independently spend the same scarce allocation without a rule explaining conflict. Offline single player makes that choice legible and lets production validate it first. Future co-op would need explicit host authority, object ownership, join/rejoin rules, decision consent and save behavior. A clean single-player architecture is useful preparation; it is not evidence that multiplayer is already solved.

### Production lessons and their evidentiary boundaries

No public source inspected here exposes enough internal evidence to assign BGE2's delay to one cause. Its long history is not proof that seamless worlds, procedural generation, co-op or ambitious art individually caused failure. Leadership changes, internal milestones and evolving descriptions are context. The following are independent recommendations for this creator's resources.

**Test the coupled experience early.** Ground movement, pursuit, cargo state, flight and narrative consequences must work together. Completing each independently can hide expensive integration failures. The greybox expedition should therefore preserve both city and orbit, even when each contains very little content.

**Control combinatorial growth.** Every additional species, body rig, weapon, vehicle, companion behavior and branch creates more combinations to validate. Shared rigs, fixed crew roles and one excellent ship protect the distinctive fantasy while bounding its production burden. Expand only after observed content throughput justifies it.

**Make continuity measurable.** The important continuity is not merely the absence of loading screens. The same cargo, damage, pursuit state, promise and objective must survive the journey. A constrained transition that preserves those facts may support the fantasy better than unrestricted empty travel. Performance and state recovery are separate acceptance gates.

**Use assets to serve authorship.** A purchased city kit supplies shapes and surfaces; it does not supply meaningful routes, collision quality, mission logic or cultural coherence. The art pipeline must select, modify and validate those assets against Orison's visual rules. AI can accelerate utilities, drafts and review, but cannot certify its own implementation or replace external playtesting.

**Protect the return.** Borrowed Sun's emotional promise depends on seeing how an action changes familiar people and places. Budget return scenes as core mission content. A spectacular launch alone cannot prove the game delivers its central relationship between the skyline and its inhabitants.

**Promise what the build demonstrates.** Target distinction through the sunseed's understandable tradeoffs, the ship's identity and the crew's specific loyalties. Awards cannot be guaranteed, and historical promotional ambition should not become a numerical scope obligation. The release earns its ambition when players remember a choice and want to inhabit the place again.


## 33 Production inventory and implementation templates

### Content ceilings for the solo release

These are maximum planned quantities, not a checklist to fill regardless of quality. Count only approved, tested content. The slice is included in these totals. Cosmetic reuse does not increase the count of meaningful missions or places.

| Deliverable | Release ceiling | Reuse and completion rule |
|---|---|---|
| Playable neighborhoods | Three compact areas, roughly 200 by 250 meters each before vertical stacking | One shared architecture family; total accessible ground footprint about 0.15 square kilometers, tuned for route density |
| City flight region | Roughly 2 by 2 kilometers with an authored ascent corridor | Skyline outside this region is a vista; map and fiction communicate boundaries |
| Orbital destinations | One maintenance ring with two small exterior encounter spaces | One docking/interior kit, revisited in changed states |
| Enterable interiors | Six reusable rooms or small suites, including the clinic and relay | Facade-only buildings clearly distinguished; each interior earns mission use |
| Hero landmarks | Three city landmarks plus the mirror | Consistent street, terrace and orbit silhouettes |
| Protagonist | One biography and rig, two approved appearance presets, two outfits with color variation | Additional heads and voice choices require reforecasting |
| Crew | Three named people | Shared rig family, three episodes, station and radio roles |
| Other named contacts | Six | Reused conversation staging, bounded state variants |
| Enemy roles | Five plus one boss assembled from existing mechanics | Three core rig/animation families maximum |
| Ambient people | Eight visual archetypes with controlled material variants | Local representation budgets decide simultaneous count |
| Player ships | One exterior and one docked interior | Two utility mounts; three candidate modules |
| Other craft | Two hostile silhouettes and one traffic family | Reuse flight and damage logic |
| Story jobs | Six including Borrowed Morning | Two supported approaches per job where meaningful; third approaches are stretch work |
| Crew and discovery content | Three crew episodes, twelve small discoveries, five ambient event recipes | Every item changes knowledge, relationship or access |
| Dialogue | 12,000 to 20,000 final words across all spoken and written narrative | Three language candidates at most before audience/budget decision; no multilingual voice promise |
| Bespoke cinematic scenes | Four short scenes, about eight minutes total ceiling | Most dialogue uses a shared gameplay camera grammar |
| Music | 35 to 50 minutes of source compositions, reused as stems | A smaller suite can pass by covering all emotional functions |
| State presentation | Three power/service variants per affected location | Common controllers, selective lamp/route/voice changes |

The assumed 900 to 1,600 creator hours for content and integration require suitable purchased rigs/animations and some specialist output. Obtain fixed-scope estimates for: three crew faces and wardrobe accents; a final ship and sunseed design; cleanup of the essential animation set; an adaptive music suite and mix; voice recording/editing for the main cast; cultural review; and external QA/accessibility review. One person may cover several packages, but each package still has an acceptance test. Creator integration time remains in the hour budget. Supplier labor is paid from the cash scenario and must not be silently counted as free AI work.

Before accepting a contractor asset, require source files, naming and rig compliance, texture/material budgets, contact and collision checks, clear usage rights, feedback rounds and delivery dates. A visually good asset that fails the project rig or cannot be packaged is unfinished. Contractor engagement can change applicable licensing tiers. [P08](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs)

If those services are unaffordable, reduce final cast customization, unique animation, voice coverage, locales and music volume, then re-estimate. The prologue can use one finished crew character, the other crew over radio, one district and a short musical suite. Quality concentrates where players spend time.

### Mission definition example

This is a design schema, not a claim that the classes already exist. Store definitions separately from player progress. Validate required IDs and the objective graph in the editor before a mission enters a packaged build.

```json
{
  "MissionId": "mission.borrowed_morning",
  "DefinitionVersion": 1,
  "Entry": "meet_seli",
  "RequiredSectors": ["orison.market", "orison.relay", "orbit.ring"],
  "Objectives": [
    {"Id": "meet_seli", "Requires": []},
    {"Id": "reach_relay", "Requires": ["meet_seli"],
     "Approaches": ["service_path", "credential"]},
    {"Id": "claim_core", "Requires": ["reach_relay"]},
    {"Id": "choose_lease", "Requires": ["claim_core"]},
    {"Id": "reach_ring", "Requires": ["choose_lease"]},
    {"Id": "return_market", "Requires": ["reach_ring"]}
  ],
  "FinalObjective": "return_market",
  "CompletionPolicy": "all_required_in_order",
  "Decision": {
    "Key": "lease.first_window",
    "Allowed": ["clinic_backup", "dock_evacuation"],
    "MutuallyExclusive": true,
    "ReversibleDuringMission": false
  },
  "RewardKey": "reward.borrowed_morning.once",
  "RecoveryAnchor": "pad.market.safe"
}
```

The example's reach_relay alternatives converge on one objective. Requires is the authoritative prerequisite graph; the validator rejects cycles, missing references and unreachable completion. Runtime events update only eligible objectives and never assume the player followed a cinematic order. Keep early-collected evidence in WorldState so it can satisfy a later objective when that objective becomes eligible, without awarding it twice.

**Objective state machine.** Locked becomes available when prerequisites pass; available becomes active on entry; active becomes completed by a validated gameplay event. Optional failure is an explicit supported terminal state. Abandoned/restarted missions must define which persistent decisions survive. Completion emits one reward transaction keyed by the unique reward ID.

**Dialogue record.** Store a stable line ID, speaker ID, text/localization key, conditions, consequence command, voice event, subtitle timing and next node. Conditions query WorldState. Presentation cannot write inventory directly. A conversation interrupted by pursuit records whether the important information was delivered and offers a natural resumable line.

**NPC behavior contract.** Guards use idle/patrol, investigate, challenge, fight, search and return states. Each transition names perception evidence and an interrupt rule. Named civilians use schedule anchors and mission overrides; they do not need minute-by-minute lives everywhere in the world. Flight AI uses a route/volume graph with steering and collision checks, separate from ground navigation. Do not ask a ground navigation mesh to solve open-space flight.

### Outcome and recovery table

| Event | State that survives | Recovery behavior |
|---|---|---|
| Detection | All committed progress, cargo and choices | Mission continues through supported combat/escape path |
| Player chooses retry after defeat | The selected validated checkpoint snapshot | Restore all resources and decisions from that same snapshot; no selective reward retention |
| Authored capture or surrender | Explicitly committed current mission outcomes | A single capture transaction relocates player/cargo and starts a supported escape or release beat |
| Optional job failure | One recorded outcome and only its specified consequences | Main story remains completable; no repeated reward on retry |
| Precommit travel cancellation | Source-side custody and current state | Remain in source sector or return to its safe route |
| Postcommit interruption in current session | Committed destination-side state | Reconstruct destination presentation once |
| Process restart | Newest intact compatible saved revision | Resolve its journal/transfer record, preserving any newer file for diagnosis rather than assuming memory was saved |

Capture is authored content and is optional in the first release. If no coherent capture route exists, use a clearly labeled retry rather than pretending a checkpoint reset is a persistent narrative consequence.

### Initial feature backlog

The first twelve-week plan remains the capacity authority. These items split its six work periods into implementable results; estimates are assigned after a short technical breakdown and must total no more than the 240-hour prototype allowance.

| Ticket | Dependency | Required result |
|---|---|---|
| BS001 Build and restore | None | Versioned blank project packages from restored source |
| BS002 Input and camera | BS001 | Rebindable walk, look, interact and pause on both devices |
| BS003 Movement gym | BS002 | Ground routes, stairs, mantle and obstruction tests |
| BS004 State authority | BS001 | Stable IDs, commands, revision checks and transaction deduplication |
| BS005 Core custody | BS004 | Exactly one sunseed across collect, socket and removal |
| BS006 Checkpoint restore | BS004, BS005 | Coherent save snapshot and corrupted-generation fallback |
| BS007 Flight gym | BS002 | Hover, cruise, brake and assisted docking |
| BS008 Pilot handoff | BS003, BS005, BS007 | Enter/exit preserves identity, tool custody and input |
| BS009 Sector readiness | BS001, BS007 | Preload and validate destination collision and anchors |
| BS010 Transfer and return | BS006, BS008, BS009 | State survives both directions and interruption cases |
| BS011 Relay challenge | BS003, BS005 | Two approaches and recoverable detection |
| BS012 Allocation and consequence | BS004, BS011 | Exclusive lease, one changed service and one changed line |
| BS013 Complete expedition | BS006, BS010, BS012 | Ten-minute ground-orbit-return loop |
| BS014 Asset intake pilot | BS001, BS003 | One sample block cooks and meets an explicit budget |
| BS015 External test and decision | BS013, BS014 | Evidence packet, failures, revised estimate and go/no-go |

The prototype can omit polished combat, final voice, advanced PCG and expensive character art. It cannot omit returning, saving or recovering from an interrupted transition. Those are architectural facts on which the release depends.

### Full vision work packages and owners

These packages make the ambition actionable after the solo slice. They are not included in the solo release budget. A role is an accountable specialty; it does not imply that one hire can safely own every adjacent system. Commission a staffed work breakdown using the measured slice and technology spikes before giving a funded expansion a date.

| Package | Accountable specialty | Dependency and proof before production |
|---|---|---|
| Planetary surface and gravity | Senior world/physics engineer plus technical artist | Small sphere with radial movement, poles, terrain LOD/collision, landing and save restoration |
| Wider celestial travel | Rendering/streaming engineer | Stable frame transformations, distant bodies, atmosphere and representation handoff under speed/reversal tests |
| Two-player co-op | Network gameplay engineer | Host authority, cargo ownership, join/rejoin, separate-scale play, decision rules and reconnect recovery in a greybox |
| Crew and NPC expansion | AI/gameplay engineer with narrative designer | Existing three-person roles prove value; crowd/crew simulation cost measured before growth |
| More cities and missions | Environment lead and mission designer | A second approved district and job meet the measured reuse/throughput target |
| Character diversity | Character artist and technical animator | A new body family passes every locomotion, seat, weapon and interaction test |
| Cinematics and performances | Narrative/cinematic lead, actors and audio specialists | Story lock, scene budget, rights and iteration plan |
| Additional platforms | Platform engineer, producer and QA | Approved access, hardware performance target, certification schedule and patch support capacity |

**Funding trigger.** Pitch the full vision only after an externally tested slice, measured content throughput, documented rights and a technical demonstration of the chosen next risk. Supply a playable build, actual footage, remaining work quantities, a bottom-up budget and a fallback release plan. Seek money for defined production work rather than claiming the entire planetary game is almost finished.

**Co-op design seed.** If funded, the host owns the world and sunseed state. Guests retain only explicitly agreed personal progression; story choices require a declared host or consensus rule. Rejoin reconstructs the latest authoritative state, and departure cannot remove the only mission core. The one-ship/two-role experience must be enjoyable before permitting independent ships and unlimited separation. Supporting co-op later remains a major implementation task even with clean interfaces today.

**Evidence packet for every gate.** Include the packaged build ID, exact hardware and engine/plugin versions, test route, observed frame/memory captures, save fixtures, player observations, accepted asset manifest, remaining defects, actual hours and current forecast. This makes decisions repeatable and keeps the workflow efficient when the creator returns after a break or brings in help.



## 34 Sources and reading list

All sources were checked for this research on 12 October 2026. Historical descriptions establish what was shown or stated at the time. Engine documentation, licenses, prices and platform policies can change; the accepted product version and agreement govern implementation and release. Source links also appear beside the claims they support.

### Beyond Good and Evil evidence

- **B01** [Beyond Good and Evil 2 — Behind Closed Doors Impressions at E3 2017](https://news.ubisoft.com/en-us/article/67WvwEbZCCchld4NShoCH1/beyond-good-and-evil-2-behind-closed-doors-impressions-at-e3-2017). Published or updated 2017-06-15. Ubisoft primary report of developer presentation; historical design and controlled prototype.
- **B02** [Beyond Good and Evil 2 — First In-Engine Demo Revealed](https://news.ubisoft.com/en-us/article/3DumSXRBRvRiApnV2cebqy/beyond-good-and-evil-2-first-inengine-demo-revealed). Published or updated 2017-06-23. Ubisoft announcement of official demo video; video located but not fully inspected.
- **B03** [Beyond Good and Evil 2 Developers Show Off New Ships and Staff Combat Gameplay](https://news.ubisoft.com/en-us/article/7pOyDJJtQNigXHtxvifvjH/beyond-good-and-evil-2-developers-show-off-new-ships-and-staff-combat-gameplay). Published or updated 2018-05-03. Ubisoft primary gameplay-update recap.
- **B04** [Beyond Good and Evil 2 — Explore a Dynamic Star System From Underground to Outer Space](https://news.ubisoft.com/en-us/article/3eqbUFFyPqb863eNgD1AOf/beyond-good-and-evil-2-explore-a-dynamic-star-system-from-underground-to-outer-space-e3-2018). Published or updated 2018-06-19. Ubisoft primary 2018 demo report; evidence of a prototype, not shipping guarantees.
- **B05** [E3 2018: Beyond Good & Evil 2's Gameplay Explained](https://www.gamespot.com/articles/e3-2018-beyond-good-and-evil-2s-gameplay-explained/1100-6459863/). Published or updated 2018-06-18. Edmond Tran, GameSpot; contemporary eyewitness report of live demonstration.
- **B06** [Beyond Good and Evil 2: Gameplay Update — Augments, Vehicles, Co-op and Spyglass](https://www.youtube.com/watch?v=Y2229DmJLIY). Published or updated 2018-12-10. Official Ubisoft video description; footage was not independently analyzed in full.
- **B07** [Space Monkey Report 4: VOD & Questions Answered](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5qRTNN5cQlNkLqZIpBFDmM/space-monkey-report-4-vod-questions-answered). Published or updated 2018-12-21. Ubisoft primary Q&A; includes historical online requirement and co-op separation claim.
- **B08** [Space Monkey Report 5: The Journey From Clone to Pirate](https://news.ubisoft.com/en-us/article/47aHrZWvifWAQuRUl0l1ui/beyond-good-and-evil-2-space-monkey-report-5-livestream-showcases-the-journey-from-clone-to-pirate). Published or updated 2019-06-05. Ubisoft primary recap; one proposed player origin.
- **B09** [Space Monkey Report 5 Livestream: Your Questions Answered](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/2WxLDvDewvgz6Smza4jCbF/space-monkey-report-5-livestream-your-questions-answered). Published or updated 2019-06-19. Ubisoft primary Q&A; character options, moral latitude, production update.
- **B10** [Beyond Good and Evil 2 — New Hero Design, Rhino Hybrids and Customization](https://news.ubisoft.com/en-us/article/6T4sPdU8DGukHxKjZUPbti/beyond-good-and-evil-2-new-hero-design-rhino-hybrids-and-customization). Published or updated 2019-10-31. Ubisoft primary recap including sound and musical identity.
- **B11** [Meet the BGE2 Characters From the E3 Trailer](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3DhkYd63K2mFPIFqsCIEfG/meet-the-bge2-characters-from-the-e3-trailer). Published or updated 2017. Ubisoft primary character descriptions; legacy localized date ambiguous.
- **B12** [Jade Is Back and More: E3 2018 Trailer Breakdown](https://www.ubisoft.com/en-gb/game/beyond-good-and-evil-2/news-updates/5DxAQDfi7VXh1XKfZxzdXX/jade-is-back-and-more-e3-2018-trailer-breakdown). Published or updated 2018-08-24. Ubisoft cinematic analysis; no proof of playable cinematic action.
- **B13** [Présentation des personnages du trailer de l'E3 2018](https://www.ubisoft.com/fr-ca/game/beyond-good-and-evil-2/news-updates/3tMnrt7UfMfaPqqfKJ964M/prsentation-des-personnages-du-trailer-de-le3-2018). Published or updated 2018-09-27. Ubisoft French primary source, paraphrased in English; legendary officers and ship roles.
- **B14** [Diwalite: The Ultimate (Re)source of Power](https://www.ubisoft.com/en-us/game/beyond-good-and-evil-2/news-updates/3UG9FX9AeIl0Lpq2iWI6PA/diwalite-the-ultimate-resource-of-power). Published or updated 2019-02-14. Ubisoft primary worldbuilding article.
- **B15** [Cityscapes: Ganesha Inspirations](https://www.ubisoft.com/en-au/game/beyond-good-and-evil-2/news-updates/5pzusN60hnwscppROS1YTs/cityscapes-ganesha-inspirations). Published or updated 2017. Ubisoft primary cultural reference statement; legacy localized date ambiguous.
- **B16** [Showcasing Our Fans in Beyond Good and Evil 2](https://www.ubisoft.com/ja-jp/game/beyond-good-and-evil-2/news-updates/3TpQdaiIiRjTpeK5t4g5ed/showcasing-our-fans-in-beyond-good-and-evil-2). Published or updated 2018-06-14. Ubisoft primary community asset contribution and payment statement; English text on localized page.
- **B17** [An Update on Beyond Good & Evil 2](https://news.ubisoft.com/en-us/article/6ptJuA0jx7qhvhTnSosl8L/an-update-on-beyond-good-evil-2). Published or updated 2020-09-18. Ubisoft senior producer Guillaume Brunier; attributed internal milestone and team change.
- **B19** [Beyond Good and Evil 2 — Space Monkey Program FAQ](https://www.ubisoft.com/en-gb/game/beyond-good-and-evil-2/space-monkey-program). Accessed 2026-10-12; no announced release date.
- **B20** [Fawzi Mesmar: Beyond Good and Evil 2 Project Status](https://www.linkedin.com/feed/update/urn:li:activity:7421227406130335744/). Published or updated 2026-01. Accessible direct primary statement; page shows relative age, timing corroborated by B25.
- **B21** [Rik Godwin: Working on Beyond Good & Evil 2](https://bsky.app/profile/rikgodwin.bsky.social/post/3mw4li3vxvc2a). Published or updated 2026-09-22. Direct public primary statement; timestamp 15:54:15 UTC.
- **B22** [Why Beyond Good & Evil 2 Has Taken So Long](https://www.gamespot.com/articles/why-beyond-good-and-evil-2-has-taken-so-long/1100-6444617/). Published or updated 2016-10-19. Mat Paget, GameSpot; secondary report quoting Ancel's retrospective developer post.
- **B23** [Watch 25 Minutes of Beyond Good and Evil 2](https://www.pcgamer.com/watch-25-minutes-of-beyond-good-and-evil-2/). Published or updated 2018-12-10. Fraser Brown, PC Gamer; contemporary secondary coverage identifying repeat E3 demo.
- **B24** [New Beyond Good & Evil 2 Footage Showcases Co-op, Recruitment and Customization](https://www.gamesradar.com/new-beyond-good-and-evil-2-footage-showcases-seamless-co-op-npc-recruitment-and-ship-customization/). Published or updated 2018-12-10. Austin Wood, GamesRadar; contemporary secondary coverage. Projected 2019 beta is not a current fact.
- **B25** [Beyond Good & Evil 2 Lead Confirms Project Unaffected by Ubisoft Changes](https://www.gamesradar.com/games/open-world/beyond-good-and-evil-2-lead-promises-to-deliver-a-remarkable-game-for-our-players-to-enjoy-as-he-confirms-the-game-is-unaffected-by-the-recent-changes-at-ubisoft/). Published or updated 2026-01-30. George Young, GamesRadar; secondary corroboration of B20 timing.
- **B26** [Beyond Good & Evil — 20th Anniversary Edition](https://www.ubisoft.com/en-au/games/beyond-good-and-evil/editions/20th-anniversary). Published or updated 2024. Ubisoft product page; Jade's past treasure hunt and original game context.
- **B27** [Beyond Good & Evil — 20th Anniversary Edition Out Now](https://news.ubisoft.com/en-gb/article/4HYZp0t1d14z2afFYmzjju/beyond-good--evil---20th-anniversary-edition-launches-on-june-25). Published or updated 2024-06-25. Mikel Reparaz, Ubisoft; primary retrospective on the original game's plot and mechanics.
- **B28** [Ubisoft Brands — Beyond Good & Evil](https://www.ubisoft.com/en-us/company/about-us/our-brands/beyond-good-evil). Ubisoft primary franchise description; government conspiracy theme. Accessed 2026-10-12.

### Unreal and KitBash technical documentation

- **T03** [KitBash3D: Cargo Unreal workflow](https://help.kitbash3d.com/en/articles/7833653-using-unreal-engine-with-cargo-workflows-and-integration). Official documentation. Verify behavior against the pinned engine or asset version.
- **T04** [KitBash3D: Supported software](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines). Official documentation. Verify behavior against the pinned engine or asset version.
- **T05** [KitBash3D: Exclude Cargo from builds](https://help.kitbash3d.com/en/articles/12689467-exclude-kitbash3d-cargo-plugin-from-unreal-engine-builds). Official documentation. Verify behavior against the pinned engine or asset version.
- **T06** [KitBash3D: Gameplay Ready](https://kitbash3d.com/pages/gameplay-ready-kits). Official documentation. Verify behavior against the pinned engine or asset version.
- **T07** [KitBash3D: Unreal structure updates](https://help.kitbash3d.com/en/articles/11698511-structural-updates-to-unreal-engine-kits-gameplay-ready). Official documentation. Verify behavior against the pinned engine or asset version.
- **T08** [Epic: World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T09** [Epic: World Partition HLOD](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T10** [Epic: PCG generation modes](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-pcg-generation-modes-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T11** [Epic: Large World Coordinates](https://dev.epicgames.com/documentation/en-us/unreal-engine/large-world-coordinates-in-unreal-engine-5). Official documentation. Verify behavior against the pinned engine or asset version.
- **T12** [Epic: Sky Atmosphere](https://dev.epicgames.com/documentation/en-us/unreal-engine/sky-atmosphere-component-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T13** [Epic: Georeferencing](https://dev.epicgames.com/documentation/en-us/unreal-engine/georeferencing-a-level-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T14** [Epic: Nanite](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T15** [Epic: Lumen performance](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-performance-guide-for-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T16** [Epic: Lumen technical details](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T17** [Epic: Gameplay Ability System](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T18** [Epic: StateTree](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T19** [Epic: MassEntity](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T22** [Epic: World Partition Navmesh](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partitioned-navigation-mesh). Official documentation. Verify behavior against the pinned engine or asset version.
- **T23** [Epic: IK retargeting](https://dev.epicgames.com/documentation/en-us/unreal-engine/ik-rig-animation-retargeting-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T24** [Epic: Save/load](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T25** [Epic: Asset Manager](https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T26** [Epic: Editor Python](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python). Official documentation. Verify behavior against the pinned engine or asset version.
- **T27** [Epic: World Partition builders](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-builder-commandlet-reference). Official documentation. Verify behavior against the pinned engine or asset version.
- **T28** [Epic: Derived Data Cache](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-derived-data-cache-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T29** [Epic: Automation Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T30** [Epic: Gauntlet](https://dev.epicgames.com/documentation/en-us/unreal-engine/gauntlet-automation-framework-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T31** [Epic: Unreal Insights](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-insights-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T32** [Epic: PSO precaching](https://dev.epicgames.com/documentation/en-us/unreal-engine/pso-precaching-for-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T33** [KitBash3D: Understanding Interiors in KitBash3D Kits](https://help.kitbash3d.com/en/articles/2445183-understanding-interiors-in-kitbash3d-kits). Published or updated 2026-08-25. Official documentation. Verify behavior against the pinned engine or asset version.
- **T34** [KitBash3D: Introducing Gameplay Ready Kits](https://kitbash3d.com/a/blog/introducing-gameplay-ready-kits-built-for-real-time-performance-in-unreal-engine). Official documentation. Verify behavior against the pinned engine or asset version.
- **T35** [KitBash3D: Free Neo City mini-kit](https://kitbash3d.com/products/mini-kit-neo-city). Official documentation. Verify behavior against the pinned engine or asset version.

### Production rights and release

- **P03** [Everspace 2 delivers a handcrafted universe brimming with space combat](https://www.unrealengine.com/developer-interviews/everspace-2-delivers-a-handcrafted-universe-brimming-with-space-combat?lang=en-US). Firsthand ROCKFISH interview hosted by Epic: 25-person studio, separate handcrafted location maps, native base classes and Blueprint children.
- **P04** [Unreal Engine End User License Agreement](https://www.unrealengine.com/eula/unreal). Controlling engine agreement: royalty-product seats, AI restrictions in section 6(e), release notice and 5%/qualifying 3.5% royalty provisions. Recheck accepted version before commercialization.
- **P05** [Xbox Accessibility Guidelines V3.2](https://learn.microsoft.com/en-us/xbox/accessibility/guidelines). Published or updated 2023-06-08. Microsoft accessibility best practices. Explicitly not a legal-compliance checklist.
- **P06** [Unreal Engine licensing options](https://www.unrealengine.com/license). Official licensing overview: royalty-based commercial games, first US$1 million lifetime revenue exclusion and engine seats distinction.
- **P07** [What's the difference between purchasing a Kit and a subscription?](https://help.kitbash3d.com/en/articles/10715523-what-s-the-difference-between-purchasing-a-kit-and-a-subscription). Published or updated 2026-04-20. Official KitBash guidance: perpetual kit vs active-term subscription rights. Includes broad AI-processing restriction for subscription assets; exact Usage Rights control.
- **P08** [KitBash3D Subscription FAQs](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs). Published or updated 2026-08-25. Individual vs Teams including employees, owners and contractors; properly incorporated Larger Works after cancellation; no new projects.
- **P09** [KitBash License Agreement](https://kitbash3d.com/pages/license-text). Official 18-page agreement, sections 3.2.2–3.2.7 on incorporation, subscription expiry and AI-training inputs; read alongside purchased Usage Rights.
- **P10** [Fab Standard License](https://www.fab.com/eula?lang=en). Official Standard License overview and CC-BY distinction; commercial incorporated use allowed, standalone redistribution prohibited. Product license matters.
- **P11** [Steamworks Content Survey](https://partner.steamgames.com/doc/gettingstarted/contentsurvey?language=english). Current Valve guidance distinguishes development efficiency from AI-generated content consumed by players; pre-generated disclosure and live-generated guardrails.
- **P12** [Copyright and Artificial Intelligence, Part 2: Copyrightability](https://www.copyright.gov/ai/Copyright-and-Artificial-Intelligence-Part-2-Copyrightability-Report.pdf). Published or updated 2025-01. U.S. Copyright Office report: human authorship, creative selection/modification, limits of prompting; jurisdiction-specific guidance.
- **P13** [Games — U.S. Copyright Office](https://www.copyright.gov/register/tx-games.html). Official distinction between game ideas/methods and protected expressive elements; not trademark clearance or blanket permission to copy.
- **P14** [Steam Direct Fee](https://partner.steamgames.com/doc/gettingstarted/appfee). Official US$100 app fee, recoupable after US$1,000 adjusted gross revenue under stated terms.
- **P15** [Steam Direct — Joining the Steamworks Distribution Program](https://partner.steamgames.com/steamdirect/?l=english). Identity/tax onboarding, first-title 30-day app-fee wait and two-week Coming Soon minimum.
- **P16** [Steamworks Release Process](https://partner.steamgames.com/doc/store/releasing). Official store and game build review requirements; store page review before build review; both approval checklists required.
- **P17** [Age Ratings Mandatory in Germany](https://partner.steamgames.com/doc/gettingstarted/contentsurvey/germany?language=english). Valve: games missing valid age rating are not displayed to customers in Germany; truthful content survey is required for Valve rating process.
- **P18** [Join ID@XBOX — submit a game concept](https://learn.microsoft.com/en-us/gaming/game-publishing/onboarding/onboarding-submit-game-concept). Official current Xbox onboarding: concept approval, Title Licensing Agreement and GDK agreement.
- **P19** [Showing your Game to PlayStation](https://sonyinteractive.com/en/news/blog/showing-your-game-to-playstation/). Official Sony partner process: approved registration and Global Developer and Publisher Agreement unlock resources.
- **P20** [Register as a Nintendo Developer](https://developer.nintendo.com/register). Official developer registration and platform access request process; registration is distinct from requested platform access.

### Firsthand design and art lessons

- **R01** [Mobius Digital Demaking Outer Wilds](https://www.mobiusdigitalgames.com/news/demaking-outer-wilds). Published or updated 2015-07-30. Firsthand account of paper and text prototypes used to test narrative comprehension before full 3D integration.
- **R02** [Mobius Digital The Intentionality of Wandering](https://www.mobiusdigitalgames.com/news/the-intentionality-of-wandering). Published or updated 2016-08-18. Firsthand discussion of path cues, informed exploration and level-design iteration.
- **R03** [Mobius Digital Our New Style](https://www.mobiusdigitalgames.com/news/our-new-style). Published or updated 2018-03-30. Firsthand art direction account addressing ground and space scales, navigation and production iteration.
