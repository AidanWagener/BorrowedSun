"""Check and draw authored settlement proposals, never certify terrain or service capacity."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
import textwrap
from pathlib import Path

from build_atlas import contains, segments_cross
from build_talweir import project, segment_distance

ROOT = Path(__file__).resolve().parents[2]
WORLD = ROOT / 'Docs/World'
CITY = WORLD / 'Cities'
NAMES = ('marroway', 'aven', 'sola', 'hesh', 'serein', 'dhal')


def number(x):
    return isinstance(x, (int, float)) and not isinstance(x, bool) and math.isfinite(x)


def point(p, n=3):
    return isinstance(p, list) and len(p) == n and all(number(v) for v in p)


def inside(p, polygon):
    return contains(p[:2], polygon) or any(segment_distance(p, a, b)[0] < .001 for a, b in zip(polygon, polygon[1:]))


def valid_polygon(ps):
    if len(ps) < 4 or not all(point(p, 2) for p in ps) or ps[0] != ps[-1]:
        return False
    if abs(sum(a[0]*b[1]-b[0]*a[1] for a,b in zip(ps,ps[1:]))) < .001:
        return False
    edges = list(zip(ps, ps[1:]))
    return not any(segments_cross(a,b,c,d) for i,(a,b) in enumerate(edges) for j,(c,d) in enumerate(edges)
                   if j > i+1 and not (i == 0 and j == len(edges)-1))


def dry(p, d):
    opposite = any(inside(p, r['points_xy_m']) for r in d['surface_polygons'])
    return (not opposite) if d['base_surface'] == 'land' else opposite


def clip_to_frame(polygon, bounds):
    """Clip straight projected control edges; do not invent shoreline detail."""
    result=[list(p) for p in polygon[:-1]]
    for axis,limit,lower in ((0,bounds[0],True),(0,bounds[2],False),(1,bounds[1],True),(1,bounds[3],False)):
        output=[]
        for a,b in zip(result,result[1:]+result[:1]):
            ai=a[axis]>=limit if lower else a[axis]<=limit
            bi=b[axis]>=limit if lower else b[axis]<=limit
            if ai != bi:
                t=(limit-a[axis])/(b[axis]-a[axis])
                output.append([a[i]+t*(b[i]-a[i]) for i in (0,1)])
            if bi: output.append(b)
        result=output
    return result+[result[0]] if result else []


def same_ring(a,b,tolerance=.02):
    if len(a)!=len(b) or not a or not all(point(p,2) for p in b): return False
    aa,bb=a[:-1],b[:-1]
    return any(all(math.dist(p,q)<=tolerance for p,q in zip(aa,ring[k:]+ring[:k]))
               for ring in (bb,list(reversed(bb))) for k in range(len(bb)))


def validate(d, atlas, geography):
    errors = []
    parent = next((c for c in atlas['cities'] if c['id'] == d['city_id']), None)
    f = d['frame']
    if parent is None or (d['city_name'], d['region_id'], d['population'], f['origin_lon_deg'], f['origin_lat_deg'], f['radius_m']) != (
            parent['name'], parent['region_id'], parent['population_core_estimate'], parent['lon_deg'], parent['lat_deg'], atlas['radius_km']*1000):
        errors.append('City identity/frame/population differs from parent atlas')
    bounds = f['bounds_m']
    if not point(bounds,4) or bounds[0] >= bounds[2] or bounds[1] >= bounds[3]:
        return errors + ['Invalid planning bounds']
    def within(p):
        return point(p) and bounds[0] <= p[0] <= bounds[2] and bounds[1] <= p[1] <= bounds[3]
    if d['base_surface'] not in ('land','water'):
        errors.append('Unknown base surface')
    groups = ('surface_polygons','channels','districts','places','transport_nodes','transport_links')
    ids = [r['id'] for group in groups for r in d[group]]
    if len(set(ids)) != len(ids): errors.append('Duplicate authored ID')
    if not 8 <= len(d['districts']) <= 10 or len(d['places']) != 24:
        errors.append('Expected 8–10 districts and24 places')
    for poly in d['surface_polygons']:
        if not valid_polygon(poly['points_xy_m']): errors.append('Invalid surface polygon: '+poly['id'])
        if poly['kind'] not in ('land','water') or poly['kind'] == d['base_surface']:
            errors.append('Surface polygon must oppose base: '+poly['id'])
        if not number(poly['surface_m']): errors.append('Invalid surface reference: '+poly['id'])
    if any(e.startswith('Invalid surface polygon') for e in errors): return errors
    polys = d['surface_polygons']
    if d.get('source_lake_id'):
        lake=next((p for p in geography['lakes'] if p['id']==d['source_lake_id']),None)
        record=d.get('source_lake_geometry',{})
        if lake is None or record.get('source_coordinates_lon_lat')!=lake['coordinates_lon_lat'] or record.get('surface_elevation_m')!=lake['surface_elevation_m']:
            errors.append('Source lake geometry/height not preserved')
        else:
            projection_frame={**f,'reference_radius_m':f['radius_m']}
            expected=[list(project(*p,projection_frame)) for p in lake['coordinates_lon_lat']]
            stored=record.get('projected_coordinates_xy_m',[])
            if len(expected)!=len(stored) or not all(point(p,2) for p in stored) or any(math.dist(p,q)>.02 for p,q in zip(expected,stored)):
                errors.append('Source lake projection differs from frame')
            local=next((p for p in polys if p['id']==record.get('render_polygon_id')),None)
            if local is None or local['kind']!='water' or local['surface_m']!=lake['surface_elevation_m'] or not same_ring(clip_to_frame(expected,bounds),local['points_xy_m']):
                errors.append('Local lake clipping/height differs from source')
    if d.get('nearby_river_context'):
        context=d['nearby_river_context']
        river=next((r for r in atlas['rivers']+geography['additional_rivers'] if r['id']==context['river_id']),None)
        terminal=None if river is None else river['points_lon_lat_elevation_m'][-1]
        if terminal is None or context.get('source_terminal_lon_lat_elevation_m')!=terminal:
            errors.append('Nearby river terminal differs from source')
        else:
            projected=[*project(*terminal[:2],{**f,'reference_radius_m':f['radius_m']}),terminal[2]]
            stored=context.get('projected_terminal_xyz_m')
            distance=context.get('distance_from_origin_m')
            if not point(stored) or math.dist(projected,stored)>.02 or not number(distance) or abs(math.hypot(*projected[:2])-distance)>.02:
                errors.append('Nearby river projection/distance differs from source')
    for i,a in enumerate(polys):
        for b in polys[i+1:]:
            ap,bp = a['points_xy_m'],b['points_xy_m']
            if any(segments_cross(p,q,r,s) for p,q in zip(ap,ap[1:]) for r,s in zip(bp,bp[1:])) or inside(ap[0],bp) or inside(bp[0],ap):
                errors.append('Overlapping surface polygons: '+a['id']+' / '+b['id'])
    districts = {r['id']:r for r in d['districts']}
    if any(type(r['population']) is not int or r['population'] < 0 for r in districts.values()) or sum(r['population'] for r in districts.values()) != d['population']:
        errors.append('District population mismatch')
    if any(not number(r['area_allowance_km2']) or r['area_allowance_km2'] <= 0 for r in districts.values()) or not math.isclose(sum(r['area_allowance_km2'] for r in districts.values()),d['gross_area_allowance_km2']):
        errors.append('District area allowance mismatch')
    for district in districts.values():
        if not within(district['center_xyz_m']) or not point(district['radii_m'],2) or min(district['radii_m']) <= 0 or not point(district['altitude_range_m'],2) or district['altitude_range_m'][0] > district['altitude_range_m'][1]:
            errors.append('Invalid district envelope: '+district['id'])
    if any(e.startswith('Invalid district envelope') for e in errors): return errors
    for place in d['places']:
        p = place['xyz_m']; district = districts.get(place['district_id'])
        if district is None or not within(p):
            errors.append('Unknown district or invalid place coordinate: '+place['id']); continue
        if sum(((p[i]-district['center_xyz_m'][i])/district['radii_m'][i])**2 for i in (0,1)) > 1.000001:
            errors.append('Place outside district envelope: '+place['id'])
        if not district['altitude_range_m'][0] <= p[2] <= district['altitude_range_m'][1]:
            errors.append('Place outside altitude envelope: '+place['id'])
        if place['placement'] == 'land':
            if not dry(p,d): errors.append('Land place in water: '+place['id'])
        elif place['placement'] == 'supported_overwater':
            access = place['shore_access_xyz_m']; surface = place['water_surface_m']
            if dry(p,d) or not within(access) or not dry(access,d) or not number(surface) or p[2] <= surface:
                errors.append('Unsupported wet placement: '+place['id'])
            if number(surface):
                matches = []
                for channel in d['channels']:
                    for a,b in zip(channel['points_xyz_m'],channel['points_xyz_m'][1:]):
                        distance,t = segment_distance(p,a,b); matches.append((distance,a[2]+t*(b[2]-a[2])))
                local_surfaces = [poly['surface_m'] for poly in polys
                                  if poly['kind'] == 'water' and inside(p, poly['points_xy_m'])]
                expected_surface = min(matches)[1] if matches else (local_surfaces[0] if local_surfaces else 0)
                if abs(surface-expected_surface) > .1: errors.append('Wet placement surface differs from local water: '+place['id'])
        else: errors.append('Unknown placement type: '+place['id'])
    water_ids = {p['id'] for p in polys if p['kind']=='water'}
    for channel in d['channels']:
        ps = channel['points_xyz_m']
        if len(ps)<2 or not all(point(p) for p in ps):
            errors.append('Invalid channel profile: '+channel['id']); continue
        if any(a[2]<b[2] for a,b in zip(ps,ps[1:])): errors.append('Channel rises downstream: '+channel['id'])
        if not channel['surface_polygon_ids'] or not set(channel['surface_polygon_ids']) <= water_ids:
            errors.append('Unknown channel surface: '+channel['id'])
        if any(dry(p,d) for p in ps): errors.append('Channel leaves authored water envelope: '+channel['id'])
    if d['source_river_id']:
        river = next((r for r in atlas['rivers']+geography['additional_rivers'] if r['id']==d['source_river_id']),None)
        controls = [] if river is None else [p for p in river['points_lon_lat_elevation_m'] if p[:2]==[f['origin_lon_deg'],f['origin_lat_deg']]]
        if len(controls)!=1 or not any(any(abs(p[0])<.001 and abs(p[1])<.001 and abs(p[2]-controls[0][2])<.001 for p in c['points_xyz_m']) for c in d['channels']):
            errors.append('Source river center/height not preserved')
    elif d['channels']: errors.append('Channels require explicit source river')
    nodes = {n['id']:n for n in d['transport_nodes']}; graph = {key:set() for key in nodes}
    for n in nodes.values():
        external_gateway = n['district_id'] is None and n['regional_gateway'] is True
        if (n['district_id'] not in districts and not external_gateway) or not within(n['xyz_m']) or not dry(n['xyz_m'],d) or type(n['regional_gateway']) is not bool:
            errors.append('Invalid dry transport interchange: '+n['id'])
    for edge in d['transport_links']:
        a,b=edge['from'],edge['to']
        if a not in nodes or b not in nodes or a==b:
            errors.append('Invalid transport endpoint: '+edge['id']); continue
        if edge['mode'] not in ('walking','tram','bus','ferry','rail','road','cable'):
            errors.append('Unknown transport mode: '+edge['id'])
        if d['base_surface']=='water' and edge['mode']!='ferry':
            common = [p for p in polys if inside(nodes[a]['xyz_m'],p['points_xy_m']) and inside(nodes[b]['xyz_m'],p['points_xy_m'])]
            if not common: errors.append('Land dependency crosses separate islands without ferry: '+edge['id'])
        graph[a].add(b);graph[b].add(a)
    starts = {n['id'] for n in nodes.values() if n['regional_gateway'] is True}
    if not starts: errors.append('Missing regional gateway')
    seen=set();pending=list(starts)
    while pending:
        n=pending.pop()
        if n not in seen: seen.add(n);pending.extend(graph[n]-seen)
    if seen != set(nodes): errors.append('Isolated transport node')
    if not set(districts) <= {nodes[n]['district_id'] for n in seen}: errors.append('District lacks connected transport')
    s=d['service_cases']; keys=[k for k in s if k!='scope']
    if any(not number(s[k]) or s[k]<0 for k in keys):
        errors.append('Invalid service quantity'); return errors
    if not 0 <= s['network_loss_fraction'] < 1: errors.append('Invalid network loss fraction')
    else:
        demand=d['population']*s['domestic_liters_per_person_per_turn']/1000+s['other_potable_m3_per_turn']
        if not math.isclose(demand,s['delivered_potable_m3_per_turn'],abs_tol=.01): errors.append('Delivered water arithmetic mismatch')
        if not math.isclose(demand/(1-s['network_loss_fraction']),s['network_input_m3_per_turn'],abs_tol=.01): errors.append('Network water arithmetic mismatch')
    if not math.isclose(d['population']*s['edible_product_kg_per_person_per_turn']/1000,s['edible_product_tonnes_per_turn'],abs_tol=.001): errors.append('Food arithmetic mismatch')
    if s['electric_peak_mw'] < s['electric_average_mw']: errors.append('Peak below average power')
    return errors


def controls(d,a,g):
    cases=[]
    def case(name,change,needle):
        x=copy.deepcopy(d);change(x);cases.append((name,x,needle))
    case('moved city origin',lambda x:x['frame'].__setitem__('origin_lon_deg',0),'parent atlas')
    case('population allocation',lambda x:x['districts'][0].__setitem__('population',0),'population mismatch')
    case('missing place district',lambda x:x['places'][0].__setitem__('district_id','missing'),'Unknown district')
    case('nonfinite place',lambda x:x['places'][0].__setitem__('xyz_m',[float('nan'),0,0]),'invalid place coordinate')
    case('place outside district',lambda x:x['places'][0].__setitem__('xyz_m',[x['frame']['bounds_m'][0],x['frame']['bounds_m'][1],x['places'][0]['xyz_m'][2]]),'Place outside district')
    case('missing gateway',lambda x:[n.__setitem__('regional_gateway',False) for n in x['transport_nodes']],'Missing regional gateway')
    case('dangling route endpoint',lambda x:x['transport_links'][0].__setitem__('to','missing'),'Invalid transport endpoint')
    case('unassigned ordinary interchange',lambda x:x['transport_nodes'][0].update(district_id=None,regional_gateway=False),'Invalid dry transport interchange')
    case('open surface polygon',lambda x:x['surface_polygons'][0]['points_xy_m'].pop(),'Invalid surface polygon')
    case('nonfinite service',lambda x:x['service_cases'].__setitem__('electric_average_mw',float('inf')),'Invalid service quantity')
    case('loss of all distribution supply',lambda x:x['service_cases'].__setitem__('network_loss_fraction',1),'Invalid network loss fraction')
    case('incorrect treated input',lambda x:x['service_cases'].__setitem__('network_input_m3_per_turn',1),'Network water arithmetic')
    case('incorrect food demand',lambda x:x['service_cases'].__setitem__('edible_product_tonnes_per_turn',1),'Food arithmetic')
    if d['source_river_id']:
        case('river center altitude changed',lambda x:[p.__setitem__(2,p[2]+1) for c in x['channels'] for p in c['points_xyz_m'] if p[0]==0 and p[1]==0],'Source river center/height')
    else:
        b=d['frame']['bounds_m']
        wet=next(([b[0]+(b[2]-b[0])*(i+.5)/41,b[1]+(b[3]-b[1])*(j+.5)/41,20]
                  for i in range(41) for j in range(41)
                  if not dry([b[0]+(b[2]-b[0])*(i+.5)/41,b[1]+(b[3]-b[1])*(j+.5)/41],d)),None)
        if wet is None: raise ValueError('No authored water found for placement control')
        case('land placed in authored water',lambda x:x['places'][0].update(xyz_m=wet,placement='land'),'Land place in water')
    supported=next((i for i,p in enumerate(d['places']) if p['placement']=='supported_overwater'),None)
    if supported is not None:
        case('wrong local water surface',lambda x:x['places'][supported].__setitem__('water_surface_m',x['places'][supported]['water_surface_m']+1),'surface differs from local water')
    if d.get('source_lake_id'):
        case('lake source altitude changed',lambda x:x['source_lake_geometry'].__setitem__('surface_elevation_m',0),'Source lake geometry/height')
        case('projected lake vertex moved',lambda x:x['source_lake_geometry']['projected_coordinates_xy_m'][1].__setitem__(0,0),'Source lake projection')
        def move_shore(x):
            polygon=next(p for p in x['surface_polygons'] if p['id']==x['source_lake_geometry']['render_polygon_id'])
            polygon['points_xy_m'][1][0]+=100
        case('local shoreline invented',move_shore,'Local lake clipping/height')
    if d.get('nearby_river_context'):
        case('nearby river moved into city',lambda x:x['nearby_river_context'].__setitem__('projected_terminal_xyz_m',[0,0,270]),'Nearby river projection/distance')
    for name,x,needle in cases:
        if not any(needle in error for error in validate(x,a,g)): raise ValueError('Regression control not rejected: '+name)
    return [name for name,_,_ in cases]


def render(d,control_names):
    name=d['city_name'];slug=name.lower();b=d['frame']['bounds_m']
    svg=['<svg xmlns="http://www.w3.org/2000/svg" width="1560" height="1570" viewBox="0 0 1560 1570" role="img" aria-labelledby="title desc">',f'<title id="title">{html.escape(name)} authored settlement plan</title>','<desc id="desc">Twenty-four named places and district envelopes. Lines represent service dependencies, not surveyed routes. No constructed city is claimed.</desc>','<rect width="1560" height="1570" fill="#f3efe2"/>','<style>text{font-family:Arial,sans-serif;fill:#233e47;font-size:16px}.small{font-size:13px}.tiny{font-size:12px}</style>']
    def text(x,y,t,attrs=''):
        svg.append(f'<text x="{x:.2f}" y="{y:.2f}" {attrs}>{html.escape(str(t))}</text>')
    text(60,47,name.upper()+' / A PLACE TO ARRIVE','style="font-size:29px"')
    text(60,79,f'{d["city_id"]} · 742 AL · {d["population"]:,} core residents, authored estimate · settlement proposal v1')
    text(60,105,'North is up · coordinates in kilometers · z is planning sea-level altitude · no surveyed terrain or operating service capacity','class="small"')
    scale=min(1035/(b[2]-b[0]),800/(b[3]-b[1]));w=(b[2]-b[0])*scale;h=(b[3]-b[1])*scale
    ox=60+(1035-w)/2;oy=145+(800-h)/2
    def xy(p):return ox+(p[0]-b[0])*scale,oy+(b[3]-p[1])*scale
    def pts(ps):return ' '.join(f'{x:.2f},{y:.2f}' for x,y in map(xy,ps))
    svg.append(f'<rect x="{ox}" y="{oy}" width="{w}" height="{h}" fill="{"#bdd9df" if d["base_surface"]=="water" else "#e3dfce"}" stroke="#8ca6aa"/>')
    for p in d['surface_polygons']:
        svg.append(f'<polygon points="{pts(p["points_xy_m"])}" fill="{"#bdd9df" if p["kind"]=="water" else "#e3dfce"}" stroke="#477f90" stroke-width="1.2"/>')
    step=2000 if max(b[2]-b[0],b[3]-b[1])<=30000 else 5000
    for x in range(math.ceil(b[0]/step)*step,int(b[2])+1,step):
        px,_=xy([x,0]);svg.append(f'<line x1="{px}" y1="{oy}" x2="{px}" y2="{oy+h}" stroke="#aebfb8" opacity=".35"/>');text(px,oy+h+23,f'{x/1000:g}','class="tiny" text-anchor="middle"')
    for y in range(math.ceil(b[1]/step)*step,int(b[3])+1,step):
        _,py=xy([0,y]);svg.append(f'<line x1="{ox}" y1="{py}" x2="{ox+w}" y2="{py}" stroke="#aebfb8" opacity=".35"/>');text(ox-9,py+4,f'{y/1000:g}','class="tiny" text-anchor="end"')
    text(ox,oy-10,'y north / km','class="tiny"');text(ox+w,oy+h+43,'x east / km','class="tiny" text-anchor="end"')
    palette=['#bd9355','#ad7464','#767da1','#b69a3b','#648c66','#527d89','#a78163','#4c917e','#7c90a1','#94769d']
    for i,r in enumerate(d['districts']):
        x,y=xy(r['center_xyz_m']);rx,ry=[v*scale for v in r['radii_m']]
        svg.append(f'<ellipse data-district="{r["id"]}" cx="{x}" cy="{y}" rx="{rx}" ry="{ry}" fill="{palette[i]}" fill-opacity=".12" stroke="{palette[i]}" stroke-opacity=".75"/>')
    nodes={n['id']:n for n in d['transport_nodes']}
    for edge in d['transport_links']:
        x,y=xy(nodes[edge['from']]['xyz_m']);u,v=xy(nodes[edge['to']]['xyz_m'])
        svg.append(f'<line x1="{x}" y1="{y}" x2="{u}" y2="{v}" stroke="{"#247089" if edge["mode"]=="ferry" else "#856c50"}" stroke-dasharray="3 7" stroke-opacity=".5"/>')
    occupied=[]
    def label(x,y,value,dark=False):
        width=33 if dark else 27;height=24
        candidates=[(10,-13),(-width-10,-13),(10,7),(-width-10,7),(0,-40),(0,23),(30,-24),(-width-30,-24),(30,25),(-width-30,25)]
        for dx,dy in candidates:
            rect=(x+dx,y+dy,x+dx+width,y+dy+height)
            if rect[0]<ox or rect[1]<oy or rect[2]>ox+w or rect[3]>oy+h:continue
            if any(rect[0]<q[2]+3 and rect[2]>q[0]-3 and rect[1]<q[3]+3 and rect[3]>q[1]-3 for q in occupied):continue
            occupied.append(rect);break
        else:raise ValueError('No clear label position: '+value)
        a,c=rect[:2]
        svg.append(f'<line x1="{x}" y1="{y}" x2="{a+width/2}" y2="{c+height/2}" stroke="#a2aaa0" stroke-width=".7"/>')
        svg.append(f'<rect class="map-label-box" x="{a}" y="{c}" width="{width}" height="{height}" rx="3" fill="{"#34535d" if dark else "#fff9e9"}" stroke="#a9b1a3"/>')
        text(a+width/2,c+17,value,f'class="small" text-anchor="middle" style="fill:{"#fff9e9" if dark else "#233e47"}"')
    for r in d['districts']:
        p=r['center_xyz_m'];label(*xy([p[0]-.28*r['radii_m'][0],p[1]+.8*r['radii_m'][1]]),r['id'].split('-')[-1],True)
    for i,p in enumerate(d['places']):
        x,y=xy(p['xyz_m']);svg.append(f'<circle data-poi="{p["id"]}" cx="{x}" cy="{y}" r="3.5" fill="#345863"/>');label(x,y,f'{i+1:02}')
    text(1140,161,'DISTRICTS','style="font-size:20px;font-weight:bold"')
    for i,r in enumerate(d['districts']):
        text(1140,198+i*54,r['id'].split('-')[-1]+' '+r['name'])
        text(1140,217+i*54,f'{r["population"]:,} residents · {r["altitude_range_m"][0]:g}–{r["altitude_range_m"][1]:g} m','class="small"')
    text(1140,702,'SURFACE CONTROLS','style="font-size:18px;font-weight:bold"')
    for i,p in enumerate(d['surface_polygons'][:5]):
        text(1140,728+i*18,p['name'],'class="small"')
    text(1140,852,'READING THE PLAN','style="font-size:18px;font-weight:bold"')
    note='Cream labels: places. Dark labels: district envelopes. Dotted lines: transport dependencies, not route alignments. Ferry points are dry interchanges; actual berths and access legs remain to be designed.'
    for i,line in enumerate(textwrap.wrap(note,43)):text(1140,880+18*i,line,'class="small"')
    text(60,1016,'PLACES WORTH FINDING','style="font-size:22px;font-weight:bold"')
    for i,p in enumerate(d['places']):
        x=60+(i//8)*490;y=1055+(i%8)*47
        for j,line in enumerate(textwrap.wrap(f'{i+1:02}  {p["name"]}',48)):text(x,y+j*18,line)
    text(60,1456,f'{d["gross_area_allowance_km2"]:g} km² gross allocation; district ellipses overlap and are not measured parcel areas.','class="small"')
    text(60,1480,'Local surfaces and heights are authored controls. Floods, depths, gradients, structures and service capacity require separate work.','class="small"')
    text(60,1504,f'Source: {slug}_settlement.json · spherical AEQD at {d["frame"]["origin_lat_deg"]:g}° latitude, {d["frame"]["origin_lon_deg"]:g}° longitude','class="small"')
    text(60,1528,'This plate locates a proposal; it does not show a built or simulated city.','class="small"')
    svg.append('</svg>');(CITY/f'{slug}_settlement.svg').write_text('\n'.join(svg)+'\n',encoding='utf8')
    lines=[f'# {name} settlement checks','', 'Authored geometry, identity, placement and demand arithmetic only; no terrain, route alignment, resource yield or service capacity validated.','',f'{len(d["districts"])} district allocations sum to {d["population"]:,} residents and {d["gross_area_allowance_km2"]:g} km². Twenty-four places have known districts, finite local positions, envelope/altitude membership and explicit dry or supported-water placement.','', 'Surface polygons are closed, non-self-intersecting and disjoint. Transport interchanges are on authored land, and every node/district connects to a declared gateway. Island land dependencies share an island; ferry lines still need actual berths and water alignments.','', 'Where applicable, source river identity, city-center altitude, nonrising local controls and water-envelope membership were checked. These do not measure depth, flood protection or hydraulic feasibility.','', 'Water demand, treated network input, distribution loss and core food product mass reconcile under declared assumptions. Treatment losses, desalination inputs and yields are separate prose obligations.','', 'Rejected regression controls: '+ '; '.join(control_names)+'.','','A browser render and visual review are separate from these checks. Source JSON and the city dossier remain authoritative.','']
    (CITY/f'{slug.upper()}_SETTLEMENT_CHECKS.md').write_text('\n'.join(lines),encoding='utf8')


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('cities',nargs='*',choices=NAMES);parser.add_argument('--check',action='store_true');args=parser.parse_args()
    atlas=json.loads((WORLD/'Atlas/nacre_atlas.json').read_text(encoding='utf8'));geography=json.loads((WORLD/'Atlas/nacre_geography.json').read_text(encoding='utf8'))
    for slug in args.cities or NAMES:
        d=json.loads((CITY/f'{slug}_settlement.json').read_text(encoding='utf8'));errors=validate(d,atlas,geography)
        if errors:raise SystemExit('\n'.join(errors))
        checks=controls(d,atlas,geography)
        if not args.check:render(d,checks)
        print(json.dumps({'city':d['city_name'],'passed':True,'districts':len(d['districts']),'places':len(d['places']),'negative_controls':checks,'scope':'Authored relationships only'}))
