"""Check authored city relationships and draw an atlas plate, never runtime terrain."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
from pathlib import Path

from build_atlas import contains, segments_cross

ROOT = Path(__file__).resolve().parents[2]
CITY = ROOT / "Docs/World/Cities"


def finite_point(p):
    return isinstance(p, list) and len(p) == 3 and all(isinstance(v, (int, float)) and not isinstance(v, bool) and math.isfinite(v) for v in p)


def validate(d):
    errors = []
    groups = ("districts", "points_of_interest", "transport_nodes", "transport_links",
              "water_nodes", "water_links", "storm_corridors", "orbital_annexes")
    ids = [r["id"] for k in groups for r in d[k]]
    if len(ids) != len(set(ids)):
        errors.append("Duplicate authored ID")
    districts = {r["id"]: r for r in d["districts"]}
    transport = {r["id"]: r for r in d["transport_nodes"]}
    nodes = {**transport, **{r["id"]: r for r in d["water_nodes"]}}
    water = d["crater"]["bay_water_display_polygon_local_m"]
    if len(water) < 4 or not all(finite_point(p) for p in water) or water[0] != water[-1]:
        return errors + ["Water display polygon must have at least three finite vertices and close"]
    area2 = sum(a[0]*b[1]-b[0]*a[1] for a, b in zip(water, water[1:]))
    if len({tuple(p[:2]) for p in water}) < 3 or abs(area2) < 1:
        return errors + ["Water display polygon is degenerate"]
    edges = list(zip(water, water[1:]))
    if any(segments_cross(a, b, c, e) for i, (a, b) in enumerate(edges) for j, (c, e) in enumerate(edges) if j > i+1 and not (i == 0 and j == len(edges)-1)):
        return errors + ["Water display polygon self-intersects"]
    if sum(r["resident_estimate"] for r in districts.values()) != d["population"]["core_estimate"]:
        errors.append("District residents disagree with city population")
    bounds = d["coordinate_frame"]["planning_bounds_local_m"]
    for group in ("districts", "points_of_interest", "transport_nodes", "water_nodes"):
        for r in d[group]:
            p = r.get("position_local_m", r.get("center_local_m"))
            if not finite_point(p):
                errors.append(f"Invalid coordinate: {r['id']}")
                continue
            if not (bounds["min_x"] <= p[0] <= bounds["max_x"] and bounds["min_y"] <= p[1] <= bounds["max_y"]):
                errors.append(f"Outside city plan bounds: {r['id']}")
            if r.get("district_id") and r["district_id"] not in districts:
                errors.append(f"Unknown district: {r['id']}")
            if group == "points_of_interest":
                if r.get("district_id") not in districts:
                    errors.append(f"POI needs an assigned known district: {r['id']}")
                if contains(p[:2], water) and not (r.get("placement_type") == "supported_overwater_structure" and r.get("shore_access_required") is True):
                    errors.append(f"POI inside water without supported access declaration: {r['id']}")
                district = districts.get(r["district_id"])
                if district:
                    c, radii = district["center_local_m"], district["extent_radii_m"]
                    if sum(((p[i] - c[i]) / radii[i]) ** 2 for i in (0, 1)) > 1.000001:
                        errors.append(f"POI outside assigned illustrative district envelope: {r['id']}")
                    if not district["altitude_range_m"][0] <= p[2] <= district["altitude_range_m"][1]:
                        errors.append(f"POI outside district height range: {r['id']}")
    for group, allowed in (("transport_links", transport), ("water_links", nodes)):
        for r in d[group]:
            if r["from_id"] not in allowed or r["to_id"] not in allowed or r["from_id"] == r["to_id"]:
                errors.append(f"Invalid network endpoint: {r['id']}")
    # Every district needs a path to a regional gateway; this does not validate a road alignment.
    graph = {key: set() for key in transport}
    for r in d["transport_links"]:
        if r["from_id"] in graph and r["to_id"] in graph:
            graph[r["from_id"]].add(r["to_id"])
            graph[r["to_id"]].add(r["from_id"])
    gateways = d["regional_gateway_ids"]
    if not gateways or any(key not in transport for key in gateways):
        errors.append("Regional gateway IDs must resolve to transport nodes")
    seen, todo = set(), list(gateways)
    while todo:
        key = todo.pop()
        if key in seen or key not in graph:
            continue
        seen.add(key)
        todo.extend(graph[key] - seen)
    for r in transport.values():
        if r.get("district_id") and r["id"] not in seen:
            errors.append(f"District transport disconnected from regional network: {r['id']}")
    connected_districts = {r.get("district_id") for r in transport.values() if r["id"] in seen}
    for key in districts:
        if key not in connected_districts:
            errors.append(f"District has no connected transport node: {key}")
    for r in d["water_links"]:
        if r["from_id"] not in nodes or r["to_id"] not in nodes:
            continue
        if r["system"].startswith("potable"):
            if nodes[r["from_id"]].get("system") != "potable" or (r["to_id"] not in transport and nodes[r["to_id"]].get("system") != "potable"):
                errors.append(f"Potable dependency crosses an incompatible water system: {r['id']}")
        rise = nodes[r["to_id"]]["position_local_m"][2] - nodes[r["from_id"]]["position_local_m"][2]
        if rise > 0 and not (r.get("hydraulic_mode") == "pumped" and isinstance(r.get("static_lift_m"), (int, float)) and math.isfinite(r["static_lift_m"]) and r["static_lift_m"] >= rise):
            errors.append(f"Uphill water dependency lacks structured pumping/lift qualification: {r['id']}")
    for r in d["storm_corridors"]:
        points = r["points_local_m"]
        if len(points) < 2 or not all(finite_point(p) for p in points):
            errors.append(f"Storm corridor needs at least two finite points: {r['id']}")
            continue
        heights = [p[2] for p in points]
        if any(a <= b for a, b in zip(heights, heights[1:])):
            errors.append(f"Storm corridor does not descend: {r['id']}")
    for r in d["orbital_annexes"]:
        if "position_local_m" in r or "center_local_m" in r:
            errors.append(f"Moving orbital annex assigned a ground coordinate: {r['id']}")
    return errors


def negative_controls(d):
    mutations = []
    x = copy.deepcopy(d); x["districts"][0]["resident_estimate"] += 1
    mutations.append(("population mismatch", x, "residents disagree"))
    x = copy.deepcopy(d); x["points_of_interest"][0]["position_local_m"] = [5000, 0, 16]
    mutations.append(("unsupported water placement", x, "inside water"))
    x = copy.deepcopy(d); x["water_links"][0]["from_id"], x["water_links"][0]["to_id"] = "ORI-W05", "ORI-W01"
    mutations.append(("unqualified uphill water", x, "Uphill water"))
    x = copy.deepcopy(d); x["transport_links"] = [r for r in x["transport_links"] if "ORI-STOP-04" not in (r["from_id"], r["to_id"])]
    mutations.append(("isolated district", x, "transport disconnected"))
    x = copy.deepcopy(d); x["orbital_annexes"][0]["position_local_m"] = [0, 0, 100]
    mutations.append(("orbital ground coordinate", x, "orbital annex"))
    x = copy.deepcopy(d); x["transport_nodes"] = [r for r in x["transport_nodes"] if r["id"] != "ORI-STOP-04"]
    x["transport_links"] = [r for r in x["transport_links"] if "ORI-STOP-04" not in (r["from_id"], r["to_id"])]
    mutations.append(("missing district transport node", x, "no connected transport node"))
    x = copy.deepcopy(d); x["points_of_interest"][0]["district_id"] = None
    mutations.append(("unassigned POI", x, "assigned known district"))
    x = copy.deepcopy(d); x["crater"]["bay_water_display_polygon_local_m"] = [[0, 0, 0], [0, 0, 0]]
    mutations.append(("degenerate coast polygon", x, "polygon must have"))
    x = copy.deepcopy(d); x["storm_corridors"][0]["points_local_m"] = [[0, 0, 0]]
    mutations.append(("one-point storm corridor", x, "at least two finite"))
    x = copy.deepcopy(d); x["storm_corridors"][0]["points_local_m"][0][2] = math.nan
    mutations.append(("nonfinite storm coordinate", x, "at least two finite"))
    x = copy.deepcopy(d); x["water_links"][11]["hydraulic_mode"] = "gravity_candidate"
    x["water_links"][11]["note"] = "No pumping or maintained head is provided."
    mutations.append(("misleading hydraulic prose", x, "structured pumping"))
    x = copy.deepcopy(d); x["points_of_interest"][11]["shore_access_required"] = "false"
    mutations.append(("nonboolean shore access", x, "inside water"))
    x = copy.deepcopy(d); x["water_links"][4]["from_id"] = "ORI-W06"
    mutations.append(("potable supply from nonpotable source", x, "incompatible water system"))
    for label, data, message in mutations:
        if not any(message in error for error in validate(data)):
            raise ValueError(f"Negative control failed: {label}")
    x = copy.deepcopy(d); x["transport_links"] = [r for r in x["transport_links"] if r["id"] != "ORI-TR-012"]
    if validate(x):
        raise ValueError("Positive control failed: alternate western/sea gateways still connect the city")
    return [name for name, _, _ in mutations]


def render(d):
    out = ['<svg xmlns="http://www.w3.org/2000/svg" width="1440" height="1450" viewBox="0 0 1440 1450" role="img" aria-labelledby="title desc">',
           '<title id="title">Orison: districts, places and the breached crater</title>',
           '<desc id="desc">An authored city plan with eleven district envelopes, twenty-four numbered places, a whole-crater inset and a measured coordinate grid. This is a planning illustration, not a terrain survey.</desc>',
           '<style>text{font-family:Arial,sans-serif;fill:#253d42;font-size:16px}.small{font-size:13px}.tiny{font-size:11px}.muted{fill:#526f70}.water{fill:#c9dee0;stroke:#538e98;stroke-width:1.5}.rim{fill:none;stroke:#97715c;stroke-width:5}.grid{stroke:#c4c4b3;stroke-width:.6}.label{font-size:12px;font-weight:bold}</style>',
           '<rect width="1440" height="1450" fill="#f1eddf"/>']

    def text(x, y, value, css="", extra=""):
        out.append(f'<text x="{x:.1f}" y="{y:.1f}" class="{css}" {extra}>{html.escape(str(value))}</text>')

    def line(x1, y1, x2, y2, extra):
        out.append(f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" {extra}/>')

    text(48, 45, "ORISON / THE WESTERN TERRACES", extra='style="font-size:28px;letter-spacing:1px"')
    text(48, 73, "CITY-001 · 742 AL · Working plan 0.1 · 420,000 residents in the core", "small")
    text(48, 95, "North is up. Distances in kilometers; heights above planning sea level. District envelopes overlap.", "small muted")
    # Main plate deliberately enlarges the occupied western shore, not the whole crater.
    scale, bx, by = .058, 55, 125
    def point(p):
        return bx + (p[0] + 13500) * scale, by + (8500 - p[1]) * scale
    def path(points, fn=point):
        return " ".join(("M" if i == 0 else "L") + f"{x:.1f},{y:.1f}" for i, (x, y) in enumerate(map(fn, points)))

    w, h = 15500 * scale, 15000 * scale
    out += [f'<defs><clipPath id="city-clip"><rect x="{bx}" y="{by}" width="{w}" height="{h}"/></clipPath></defs>',
            '<g clip-path="url(#city-clip)">', f'<rect x="{bx}" y="{by}" width="{w}" height="{h}" fill="#e4ddc9"/>',
            f'<path class="water" d="{path(d["crater"]["bay_water_display_polygon_local_m"])} Z"/>',
            f'<path class="rim" d="{path(d["crater"]["surviving_rim_trace_local_m"])}"/>']
    for x in range(-12000, 2001, 2000):
        px, _ = point([x, 0]); line(px, by, px, by+h, 'class="grid"')
    for y in range(-6000, 8001, 2000):
        _, py = point([0, y]); line(bx, py, bx+w, py, 'class="grid"')
    palette = {"Low Tides": "#759b9a", "Ladder": "#bd975f", "Crown": "#9e858e"}
    for r in d["districts"]:
        x, y = point(r["center_local_m"]); rx, ry = [v*scale for v in r["extent_radii_m"]]
        color = palette.get(r["city_layer"], "#adad75")
        out.append(f'<ellipse data-district="{r["id"]}" cx="{x:.1f}" cy="{y:.1f}" rx="{rx:.1f}" ry="{ry:.1f}" fill="{color}" fill-opacity=".19" stroke="{color}" stroke-width="1.5"/>')
    for r in d["storm_corridors"]:
        out.append(f'<path d="{path(r["points_local_m"])}" fill="none" stroke="#5481a1" stroke-width="2" stroke-dasharray="4 4"/>')
    out.append('</g>')
    out.append(f'<rect x="{bx}" y="{by}" width="{w}" height="{h}" fill="none" stroke="#92978a"/>')
    for x in range(-12000, 2001, 2000):
        px, _ = point([x, 0]); text(px, by+h+23, f"{x/1000:g}", "tiny", 'text-anchor="middle"')
    for y in range(-6000, 8001, 2000):
        _, py = point([0, y]); text(bx-8, py+4, f"{y/1000:g}", "tiny", 'text-anchor="end"')
    text(bx+w, by+h+43, "x east / km", "tiny", 'text-anchor="end"')
    text(bx, by-9, "y north / km", "tiny")
    occupied = []
    # Place labels with leader lines; the geographic points themselves never move.
    def label(px, py, value, district=False):
        lw, lh = (34, 20) if district else (25, 20)
        chosen = None
        for radius in (18, 30, 44, 60, 78):
            for angle in (-45, 45, -135, 135, 0, 180, -90, 90):
                cx = px + radius*math.cos(math.radians(angle)); cy = py + radius*math.sin(math.radians(angle))
                box = (cx-lw/2, cy-lh/2, cx+lw/2, cy+lh/2)
                if box[0] < bx+4 or box[2] > bx+w-4 or box[1] < by+4 or box[3] > by+h-4:
                    continue
                if any(not (box[2]+3 < q[0] or box[0]-3 > q[2] or box[3]+3 < q[1] or box[1]-3 > q[3]) for q in occupied):
                    continue
                chosen = (cx, cy, box); break
            if chosen:
                break
        if not chosen:
            raise ValueError(f"No clear label position for {value}")
        cx, cy, box = chosen; occupied.append(box)
        line(px, py, cx, cy, 'stroke="#586767" stroke-width=".8"')
        fill, ink = ("#314e56", "#fff9e7") if district else ("#fff9e7", "#243d42")
        out.append(f'<rect class="map-label-box" x="{box[0]:.1f}" y="{box[1]:.1f}" width="{lw}" height="{lh}" rx="3" fill="{fill}" stroke="#7c8881" stroke-width=".7"/>')
        text(cx, cy+4, value, "label", f'text-anchor="middle" style="fill:{ink}"')
    for r in d["points_of_interest"]:
        x, y = point(r["position_local_m"])
        out.append(f'<circle data-poi="{r["id"]}" cx="{x:.1f}" cy="{y:.1f}" r="3.5" fill="#2d5059"/>')
        label(x, y, r["id"][-2:])
    for r in d["districts"]:
        c, radii = r["center_local_m"], r["extent_radii_m"]
        x, y = point([c[0]-radii[0]*.60, c[1]+radii[1]*.65])
        label(x, y, r["id"].split("-")[-1], True)
    x, y = point([500, 200]); text(x, y, "ORISON BAY", "muted", 'text-anchor="middle" transform="rotate(-90 '+f'{x:.1f} {y:.1f}'+')"')
    text(987, 146, "ELEVEN DISTRICTS", extra='style="font-size:18px;font-weight:bold"')
    for i, r in enumerate(d["districts"]):
        y = 180+i*46
        text(987, y, f'{r["id"].split("-")[-1]}  {r["name"]}', "small")
        text(1024, y+17, f'{r["resident_estimate"]:,} residents · {r["altitude_range_m"][0]}–{r["altitude_range_m"][1]} m', "tiny muted")
    text(987, 720, "THE WHOLE CRATER", extra='style="font-size:18px;font-weight:bold"')
    sx, sy, ss = 1185, 875, .012
    def inset(p): return sx+p[0]*ss, sy-p[1]*ss
    out += ['<rect x="1016" y="735" width="338" height="280" fill="#e4ddc9" stroke="#92978a"/>',
            f'<path class="water" d="{path(d["crater"]["bay_water_display_polygon_local_m"], inset)} Z"/>',
            f'<path d="{path(d["crater"]["surviving_rim_trace_local_m"], inset)}" fill="none" stroke="#97715c" stroke-width="3"/>']
    for r in d["districts"]:
        x, y = inset(r["center_local_m"])
        out.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="3" fill="#304e55"/>')
    text(1185, 1033, "~24 km east–west · east breach →", "tiny", 'text-anchor="middle"')
    text(987, 1053, "Dark labels: district envelopes", "small")
    text(987, 1074, "Cream labels: individual places", "small")
    text(987, 1095, "Dotted blue: proposed storm corridor", "small")
    text(55, 1075, "PLACES WORTH FINDING", extra='style="font-size:19px;font-weight:bold"')
    for i, r in enumerate(d["points_of_interest"]):
        col, row = i//8, i%8
        text(55+col*465, 1110+row*28, f'{r["id"][-2:]}  {r["name"]}', "small")
    text(55, 1362, "Three waterfront locations are supported overwater structures; shore access must be designed.", "small muted")
    text(55, 1385, "Mirror Ring is a moving orbital annex and has no ground coordinate. No actual streets, terrain, bathymetry or capacity simulation is claimed.", "small muted")
    text(55, 1408, "Source: orison_city_plan.json · Azimuthal equidistant from 17° N, 28° E on the adopted 5,900 km sphere.", "small muted")
    out.append('</svg>')
    (CITY / "orison_city_plan.svg").write_text("\n".join(out)+"\n", encoding="utf-8")


def write_network_register(d, controls):
    nodes = {r["id"]: r for k in ("transport_nodes", "water_nodes") for r in d[k]}
    rows = ["# Orison network and placement checks", "", "Generated from the authored city plan. Connection distances are straight horizontal lower bounds, not surveyed route lengths. Endpoint grade is not a buildable alignment; vertical transfers and switchbacks require later design.", "", "## Transport dependencies", "", "| Link | Service | Endpoints | Lower bound km | Height change m |", "|---|---|---|---:|---:|"]
    for r in d["transport_links"]:
        a, b = [nodes[r[k]]["position_local_m"] for k in ("from_id", "to_id")]
        distance = math.hypot(b[0]-a[0], b[1]-a[1])/1000
        rows.append(f'| {r["id"]} | {r["service"]} | {r["from_id"]} → {r["to_id"]} | {distance:.3f} | {b[2]-a[2]:+g} |')
    rows += ["", "Ferry/vessel services connect administrative interchanges to the sea approach. Their positive interchange altitudes are not berth positions; access legs, sea-level berths and navigation corridors remain to be designed.", "", "## Water dependencies", "", "| Link | System | Endpoints | Rise m | Qualification |", "|---|---|---|---:|---|"]
    for r in d["water_links"]:
        a, b = [nodes[r[k]]["position_local_m"] for k in ("from_id", "to_id")]
        rows.append(f'| {r["id"]} | {r["system"]} | {r["from_id"]} → {r["to_id"]} | {b[2]-a[2]:+g} | {r["note"]} |')
    rows += ["", "## Checks and limits", "", "Validated: unique IDs, district population total, bounded finite coordinates, POI district envelopes/heights, explicit supported access for water placements, resolvable network endpoints, district transport connectivity, rising-water pumping qualifications, descending storm corridor and absence of ground coordinates for Mirror Ring.", "", "Negative controls rejected: " + "; ".join(controls) + ".", "", "These checks do not establish viable building sites, structural supports, river/channel capacities, pressure losses, storage dispatch, coastal ecology, operational travel or runtime behavior. Water systems are incomplete dependency graphs, not full distribution/sewer networks.", ""]
    (CITY / "ORISON_NETWORK_CHECKS.md").write_text("\n".join(rows), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(); parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    data = json.loads((CITY / "orison_city_plan.json").read_text(encoding="utf-8"))
    errors = validate(data)
    if errors:
        raise SystemExit("\n".join(errors))
    controls = negative_controls(data)
    if not args.check:
        render(data); write_network_register(data, controls)
    print(json.dumps({"passed": True, "districts": len(data["districts"]), "places": len(data["points_of_interest"]), "negative_controls": controls, "scope": "Authored relationships only; no terrain or runtime validation"}))


if __name__ == "__main__":
    main()
