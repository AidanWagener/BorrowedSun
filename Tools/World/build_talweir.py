"""Validate and draw Talweir's authored controls, not terrain or working hydraulics."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
from pathlib import Path

from build_atlas import contains, distance_km, segments_cross

ROOT = Path(__file__).resolve().parents[2]
CITY = ROOT / "Docs/World/Cities"
ATLAS = ROOT / "Docs/World/Atlas"
POSITION_TOLERANCE_M = 0.02  # Source projections are rounded to millimeters.
WATER_MODES = {"local_ferry", "regional_boat_connection", "through_waterway"}


def number(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value)


def finite_point(point):
    return isinstance(point, list) and len(point) == 3 and all(number(v) for v in point)


def project(lon, lat, frame):
    """Spherical azimuthal equidistant; returns east/north meters, never a z value."""
    lon, lat = math.radians(lon), math.radians(lat)
    lon0, lat0 = math.radians(frame["origin_lon_deg"]), math.radians(frame["origin_lat_deg"])
    delta = (lon - lon0 + math.pi) % (2 * math.pi) - math.pi
    cosine = math.sin(lat0) * math.sin(lat) + math.cos(lat0) * math.cos(lat) * math.cos(delta)
    angle = math.acos(max(-1.0, min(1.0, cosine)))
    if angle < 1e-10:
        return (0.0, 0.0)
    if abs(math.pi - angle) < 1e-10:
        raise ValueError("AEQD is singular at the antipode")
    scale = frame["reference_radius_m"] * angle / math.sin(angle)
    return (scale * math.cos(lat) * math.sin(delta),
            scale * (math.cos(lat0) * math.sin(lat) - math.sin(lat0) * math.cos(lat) * math.cos(delta)))


def near(a, b, tolerance=POSITION_TOLERANCE_M):
    return len(a) == len(b) and all(abs(x - y) <= tolerance for x, y in zip(a, b))


def segment_distance(point, a, b):
    dx, dy = b[0] - a[0], b[1] - a[1]
    length2 = dx * dx + dy * dy
    t = max(0, min(1, ((point[0] - a[0]) * dx + (point[1] - a[1]) * dy) / length2)) if length2 else 0
    return math.hypot(point[0] - a[0] - t * dx, point[1] - a[1] - t * dy), t


def in_polygon(point, polygon):
    return contains(point[:2], polygon) or any(segment_distance(point, a, b)[0] <= .005 for a, b in zip(polygon, polygon[1:]))


def valid_polygon(points):
    if len(points) < 4 or not all(finite_point(p) for p in points) or points[0] != points[-1]:
        return False
    if len({tuple(p[:2]) for p in points}) < 3:
        return False
    if abs(sum(a[0] * b[1] - b[0] * a[1] for a, b in zip(points, points[1:]))) < .01:
        return False
    edges = list(zip(points, points[1:]))
    return not any(segments_cross(a, b, c, e)
                   for i, (a, b) in enumerate(edges) for j, (c, e) in enumerate(edges)
                   if j > i + 1 and not (i == 0 and j == len(edges) - 1))


def reachable(graph, starts):
    seen, pending = set(), list(starts)
    while pending:
        key = pending.pop()
        if key in seen or key not in graph:
            continue
        seen.add(key)
        pending.extend(graph[key] - seen)
    return seen


def validate(d, atlas, geography):
    """Validate explicit relationships only. Node altitudes are not hydraulic grades."""
    errors = []
    groups = ("districts", "points_of_interest", "transport_nodes", "transport_links", "water_nodes",
              "water_links", "channel_centerlines", "local_water_polygons", "locks")
    ids = [r["id"] for group in groups for r in d[group]]
    if len(ids) != len(set(ids)):
        errors.append("Duplicate local authored ID")
    if len(d["districts"]) != 9 or len(d["points_of_interest"]) != 24:
        errors.append("Expected nine district envelopes and twenty-four POIs")
    frame, controls = d["coordinate_frame"], d["source_controls"]
    parent = next((c for c in atlas["cities"] if c["id"] == d["city_id"]), None)
    if (parent is None or parent["region_id"] != d["region_id"]
            or not near([parent["lon_deg"], parent["lat_deg"]], [frame["origin_lon_deg"], frame["origin_lat_deg"]], 1e-9)
            or frame["reference_radius_m"] != atlas["radius_km"] * 1000):
        errors.append("City frame disagrees with parent atlas identity/origin/radius")
    if frame["x_axis"] != "east" or frame["y_axis"] != "north" or frame["horizontal_units"] != "meters":
        errors.append("City frame must use east/north meters")
    source_lakes = {r["id"]: r for r in geography["lakes"]}
    lakes = {r["id"]: r for r in d["whole_lake_outlines"]}
    if len(lakes) != len(d["whole_lake_outlines"]) or set(lakes) != set(controls["lake_ids"]):
        errors.append("Whole-lake IDs disagree with declared source lakes")
    for lake in lakes.values():
        parent_lake = source_lakes.get(lake["id"])
        if parent_lake is None:
            errors.append(f"Unknown parent lake: {lake['id']}")
            continue
        if (lake["source_coordinates_lon_lat"] != parent_lake["coordinates_lon_lat"]
                or lake["surface_elevation_m"] != parent_lake["surface_elevation_m"]):
            errors.append(f"Lake source geometry/surface disagrees with geography: {lake['id']}")
        expected = [[*project(*p, frame), parent_lake["surface_elevation_m"]] for p in parent_lake["coordinates_lon_lat"]]
        if len(expected) != len(lake["coordinates_local_m"]) or any(not finite_point(b) or not near(a, b) for a, b in zip(expected, lake["coordinates_local_m"])):
            errors.append(f"Lake local coordinates disagree with AEQD projection: {lake['id']}")
    river = next((r for r in atlas["rivers"] if r["id"] == controls["river_id"]), None)
    fixed = controls["fixed_controls_lon_lat_altitude_m"]
    if river is None or not any(river["points_lon_lat_elevation_m"][i:i+len(fixed)] == fixed for i in range(len(river["points_lon_lat_elevation_m"]))):
        errors.append("Fixed lake/junction controls disagree with parent river")
    connection = next((r for r in geography["connections"] if r["id"] == controls["connection_id"]), None)
    waterway = d["waterway_controls"]
    if (connection is None or connection["from"] != "LAK-001" or connection["to"] != "LAK-002"
            or connection["via_city"] != d["city_id"] or connection["direction"] != "downstream"
            or connection["drop_m"] != waterway["total_nominal_head_m"]):
        errors.append("Parent lake connection disagrees with local downstream head")
    if len(fixed) != 3 or not all(finite_point(p) for p in fixed):
        return errors + ["Exactly three finite fixed river controls required"]
    projected = [[*project(p[0], p[1], frame), p[2]] for p in fixed]
    for name, expected in zip(("namar_outlet_local_m", "junction_local_m", "iren_inlet_local_m"), projected):
        if not finite_point(waterway[name]) or not near(expected, waterway[name]):
            errors.append(f"Fixed local river control disagrees with AEQD: {name}")
    if not near([waterway["nominal_upstream_surface_m"], waterway["nominal_downstream_surface_m"], waterway["total_nominal_head_m"]], [180, 176, 4], 1e-9):
        errors.append("Namar/Iren surfaces must retain the adopted 180/176 m and 4 m head")
    # The source distance is spherical, while local intermediate lines are planning geometry.
    direct = distance_km(fixed[0], fixed[2], atlas["radius_km"]) * 1000
    dogleg = sum(distance_km(a, b, atlas["radius_km"]) * 1000 for a, b in zip(fixed, fixed[1:]))
    if not near([waterway["direct_outlet_inlet_distance_m"], waterway["nominal_control_dogleg_distance_m"]], [direct, dogleg], .1):
        errors.append("Authored lake distance arithmetic disagrees with source controls")

    channels = {r["id"]: r for r in d["channel_centerlines"]}
    water = {r["id"]: r for r in d["whole_lake_outlines"] + d["local_water_polygons"]}
    for body in water.values():
        if not valid_polygon(body["coordinates_local_m"]):
            errors.append(f"Invalid water polygon: {body['id']}")
    if any(e.startswith("Invalid water polygon") for e in errors):
        return errors  # Do not feed malformed polygons to placement checks.
    expected_ends = {"LAK-001": projected[0], "TAL-JUNCTION": projected[1], "LAK-002": projected[2]}
    for channel in channels.values():
        points, widths = channel["points_local_m"], channel["width_targets_m"]
        if len(points) < 2 or not all(finite_point(p) for p in points):
            errors.append(f"Channel needs at least two finite points: {channel['id']}")
            continue
        if len(widths) != len(points) or not all(number(w) and w > 0 for w in widths):
            errors.append(f"Invalid channel widths: {channel['id']}")
        if any(a[2] < b[2] - 1e-6 for a, b in zip(points, points[1:])):
            errors.append(f"Channel surface rises downstream: {channel['id']}")
        for field, endpoint in (("connection_from", points[0]), ("connection_to", points[-1])):
            key = channel[field]
            if key in expected_ends:
                if not near(endpoint, expected_ends[key]):
                    errors.append(f"Disconnected fixed channel endpoint: {channel['id']} / {key}")
            elif key in channels:
                host = channels[key]
                matches = []
                for i, (a, b) in enumerate(zip(host["points_local_m"], host["points_local_m"][1:])):
                    distance, t = segment_distance(endpoint, a, b)
                    width = host["width_targets_m"][i] * (1-t) + host["width_targets_m"][i+1] * t
                    surface = a[2] * (1-t) + b[2] * t
                    matches.append(distance <= width/2 + .02 and abs(endpoint[2] - surface) <= .1)
                if not any(matches):
                    errors.append(f"Branch misses receiving channel envelope/surface: {channel['id']}")
            else:
                errors.append(f"Unresolved channel connection: {channel['id']} / {key}")
    for lock in d["locks"]:
        upper, lower = lock["upper_endpoint_local_m"], lock["lower_endpoint_local_m"]
        if not finite_point(upper) or not finite_point(lower) or not number(lock["nominal_lift_m"]):
            errors.append(f"Invalid lock controls: {lock['id']}")
            continue
        if lock["nominal_lift_m"] <= 0 or abs(upper[2] - lower[2] - lock["nominal_lift_m"]) > 1e-6:
            errors.append(f"Lock lift disagrees with endpoint surfaces: {lock['id']}")
        containing = [c["id"] for c in channels.values() if any(near(a, upper) and near(b, lower) for a, b in zip(c["points_local_m"], c["points_local_m"][1:]))]
        if len(containing) != 1 or lock["bypass_id"] not in channels or lock["bypass_id"] in containing:
            errors.append(f"Lock lacks one matching passage and distinct bypass: {lock['id']}")
    for body in d["local_water_polygons"]:
        relation = body.get("derived_from", body.get("connected_to"))
        if relation is not None and relation not in channels and relation not in lakes:
            errors.append(f"Unresolved local water relationship: {body['id']}")
        if "derived_from" not in body and any(abs(p[2] - body["coordinates_local_m"][0][2]) > 1e-6 for p in body["coordinates_local_m"]):
            errors.append(f"Standing local water polygon has inconsistent surface: {body['id']}")

    districts = {r["id"]: r for r in d["districts"]}
    bounds = frame["planning_bounds_local_m"]
    def within(point):
        return bounds["min_x"] <= point[0] <= bounds["max_x"] and bounds["min_y"] <= point[1] <= bounds["max_y"]
    def wet(point):
        return [body for body in water.values() if in_polygon(point, body["coordinates_local_m"])]
    def nominal_surface_at(body, point):
        # An envelope can descend. Its lowest vertex is not the water level at a deck.
        source = channels.get(body.get("derived_from"))
        if source:
            candidates = []
            for a, b in zip(source["points_local_m"], source["points_local_m"][1:]):
                distance, t = segment_distance(point, a, b)
                candidates.append((distance, a[2] * (1-t) + b[2] * t))
            return min(candidates)[1]
        return body["coordinates_local_m"][0][2]
    for group in ("districts", "points_of_interest", "transport_nodes", "water_nodes"):
        for row in d[group]:
            point = row.get("local_m", row.get("center_local_m"))
            if not finite_point(point) or not within(point):
                errors.append(f"Invalid or out-of-bounds local coordinate: {row['id']}")
    if sum(r["resident_estimate"] for r in districts.values()) != d["population"]["core_resident_estimate"]:
        errors.append("District residents disagree with city population")
    if not math.isclose(sum(r["gross_area_allowance_km2"] for r in districts.values()), d["population"]["gross_area_allowance_km2"]):
        errors.append("District gross allocations disagree with city allocation")
    for poi in d["points_of_interest"]:
        district, point = districts.get(poi.get("district_id")), poi["local_m"]
        if district is None:
            errors.append(f"POI needs an assigned known district: {poi['id']}")
            continue
        if not finite_point(point):
            continue
        radii = district["extent_radii_m"]
        if len(radii) != 2 or not all(number(r) and r > 0 for r in radii):
            errors.append(f"Invalid district radii: {district['id']}")
            continue
        if sum(((point[i] - district["center_local_m"][i]) / radii[i]) ** 2 for i in (0, 1)) > 1.000001:
            errors.append(f"POI outside district envelope: {poi['id']}")
        if not district["altitude_range_m"][0] <= point[2] <= district["altitude_range_m"][1]:
            errors.append(f"POI outside district altitude range: {poi['id']}")
        placement = poi["placement_type"]
        if placement == "terrestrial_structure_or_ground":
            if wet(point):
                errors.append(f"Terrestrial POI placed inside authored water: {poi['id']}")
        elif placement in {"supported_overwater_structure", "shore_access_landmark"}:
            body = water.get(poi.get("water_body_id"))
            if poi.get("shore_access_required") is not True or body is None or not in_polygon(point, body["coordinates_local_m"]):
                errors.append(f"Water POI lacks matching body and explicit shore access: {poi['id']}")
            elif point[2] <= nominal_surface_at(body, point):
                errors.append(f"POI standing/focus altitude is not above named water: {poi['id']}")
            if placement == "shore_access_landmark":
                access = poi.get("visitor_access_local_m")
                if not finite_point(access) or not within(access) or wet(access):
                    errors.append(f"Shore landmark needs a finite dry visitor access point: {poi['id']}")
        else:
            errors.append(f"Unknown POI placement type: {poi['id']}")

    transport = {r["id"]: r for r in d["transport_nodes"]}
    graph = {key: set() for key in transport}
    for link in d["transport_links"]:
        a, b = link["from_id"], link["to_id"]
        if a not in graph or b not in graph or a == b:
            errors.append(f"Invalid transport endpoint: {link['id']}")
            continue
        graph[a].add(b); graph[b].add(a)
        if link["mode"] in WATER_MODES:
            if link.get("access_leg_required") is not True:
                errors.append(f"Water service lacks explicit access-leg requirement: {link['id']}")
            if any(c not in channels for c in link.get("waterway_alignment_ids", [])):
                errors.append(f"Unresolved waterway alignment reference: {link['id']}")
            if any(transport[key]["endpoint_type"] == "land_based_interchange" and transport[key].get("access_leg_required") is not True for key in (a, b)):
                errors.append(f"Land interchange lacks water access declaration: {link['id']}")
    gateways = d.get("regional_gateway_ids", [])
    if not gateways or any(key not in transport for key in gateways):
        errors.append("Regional gateway IDs must resolve to transport nodes")
    reached = reachable(graph, gateways)
    if set(transport) - reached:
        errors.append("Transport node disconnected from all regional gateways")
    for key in districts:
        if not any(n.get("district_id") == key and n["id"] in reached for n in transport.values()):
            errors.append(f"District has no connected transport node: {key}")
    for node in transport.values():
        if node.get("district_id") is not None and node["district_id"] not in districts:
            errors.append(f"Transport node has unknown district: {node['id']}")
        if node["endpoint_type"] == "open_water_route_control" and not any(near(node["local_m"], p) for p in (projected[0], projected[2])):
            errors.append(f"Open-water approach disagrees with fixed surface/control: {node['id']}")

    nodes = {r["id"]: r for r in d["water_nodes"]}
    supply = {key: set() for key in nodes}
    treated = {"treatment_works", "treated_water_surface_control"}
    deliveries = {"regulated_delivery_zone"}
    compatible = {
        "potable_raw": ({"raw_water_intake"}, {"treatment_works"}),
        "potable_raw_reserve": ({"raw_water_intake"}, {"treatment_works"}),
        "potable": (treated, treated | deliveries),
        "potable_reserve": (treated, deliveries),
        "sewer": ({"sewer_collection_pump"}, {"sewer_treatment_works"}),
        "treated_residual": ({"sewer_treatment_works"}, {"treated_discharge_outfall"}),
        "reclaimed": ({"sewer_treatment_works"}, {"reclaimed_water_surface_control"}),
        "reclaimed_industrial": ({"reclaimed_water_surface_control"}, deliveries),
    }
    for link in d["water_links"]:
        a, b = nodes.get(link["from_id"]), nodes.get(link["to_id"])
        if a is None or b is None or a["id"] == b["id"]:
            errors.append(f"Invalid water dependency endpoint: {link['id']}")
            continue
        contract = compatible.get(link["system"])
        if contract is None or a["type"] not in contract[0] or b["type"] not in contract[1]:
            errors.append(f"Incompatible water system/type relationship: {link['id']}")
        elif link["system"].startswith("potable"):
            supply[a["id"]].add(b["id"])
        mode = link["hydraulic_mode"]
        if not (mode.startswith("pumped") or mode in {"gravity_candidate", "gravity_candidate_regulated"}):
            errors.append(f"Unknown hydraulic mode: {link['id']}")
        if b["local_m"][2] > a["local_m"][2] and not mode.startswith("pumped"):
            errors.append(f"Rising water dependency lacks declared pumping: {link['id']}")
    served = reachable(supply, [key for key, node in nodes.items() if node["type"] == "raw_water_intake"])
    for key, node in nodes.items():
        if node["type"] in deliveries and key not in served:
            errors.append(f"Delivery zone lacks typed intake/treatment supply path: {key}")
    return errors


def negative_controls(d, atlas, geography):
    cases = []
    def reject(label, mutate, expected):
        changed = copy.deepcopy(d)
        mutate(changed)
        errors = validate(changed, atlas, geography)
        if not any(expected in error for error in errors):
            raise ValueError(f"Negative control failed: {label}: {errors}")
        cases.append(label)
    reject("wrong reference radius", lambda x: x["coordinate_frame"].update(reference_radius_m=6371000), "parent atlas")
    reject("projection translation", lambda x: x["whole_lake_outlines"][0]["coordinates_local_m"][0].__setitem__(0, -70000), "AEQD projection")
    reject("changed source lake surface", lambda x: x["whole_lake_outlines"][0].update(surface_elevation_m=181), "source geometry/surface")
    reject("junction shifted off river control", lambda x: x["waterway_controls"]["junction_local_m"].__setitem__(0, 100), "Fixed local river")
    reject("broken downstream connection", lambda x: x["channel_centerlines"][1]["points_local_m"][0].__setitem__(0, 150), "Disconnected fixed")
    reject("rising bypass surface", lambda x: x["channel_centerlines"][1]["points_local_m"][2].__setitem__(2, 181), "rises downstream")
    reject("incorrect lock lift", lambda x: x["locks"][0].update(nominal_lift_m=4), "Lock lift")
    reject("invalid water polygon", lambda x: x["local_water_polygons"][0].update(coordinates_local_m=[[0, 0, 0]]), "Invalid water polygon")
    reject("unassigned POI", lambda x: x["points_of_interest"][0].update(district_id=None), "assigned known district")
    reject("POI outside envelope", lambda x: x["points_of_interest"][0]["local_m"].__setitem__(0, -9000), "outside district envelope")
    reject("unsupported wet POI", lambda x: x["points_of_interest"][0].update(local_m=[0, 0, 195]), "inside authored water")
    reject("submerged deck", lambda x: x["points_of_interest"][7]["local_m"].__setitem__(2, 170), "not above named water")
    reject("deck below local descending surface", lambda x: x["points_of_interest"][7]["local_m"].__setitem__(2, 179.8), "not above named water")
    reject("nonboolean shore access", lambda x: x["points_of_interest"][7].update(shore_access_required="false"), "explicit shore access")
    reject("pond visitor in water", lambda x: x["points_of_interest"][15].update(visitor_access_local_m=[-6800, -1400, 230]), "dry visitor")
    reject("population mismatch", lambda x: x["districts"][0].update(resident_estimate=95001), "residents disagree")
    reject("missing gateway", lambda x: x.update(regional_gateway_ids=["MISSING"]), "gateway IDs")
    reject("isolated district", lambda x: x.update(transport_links=[r for r in x["transport_links"] if "TAL-STOP-03" not in (r["from_id"], r["to_id"])]), "disconnected")
    reject("missing ferry access", lambda x: x["transport_links"][17].update(access_leg_required=False), "access-leg")
    reject("water approach on wrong level", lambda x: x["transport_nodes"][9]["local_m"].__setitem__(2, 185), "Open-water approach")
    reject("unqualified rising supply", lambda x: x["water_links"][0].update(hydraulic_mode="gravity_candidate"), "declared pumping")
    reject("raw sewer into potable supply", lambda x: x["water_links"][3].update(from_id="TAL-W12"), "Incompatible water")
    reject("missing delivery supply", lambda x: x.update(water_links=[r for r in x["water_links"] if r["to_id"] != "TAL-W21"]), "lacks typed")
    reject("dangling water endpoint", lambda x: x["water_links"][0].update(to_id="MISSING"), "Invalid water dependency")
    alternate = copy.deepcopy(d)
    alternate["transport_links"] = [r for r in alternate["transport_links"] if r["id"] != "TAL-TR-002"]
    if validate(alternate, atlas, geography):
        raise ValueError("Positive control failed: remaining alternate transport paths should connect")
    # Independent projection geometries catch wrong orientation, units and longitude wrapping.
    unit = {"origin_lon_deg": 0, "origin_lat_deg": 0, "reference_radius_m": 1}
    if not near(project(90, 0, unit), [math.pi/2, 0], 1e-10) or not near(project(0, 90, unit), [0, math.pi/2], 1e-10):
        raise ValueError("AEQD cardinal geometry failed")
    unit["origin_lon_deg"] = 179
    if not near(project(-179, 0, unit), [math.pi/90, 0], 1e-10):
        raise ValueError("AEQD dateline geometry failed")
    return cases


def render(d):
    """North-up local plate. Label displacement never moves the authored markers."""
    width, height = 1560, 1560
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" role="img" aria-labelledby="title desc">',
           '<title id="title">Talweir: the city between Namar and Iren</title>',
           '<desc id="desc">Nine overlapping district envelopes and twenty-four authored places in a north-up azimuthal equidistant city plan. Whole-lake inset preserves atlas projection. Water heights and channel shapes are planning controls, not terrain or a hydraulic model.</desc>',
           '<style>text{font-family:Arial,sans-serif;fill:#253e43;font-size:16px}.small{font-size:13px}.tiny{font-size:11px}.muted{fill:#566e70}.water{fill:#bdd7db;stroke:#397d8b;stroke-width:1.2}.label{font-size:12px;font-weight:bold}.grid{stroke:#b9bbae;stroke-width:.6}</style>',
           f'<rect width="{width}" height="{height}" fill="#f4f0e5"/>']
    def text(x, y, value, css="", attrs=""):
        out.append(f'<text x="{x:.2f}" y="{y:.2f}" class="{css}" {attrs}>{html.escape(str(value))}</text>')
    def line(x1, y1, x2, y2, attrs):
        out.append(f'<line x1="{x1:.2f}" y1="{y1:.2f}" x2="{x2:.2f}" y2="{y2:.2f}" {attrs}/>')
    def rect(x, y, w, h, attrs=""):
        out.append(f'<rect x="{x:.2f}" y="{y:.2f}" width="{w:.2f}" height="{h:.2f}" {attrs}/>')
    text(60, 46, "TALWEIR / THE CITY BETWEEN TWO LAKES", attrs='style="font-size:28px;letter-spacing:1px"')
    text(60, 74, f"CITY-005 · 742 AL · Working plan {d['edition']} · {d['population']['core_resident_estimate']:,} residents, authored estimate", "small")
    text(60, 98, "North is up · coordinates in kilometers · z is planning sea-level altitude · envelopes overlap; no surveyed terrain or service capacity", "small muted")
    b = d["coordinate_frame"]["planning_bounds_local_m"]
    bx, by, scale = 60, 140, 1035 / (b["max_x"] - b["min_x"])
    w, h = (b["max_x"] - b["min_x"]) * scale, (b["max_y"] - b["min_y"]) * scale
    def point(p):
        return bx + (p[0] - b["min_x"]) * scale, by + (b["max_y"] - p[1]) * scale
    def path(points, transform=point):
        return " ".join(("M" if i == 0 else "L") + f"{x:.2f},{y:.2f}" for i, (x, y) in enumerate(map(transform, points)))
    out.append(f'<defs><clipPath id="city-clip"><rect x="{bx}" y="{by}" width="{w}" height="{h}"/></clipPath></defs>')
    out.append('<g clip-path="url(#city-clip)">')
    rect(bx, by, w, h, 'fill="#e5dfce"')
    for x in range(-10000, 13001, 2000):
        px, _ = point([x, 0]); line(px, by, px, by+h, 'class="grid"')
    for y in range(-6000, 12001, 2000):
        _, py = point([0, y]); line(bx, py, bx+w, py, 'class="grid"')
    colors = ["#b89261", "#a97773", "#898ea0", "#b89957", "#719a79", "#749ca4", "#ab8465", "#74a397", "#8d9eac"]
    for district, color in zip(d["districts"], colors):
        x, y = point(district["center_local_m"])
        rx, ry = [r * scale for r in district["extent_radii_m"]]
        out.append(f'<ellipse data-district="{district["id"]}" cx="{x:.2f}" cy="{y:.2f}" rx="{rx:.2f}" ry="{ry:.2f}" fill="{color}" fill-opacity=".22" stroke="{color}" stroke-width="1.6"/>')
    for lake in d["whole_lake_outlines"] + d["local_water_polygons"]:
        out.append(f'<path class="water" d="{path(lake["coordinates_local_m"])} Z"/>')
    nodes = {r["id"]: r for r in d["transport_nodes"]}
    for link in d["transport_links"]:
        if link["mode"] != "surface_tram":
            continue
        a, c = [point(nodes[link[k]]["local_m"]) for k in ("from_id", "to_id")]
        line(*a, *c, 'stroke="#758783" stroke-width="1.2" stroke-dasharray="3 6" opacity=".65"')
    for lock in d["locks"]:
        a, c = point(lock["upper_endpoint_local_m"]), point(lock["lower_endpoint_local_m"])
        line(*a, *c, 'stroke="#965e3b" stroke-width="5"')
    out.append('</g>')
    rect(bx, by, w, h, 'fill="none" stroke="#89958b"')
    for x in range(-10000, 13001, 2000):
        px, _ = point([x, 0]); text(px, by+h+23, f"{x/1000:g}", "tiny", 'text-anchor="middle"')
    for y in range(-6000, 12001, 2000):
        _, py = point([0, y]); text(bx-8, py+4, f"{y/1000:g}", "tiny", 'text-anchor="end"')
    text(bx, by-12, "y north / km", "tiny")
    text(bx+w, by+h+42, "x east / km", "tiny", 'text-anchor="end"')
    line(92, 185, 92, 155, 'stroke="#324f56" stroke-width="2"')
    out.append('<path d="M86,164 L92,153 L98,164" fill="none" stroke="#324f56" stroke-width="2"/>')
    text(107, 165, "N", "label")
    occupied = []
    all_markers = [point(p["local_m"]) for p in d["points_of_interest"]]
    def label(px, py, value, district=False):
        lw, lh = (35, 21) if district else (26, 21)
        chosen = None
        for radius in (20, 32, 46, 62, 82, 105):
            for angle in (-45, 45, -135, 135, 0, 180, -90, 90):
                cx, cy = px + radius * math.cos(math.radians(angle)), py + radius * math.sin(math.radians(angle))
                box = (cx-lw/2, cy-lh/2, cx+lw/2, cy+lh/2)
                if box[0] < bx+5 or box[2] > bx+w-5 or box[1] < by+5 or box[3] > by+h-5:
                    continue
                if any(not (box[2]+4 < q[0] or box[0]-4 > q[2] or box[3]+4 < q[1] or box[1]-4 > q[3]) for q in occupied):
                    continue
                if any(box[0]-4 <= mx <= box[2]+4 and box[1]-4 <= my <= box[3]+4 for mx, my in all_markers):
                    continue
                chosen = cx, cy, box
                break
            if chosen:
                break
        if not chosen:
            raise ValueError(f"No clear label position for {value}")
        cx, cy, box = chosen
        occupied.append(box)
        line(px, py, cx, cy, 'stroke="#657675" stroke-width=".8"')
        fill, ink = ("#334f57", "#fff9e8") if district else ("#fff9e8", "#253e43")
        rect(box[0], box[1], lw, lh, f'class="map-label-box" rx="3" fill="{fill}" stroke="#7d8b85" stroke-width=".7"')
        text(cx, cy+4, value, "label", f'text-anchor="middle" style="fill:{ink}"')
    for poi in d["points_of_interest"]:
        x, y = point(poi["local_m"])
        out.append(f'<circle data-poi="{poi["id"]}" cx="{x:.2f}" cy="{y:.2f}" r="3.5" fill="#2c505a"/>')
        label(x, y, poi["id"][-2:])
    for district in d["districts"]:
        center, radii = district["center_local_m"], district["extent_radii_m"]
        x, y = point([center[0]-radii[0]*.52, center[1]+radii[1]*.62])
        label(x, y, district["id"].split("-")[-1], True)
    text(1140, 155, "NINE DISTRICTS", attrs='style="font-size:18px;font-weight:bold"')
    for i, district in enumerate(d["districts"]):
        y = 189 + i * 44
        text(1140, y, f'{district["id"].split("-")[-1]}  {district["name"]}', "small")
        low, high = district["altitude_range_m"]
        text(1176, y+17, f'{district["resident_estimate"]:,} residents · {low}–{high} m', "tiny muted")
    text(1140, 607, "THE WHOLE LAKES", attrs='style="font-size:18px;font-weight:bold"')
    ix, iy, iw, ih = 1140, 625, 340, 320
    rect(ix, iy, iw, ih, 'fill="#e5dfce" stroke="#89958b"')
    points = [p for lake in d["whole_lake_outlines"] for p in lake["coordinates_local_m"]]
    minx, maxx, miny, maxy = min(p[0] for p in points), max(p[0] for p in points), min(p[1] for p in points), max(p[1] for p in points)
    ss = min((iw-40)/(maxx-minx), (ih-40)/(maxy-miny))
    def inset(p):
        return ix+iw/2+(p[0]-(minx+maxx)/2)*ss, iy+ih/2-(p[1]-(miny+maxy)/2)*ss
    for lake in d["whole_lake_outlines"]:
        out.append(f'<path class="water" d="{path(lake["coordinates_local_m"], inset)} Z"/>')
    # Inset footprint is the main map boundary, not a claim to the urbanized extent.
    x0, y0 = inset([b["min_x"], b["max_y"]])
    rect(x0, y0, (b["max_x"]-b["min_x"])*ss, (b["max_y"]-b["min_y"])*ss, 'fill="none" stroke="#9a5c35" stroke-width="1.4"')
    text(1160, 658, "NAMAR · 180 m", "tiny")
    text(1160, 913, "IREN · 176 m", "tiny")
    text(1140, 967, "Orange box: main planning frame", "tiny muted")
    text(1140, 985, "Projected source outlines; no bathymetry", "tiny muted")
    text(60, 1007, "Cream labels: places · dark labels: district envelopes · dotted lines: tram dependencies, not route alignments", "small muted")
    rect(60, 1030, 1420, 112, 'rx="6" fill="#e3e8de"')
    text(80, 1058, "THE DOWNHILL RELATIONSHIP", "label")
    text(80, 1084, "Namar outlet 180 m  →  Talweir junction 179.7 m  →  Iren inlet 176 m", attrs='style="font-size:20px"')
    text(80, 1108, "Navigation cut: 179.6 → 177.8 → 176 m, two 1.8 m locks · flowing bypass remains separate", "small")
    text(80, 1128, "The 4 m total head does not establish flow, useful power, vessel clearance, lock throughput or pipe pressure.", "small muted")
    text(60, 1185, "PLACES WORTH FINDING", attrs='style="font-size:19px;font-weight:bold"')
    for i, poi in enumerate(d["points_of_interest"]):
        col, row = i // 8, i % 8
        text(60+col*485, 1218+row*29, f'{poi["id"][-2:]}  {poi["name"]}', "small")
    text(60, 1470, "08 and 18 are supported overwater decks; 16 marks Trial Pond, with a separate dry visitor-access control.", "small muted")
    text(60, 1494, "Ferry interchanges are on land. Access legs, actual berths, streets, structures and navigable alignments remain to be designed.", "small muted")
    text(60, 1518, "Source: talweir_city_plan.json · AEQD from 28° N, 124° E, radius 5,900 km · authored controls, not constructed terrain.", "small muted")
    out.append('</svg>')
    (CITY / "talweir_city_plan.svg").write_text("\n".join(out)+"\n", encoding="utf-8")


def write_report(d, controls):
    lines = ["# Talweir plan and relationship checks", "", f"Generated from working city edition {d['edition']}. These checks concern authored controls and relationships, not surveyed terrain, navigability, hydraulic capacity or runtime behavior.", "",
             "## Placement and parent geography", "", "Nine district envelopes and twenty-four POIs; district population and gross-area allocations reconcile. POIs have known districts, envelope/altitude checks and explicit wet-placement rules. Deck altitude is checked against the nearest interpolated nominal channel surface, or the standing-water surface, rather than a descending envelope's lowest vertex. Trial Pond's marker is a focus with a separate dry visitor-access point. The two supported decks still require engineering.", "",
             "Whole Namar and Iren polygons are compared to the parent geography and independently projected using spherical AEQD, with a 0.02 m rounding tolerance. Reference frame identity, radius, orientation, source river controls and the parent lake connection agree. Cardinal and dateline projection controls also pass.", "",
             "Namar 180 m → junction 179.7 m → Iren 176 m retains the approximately 19.827 km source-control dogleg. The direct outlet-to-inlet spherical distance is approximately 10.457 km. These are control distances, not measured navigable routes.", "",
             "## Transport dependencies", "", "All transport nodes and every district connect to a declared regional gateway in the undirected service graph. This proves relationship connectivity only. Water services require access legs; land interchanges are not berths. Empty alignment lists remain unresolved.", "",
             "Separations below are straight horizontal distances in the local projection, not route lengths or guaranteed geodesic lower bounds.", "",
             "| Link | Service | Mode | Straight local separation km |", "|---|---|---|---:|"]
    nodes = {r["id"]: r for r in d["transport_nodes"]}
    for link in d["transport_links"]:
        a, b = [nodes[link[k]]["local_m"] for k in ("from_id", "to_id")]
        lines.append(f"| {link['id']} | {link['service']} | {link['mode']} | {math.hypot(b[0]-a[0], b[1]-a[1])/1000:.3f} |")
    lines += ["", "## Water and channels", "", "Main channel endpoints retain source surfaces; the navigation branch meets its host envelope within 0.1 m of interpolated nominal water level. Declared lock lifts match their adjacent passage controls and refer to a distinct bypass. This is a geometric screening tolerance, not a hydraulic accuracy claim.", "",
              "| Link | System | Endpoints | Endpoint altitude change m | Declared mode |", "|---|---|---|---:|---|"]
    water = {r["id"]: r for r in d["water_nodes"]}
    for link in d["water_links"]:
        a, b = [water[link[k]]["local_m"] for k in ("from_id", "to_id")]
        lines.append(f"| {link['id']} | {link['system']} | {link['from_id']} → {link['to_id']} | {b[2]-a[2]:+.2f} | {link['hydraulic_mode']} |")
    lines += ["", "Water links obey typed raw/treatment/potable/sewer/reclaimed relationships. Every delivery zone has an intake-to-treatment supply dependency. A shared zone coordinate does not mix reclaimed and potable pipework. Rising endpoint dependencies require an explicit pumped mode; no pump head, invert, hydraulic grade or delivery pressure is inferred from facility altitude. Unmodeled losses may require pumping even when endpoints descend.", "",
              "## Regression controls", "", "Rejected negative controls: " + "; ".join(controls) + ".", "", "Positive control: removing one redundant tram dependency retains connectivity through the remaining graph. AEQD cardinal and longitude-seam cases pass.", "",
              "## Remaining work", "", "No claim is made for surveyed banks, flood levels, channel cross-sections, slopes between controls, catchments, water yields, treatment efficacy, operating lock water use, structural supports, accessibility, electricity coverage, food production, transit throughput or Unreal placement. Whole-lake curves and local bank refinements need terrain reconciliation. District ellipses overlap and are not cadastral boundaries or additive area measurements.", "", "Visual inspection is separate: render this plate through inspect_world_plate.cjs with the talweir key; its report checks marker counts, clipped text and label-box intersections. A human/model image review is still needed for readability.", ""]
    (CITY / "TALWEIR_PLAN_CHECKS.md").write_text("\n".join(lines), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="Validate including regression controls without writing")
    args = parser.parse_args()
    d = json.loads((CITY / "talweir_city_plan.json").read_text(encoding="utf-8"))
    atlas = json.loads((ATLAS / "nacre_atlas.json").read_text(encoding="utf-8"))
    geography = json.loads((ATLAS / "nacre_geography.json").read_text(encoding="utf-8"))
    errors = validate(d, atlas, geography)
    if errors:
        raise SystemExit("\n".join(errors))
    controls = negative_controls(d, atlas, geography)
    if not args.check:
        render(d)
        write_report(d, controls)
    print(json.dumps({"passed": True, "districts": len(d["districts"]), "pois": len(d["points_of_interest"]), "negative_controls": controls,
                      "scope": "Authored geometry and relationships only; no terrain or hydraulic capacity validated"}))


if __name__ == "__main__":
    main()
