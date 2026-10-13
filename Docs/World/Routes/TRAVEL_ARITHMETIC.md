# Travel arithmetic — authoring study 0.1

These are geometric lower bounds or explicitly assumed local lengths. They are not timetables, navigable paths, vehicle performance or a promise of nonstop service. Speeds are illustrative groundspeed/still-water comparisons; wind, current, stops and actual courses remain unresolved.

## Existing city connections

| ID | Connection | Spherical separation km | At illustrative 650 km/h, hours |
|---|---|---:|---:|
| ROUTE-001 | Orison → Marroway | 1,342.47 | 2.065 |
| ROUTE-002 | Orison → Hesh | 4,409.64 | 6.784 |
| ROUTE-003 | Hesh → Serein | 5,382.85 | 8.281 |
| ROUTE-004 | Orison → Sola | 3,056.87 | 4.703 |
| ROUTE-005 | Sola → Aven | 5,821.34 | 8.956 |
| ROUTE-006 | Aven → Talweir | 4,349.16 | 6.691 |
| ROUTE-007 | Talweir → Dhal | 4,796.82 | 7.380 |
| ROUTE-008 | Dhal → Merrow | 3,588.87 | 5.521 |
| ROUTE-009 | Merrow → Tern | 7,838.33 | 12.059 |
| ROUTE-010 | Tern → Velis | 2,423.63 | 3.729 |
| ROUTE-011 | Velis → Serein | 9,191.91 | 14.141 |
| ROUTE-012 | Serein → Brin | 3,927.04 | 6.042 |
| ROUTE-013 | Orison → Talweir | 8,978.48 | 13.813 |

## Timing cases

Fixed handling/access is an authored assumption, not a surveyed minimum. Dividing by 24 gives civil turns; dividing by136.755 would answer a different question.

| Case | Distance basis | km | Mode km/h | Moving h | Fixed h | Total h | Civil turns |
|---|---|---:|---:|---:|---:|---:|---:|
| MOB-001 A two-kilometer local walk | declared_local_length | 2.00 | 4.5 | 0.444 | 0 | 0.444 | 0.019 |
| MOB-002 Orison to Marroway air comparison | city_pair_lower_bound | 1,342.47 | 650 | 2.065 | 1.5 | 3.565 | 0.149 |
| MOB-003 Marroway to Neris mouth river comparison | river_controls_lower_bound | 436.65 | 18 | 24.258 | 0 | 24.258 | 1.011 |
| MOB-004 Sola to Aven air comparison | city_pair_lower_bound | 5,821.34 | 650 | 8.956 | 2 | 10.956 | 0.456 |
| MOB-005 Orison to Talweir direct air comparison | city_pair_lower_bound | 8,978.48 | 650 | 13.813 | 1.5 | 15.313 | 0.638 |
| MOB-006 Tern to Velis sea comparison | city_pair_lower_bound | 2,423.63 | 42 | 57.705 | 4 | 61.705 | 2.571 |
| MOB-007 Merrow to Tern seam-crossing comparison | city_pair_lower_bound | 7,838.33 | 42 | 186.627 | 4 | 190.627 | 7.943 |
| MOB-008 Talweir local navigation timing study | declared_local_length | 20.00 | 18 | 1.111 | 0.9 | 2.011 | 0.084 |

- **MOB-001:** Illustrative route length; terrain, pauses and individual pace vary.
- **MOB-002:** 1.5h assumed handling/access; no route range, airfields or available service proven.
- **MOB-003:** Direct endpoint distance excludes channel bends, current and handling; source water drops18m, not a sea approach at city center.
- **MOB-004:** 2h assumed access/handling; Aven is inland, and range/landing/weather remain unproven.
- **MOB-005:** 1.5h handling assumption; does not establish nonstop capability, reserve or a timetable.
- **MOB-006:** 4h assumed handling; coast-following course, entrance depths, currents and weather still needed.
- **MOB-007:** Shortest spherical separation crosses longitude seam; no navigable alignment or refueling chain inferred.
- **MOB-008:** 20km rounded local course assumption plus two21-minute lock cycles and12minutes other handling; study values, not demonstrated lock operation.

## Hypothetical circular orbit

This is not Mirror Ring's adopted orbit. Two-body reference only, at 300 km altitude:

- Circular speed: **6.960779 km/s**.
- Period: **93.274401 minutes**.
- Ideal mechanical energy change from a stationary surface reference: **26.689905 MJ/kg**.
- For a constant comparison mass of **10,000 kg**: **74.138625 MWh** mechanical change.

Surface rotation, atmospheric drag, gravity losses during powered ascent, parent perturbations, vehicle mass changes, propellant, thermal rejection and rendezvous are not included. Electrical input or onboard fuel demand cannot be inferred directly from this ideal mechanical value.

`mu = g R²; r = R + h; v = sqrt(mu/r); period = 2πr/v; delta specific mechanical energy = mu(1/R − 1/r) + v²/2`.

## Check scope

Rejected controls: zero speed; nonfinite speed; negative handling; unknown city; reversed river controls; noninteger river index; natural day used as civil turn; missing route; nonfinite orbital height; finite speed yields infinite duration; finite orbital height overflows meters. Positive arithmetic controls cover the longitude seam, kilometer/hour conversion and an independently recomposed circular-period relation. Existing route IDs and physical/civil references agree. No route, ship, atmosphere or playable travel has been validated.
