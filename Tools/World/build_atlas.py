"""Validate and render the authored geographic concept; never generates game terrain."""
from __future__ import annotations

import argparse
import html
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ATLAS = ROOT / "Docs" / "World" / "Atlas"


def distance_km(a, b, radius):
    lat1, lat2 = map(math.radians, (a[1], b[1]))
    dlat = lat2 - lat1
    dlon = math.radians(b[0] - a[0])
    h = math.sin(dlat / 2) ** 2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2) ** 2
    return 2 * radius * math.asin(math.sqrt(max(0, min(1, h))))


def contains(point, polygon):
    x, y = point
    inside = False
    for a, b in zip(polygon, polygon[1:]):
        if (a[1] > y) != (b[1] > y):
            edge_x = (b[0] - a[0]) * (y - a[1]) / (b[1] - a[1]) + a[0]
            if x < edge_x:
                inside = not inside
    return inside


def sampled_coast_distance(point, polygons, radius):
    """Upper bound on nearest distance to authored lon/lat-linear edges; <=~1.5km sampling gap."""
    nearest = math.inf
    for polygon in polygons:
        for a, b in zip(polygon, polygon[1:]):
            steps = max(1, math.ceil(max(abs(b[0]-a[0]), abs(b[1]-a[1])) / 0.02))
            for i in range(steps + 1):
                t = i / steps
                q = (a[0] + t*(b[0]-a[0]), a[1] + t*(b[1]-a[1]))
                nearest = min(nearest, distance_km(point, q, radius))
    return nearest


def segments_cross(a, b, c, d):
    def orient(p, q, r):
        return (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0])
    return orient(a, b, c) * orient(a, b, d) < 0 and orient(c, d, a) * orient(c, d, b) < 0


def validate(data):
    errors = []
    for collection in ("regions", "cities", "rivers", "routes"):
        ids = [item["id"] for item in data[collection]]
        if len(ids) != len(set(ids)):
            errors.append(f"Duplicate IDs in {collection}")
    if data["radius_km"] <= 0:
        errors.append("Radius must be positive")
    regions = {r["id"] for r in data["regions"]}
    cities = {c["id"]: c for c in data["cities"]}
    for land in data["landmasses"]:
        pts = land["coordinates_lon_lat"]
        if pts[0] != pts[-1] or len(pts) < 4:
            errors.append(f"Invalid closed coastline: {land['name']}")
        for lon, lat in pts:
            if not (-180 <= lon <= 180 and -90 <= lat <= 90):
                errors.append(f"Out-of-range coastline coordinate: {land['name']}")
        edges = list(zip(pts, pts[1:]))
        for i, (a, b) in enumerate(edges):
            for j, (c, d) in enumerate(edges):
                if j > i + 1 and not (i == 0 and j == len(edges) - 1) and segments_cross(a, b, c, d):
                    errors.append(f"Self-intersecting coastline: {land['name']}")
    for i, a in enumerate(data["landmasses"]):
        for b in data["landmasses"][i + 1:]:
            pa, pb = a["coordinates_lon_lat"], b["coordinates_lon_lat"]
            crossing = any(segments_cross(x, y, u, v) for x, y in zip(pa, pa[1:]) for u, v in zip(pb, pb[1:]))
            if crossing or contains(pa[0], pb) or contains(pb[0], pa):
                errors.append(f"Overlapping coastline polygons: {a['name']} / {b['name']}")
    for city in cities.values():
        point = city["lon_deg"], city["lat_deg"]
        if city["region_id"] not in regions:
            errors.append(f"Unknown region for {city['id']}")
        if not (-180 <= point[0] <= 180 and -90 <= point[1] <= 90):
            errors.append(f"Out-of-range city: {city['id']}")
        if not any(land["name"] == city["landmass"] and contains(point, land["coordinates_lon_lat"]) for land in data["landmasses"]):
            errors.append(f"City outside assigned concept coastline: {city['name']}")
        if "coastal_approach_max_km" in city:
            polygons = [land["coordinates_lon_lat"] for land in data["landmasses"] if land["name"] == city["landmass"]]
            coast_distance = sampled_coast_distance(point, polygons, data["radius_km"])
            if coast_distance > city["coastal_approach_max_km"]:
                errors.append(f"Coastal city too far from concept shore: {city['name']} ({coast_distance:.1f}km)")
    for region in data["regions"]:
        point = region["anchor"]["lon_deg"], region["anchor"]["lat_deg"]
        if not (-180 <= point[0] <= 180 and -90 <= point[1] <= 90):
            errors.append(f"Out-of-range regional anchor: {region['id']}")
    for river in data["rivers"]:
        elevations = [p[2] for p in river["points_lon_lat_elevation_m"]]
        flat_segments = {(s["from_index"], s["to_index"]) for s in river.get("flat_lake_segments", [])}
        if not all(a > b or (a == b and (i, i+1) in flat_segments) for i, (a, b) in enumerate(zip(elevations, elevations[1:]))):
            errors.append(f"River rises or has an undeclared flat reach: {river['id']}")
    for route in data["routes"]:
        if route["from"] not in cities or route["to"] not in cities or route["from"] == route["to"]:
            errors.append(f"Invalid route endpoint: {route['id']}")
    # Independent known geometries protect units and dateline treatment.
    if abs(distance_km((0, 0), (90, 0), 1) - math.pi / 2) > 1e-12:
        errors.append("Quarter-circle distance invariant failed")
    if abs(distance_km((179, 0), (-179, 0), 1) - math.pi / 90) > 1e-12:
        errors.append("Dateline distance invariant failed")
    return errors


