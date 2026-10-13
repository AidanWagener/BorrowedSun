# Talweir plan and relationship checks

Generated from working city edition 0.1. These checks concern authored controls and relationships, not surveyed terrain, navigability, hydraulic capacity or runtime behavior.

## Placement and parent geography

Nine district envelopes and twenty-four POIs; district population and gross-area allocations reconcile. POIs have known districts, envelope/altitude checks and explicit wet-placement rules. Deck altitude is checked against the nearest interpolated nominal channel surface, or the standing-water surface, rather than a descending envelope's lowest vertex. Trial Pond's marker is a focus with a separate dry visitor-access point. The two supported decks still require engineering.

Whole Namar and Iren polygons are compared to the parent geography and independently projected using spherical AEQD, with a 0.02 m rounding tolerance. Reference frame identity, radius, orientation, source river controls and the parent lake connection agree. Cardinal and dateline projection controls also pass.

Namar 180 m → junction 179.7 m → Iren 176 m retains the approximately 19.827 km source-control dogleg. The direct outlet-to-inlet spherical distance is approximately 10.457 km. These are control distances, not measured navigable routes.

## Transport dependencies

All transport nodes and every district connect to a declared regional gateway in the undirected service graph. This proves relationship connectivity only. Water services require access legs; land interchanges are not berths. Empty alignment lists remain unresolved.

Separations below are straight horizontal distances in the local projection, not route lengths or guaranteed geodesic lower bounds.

| Link | Service | Mode | Straight local separation km |
|---|---|---|---:|
| TAL-TR-001 | West Line | surface_tram | 4.319 |
| TAL-TR-002 | West Line | surface_tram | 3.384 |
| TAL-TR-003 | Market Line | surface_tram | 3.905 |
| TAL-TR-004 | Market Line | surface_tram | 2.907 |
| TAL-TR-005 | Upper Line | surface_tram | 3.536 |
| TAL-TR-006 | Upper Line | surface_tram | 8.246 |
| TAL-TR-007 | East Line | surface_tram | 4.319 |
| TAL-TR-008 | East Line | surface_tram | 3.956 |
| TAL-TR-009 | South Line | surface_tram | 5.459 |
| TAL-TR-010 | South Line | surface_tram | 6.021 |
| TAL-TR-011 | Eastern crosslink | local_transit | 1.803 |
| TAL-TR-012 | Eastern crosslink | local_transit | 6.227 |
| TAL-TR-013 | Regional freight | freight_rail | 3.106 |
| TAL-TR-014 | Urban freight connection | freight_rail | 3.905 |
| TAL-TR-015 | Upper freight connection | freight_rail | 16.815 |
| TAL-TR-016 | Yard freight connection | freight_rail | 6.227 |
| TAL-TR-017 | Yard freight connection | freight_rail | 2.302 |
| TAL-TR-018 | Upper water service | local_ferry | 8.246 |
| TAL-TR-019 | Upper water service | local_ferry | 3.536 |
| TAL-TR-020 | Bend crossing | local_ferry | 4.319 |
| TAL-TR-021 | Lower water service | local_ferry | 2.720 |
| TAL-TR-022 | Lower water service | local_ferry | 6.021 |
| TAL-TR-023 | Namar regional approach | regional_boat_connection | 1.556 |
| TAL-TR-024 | Iren regional approach | regional_boat_connection | 3.558 |
| TAL-TR-025 | Namar through navigation | through_waterway | 12.556 |
| TAL-TR-026 | Lower through navigation | through_waterway | 8.470 |
| TAL-TR-027 | Western approach road | regional_road | 3.106 |

## Water and channels

Main channel endpoints retain source surfaces; the navigation branch meets its host envelope within 0.1 m of interpolated nominal water level. Declared lock lifts match their adjacent passage controls and refer to a distinct bypass. This is a geometric screening tolerance, not a hydraulic accuracy claim.

