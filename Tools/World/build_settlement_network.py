"""Check population bookkeeping and concept placement; render an authored settlement atlas."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
from pathlib import Path

from build_atlas import contains, distance_km, sampled_coast_distance

ROOT = Path(__file__).resolve().parents[2]
WORLD = ROOT / 'Docs' / 'World'
OUT = WORLD / 'Settlement'


def number(x):
    return type(x) in (int, float) and math.isfinite(x)


def point(p):
    return isinstance(p, list) and len(p) == 2 and all(number(v) for v in p) and -180 <= p[0] <= 180 and -90 <= p[1] <= 90


def validate(d, a, g):
    errors = []
    regions = {r['region_id']: r for r in d['regions']}
    expected_regions = {r['id'] for r in a['regions']}
    region_landmass = {r['id']: r['landmass'] for r in a['regions']}
    if len(regions) != len(d['regions']) or set(regions) != expected_regions:
        errors.append('Regional coverage/identity mismatch')
    if d['population_estimate'] != 34800000 or d['present_year_AL'] != a['present_year_AL']:
        errors.append('Reference population/year changed')
    if not number(d['uncertainty_fraction']) or not 0 <= d['uncertainty_fraction'] < 1:
        errors.append('Invalid uncertainty')
    keys = set(d['buckets'])
    expected_keys = {'core_city','associated_settlement_belt','other_towns','dispersed_and_station','mobile_usual_residents'}
    if keys != expected_keys or len(keys) != len(d['buckets']):
        errors.append('Population bucket identity mismatch')
    for r in d['regions']:
        alloc = r['allocation']
        if set(alloc) != expected_keys or any(type(v) is not int or v < 0 for v in alloc.values()) or type(r['total_habitual_residents']) is not int or r['total_habitual_residents'] < 0:
            errors.append('Invalid regional population: '+r['region_id'])
            continue
        if sum(alloc.values()) != r['total_habitual_residents']:
            errors.append('Regional population does not close: '+r['region_id'])
        core = sum(c['population_core_estimate'] for c in a['cities'] if c['region_id'] == r['region_id'])
        if alloc['core_city'] != core:
            errors.append('Core city population differs from atlas: '+r['region_id'])
    if all(type(r['total_habitual_residents']) is int for r in d['regions']) and sum(r['total_habitual_residents'] for r in d['regions']) != d['population_estimate']:
        errors.append('World population does not close')
    ids = [s['id'] for s in d['settlements']]
    if len(ids) != len(set(ids)):
        errors.append('Duplicate settlement ID')
    names = [s['name'] for s in d['settlements']]
    if len(names) != len(set(names)):
        errors.append('Duplicate settlement name')
    assigned = {}
    for s in d['settlements']:
        q = [s['lon_deg'], s['lat_deg']]
        region = regions.get(s['region_id'])
        if region is None or s['allocation_bucket'] not in keys or type(s['habitual_residents']) is not int or s['habitual_residents'] <= 0:
            errors.append('Invalid settlement allocation: '+s['id'])
            continue
        if s['landmass'] != region_landmass[s['region_id']]:
            errors.append('Settlement landmass differs from region: '+s['id'])
        if s['allocation_bucket'] == 'core_city':
            errors.append('Secondary settlement duplicates major-city core: '+s['id'])
        k = (s['region_id'], s['allocation_bucket'])
        assigned[k] = assigned.get(k, 0) + s['habitual_residents']
        if not point(q):
            errors.append('Invalid settlement coordinate: '+s['id'])
            continue
        lands = [l for l in a['landmasses'] if contains(q, l['coordinates_lon_lat'])]
        lakes = [l for l in g['lakes'] if contains(q, l['coordinates_lon_lat'])]
        if s['position_kind'] == 'fixed_land_proposal':
            if not any(l['name'] == s['landmass'] for l in lands) or lakes:
                errors.append('Land settlement outside dry assigned landmass: '+s['id'])
        elif s['position_kind'] in ('fixed_marine_proposal', 'mobile_reference'):
            if lands or lakes or s['landmass'] != 'Ocean':
                errors.append('Marine reference lies on land or lake: '+s['id'])
            if s['position_kind'] == 'mobile_reference' and s['allocation_bucket'] != 'mobile_usual_residents':
                errors.append('Mobile community counted outside mobile bucket: '+s['id'])
        else:
            errors.append('Unknown settlement position kind: '+s['id'])
        if 'concept_coast_max_km' in s:
            limit = s['concept_coast_max_km']
            polygons = [l['coordinates_lon_lat'] for l in a['landmasses'] if l['name'] == s['landmass']]
            if not number(limit) or limit <= 0 or not polygons or sampled_coast_distance(q, polygons, a['radius_km']) > limit:
                errors.append('Coastal settlement too far from concept shore: '+s['id'])
    for (rid, bucket), n in assigned.items():
        if n > regions[rid]['allocation'][bucket]:
            errors.append('Named settlements exceed regional bucket: '+rid+'/'+bucket)
    functional_ids = [f['id'] for f in d['functional_regions']]
    if len(functional_ids) != len(set(functional_ids)):
        errors.append('Duplicate functional-region ID')
    for f in d['functional_regions']:
        region = regions.get(f['within_region_id'])
        core = next((c for c in a['cities'] if c['id'] == f['includes_core_city']), None)
        if region is None or core is None or core['region_id'] != f['within_region_id'] or type(f['population']) is not int or not core['population_core_estimate'] <= f['population'] <= region['total_habitual_residents']:
            errors.append('Invalid functional subset population')
    known = set(ids) | {c['id'] for c in a['cities']}
    node_positions = {s['id']: [s['lon_deg'],s['lat_deg']] for s in d['settlements']}
    node_positions.update({c['id']: [c['lon_deg'],c['lat_deg']] for c in a['cities']})
    route_ids = [r['id'] for r in d['route_proposals']]
    if len(route_ids) != len(set(route_ids)):
        errors.append('Duplicate route ID')
    for route in d['route_proposals']:
        nodes = [route['from'], *route.get('via', []), route['to']]
        if any(x not in known for x in nodes):
            errors.append('Unknown route endpoint: '+route['id'])
            continue
        ps = route['waypoints_lon_lat']
        if len(ps) < 2 or not all(point(q) for q in ps):
            errors.append('Invalid route coordinates: '+route['id'])
            continue
        if route['mode'] not in ('marine_with_port_transfer', 'land_multimodal'):
            errors.append('Unknown route mode: '+route['id'])
            continue
        if route['mode'] == 'land_multimodal':
            indices = route.get('waypoint_indices_for_nodes', [])
            if len(indices) != len(nodes) or any(type(i) is not int or not 0 <= i < len(ps) for i in indices) or indices != sorted(set(indices)) or indices[0] != 0 or indices[-1] != len(ps)-1:
                errors.append('Invalid inland node/control bindings: '+route['id'])
            elif any(ps[i] != node_positions[node] for node,i in zip(nodes,indices)):
                errors.append('Inland node differs from bound control: '+route['id'])
        else:
            transfers = route.get('port_transfers', [])
            if route.get('via') or len(transfers) != 2 or [t.get('node_id') for t in transfers] != nodes or [t.get('waypoint_index') for t in transfers] != [0,len(ps)-1]:
                errors.append('Invalid marine port-transfer bindings: '+route['id'])
            else:
                for transfer in transfers:
                    limit=transfer.get('max_anchor_separation_km')
                    if not number(limit) or limit <= 0 or distance_km(node_positions[transfer['node_id']],ps[transfer['waypoint_index']],a['radius_km']) > limit:
                        errors.append('Marine port transfer exceeds declared anchor separation: '+route['id'])
        # Samples concern the coarse lon/lat-linear concept only, not navigation or exact coast clearance.
        for x, y in zip(ps, ps[1:]):
            for i in range(101):
                q = [x[k] + (y[k]-x[k])*i/100 for k in (0,1)]
                lands = [l['name'] for l in a['landmasses'] if contains(q,l['coordinates_lon_lat'])]
                if route['mode'] == 'marine_with_port_transfer' and lands:
                    errors.append('Marine concept crosses land: '+route['id']); break
                if route['mode'] == 'land_multimodal' and 'Morrow' not in lands:
                    errors.append('Inland concept leaves Morrow: '+route['id']); break
    return errors


def controls(d, a, g):
    cases = []
    def case(name, edit, expected):
        x = copy.deepcopy(d); edit(x); cases.append((name,x,expected))
    case('duplicate region',lambda x:x['regions'].append(copy.deepcopy(x['regions'][0])),'Regional coverage')
    case('unallocated population',lambda x:x['regions'][0]['allocation'].__setitem__('other_towns',0),'Regional population does not close')
    case('core population reassigned',lambda x:x['regions'][0]['allocation'].__setitem__('core_city',1),'Core city population differs')
    case('negative population',lambda x:x['regions'][0]['allocation'].__setitem__('other_towns',-1),'Invalid regional population')
    case('boolean population',lambda x:x['regions'][0]['allocation'].__setitem__('other_towns',True),'Invalid regional population')
    case('world estimate changed',lambda x:x.__setitem__('population_estimate',1),'Reference population')
    case('nonfinite uncertainty',lambda x:x.__setitem__('uncertainty_fraction',float('nan')),'Invalid uncertainty')
    case('duplicate settlement',lambda x:x['settlements'].append(copy.deepcopy(x['settlements'][0])),'Duplicate settlement ID')
    case('unknown region',lambda x:x['settlements'][0].__setitem__('region_id','REG-999'),'Invalid settlement allocation')
    case('wrong regional landmass',lambda x:x['settlements'][0].__setitem__('landmass','Morrow'),'Settlement landmass differs')
    case('secondary settlement counted as core',lambda x:x['settlements'][0].__setitem__('allocation_bucket','core_city'),'Secondary settlement duplicates')
    case('overallocated settlement',lambda x:x['settlements'][0].__setitem__('habitual_residents',100000000),'Named settlements exceed')
    case('nonfinite position',lambda x:x['settlements'][0].__setitem__('lon_deg',float('inf')),'Invalid settlement coordinate')
    case('land community in sea',lambda x:x['settlements'][0].update(lon_deg=-115,lat_deg=-8),'Land settlement outside')
    case('marine community on land',lambda x:x['settlements'][44].update(lon_deg=28,lat_deg=17),'Marine reference lies on land')
    case('mobile community in town bucket',lambda x:x['settlements'][44].__setitem__('allocation_bucket','other_towns'),'Mobile community counted outside')
    case('functional region exceeds parent',lambda x:x['functional_regions'][0].__setitem__('population',100000000),'Invalid functional subset')
    case('duplicate functional-region ID',lambda x:x['functional_regions'].append(copy.deepcopy(x['functional_regions'][0])),'Duplicate functional-region ID')
    case('duplicate route ID',lambda x:x['route_proposals'].append(copy.deepcopy(x['route_proposals'][0])),'Duplicate route ID')
    case('route endpoint absent',lambda x:x['route_proposals'][0].__setitem__('to','SET-999'),'Unknown route endpoint')
    case('inland route names wrong source',lambda x:x['route_proposals'][1].__setitem__('from','CITY-005'),'Inland node differs')
    case('inland route names wrong intermediate town',lambda x:x['route_proposals'][1].__setitem__('via',['SET-027']),'Inland node differs')
    case('inland binding omitted',lambda x:x['route_proposals'][1].__setitem__('waypoint_indices_for_nodes',[]),'Invalid inland node/control')
    case('marine route names wrong port',lambda x:x['route_proposals'][0].__setitem__('from','CITY-005'),'Invalid marine port-transfer')
    case('marine transfer allowance too short',lambda x:x['route_proposals'][0]['port_transfers'][0].__setitem__('max_anchor_separation_km',1),'Marine port transfer exceeds')
    case('marine transfer nonfinite limit',lambda x:x['route_proposals'][0]['port_transfers'][0].__setitem__('max_anchor_separation_km',float('inf')),'Marine port transfer exceeds')
    case('marine route through Kesh',lambda x:x['route_proposals'][0]['waypoints_lon_lat'].__setitem__(1,[28,17]),'Marine concept crosses land')
    case('inland route across ocean',lambda x:x['route_proposals'][1]['waypoints_lon_lat'].__setitem__(1,[60,0]),'Inland concept leaves Morrow')
    for name,x,expected in cases:
        if not any(expected in e for e in validate(x,a,g)):
            raise ValueError('Control not rejected: '+name)
    return [name for name,_,_ in cases]


def render(d,a,control_names):
    esc=html.escape
    def xy(p): return (80+(p[0]+180)*4.4,120+(90-p[1])*4.4)
    svg=['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1800 1660">', '<style>text{font-family:Arial,sans-serif;fill:#213c47;font-size:16px}.small{font-size:13px}.heading{font-size:25px;font-weight:bold}</style>','<rect width="1800" height="1660" fill="#f3efdf"/>']
    def text(x,y,s,attrs=''):svg.append(f'<text x="{x}" y="{y}" {attrs}>{esc(str(s))}</text>')
    text(60,48,'NACRE / HOMES BETWEEN THE CITIES','class="heading"')
    text(60,80,'48 authored secondary settlements and communities · 12 major-city anchors · 742 AL census allocation')
    text(60,104,'Equirectangular concept map; angular scale distorts distance. Marine symbols are reference anchors, not engineered settlement footprints.','class="small"')
    svg.append('<rect x="80" y="120" width="1584" height="792" fill="#c6dde3"/>')
    for lon in range(-180,181,30):
        x,_=xy([lon,0]);svg.append(f'<path d="M{x} 120V912" stroke="#a7c1c6" stroke-width=".7"/>');text(x,933,lon,'class="small" text-anchor="middle"')
    for lat in range(-60,91,30):
        _,y=xy([0,lat]);svg.append(f'<path d="M80 {y}H1664" stroke="#a7c1c6" stroke-width=".7"/>');text(71,y+4,lat,'class="small" text-anchor="end"')
    for land in a['landmasses']:
        pts=' '.join(f'{x},{y}' for x,y in map(xy,land['coordinates_lon_lat']))
        svg.append(f'<polygon points="{pts}" fill="#d9dac0" stroke="#8d9e8b"/>')
    for route in d['route_proposals']:
        pts=' '.join(f'{x},{y}' for x,y in map(xy,route['waypoints_lon_lat']))
        svg.append(f'<polyline data-route="{route["id"]}" points="{pts}" fill="none" stroke="#b36643" stroke-width="3" stroke-dasharray="6 5"/>')
    occupied=[]
    for c in a['cities']:
        x,y=xy([c['lon_deg'],c['lat_deg']]);svg.append(f'<path data-city="{c["id"]}" d="M{x} {y-5}l5 5l-5 5l-5-5Z" fill="#875039"><title>{esc(c["name"])}</title></path>');occupied.append((x-6,y-6,x+6,y+6))
    for i,s in enumerate(d['settlements']):
        x,y=xy([s['lon_deg'],s['lat_deg']]);marine=s['position_kind']!='fixed_land_proposal'
        svg.append(f'<circle data-settlement="{s["id"]}" cx="{x}" cy="{y}" r="4" fill="{"#276e91" if marine else "#294d49"}"><title>{esc(s["name"])}</title></circle>')
        found=None
        for radius in (10,28,46,64,82):
            for dx,dy in [(radius,-12),(-radius-27,-12),(radius,9),(-radius-27,9),(0,-radius-20),(0,radius)]:
                box=(x+dx,y+dy,x+dx+27,y+dy+22)
                if box[0]<80 or box[1]<120 or box[2]>1664 or box[3]>912:continue
                if any(box[0]<b[2]+2 and box[2]>b[0]-2 and box[1]<b[3]+2 and box[3]>b[1]-2 for b in occupied):continue
                found=box;break
            if found:break
        if found is None:raise ValueError('Cannot place map label: '+s['name'])
        occupied.append(found);u,v=found[:2]
        svg.append(f'<path d="M{x} {y}L{u+13.5} {v+11}" stroke="#73857d" stroke-width=".8"/>')
        svg.append(f'<rect class="map-label-box" x="{u}" y="{v}" width="27" height="22" rx="3" fill="#fff9e9" stroke="#83988e"/>')
        text(u+13.5,v+16,f'{i+1:02}','class="small" text-anchor="middle"')
    text(60,978,'NUMBERED COMMUNITIES','class="heading"')
    text(60,1005,'Rust diamonds: existing major cities. Blue dots: marine communities. Dashed rust line: proposed Sola–Veyra–Aven connection.','class="small"')
    for i,s in enumerate(d['settlements']):
        x=60+(i//12)*440;y=1047+(i%12)*40
        text(x,y,f'{i+1:02}  {s["name"]}')
        text(x,y+17,f'{s["habitual_residents"]:,} habitual residents · {s["region_id"]}','class="small"')
    text(60,1575,'34.8 million habitual residents allocated once across 24 regional accounts. Named places are subsets of those accounts.','class="small"')
    text(60,1598,'Concept placement and bookkeeping checked; settlement footprints, source capacity, terrain and operating transport remain to be developed.','class="small"')
    text(60,1621,'Source: nacre_population_and_settlements.json and parent atlas. This plate is not a playable world or a population simulation.','class="small"')
    svg.append('</svg>');(OUT/'nacre_settlement_network.svg').write_text('\n'.join(svg)+'\n',encoding='utf-8')
    totals={k:sum(r['allocation'][k] for r in d['regions']) for k in d['buckets']}
    lines=['# Population and settlement arithmetic','', 'Authored allocations and concept placement only. No productive capacity, navigability, terrain, local population footprint or simulation is established.','',f'World total: **{d["population_estimate"]:,}** habitual residents; reference uncertainty ±{d["uncertainty_fraction"]*100:g} percent. Regional allocations are not independently measured confidence intervals.','', '| Exclusive population bucket | Residents | Share |','|---|---:|---:|']
    for k,n in totals.items():lines.append(f'| {k} | {n:,} | {n/d["population_estimate"]*100:.3f}% |')
    lines+=['','## Regional accounts','','| Region | Total | Core | Associated belt | Other towns | Dispersed/station | Mobile | Named subset |','|---|---:|---:|---:|---:|---:|---:|---:|']
    for r in d['regions']:
        row=[r['region_id']+' '+r['region_name'],f'{r["total_habitual_residents"]:,}',*[f'{r["allocation"][k]:,}' for k in d['buckets']],f'{sum(s["habitual_residents"] for s in d["settlements"] if s["region_id"]==r["region_id"]):,}']
        lines.append('| '+' | '.join(row)+' |')
    lines+=['','## Route geometry','','Distances below are sums of short great-circle distances between authored controls. Surface sampling uses the drawn lon/lat-linear concept segments. Neither is a surveyed route. Transfers at both ends of the marine chain are additional.','']
    for route in d['route_proposals']:
        ps=route['waypoints_lon_lat'];km=sum(distance_km(x,y,a['radius_km']) for x,y in zip(ps,ps[1:]))
        lines.append(f'- **{route["name"]}: {km:,.3f} km** control-chain length. {route["status"]}.')
        if route['mode']=='marine_with_port_transfer':
            positions={s['id']:[s['lon_deg'],s['lat_deg']] for s in d['settlements']}
            positions.update({c['id']:[c['lon_deg'],c['lat_deg']] for c in a['cities']})
            for t in route['port_transfers']:
                separation=distance_km(positions[t['node_id']],ps[t['waypoint_index']],a['radius_km'])
                lines.append(f'  - {t["node_id"]} anchor separation: {separation:.3f} km within declared {t["max_anchor_separation_km"]:g} km screen. This is a direct coordinate separation, not designed transfer distance or time.')
    lines+=['','## Rejected controls','', '; '.join(control_names)+'.','', 'Every named land anchor lies within its assigned concept landmass and outside mapped lake polygons. Marine references lie outside those land/lake polygons. Six coastal candidates also pass their 40 km coarse-shore screens. The map contains 48 new named community anchors; this is not a complete register of all settlements or 48 completed city plans.','']
    (OUT/'POPULATION_AND_SETTLEMENT_CHECKS.md').write_text('\n'.join(lines),encoding='utf-8')
    return totals


if __name__ == '__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
    d=json.loads((OUT/'nacre_population_and_settlements.json').read_text(encoding='utf-8'))
    a=json.loads((WORLD/'Atlas/nacre_atlas.json').read_text(encoding='utf-8'))
    g=json.loads((WORLD/'Atlas/nacre_geography.json').read_text(encoding='utf-8'))
    errors=validate(d,a,g)
    if errors:raise SystemExit('\n'.join(errors))
    names=controls(d,a,g)
    if not args.check:render(d,a,names)
    print(json.dumps({'passed':True,'regions':len(d['regions']),'settlements':len(d['settlements']),'population':d['population_estimate'],'negative_controls':len(names),'scope':'Authored allocations and concept placement only'}))
