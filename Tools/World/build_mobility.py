"""Travel lower bounds and a two-body orbital example, not actual navigation."""
from __future__ import annotations
import argparse
import copy
import json
import math
from pathlib import Path
from build_atlas import distance_km

ROOT=Path(__file__).resolve().parents[2]
WORLD=ROOT/'Docs/World'
OUT=WORLD/'Routes'


def finite(x):
    return isinstance(x,(int,float)) and not isinstance(x,bool) and math.isfinite(x)


def validate(d,a):
    errors=[]
    if d['radius_km']!=a['radius_km'] or d['surface_gravity_m_s2']!=8.629852:errors.append('Reference sphere/gravity mismatch')
    if d['civil_turn_hours']!=24 or d['solar_cycle_hours']!=136.755:errors.append('Civil and solar units mismatch')
    if len(d['route_ids'])!=len(set(d['route_ids'])) or set(d['route_ids'])!={r['id'] for r in a['routes']}:errors.append('Route identities mismatch')
    modes={m['id']:m for m in d['modes']};cities={c['id'] for c in a['cities']};rivers={r['id']:r for r in a['rivers']}
    if len(modes)!=len(d['modes']) or any(not finite(m['km_per_hour']) or m['km_per_hour']<=0 for m in modes.values()):errors.append('Invalid transport speed or identity')
    if len({c['id'] for c in d['cases']})!=len(d['cases']):errors.append('Duplicate timing case')
    for c in d['cases']:
        if c['mode_id'] not in modes or not finite(c['fixed_hours']) or c['fixed_hours']<0:errors.append('Invalid timing case: '+c['id'])
        g=c['geometry']
        if g['kind']=='city_pair_lower_bound':
            if g['from'] not in cities or g['to'] not in cities or g['from']==g['to']:errors.append('Invalid city pair: '+c['id'])
        elif g['kind']=='declared_local_length':
            if not finite(g['km']) or g['km']<=0:errors.append('Invalid local length: '+c['id'])
        elif g['kind']=='river_controls_lower_bound':
            r=rivers.get(g['river_id']);i,j=g['from_index'],g['to_index']
            if r is None or type(i) is not int or type(j) is not int or not 0<=i<j<len(r['points_lon_lat_elevation_m']):errors.append('Invalid downstream river controls: '+c['id'])
        else:errors.append('Unknown geometry kind: '+c['id'])
    o=d['orbit_example']
    if any(not finite(o[k]) or o[k]<=0 for k in ('height_km','mass_kg_for_energy_comparison')):errors.append('Invalid orbit example')
    if not errors:
        try:
            times=[length(c,a)/modes[c['mode_id']]['km_per_hour']+c['fixed_hours'] for c in d['cases']]
            if any(not finite(t) or t<=0 for t in times):errors.append('Nonfinite or unrepresentable derived travel time')
            # Also cover every comparison route, even if a case does not use AIR.
            cities_by_id={c['id']:c for c in a['cities']}
            for route in a['routes']:
                u,v=cities_by_id[route['from']],cities_by_id[route['to']]
                t=distance_km([u['lon_deg'],u['lat_deg']],[v['lon_deg'],v['lat_deg']],a['radius_km'])/modes['AIR']['km_per_hour']
                if not finite(t) or t<=0:errors.append('Nonfinite or unrepresentable derived travel time');break
            if any(not finite(v) or v<=0 for v in orbit(d).values()):errors.append('Unrepresentable derived orbital quantity')
        except (ArithmeticError,ValueError):errors.append('Unrepresentable derived orbital or travel quantity')
    return errors


def length(c,a):
    g=c['geometry'];cities={c['id']:c for c in a['cities']}
    if g['kind']=='declared_local_length':return g['km']
    if g['kind']=='city_pair_lower_bound':
        u,v=cities[g['from']],cities[g['to']]
        return distance_km([u['lon_deg'],u['lat_deg']],[v['lon_deg'],v['lat_deg']],a['radius_km'])
    r=next(r for r in a['rivers'] if r['id']==g['river_id'])['points_lon_lat_elevation_m']
    return sum(distance_km(u,v,a['radius_km']) for u,v in zip(r[g['from_index']:g['to_index']],r[g['from_index']+1:g['to_index']+1]))


def orbit(d):
    radius=d['radius_km']*1000;alt=d['orbit_example']['height_km']*1000
    mu=d['surface_gravity_m_s2']*radius**2;r=radius+alt
    speed=math.sqrt(mu/r);period=2*math.pi*r/speed
    energy=mu*(1/radius-1/r)+speed**2/2
    return {'speed_m_s':speed,'period_s':period,'mechanical_change_MJ_per_kg':energy/1e6,'mechanical_change_MWh_for_example_mass':energy*d['orbit_example']['mass_kg_for_energy_comparison']/3.6e9}