| Link | System | Endpoints | Endpoint altitude change m | Declared mode |
|---|---|---|---:|---|
| TAL-WL-001 | potable_raw | TAL-W01 → TAL-W02 | +48.04 | pumped |
| TAL-WL-002 | potable | TAL-W02 → TAL-W03 | +10.00 | pumped |
| TAL-WL-003 | potable | TAL-W02 → TAL-W04 | +27.00 | pumped |
| TAL-WL-004 | potable | TAL-W03 → TAL-W05 | -43.00 | gravity_candidate_regulated |
| TAL-WL-005 | potable | TAL-W03 → TAL-W06 | -39.00 | gravity_candidate_regulated |
| TAL-WL-006 | potable | TAL-W03 → TAL-W07 | -36.00 | gravity_candidate_regulated |
| TAL-WL-007 | potable | TAL-W04 → TAL-W08 | -23.00 | gravity_candidate_regulated |
| TAL-WL-008 | potable | TAL-W03 → TAL-W09 | -48.00 | gravity_candidate_regulated |
| TAL-WL-009 | potable | TAL-W03 → TAL-W10 | -43.00 | gravity_candidate_regulated |
| TAL-WL-010 | potable | TAL-W03 → TAL-W11 | -26.00 | pumped_boosted_transfer |
| TAL-WL-011 | potable | TAL-W11 → TAL-W20 | -22.00 | gravity_candidate_regulated |
| TAL-WL-012 | potable | TAL-W03 → TAL-W21 | -20.00 | gravity_candidate_regulated |
| TAL-WL-013 | potable | TAL-W03 → TAL-W22 | -50.00 | gravity_candidate_regulated |
| TAL-WL-014 | sewer | TAL-W15 → TAL-W12 | +7.00 | pumped |
| TAL-WL-015 | sewer | TAL-W13 → TAL-W14 | +3.00 | pumped |
| TAL-WL-016 | treated_residual | TAL-W12 → TAL-W16 | -13.75 | gravity_candidate |
| TAL-WL-017 | treated_residual | TAL-W14 → TAL-W16 | -9.75 | gravity_candidate |
| TAL-WL-018 | reclaimed | TAL-W14 → TAL-W17 | +14.00 | pumped |
| TAL-WL-019 | reclaimed_industrial | TAL-W17 → TAL-W09 | -10.00 | gravity_candidate_regulated |
| TAL-WL-020 | reclaimed_industrial | TAL-W17 → TAL-W08 | +32.00 | pumped |
| TAL-WL-021 | potable_raw_reserve | TAL-W18 → TAL-W19 | +11.00 | pumped |
| TAL-WL-022 | potable_reserve | TAL-W19 → TAL-W10 | +8.00 | pumped_normally_isolated |

Water links obey typed raw/treatment/potable/sewer/reclaimed relationships. Every delivery zone has an intake-to-treatment supply dependency. A shared zone coordinate does not mix reclaimed and potable pipework. Rising endpoint dependencies require an explicit pumped mode; no pump head, invert, hydraulic grade or delivery pressure is inferred from facility altitude. Unmodeled losses may require pumping even when endpoints descend.

## Regression controls

Rejected negative controls: wrong reference radius; projection translation; changed source lake surface; junction shifted off river control; broken downstream connection; rising bypass surface; incorrect lock lift; invalid water polygon; unassigned POI; POI outside envelope; unsupported wet POI; submerged deck; deck below local descending surface; nonboolean shore access; pond visitor in water; population mismatch; missing gateway; isolated district; missing ferry access; water approach on wrong level; unqualified rising supply; raw sewer into potable supply; missing delivery supply; dangling water endpoint.

Positive control: removing one redundant tram dependency retains connectivity through the remaining graph. AEQD cardinal and longitude-seam cases pass.

## Remaining work

No claim is made for surveyed banks, flood levels, channel cross-sections, slopes between controls, catchments, water yields, treatment efficacy, operating lock water use, structural supports, accessibility, electricity coverage, food production, transit throughput or Unreal placement. Whole-lake curves and local bank refinements need terrain reconciliation. District ellipses overlap and are not cadastral boundaries or additive area measurements.

Visual inspection is separate: render this plate through inspect_world_plate.cjs with the talweir key; its report checks marker counts, clipped text and label-box intersections. A human/model image review is still needed for readability.
