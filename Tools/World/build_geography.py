"""Check and draw geological/drainage planning controls, without generating terrain."""
import json
import math
from pathlib import Path
import xml.etree.ElementTree as ET

from build_atlas import ATLAS, contains, distance_km, sampled_coast_distance, segments_cross, validate

SVG = "http://www.w3.org/2000/svg"
ET.register_namespace("", SVG)


def check(atlas, geo):
    """Check authored references and controls; this does not derive catchment boundaries."""
    errors = validate(atlas)
    radius = atlas["radius_km"]
    land = [p["coordinates_lon_lat"] for p in atlas["landmasses"]]
    lakes = {l["id"]: l for l in geo["lakes"]}
    cities = {c["id"]: c for c in atlas["cities"]}
    regions = {r["id"] for r in atlas["regions"]}
    river_list = atlas["rivers"] + geo["additional_rivers"]
    rivers = {r["id"]: r for r in river_list}
    if len(rivers) != len(river_list):
        errors.append("Duplicate river IDs across the two sources")
    for key in ("reliefs", "lakes", "water_systems", "connections"):
        ids = [r["id"] for r in geo[key]]
        if len(ids) != len(set(ids)):
            errors.append(f"Duplicate {key} IDs")
    for feature in geo["reliefs"]:
        for lon, lat, height in feature["points_lon_lat_reference_height_m"]:
            if not (-180 <= lon <= 180 and -90 <= lat <= 90 and math.isfinite(height)):
                errors.append(f"Invalid relief control: {feature['id']}")
    for lake in lakes.values():
        pts = lake["coordinates_lon_lat"]
        if pts[0] != pts[-1]:
            errors.append(f"Open lake polygon: {lake['id']}")
        for point in pts:
            if not any(l["name"] == lake["landmass"] and contains(point, l["coordinates_lon_lat"]) for l in atlas["landmasses"]):
                errors.append(f"Lake vertex outside its continent: {lake['id']}")
        edges = list(zip(pts, pts[1:]))
        for i, (a, b) in enumerate(edges):
            for j, (c, d) in enumerate(edges):
                if j > i+1 and not (i == 0 and j == len(edges)-1) and segments_cross(a,b,c,d):
                    errors.append(f"Self-intersecting lake: {lake['id']}")
    for river in river_list:
        pts = river["points_lon_lat_elevation_m"]
        flats = {(s["from_index"], s["to_index"]):s for s in river.get("flat_lake_segments", [])}
        for i, (a,b) in enumerate(zip(pts,pts[1:])):
            if a[2] < b[2] or (a[2] == b[2] and (i,i+1) not in flats):
                errors.append(f"Non-descending unqualified river segment: {river['id']}:{i}")
        for (start, end), segment in flats.items():
            lake = lakes.get(segment["lake_id"])
            if not lake or end != start+1 or not (0 <= start < end < len(pts)):
                errors.append(f"Invalid lake segment: {river['id']}")
                continue
            for index in (start,end):
                p=pts[index]
                if p[2] != lake["surface_elevation_m"]:
                    errors.append(f"Lake surface mismatch: {river['id']} / {lake['id']}")
                if not contains(p[:2], lake["coordinates_lon_lat"]) and sampled_coast_distance(p[:2],[lake["coordinates_lon_lat"]],radius) > .2:
                    errors.append(f"Lake segment endpoint outside lake: {river['id']} / {lake['id']}")
    for system in geo["water_systems"]:
        river = rivers.get(system["river_id"])
        if not river or any(r not in regions for r in system["region_ids"]) or system["focus_region_id"] not in regions:
            errors.append(f"Unresolved water system reference: {system['id']}")
            continue
        if system["focus_region_id"] not in system["region_ids"]:
            errors.append(f"Water system focus outside its listed regions: {system['id']}")
        end = river["points_lon_lat_elevation_m"][-1]
        if system["outlet_type"] == "ocean":
            if end[2] != 0 or sampled_coast_distance(end[:2],land,radius) > 2:
                errors.append(f"River ocean outlet misses datum/coast: {system['id']}")
        elif system["outlet_type"] == "closed basin":
            # The current schema links terminal waters spatially, without basin polygons.
            # Require one closed lake at the mouth and its authored surface height.
            terminal_lakes = [lake for lake in lakes.values()
                              if "outlet" in lake and lake["outlet"] is None
                              and (contains(end[:2], lake["coordinates_lon_lat"])
                                   or sampled_coast_distance(end[:2], [lake["coordinates_lon_lat"]], radius) <= .2)]
            if len(terminal_lakes) != 1:
                errors.append(f"Closed-basin river mouth must link to one terminal lake: {system['id']}")
            elif not math.isclose(end[2], terminal_lakes[0]["surface_elevation_m"], rel_tol=0, abs_tol=1e-6):
                errors.append(f"Closed-basin river mouth misses lake surface: {system['id']} / {terminal_lakes[0]['id']}")
        else:
            errors.append(f"Unknown water system outlet type: {system['id']}")
    for connection in geo["connections"]:
        a, b = lakes.get(connection.get("from")), lakes.get(connection.get("to"))
        city = cities.get(connection.get("via_city"))
        if a is None or b is None or city is None:
            errors.append(f"Unresolved lake connection reference: {connection['id']}")
            continue
        drop = connection.get("drop_m")
        if (connection.get("direction") != "downstream"
                or not isinstance(drop, (int, float)) or isinstance(drop, bool)
                or not math.isfinite(drop) or drop <= 0
                or not math.isclose(a["surface_elevation_m"] - b["surface_elevation_m"], drop, rel_tol=0, abs_tol=1e-6)):
            errors.append(f"Invalid downstream lake direction/drop: {connection['id']}")
        for lake in (a,b):
            if sampled_coast_distance((city["lon_deg"],city["lat_deg"]),[lake["coordinates_lon_lat"]],radius) > 15:
                errors.append(f"Lake-transfer city not near both lake shores: {connection['id']}")
    return errors


