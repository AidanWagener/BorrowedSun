# Research foundations for the living planet

Checked 13 October 2026. This is an initial research ledger, not the completed research sprint. It records what a source actually establishes, its limits, and the original design question it helps answer. Fictional choices and production hypotheses remain distinct from researched facts.

The subsequent [physical foundation review](Drafts/PHYSICAL_FOUNDATION_REVIEW.md) extends the astronomical investigation with equations, numerical checks and additional primary sources. The [ecology](Drafts/ECOLOGY_FOUNDATION.md) and [societies](Drafts/SOCIETIES_FOUNDATION.md) foundations provide their own bounded research ledgers. The initial abstract-only notes below are historical scope statements, not a claim that the later review stopped there.

## Astronomy and time

**NASA — tidal locking.** Synchronous rotation describes a body's rotation relative to its orbital partner. It does not mean the body has stopped rotating. Use this to define the moon's coordinate frame and parent-facing hemisphere. It does not establish our fictional orbit, atmosphere or habitability. [Source](https://science.nasa.gov/moon/tidal-locking/)

**NASA — Moon facts.** The lunar far side also receives sunlight: parent-facing orientation and stellar illumination are different relationships. The fictional sky model must therefore track star direction independently of the parent planet's apparent direction. [Source](https://science.nasa.gov/moon/facts/)

**Heller and Barnes — Exomoon habitability constrained by illumination and tidal heating.** The authors consider stellar and planetary illumination, eclipses and tidal heating together; some configurations can exceed a runaway-greenhouse threshold. Their paper supports treating habitability as a coupled problem, not selecting an attractive orbital distance in isolation. Abstract reviewed; full equations, assumptions and parameter applicability require further examination before numerical adoption. [Paper, arXiv:1209.5323](https://arxiv.org/abs/1209.5323)

**Heller — Exomoon habitability constrained by energy flux and orbital stability.** This work adds eclipse effects and discusses the tension between close, stable moon orbits and excessive heating in low-mass-star systems. Its bounds depend on model assumptions and individual system dynamics. Do not present its example values as universal guarantees for our world. Abstract reviewed; full-method review pending. [Paper, arXiv:1209.0050](https://arxiv.org/abs/1209.0050)

**Design work required:** jointly select star mass/luminosity, parent mass/radius/orbit, moon mass/radius/orbit, eccentricity, obliquity, atmospheric assumptions and age. Calculate gravity, circumference, surface area, orbital and solar periods, apparent parent size, eclipse conditions and approximate stability margins. Distinguish a plausibility calculation from a full dynamical or climate simulation. Establish the calendar only after those quantities agree.

The bible must distinguish physical time from game pacing. A long astronomical day may contain human work/rest cycles and local mirror-service schedules. Those social cycles should have practical explanations; they cannot silently replace the natural sky. Any time acceleration or travel compression in the eventual game must preserve ordering of departures, arrivals, weather, appointments and saved consequences.

## Climate, land and water

**NOAA — Global atmospheric circulations.** Earth's circulation reflects differential heating, rotation, tilt and uneven land/water distribution. It supplies a baseline vocabulary for winds and pressure belts, but copying Earth's three-cell pattern onto a differently rotating moon is an untested assumption. [Source](https://www.noaa.gov/jetstream/global/global-atmospheric-circulations)

**NOAA — The marine layer.** Cold coastal water, inversions and relief help explain why fog persists in some places and penetrates unevenly inland. This is useful for a coherent coastal region, not a justification for putting a fog preset at arbitrary coordinates. [Source](https://www.noaa.gov/jetstream/ocean/marine-layer)

**USGS — Understanding plate motions.** Divergence, convergence and continental collision produce different geological structures. Use those distinctions to place coherent mountain chains, basins and volcanic provinces before decorating them with settlements. This does not prove that the fictional moon sustains Earth-like plate tectonics. [Source](https://pubs.usgs.gov/gip/dynamic/understanding.html)

**USGS — Rivers, streams and creeks.** Rivers belong to drainage basins, and groundwater and runoff contribute differently to their flow. The atlas must preserve downhill flow and account for closed basins instead of drawing rivers as decorative lines between landmarks. [Source](https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks)

**Design work required:** create elevation and basin structure, establish seasonal insolation and provisional circulation, place ocean currents and major water bodies, then derive precipitation, snow/ice, vegetation and agricultural suitability. Expose assumptions where no numerical climate model is used. Keep transitional ecotones rather than hard biome boundaries. Maps need legends, scale, projection, coordinate conventions and revision IDs.

A desert can have rivers supplied by distant mountains; a wet coastline can border a dry interior; a high plateau can be cold at a latitude where a low port is warm. Each such case must be explained in the atlas. Regional weather tables should express related regimes—fronts, onset, sustained conditions and clearing—rather than an independent random selection every few minutes.

## Unreal: what the existing capabilities establish

**Epic — World Partition.** The documented system divides a persistent world into cells and streams them around sources, with related tools for actor files, data layers and hierarchical detail. This supplies content management and streaming infrastructure. It does not by itself define a spherical terrain hierarchy, radial movement, planetary ecology or the location of every settlement. [UE 5.8 documentation](https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine)

**Epic — Large World Coordinates.** Double-precision variants improve placement and orientation precision; the overview still identifies the feature as Beta. Precision support is one part of a planet implementation, not evidence that all dependent systems support arbitrary planetary play. [UE 5.8 documentation](https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-unreal-engine-5)

**Local pinned engine check.** UE 5.8.2 CL 56702186, `Engine/Source/Runtime/Engine/Public/EngineDefines.h`, lines 37–57, defines `UE_LARGE_WORLD_MAX` as `8796093022208.0` engine units and defaults `UE_USE_UE4_WORLD_MAX` to zero. The comment describes approximately 87,960,930.2 km across. This checks the constant in the installed source; it is not a measured usable terrain, physics, streaming or rendering limit.

**Documentation discrepancy.** Epic's older conversion guidance retains wording about the initial UE 5.0 default of roughly 21 km, whereas the overview and installed header above describe the large-world setting. Retain engine/version evidence when evaluating limits instead of combining conflicting passages into a new claim. [Conversion guidance](https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-project-conversion-guidelines-in-unreal-engine-5)

**Epic — Georeferencing.** The plugin documents transformations between coordinate reference systems and distinguishes flat and round planet cases. Evaluate custom-world coordinate support and data ownership before adopting it for a fictional world; Earth georeferencing examples do not automatically provide a planetary terrain generator. [UE 5.8 documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/georeferencing-a-level-in-unreal-engine)

**Epic — LWC rendering and Niagara.** Rendering uses specialized precision representations, while Niagara handles large positions using a tile-relative approach. World-scale shaders, effects and coordinate conversions therefore need their own checks. A double-precision position in game code is insufficient evidence for every visual effect. [Rendering](https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-rendering-in-unreal-engine-5?lang=en-US), [Niagara](https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-niagara-for-unreal-engine)

**Design work required:** choose one authoritative planetary coordinate convention, stable location identifiers and reversible local-frame transforms. Compare production architectures against the actual canon: surface traversal, intercontinental flight, horizon views, landing freedom, underground/water spaces and regional persistence. Validate seam crossings, polar travel, collision availability, save/reload and repeated return journeys in later construction. Research remains separate from enabling plugins or changing the current game map.

## Remaining research programme

The next investigations must cover terrain/erosion and drainage generation; exomoon atmospheric and ocean assumptions; biodiversity and food-web design; settlement networks and historical urban growth; vernacular climate adaptation; migration and cultural exchange; maritime logistics; energy/water/food/waste systems; linguistic worldbuilding; archaeology and contested memory; exploration and knowledge-led discovery; authored NPC social simulation; event propagation; soundscapes; asset reuse; and the practical planet production stack.

Use primary institutional, scholarly, creator and official technical sources. Record direct observations, analogies, design inferences and unresolved questions separately. Do not convert an external developer's result into a promised solo throughput. Do not substitute a giant repository list for a tested integration advantage. When sourcing assets later, inspect the actual kit's contents, rights, version and fit to a defined world requirement.
