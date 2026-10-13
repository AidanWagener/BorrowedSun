# Orison network and placement checks

Generated from the authored city plan. Connection distances are straight horizontal lower bounds, not surveyed route lengths. Endpoint grade is not a buildable alignment; vertical transfers and switchbacks require later design.

## Transport dependencies

| Link | Service | Endpoints | Lower bound km | Height change m |
|---|---|---|---:|---:|
| ORI-TR-001 | Shore Line | ORI-STOP-11 → ORI-STOP-01 | 3.108 | +15 |
| ORI-TR-002 | Shore Line | ORI-STOP-01 → ORI-STOP-05 | 4.115 | +120 |
| ORI-TR-003 | Shore Line | ORI-STOP-05 → ORI-STOP-06 | 2.687 | -130 |
| ORI-TR-004 | Ladder Line | ORI-STOP-01 → ORI-STOP-02 | 1.031 | +60 |
| ORI-TR-005 | Ladder Line | ORI-STOP-02 → ORI-STOP-03 | 1.883 | +130 |
| ORI-TR-006 | Ladder Line | ORI-STOP-03 → ORI-STOP-04 | 1.701 | +140 |
| ORI-TR-007 | Ladder Line | ORI-STOP-04 → ORI-STOP-07 | 1.422 | +255 |
| ORI-TR-008 | Ladder Line | ORI-STOP-07 → ORI-STOP-08 | 3.252 | -30 |
| ORI-TR-009 | Crown Line | ORI-STOP-08 → ORI-STOP-09 | 2.151 | +380 |
| ORI-TR-010 | Crown Line | ORI-STOP-09 → ORI-STOP-RIDGE | 1.414 | +190 |
| ORI-TR-011 | Ridge staff connection | ORI-STOP-RIDGE → ORI-STOP-10 | 0.851 | +120 |
| ORI-TR-012 | North freight branch | ORI-GATE-NORTH → ORI-STOP-06 | 2.977 | -55 |
| ORI-TR-013 | Port ferry | ORI-STOP-11 → ORI-STOP-01 | 3.108 | +15 |
| ORI-TR-014 | Port ferry | ORI-STOP-01 → ORI-STOP-06 | 6.197 | -10 |
| ORI-TR-015 | Sea approach | ORI-HARBOR-EAST → ORI-STOP-11 | 15.293 | +20 |
| ORI-TR-016 | Western road | ORI-GATE-INLAND → ORI-STOP-08 | 5.682 | -850 |
| ORI-TR-017 | Garden civic walk | ORI-STOP-07 → ORI-STOP-09 | 1.890 | +350 |
| ORI-TR-018 | Middle terrace connection | ORI-STOP-03 → ORI-STOP-08 | 2.470 | +365 |
| ORI-TR-019 | Workshop dock connection | ORI-STOP-02 → ORI-STOP-11 | 2.778 | -75 |
| ORI-TR-020 | Lower civic connection | ORI-STOP-03 → ORI-STOP-05 | 3.008 | -70 |

Ferry/vessel services connect administrative interchanges to the sea approach. Their positive interchange altitudes are not berth positions; access legs, sea-level berths and navigation corridors remain to be designed.

## Water dependencies

| Link | System | Endpoints | Rise m | Qualification |
|---|---|---|---:|---|
| ORI-WL-001 | potable | ORI-W01 → ORI-W02 | -300 | Regional input yield remains unverified |
| ORI-WL-002 | potable | ORI-W02 → ORI-W03 | -400 | Pressure reduction required |
| ORI-WL-003 | potable | ORI-W03 → ORI-W04 | -500 | Pressure reduction required |
| ORI-WL-004 | potable | ORI-W04 → ORI-W05 | -165 | Separate low pressure zone |
| ORI-WL-005 | potable_distribution | ORI-W05 → ORI-STOP-01 | -60 | Schematic service connection |
| ORI-WL-006 | potable_distribution | ORI-W04 → ORI-STOP-05 | -105 | Higher Three Drops distribution at 260 m feeds Common Table interchange at 155 m through pressure regulation; no unassisted rise from lower 95 m storage |
| ORI-WL-007 | potable_distribution | ORI-W03 → ORI-STOP-07 | -140 | Schematic service connection |
| ORI-WL-008 | potable_distribution | ORI-W02 → ORI-STOP-09 | -190 | Pressure regulation required |
| ORI-WL-009 | wastewater | ORI-STOP-01 → ORI-W07 | -15 | Concept catchment transfer, not pipe alignment |
| ORI-WL-010 | wastewater | ORI-STOP-05 → ORI-W07 | -135 | Concept catchment transfer, not pipe alignment |
| ORI-WL-011 | wastewater | ORI-STOP-07 → ORI-W07 | -600 | Concept catchment transfer, not pipe alignment |
| ORI-WL-012 | nonpotable_treatment | ORI-W07 → ORI-W09 | +2 | Treatment feed pumping provides the local 2 m lift; quality checks required before reuse |
| ORI-WL-013 | nonpotable | ORI-W09 → ORI-W06 | +13 | Pumping and independent distribution required |
| ORI-WL-014 | treated_discharge | ORI-W07 → ORI-W08 | -45 | Discharge conditions, route and ecology unresolved |

## Checks and limits

Validated: unique IDs, district population total, bounded finite coordinates, POI district envelopes/heights, explicit supported access for water placements, resolvable network endpoints, district transport connectivity, rising-water pumping qualifications, descending storm corridor and absence of ground coordinates for Mirror Ring.

Negative controls rejected: population mismatch; unsupported water placement; unqualified uphill water; isolated district; orbital ground coordinate; missing district transport node; unassigned POI; degenerate coast polygon; one-point storm corridor; nonfinite storm coordinate; misleading hydraulic prose; nonboolean shore access; potable supply from nonpotable source.

These checks do not establish viable building sites, structural supports, river/channel capacities, pressure losses, storage dispatch, coastal ecology, operational travel or runtime behavior. Water systems are incomplete dependency graphs, not full distribution/sewer networks.
