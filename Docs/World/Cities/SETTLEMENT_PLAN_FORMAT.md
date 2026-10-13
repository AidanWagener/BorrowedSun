# Settlement plan format — authoring version 1

This common format supports the next city dossiers. Orison and Talweir retain their existing, more specialized records. Coordinates are spherical azimuthal equidistant meters about the unchanged atlas city center, positive x east and y north; z is planning sea-level altitude. A local proposal refines the coarse atlas rather than pretending its polygon is surveyed terrain.

Each `<city>_settlement.json` contains:

```json
{
  "schema_version": 1,
  "city_id": "existing CITY ID",
  "city_name": "existing name",
  "region_id": "existing REG ID",
  "status": "authored city proposal; no terrain, service capacity or runtime validated",
  "source_bible": "CITYNAME.md",
  "frame": {"origin_lon_deg": 0, "origin_lat_deg": 0, "radius_m": 5900000, "bounds_m": [-10000, -8000, 10000, 8000]},
  "population": 1,
  "gross_area_allowance_km2": 1,
  "base_surface": "land",
  "surface_polygons": [
    {"id": "unique", "name": "name", "kind": "water", "points_xy_m": [[0,0],[1,0],[1,1],[0,0]], "surface_m": 0, "status": "authored control"}
  ],
  "source_river_id": null,
  "channels": [
    {"id": "unique", "name": "name", "points_xyz_m": [[0,0,0],[1,-1,-1]], "surface_polygon_ids": ["water polygon ID"], "status": "authored nonrising nominal controls, not hydraulics"}
  ],
  "districts": [
    {"id": "PREFIX-D01", "name": "name", "center_xyz_m": [0,0,20], "radii_m": [1000,1000], "altitude_range_m": [10,100], "population": 1, "area_allowance_km2": 1, "character": "distinct role"}
  ],
  "places": [
    {"id": "PREFIX-POI-001", "name": "name", "district_id": "PREFIX-D01", "xyz_m": [0,0,20], "placement": "land", "water_surface_m": null, "shore_access_xyz_m": null}
  ],
  "transport_nodes": [
    {"id": "PREFIX-T01", "name": "name", "district_id": "PREFIX-D01", "xyz_m": [0,0,20], "regional_gateway": true}
  ],
  "transport_links": [
    {"id": "PREFIX-L01", "from": "node ID", "to": "node ID", "mode": "walking or tram or bus or ferry or rail or road or cable", "status": "service dependency; alignment and capacity unresolved"}
  ],
  "service_cases": {
    "domestic_liters_per_person_per_turn": 0,
    "other_potable_m3_per_turn": 0,
    "delivered_potable_m3_per_turn": 0,
    "network_loss_fraction": 0,
    "network_input_m3_per_turn": 0,
    "edible_product_kg_per_person_per_turn": 0,
    "edible_product_tonnes_per_turn": 0,
    "electric_average_mw": 0,
    "electric_peak_mw": 0,
    "scope": "demand assumptions; productive yield, reliability and nutrition unverified"
  },
  "unresolved": ["specific obligations"]
}
```

Use 8–10 districts and exactly 24 substantive places per new city. Population and area allocations must sum to their city totals. Every district has a transport node connected to a declared gateway; lines depict dependencies, not usable routes. Do not introduce random IDs. Stable city prefixes: MAR (Marroway), AVE (Aven), SOL (Sola).

A regional gateway outside the district envelopes may use `district_id: null` with `regional_gateway: true`. Its coordinate must still be finite, on authored land and inside the planning frame. Ordinary interchanges require a known district; an external gateway does not count as service within an unserved district.

`base_surface` is either `land` or `water`; local polygons of the opposite kind define channels or islands. Polygons are closed, non-self-intersecting and may not overlap one another. Use one polygon per connected shape; a confluence is one coherent water polygon, not overlapping rectangles. District envelopes may overlap; their area allowances are separately authored and are not ellipse areas. The drawn frame is north-up with equal x/y scale.

`placement` is `land` or `supported_overwater`. A supported place requires an explicit local water surface and a finite, dry shore-access point; its deck z must exceed that surface. Transport nodes are land interchanges, including ferry connections; actual berths, water legs, clearances and ramp gradients remain separately required. At least some POIs should be homes, pleasures and quiet discoveries rather than public-infrastructure problems.

For river cities, preserve the river altitude at the atlas center and the broad incoming/outgoing direction, but describe local channel shapes as authored refinement. A source river must appear in the atlas or additional geographic controls. Channel profiles are nonrising and pass through [0,0,source height]. Water polygon `surface_m` is only a reference level for a flowing reach; local positions must use the channel's interpolated height where needed. Sola's nominal sea reference is zero; tides and bathymetry remain open. Do not move parent atlas city coordinates to make a drawing easier.

The service case uses 24-hour civil turns, finite nonnegative quantities and a network loss fraction from zero inclusive to one exclusive. Network input means treated supply entering distribution; it excludes raw-water treatment losses and desalination reject flow. Describe those separate process streams in prose instead of mistaking seawater intake for potable production. Food is product mass, not proof of dietary adequacy. The dossier must explain sources, waste, losses, access, closure and recovery beyond these simple totals. Larger regional exports are additional to resident demand. No power or supply capacity is established by choosing a demand number.