def render(data):
    def point(p):
        return 60 + (p[0] + 180) * 3, 70 + (90 - p[1]) * 3
    def path(points):
        return " ".join(("M" if i == 0 else "L") + f"{x:.1f},{y:.1f}" for i, (x, y) in enumerate(map(point, points)))
    svg = ['<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="840" viewBox="0 0 1200 840" role="img" aria-labelledby="title desc">',
           f'<title id="title">Nacre geographic concept, edition {data["edition"]}</title>',
           '<desc id="desc">Equirectangular concept map of four continents, two island arcs and twelve city anchors. Coastlines are schematic; no terrain or biome boundaries are surveyed.</desc>',
           '<style>text{font-family:Arial,sans-serif;fill:#e8e2d4;font-size:15px}.small{font-size:12px}.land{fill:#b8aa86;stroke:#d4c9a9;stroke-width:1}.river{fill:none;stroke:#315f73;stroke-width:2}.grid{stroke:#63838d;stroke-width:.7;opacity:.36}</style>',
           '<rect width="1200" height="840" fill="#122e38"/>',
           f'<text x="60" y="32" style="font-size:24px">NACRE · GEOGRAPHIC CONCEPT {data["edition"]}</text>',
           '<text x="60" y="54" class="small">742 AL · Radius 5,900 km · Equirectangular · Coastlines and regional placement remain under development</text>']
    for lon in range(-180, 181, 30):
        x, _ = point((lon, 0))
        svg += [f'<line class="grid" x1="{x}" y1="70" x2="{x}" y2="610"/>', f'<text x="{x}" y="633" text-anchor="middle" class="small">{lon}°</text>']
    for lat in range(-90, 91, 30):
        _, y = point((0, lat))
        svg += [f'<line class="grid" x1="60" y1="{y}" x2="1140" y2="{y}"/>', f'<text x="49" y="{y + 4}" text-anchor="end" class="small">{lat}°</text>']
    for land in data["landmasses"]:
        svg.append(f'<path class="land" d="{path(land["coordinates_lon_lat"])} Z"/>')
    for river in data["rivers"]:
        svg.append(f'<path class="river" d="{path([p[:2] for p in river["points_lon_lat_elevation_m"]])}"/>')
    for name, lon, lat in [("KESH", -26, 40), ("MORROW", 123, 48), ("PALE REACH", -123, 72), ("SERE", -110, -62), ("ASTER", 56, -20), ("NERAL", -166, 26)]:
        x, y = point((lon, lat))
        fill = "#e8e2d4" if name in ("ASTER", "NERAL") else "#102f37"
        svg.append(f'<text x="{x}" y="{y}" text-anchor="middle" style="fill:{fill};font-size:16px">{name}</text>')
    x, y = point((0, 0))
    svg += [f'<path d="M{x-5},{y}H{x+5} M{x},{y-5}V{y+5}" stroke="#102f37"/>', f'<text x="{x-9}" y="{y+19}" text-anchor="end" class="small" style="fill:#102f37">Reference sub-Rauth point</text>']
    for i, city in enumerate(data["cities"], 1):
        x, y = point((city["lon_deg"], city["lat_deg"]))
        svg += [f'<circle cx="{x}" cy="{y}" r="10" fill="#143541" stroke="#efe4c9"/>', f'<text x="{x}" y="{y+4}" text-anchor="middle" class="small">{i}</text>']
        col, row = (i - 1) // 4, (i - 1) % 4
        svg.append(f'<text x="{60+col*365}" y="{680+row*28}">{i:02d}  {html.escape(city["name"])} · {city["lat_deg"]}°, {city["lon_deg"]}°</text>')
    svg += ['<text x="60" y="807" class="small">Ocean separation, city containment and downhill trunk elevations checked. This map is not a climate model or a playable planet.</text>', '</svg>']
    (ATLAS / "nacre_geographic_concept.svg").write_text("\n".join(svg) + "\n", encoding="utf-8")
    cities = {c["id"]: c for c in data["cities"]}
    rows = ["# Atlas arithmetic — edition " + data["edition"], "", "Generated from `nacre_atlas.json` by `Tools/World/build_atlas.py`. Distances are spherical shortest-path lower bounds, not surveyed transport routes. Constant cruise times exclude ascent, routing, weather, stops, waiting and landing. No game time acceleration is specified.", "", "| Connection | Great-circle minimum | At 650 km/h | At 42 km/h |", "|---|---:|---:|---:|"]
    for r in data["routes"]:
        a, b = cities[r["from"]], cities[r["to"]]
        d = distance_km((a["lon_deg"], a["lat_deg"]), (b["lon_deg"], b["lat_deg"]), data["radius_km"])
        rows.append(f'| {a["name"]} — {b["name"]} | {d:,.0f} km | {d/650:.2f} h | {d/42:.1f} h |')
    radius = data["radius_km"]
    rows += ["", f"Reference circumference: **{2*math.pi*radius:,.1f} km**. Surface area: **{4*math.pi*radius**2:,.0f} km²**.", "", "The 42 km/h column is a mathematical speed comparison, including for inland connections; it does not imply a navigable sea route. Land fraction is not inferred from this distorted projection. Regional focus points are not biome or political borders.", ""]
    (ATLAS / "ATLAS_ARITHMETIC.md").write_text("\n".join(rows), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="Validate without writing the map or arithmetic")
    args = parser.parse_args()
    data = json.loads((ATLAS / "nacre_atlas.json").read_text(encoding="utf-8"))
    errors = validate(data)
    if errors:
        raise SystemExit("\n".join(errors))
    if not args.check:
        render(data)
    print(f"Atlas checked: {len(data['regions'])} regions, {len(data['cities'])} cities, {len(data['rivers'])} river trunks, {len(data['routes'])} route intents. Terrain and climate not validated.")


if __name__ == "__main__":
    main()