def render(atlas, geo):
    tree=ET.parse(ATLAS / "nacre_geographic_concept.svg")
    root=tree.getroot()
    def tag(name): return f"{{{SVG}}}{name}"
    def point(p): return 60+(p[0]+180)*3, 70+(90-p[1])*3
    def path(points):
        commands=[]
        previous=None
        for p in points:
            x,y=point(p)
            command="M" if previous is None or abs(p[0]-previous[0]) > 180 else "L"
            commands.append(f"{command}{x:.2f},{y:.2f}")
            previous=p
        return " ".join(commands)
    root.find(tag('title')).text=f"Nacre relief and drainage controls, edition {geo['edition']}"
    root.find(tag('desc')).text="Authored geological province lines, three lake concepts and eight non-rising water trunks. Brown lines are province controls, not a continuous terrain surface."
    for text in root.findall(tag('text')):
        if text.attrib.get('y')=='32': text.text=f"NACRE · RELIEF AND DRAINAGE {geo['edition']}"
        elif text.attrib.get('y')=='54': text.text="Brown: relief/province controls · Blue: lake surfaces and water trunks · All heights and outlines are authoring concepts"
        elif text.attrib.get('y')=='807': text.text="No DEM, rainfall, discharge, hydraulic model or continuous plate-motion reconstruction is claimed by these controls."
    index=next(i for i,el in enumerate(root) if el.attrib.get('class')=='river')
    for feature in geo['reliefs']:
        color='#9a543d'
        attrs={'d':path([p[:2] for p in feature['points_lon_lat_reference_height_m']]),'fill':'none','stroke':color,'stroke-width':'5','opacity':'.75'}
        if feature['id'] in ('REL-007','REL-009','REL-010'): attrs['stroke-dasharray']='5 4'
        root.insert(index,ET.Element(tag('path'),attrs)); index+=1
    for lake in geo['lakes']:
        root.insert(index,ET.Element(tag('path'),{'d':path(lake['coordinates_lon_lat'])+' Z','fill':'#418398','stroke':'#315f73','stroke-width':'1'}));index+=1
    for river in geo['additional_rivers']:
        root.insert(index,ET.Element(tag('path'),{'d':path([p[:2] for p in river['points_lon_lat_elevation_m']]),'class':'river'}));index+=1
    for i,feature in enumerate(geo['reliefs'],1):
        x,y=point(feature['label_lon_lat'])
        color='#eac09a' if feature['id'] in ('REL-009','REL-010') else '#8a2d16'
        label=ET.SubElement(root,tag('text'),{'x':str(x),'y':str(y),'class':'small','style':f'fill:{color};font-weight:bold'})
        label.text=f"R{i}"
    tree.write(ATLAS/'nacre_relief_and_water.svg',encoding='utf-8',xml_declaration=True)
    lines=['# Relief and water controls — edition '+geo['edition'],'','Authored controls, not terrain samples. See `01_LAND_WATER_AND_DEEP_TIME.md` for the physical and fictional assumptions.','','| Map key | Province | Reference height range |','|---|---|---:|']
    for i,f in enumerate(geo['reliefs'],1):
        heights=[p[2] for p in f['points_lon_lat_reference_height_m']]
        lines.append(f"| R{i} | {f['name']} | {min(heights):,}–{max(heights):,} m |")
    lines+=['','The ranges are authored controls, not full regional minimum/maximum elevations. Dashed lines identify rift or island-chain axes, which need not be continuous high ground.','','| Lake | Planning surface | Water type |','|---|---:|---|']
    for lake in geo['lakes']: lines.append(f"| {lake['name']} | {lake['surface_elevation_m']} m | {lake['water_type']} |")
    lines+=['','| Coastal city | Sampled distance to concept shore |','|---|---:|']
    for c in atlas['cities']:
        if 'coastal_approach_max_km' not in c: continue
        polygons=[l['coordinates_lon_lat'] for l in atlas['landmasses'] if l['name']==c['landmass']]
        d=sampled_coast_distance((c['lon_deg'],c['lat_deg']),polygons,atlas['radius_km'])
        lines.append(f"| {c['name']} | {d:.1f} km |")
    lines+=['','Distance samples are an upper bound to lon/lat-linear concept edges, with a sampling gap of roughly 1.5 km or less. They establish proximity only; city-scale shores, landing approaches and bathymetry remain separate work.','']
    (ATLAS/'GEOGRAPHY_CONTROLS.md').write_text('\n'.join(lines),encoding='utf-8')


if __name__=='__main__':
    atlas=json.loads((ATLAS/'nacre_atlas.json').read_text(encoding='utf-8'))
    geo=json.loads((ATLAS/'nacre_geography.json').read_text(encoding='utf-8'))
    errors=check(atlas,geo)
    if errors: raise SystemExit('\n'.join(errors))
    render(atlas,geo)
    print(f"Geography checked: {len(geo['reliefs'])} relief provinces, {len(geo['lakes'])} lake surfaces, {len(atlas['rivers'])+len(geo['additional_rivers'])} water trunks. No continuous terrain/hydrology validation.")