def controls(d,a):
    tests=[]
    def add(name,fn,expected):
        x=copy.deepcopy(d);fn(x);tests.append((name,x,expected))
    add('zero speed',lambda x:x['modes'][0].__setitem__('km_per_hour',0),'Invalid transport speed')
    add('nonfinite speed',lambda x:x['modes'][0].__setitem__('km_per_hour',float('nan')),'Invalid transport speed')
    add('negative handling',lambda x:x['cases'][0].__setitem__('fixed_hours',-1),'Invalid timing case')
    add('unknown city',lambda x:x['cases'][1]['geometry'].__setitem__('to','missing'),'Invalid city pair')
    add('reversed river controls',lambda x:x['cases'][2]['geometry'].__setitem__('from_index',4),'Invalid downstream river')
    add('noninteger river index',lambda x:x['cases'][2]['geometry'].__setitem__('from_index',3.1),'Invalid downstream river')
    add('natural day used as civil turn',lambda x:x.__setitem__('civil_turn_hours',136.755),'Civil and solar')
    add('missing route',lambda x:x['route_ids'].pop(),'Route identities')
    add('nonfinite orbital height',lambda x:x['orbit_example'].__setitem__('height_km',float('inf')),'Invalid orbit example')
    add('finite speed yields infinite duration',lambda x:next(m for m in x['modes'] if m['id']=='AIR').__setitem__('km_per_hour',1e-320),'derived travel time')
    add('finite orbital height overflows meters',lambda x:x['orbit_example'].__setitem__('height_km',1e308),'Unrepresentable derived orbital')
    for name,x,expected in tests:
        if not any(expected in e for e in validate(x,a)):raise ValueError('Negative control not rejected: '+name)
    seam=distance_km([179,0],[-179,0],a['radius_km'])
    if not math.isclose(seam,math.radians(2)*a['radius_km'],rel_tol=1e-12):raise ValueError('Longitude seam distance failed')
    if not math.isclose(length(d['cases'][0],a)/4.5,4/9):raise ValueError('Kilometer/hour conversion failed')
    # Kepler circular relation is independently recomposed from the reported period.
    o=orbit(d);r=(d['radius_km']+d['orbit_example']['height_km'])*1000;mu=d['surface_gravity_m_s2']*(d['radius_km']*1000)**2
    if not math.isclose(o['period_s']**2,4*math.pi**2*r**3/mu,rel_tol=1e-12):raise ValueError('Circular period relation failed')
    return [n for n,_,_ in tests]


def render(d,a,tests):
    cities={c['id']:c for c in a['cities']};modes={m['id']:m for m in d['modes']}
    lines=['# Travel arithmetic — authoring study 0.1','','These are geometric lower bounds or explicitly assumed local lengths. They are not timetables, navigable paths, vehicle performance or a promise of nonstop service. Speeds are illustrative groundspeed/still-water comparisons; wind, current, stops and actual courses remain unresolved.','','## Existing city connections','',f'| ID | Connection | Spherical separation km | At illustrative {modes["AIR"]["km_per_hour"]:g} km/h, hours |','|---|---|---:|---:|']
    for r in a['routes']:
        u,v=cities[r['from']],cities[r['to']];km=distance_km([u['lon_deg'],u['lat_deg']],[v['lon_deg'],v['lat_deg']],a['radius_km'])
        lines.append(f'| {r["id"]} | {u["name"]} → {v["name"]} | {km:,.2f} | {km/modes["AIR"]["km_per_hour"]:.3f} |')
    lines+=['','## Timing cases','','Fixed handling/access is an authored assumption, not a surveyed minimum. Dividing by 24 gives civil turns; dividing by136.755 would answer a different question.','','| Case | Distance basis | km | Mode km/h | Moving h | Fixed h | Total h | Civil turns |','|---|---|---:|---:|---:|---:|---:|---:|']
    for c in d['cases']:
        km=length(c,a);speed=modes[c['mode_id']]['km_per_hour'];moving=km/speed;total=moving+c['fixed_hours']
        lines.append(f'| {c["id"]} {c["name"]} | {c["geometry"]["kind"]} | {km:,.2f} | {speed:g} | {moving:.3f} | {c["fixed_hours"]:g} | {total:.3f} | {total/24:.3f} |')
    lines+=['']+[f'- **{c["id"]}:** {c["qualification"]}' for c in d['cases']]
    o=orbit(d);ex=d['orbit_example']
    lines+=['','## Hypothetical circular orbit','','This is not Mirror Ring\'s adopted orbit. Two-body reference only, at '+str(ex['height_km'])+' km altitude:','',f'- Circular speed: **{o["speed_m_s"]/1000:.6f} km/s**.',f'- Period: **{o["period_s"]/60:.6f} minutes**.',f'- Ideal mechanical energy change from a stationary surface reference: **{o["mechanical_change_MJ_per_kg"]:.6f} MJ/kg**.',f'- For a constant comparison mass of **{ex["mass_kg_for_energy_comparison"]:,} kg**: **{o["mechanical_change_MWh_for_example_mass"]:.6f} MWh** mechanical change.','', 'Surface rotation, atmospheric drag, gravity losses during powered ascent, parent perturbations, vehicle mass changes, propellant, thermal rejection and rendezvous are not included. Electrical input or onboard fuel demand cannot be inferred directly from this ideal mechanical value.','', '`mu = g R²; r = R + h; v = sqrt(mu/r); period = 2πr/v; delta specific mechanical energy = mu(1/R − 1/r) + v²/2`.','', '## Check scope','', 'Rejected controls: '+'; '.join(tests)+'. Positive arithmetic controls cover the longitude seam, kilometer/hour conversion and an independently recomposed circular-period relation. Existing route IDs and physical/civil references agree. No route, ship, atmosphere or playable travel has been validated.','']
    (OUT/'TRAVEL_ARITHMETIC.md').write_text('\n'.join(lines),encoding='utf8')
    return o


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
    d=json.loads((OUT/'nacre_mobility.json').read_text());a=json.loads((WORLD/'Atlas/nacre_atlas.json').read_text())
    errors=validate(d,a)
    if errors:raise SystemExit('\n'.join(errors))
    tests=controls(d,a)
    if not args.check:render(d,a,tests)
    print(json.dumps({'passed':True,'routes':len(d['route_ids']),'cases':len(d['cases']),'negative_controls':tests,'orbit_example':orbit(d),'scope':'Declared arithmetic only; not navigation or vehicle validation'}))
