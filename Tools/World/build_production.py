"""Size a declared subset of food obligations; arithmetic is not agricultural evidence."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORLD = ROOT / 'Docs/World'
OUT = WORLD / 'Resources'


def finite(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value)


def calculate(d, exchange):
    def require(condition, message):
        if not condition:
            raise ValueError(message)
    def positive(value, label):
        require(finite(value) and value > 0, label)
    def fraction(value, label):
        require(finite(value) and 0 < value <= 1, label)
    for key in ('civil_year_turns', 'gravity_m_s2', 'water_density_kg_m3'):
        positive(d[key], key)
    require(math.isclose(d['civil_year_turns'], 325+22/115, abs_tol=1e-10), 'civil year differs from calendar')
    require(math.isclose(d['gravity_m_s2'], 8.629852, abs_tol=1e-9), 'gravity differs from canon')
    fraction(d['productive_fraction_of_farm_land'], 'productive land fraction')
    for crop in d['crop_cases'].values():
        positive(crop['gross_harvest_t_per_canopy_ha_per_civil_year'], 'crop yield')
        fraction(crop['retained_fraction'], 'retained harvest')
        positive(crop['annual_crop_et_mm'], 'crop evapotranspiration')
    culture=d['crop_cases']['cultured_products']
    for key in ('product_dry_fraction', 'dry_biomass_per_dry_substrate', 'fermentable_substrate_per_retained_harvest'):
        fraction(culture[key], key)
    origins={o['delivery_id']:o for o in d['origins']}
    require(len(origins)==len(d['origins']), 'duplicate origin')
    for o in list(origins.values())+[d['urban_fresh']]:
        for key in ('effective_rain_mm_per_civil_year','extra_leaching_mm_per_civil_year','pump_head_m'):
            require(finite(o[key]) and o[key]>=0, key)
        fraction(o['irrigation_delivery_fraction'], 'irrigation delivery fraction')
        fraction(o['pump_efficiency'], 'pump efficiency')
    recovery=d['urban_fresh']['condensate_recovery_fraction']
    require(finite(recovery) and 0<=recovery<1, 'condensate recovery fraction')
    source=exchange['orison_food']; deliveries={s['id']:s for s in source['deliveries']}
    require(len(deliveries)==len(source['deliveries']), 'duplicate food delivery')
    require(set(origins)<=set(deliveries), 'unknown source delivery')
    require(set(source['categories'])==set(d['crop_cases'])|set(d['unmodeled_categories']), 'category coverage differs')
    require(not set(d['crop_cases']) & set(d['unmodeled_categories']), 'category modeled twice')
    for s in deliveries.values():
        require(set(s['category_tonnes_per_turn'])==set(source['categories']), 'missing food category')
        require(all(finite(v) and v>=0 for v in s['category_tonnes_per_turn'].values()), 'invalid source food')
        require(finite(s['row_total_tonnes_per_turn']) and finite(sum(s['category_tonnes_per_turn'].values())), 'nonfinite source row total')
        require(math.isclose(sum(s['category_tonnes_per_turn'].values()),s['row_total_tonnes_per_turn']), 'source row sum')
    for cat in source['categories']:
        require(finite(source['category_targets'][cat]) and finite(sum(s['category_tonnes_per_turn'][cat] for s in deliveries.values())), 'nonfinite source category total')
        require(math.isclose(sum(s['category_tonnes_per_turn'][cat] for s in deliveries.values()),source['category_targets'][cat]), 'source category sum')
    require(finite(source['baseline_total']) and finite(sum(source['category_targets'].values())), 'nonfinite source grand total')
    require(math.isclose(sum(source['category_targets'].values()),source['baseline_total']), 'source grand total')
    urban=source['urban_fresh_subset']; urban_id=urban['included_in_delivery_id']; urban_t=urban['tonnes_per_civil_turn']
    require(urban_id==d['urban_fresh']['delivery_id'] and urban_id in origins, 'urban origin mismatch')
    require(finite(urban_t) and 0<=urban_t<=deliveries[urban_id]['category_tonnes_per_turn']['introduced_fresh_produce'], 'urban subset exceeds allocation')
    rows=[]
    def append(supply_id, cat, tonnes, origin, urban_case=False):
        crop=d['crop_cases'][cat]; retained=tonnes
        if cat=='cultured_products':
            retained=tonnes*crop['product_dry_fraction']/crop['dry_biomass_per_dry_substrate']/crop['fermentable_substrate_per_retained_harvest']
        gross=retained/crop['retained_fraction']
        area=gross*d['civil_year_turns']/crop['gross_harvest_t_per_canopy_ha_per_civil_year']
        et=area*crop['annual_crop_et_mm']*10
        deficit=max(0,crop['annual_crop_et_mm']-origin['effective_rain_mm_per_civil_year'])
        recovered=et*recovery if urban_case else 0
        # Urban closed-room case is separate from rain-fed outdoor fields; no rainfall is credited.
        require(not urban_case or origin['effective_rain_mm_per_civil_year']==0, 'urban rain and condensate double credit')
        irrigation=max(0,area*deficit*10-recovered)+area*origin['extra_leaching_mm_per_civil_year']*10
        source_water=irrigation/origin['irrigation_delivery_fraction']
        pump=source_water*d['water_density_kg_m3']*d['gravity_m_s2']*origin['pump_head_m']/origin['pump_efficiency']/3.6e9
        row={'id':supply_id+'-'+('urban' if urban_case else cat),'source_id':supply_id,'origin':origin['label'],'category':cat,'delivered_t_per_turn':tonnes,'gross_crop_t_per_turn':gross,'canopy_ha':area,'farm_land_ha':area if urban_case else area/d['productive_fraction_of_farm_land'],'crop_et_m3_per_year':et,'recovered_condensate_m3_per_year':recovered,'irrigation_source_m3_per_year':source_water,'pump_mwh_per_year':pump,'urban_case':urban_case}
        require(all(finite(v) and v>=0 for v in row.values() if isinstance(v,(int,float)) and not isinstance(v,bool)), 'nonfinite derived quantity')
        rows.append(row)
    for supply_id,s in deliveries.items():
        for cat in d['crop_cases']:
            tonnes=s['category_tonnes_per_turn'][cat]
            if tonnes==0: continue
            require(supply_id in origins, 'modeled demand has no origin assumptions')
            if supply_id==urban_id and cat=='introduced_fresh_produce':
                if tonnes-urban_t: append(supply_id,cat,tonnes-urban_t,origins[supply_id])
                if urban_t: append(supply_id,cat,urban_t,d['urban_fresh'],True)
            else: append(supply_id,cat,tonnes,origins[supply_id])
    require(len({r['id'] for r in rows})==len(rows), 'duplicate derived allocation')
    totals={k:sum(r[k] for r in rows) for k in ('delivered_t_per_turn','gross_crop_t_per_turn','canopy_ha','farm_land_ha','crop_et_m3_per_year','recovered_condensate_m3_per_year','irrigation_source_m3_per_year','pump_mwh_per_year')}
    missing=sum(source['category_targets'][cat] for cat in d['unmodeled_categories'])
    require(finite(missing), 'nonfinite excluded food aggregate')
    require(math.isclose(totals['delivered_t_per_turn']+missing,source['baseline_total']), 'food lost or double counted')
    light=d['lighting_case']
    for key in ('canopy_ppfd_umol_m2_s','effective_canopy_efficacy_umol_per_joule'):
        positive(light[key], key)
    require(finite(light['scheduled_light_hours_per_turn']) and 0<light['scheduled_light_hours_per_turn']<=24, 'photoperiod')
    fraction(light['scheduled_light_in_dark_fraction'], 'dark overlap')
    require(type(light['stack_layers']) is int and light['stack_layers']>=1, 'stack layers')
    power_density=light['canopy_ppfd_umol_m2_s']/light['effective_canopy_efficacy_umol_per_joule']
    all_peak=totals['canopy_ha']*10000*power_density/1e6
    urban_area=sum(r['canopy_ha'] for r in rows if r['urban_case'])
    urban_peak=urban_area*10000*power_density/1e6
    duty=light['scheduled_light_hours_per_turn']/24
    frac=light['scheduled_light_in_dark_fraction'];layers=light['stack_layers']
    lighting={'power_density_w_m2':power_density,'all_canopy_peak_mw':all_peak,'all_daylit_average_mw':all_peak*duty*frac,'urban_canopy_ha':urban_area,'urban_daylit_average_mw':urban_peak*duty*frac,'urban_stacked_average_mw':urban_peak*duty*((layers-1+frac)/layers),'urban_stack_footprint_ha':urban_area/layers}
    for key in ('harvest_fraction_after_shock','regional_irrigation_supply_fraction'):
        fraction(d['stress_cases'][key],key)
    shock=d['stress_cases']['harvest_fraction_after_shock']
    stress={'delivered_after_harvest_shock_t':totals['delivered_t_per_turn']*shock,'modeled_shortfall_t':totals['delivered_t_per_turn']*(1-shock),'replacement_area_multiplier':1/shock,'irrigation_source_shortfall_m3_per_year':totals['irrigation_source_m3_per_year']*(1-d['stress_cases']['regional_irrigation_supply_fraction'])}
    require(all(finite(v) for section in (totals,lighting,stress) for v in section.values()), 'nonfinite derived aggregate')
    return {'rows':rows,'totals':totals,'lighting':lighting,'stress':stress,'unmodeled_food_t_per_turn':missing}


def controls(d, e):
    cases=[]
    def case(name,fn,needle):
        x,y=copy.deepcopy(d),copy.deepcopy(e);fn(x,y);cases.append((name,x,y,needle))
    case('zero yield',lambda x,y:x['crop_cases']['introduced_dry_staples'].__setitem__('gross_harvest_t_per_canopy_ha_per_civil_year',0),'crop yield')
    case('nonfinite yield',lambda x,y:x['crop_cases']['introduced_dry_staples'].__setitem__('gross_harvest_t_per_canopy_ha_per_civil_year',float('nan')),'crop yield')
    case('overflow from finite yield',lambda x,y:x['crop_cases']['introduced_dry_staples'].__setitem__('gross_harvest_t_per_canopy_ha_per_civil_year',1e-310),'nonfinite derived')
    case('loss-free extra harvest',lambda x,y:x['crop_cases']['introduced_fresh_produce'].__setitem__('retained_fraction',1.1),'retained harvest')
    case('zero pump efficiency',lambda x,y:x['origins'][0].__setitem__('pump_efficiency',0),'pump efficiency')
    case('negative effective rain',lambda x,y:x['origins'][0].__setitem__('effective_rain_mm_per_civil_year',-1),'effective_rain')
    case('duplicate supply origin',lambda x,y:x['origins'].append(copy.deepcopy(x['origins'][0])),'duplicate origin')
    case('unknown supply origin',lambda x,y:x['origins'][0].__setitem__('delivery_id','missing'),'unknown source')
    case('duplicate source row',lambda x,y:y['orison_food']['deliveries'].append(copy.deepcopy(y['orison_food']['deliveries'][0])),'duplicate food')
    case('urban allocation counted twice',lambda x,y:y['orison_food']['urban_fresh_subset'].__setitem__('tonnes_per_civil_turn',100),'urban subset')
    case('omitted food category',lambda x,y:x['unmodeled_categories'].pop(),'category coverage')
    case('modeled and excluded category',lambda x,y:x['unmodeled_categories'].append('cultured_products'),'modeled twice')
    case('wrong gravity',lambda x,y:x.__setitem__('gravity_m_s2',9.81),'gravity differs')
    case('wrong year length',lambda x,y:x.__setitem__('civil_year_turns',365.25),'calendar')
    case('zero biomass yield',lambda x,y:x['crop_cases']['cultured_products'].__setitem__('dry_biomass_per_dry_substrate',0),'dry_biomass')
    case('longer than daily photoperiod',lambda x,y:x['lighting_case'].__setitem__('scheduled_light_hours_per_turn',25),'photoperiod')
    case('impossible light efficacy',lambda x,y:x['lighting_case'].__setitem__('effective_canopy_efficacy_umol_per_joule',0),'effective_canopy_efficacy')
    case('full condensate recovery',lambda x,y:x['urban_fresh'].__setitem__('condensate_recovery_fraction',1),'condensate recovery')
    case('rain credited in closed room',lambda x,y:x['urban_fresh'].__setitem__('effective_rain_mm_per_civil_year',400),'double credit')
    def overflow_excluded(x,y):
        source=y['orison_food']
        for row in source['deliveries']:
            row['category_tonnes_per_turn']['qualified_native_fractions']=1e308
            row['row_total_tonnes_per_turn']=sum(row['category_tonnes_per_turn'].values())
        for cat in source['categories']:
            source['category_targets'][cat]=sum(row['category_tonnes_per_turn'][cat] for row in source['deliveries'])
        source['baseline_total']=sum(source['category_targets'].values())
    case('finite excluded food overflows total',overflow_excluded,'nonfinite source category')
    for name,x,y,needle in cases:
        try: calculate(x,y)
        except ValueError as error:
            if needle not in str(error): raise ValueError(name+': wrong rejection '+str(error)) from error
        else: raise ValueError('Control not rejected: '+name)
    # Reordering input is irrelevant; higher rain cannot increase calculated irrigation.
    baseline=calculate(d,e);x=copy.deepcopy(d);x['origins'].reverse()
    assert calculate(x,e)==baseline
    x=copy.deepcopy(d)
    for o in x['origins']:o['effective_rain_mm_per_civil_year']+=100
    assert calculate(x,e)['totals']['irrigation_source_m3_per_year']<=baseline['totals']['irrigation_source_m3_per_year']
    x=copy.deepcopy(d);x['lighting_case']['stack_layers']=1
    one=calculate(x,e)['lighting'];assert math.isclose(one['urban_stacked_average_mw'],one['urban_daylit_average_mw'])
    x['stress_cases'].update(harvest_fraction_after_shock=.8,regional_irrigation_supply_fraction=.9)
    alternate=calculate(x,e);report,_=render(x,alternate,[])
    assert 'uniform 20% harvest loss' in report and 'separate 10% irrigation-source cut' in report
    assert 'Using 1 layer' in report and '0 shaded layers' in report
    return [c[0] for c in cases]


def render(d,r,checks):
    t=r['totals'];l=r['lighting'];s=r['stress'];light=d['lighting_case'];stress=d['stress_cases']
    urban_t=sum(row['delivered_t_per_turn'] for row in r['rows'] if row['urban_case'])
    layers=light['stack_layers'];layer_word='layer' if layers==1 else 'layers';harvest_loss=100*(1-stress['harvest_fraction_after_shock']);water_loss=100*(1-stress['regional_irrigation_supply_fraction'])
    lines=['# Orison productive-landscape arithmetic','',d['scope']+'.','', f"**Sizing assumptions, not observed agricultural performance or a complete food-security model.** Output reads the existing food ledger; it does not alter its allocation or certify the unresolved {r['unmodeled_food_t_per_turn']:g} t portion.",'', '| Source / product | Delivered t/turn | Gross crop t/turn | Productive canopy ha | Source irrigation million m³/civil year | Pump MWh/year |','|---|---:|---:|---:|---:|---:|']
    for row in r['rows']:
        lines.append(f"| {row['origin']} / {row['category'].replace('_',' ')} | {row['delivered_t_per_turn']:,.2f} | {row['gross_crop_t_per_turn']:,.2f} | {row['canopy_ha']:,.2f} | {row['irrigation_source_m3_per_year']/1e6:,.3f} | {row['pump_mwh_per_year']:,.1f} |")
    lines+=['',f"Modeled final delivery **{t['delivered_t_per_turn']:,.1f} t/turn**; excluded native/preserved categories **{r['unmodeled_food_t_per_turn']:,.1f} t/turn**. Productive canopy **{t['canopy_ha']:,.2f} ha**; total reserved farm land plus unstacked urban canopy **{t['farm_land_ha']/100:,.2f} km²**. This is allocated to Orison alone, not all residents of supplying regions.", '', f"Crop evapotranspiration: **{t['crop_et_m3_per_year']/1e6:,.3f} million m³/year**. Recovered urban condensate: **{t['recovered_condensate_m3_per_year']/1e6:,.3f} million m³/year**. Irrigation source allowance after effective rain, stated recovery/leaching and delivery efficiency: **{t['irrigation_source_m3_per_year']/1e6:,.3f} million m³/year**, averaged **{t['irrigation_source_m3_per_year']/d['civil_year_turns']:,.0f} m³/turn**. These seasonal agricultural flows are separate from municipal drinking-water networks.", '', f"Hydraulic pumping only: **{t['pump_mwh_per_year']:,.1f} MWh/year**, **{t['pump_mwh_per_year']/(d['civil_year_turns']*24):,.3f} MW** mean. Excludes source treatment, drainage, refrigeration, lamps, air movement, humidity control and cultivation machinery.", '', f"The alternative artificial-light study requires **{l['power_density_w_m2']:,.3f} W/m²** at its declared PPFD/efficacy. Lighting all modeled productive canopy gives **{l['all_canopy_peak_mw']:,.1f} MW** coincident fixture load and **{l['all_daylit_average_mw']:,.1f} MW** annual-mean electric supplementation under the assumed {light['scheduled_light_in_dark_fraction']*100:g}% overlap. Those are a scale warning, not adopted regional power demand.", '',f"The {urban_t:g} t urban fresh subset occupies **{l['urban_canopy_ha']:,.2f} ha** productive canopy in the yield case. Fully daylit equivalent canopies require **{l['urban_daylit_average_mw']:,.2f} MW** mean lighting under the same schedule. Using {layers} {layer_word} gives **{l['urban_stack_footprint_ha']:,.2f} ha** theoretical crop footprint, and {layers-1} shaded layers yield mean lighting to **{l['urban_stacked_average_mw']:,.2f} MW**. Neither footprint includes aisles, plant services or structural setbacks.", '',f"A uniform {harvest_loss:g}% harvest loss leaves **{s['delivered_after_harvest_shock_t']:,.1f} t/turn** of this subset, a **{s['modeled_shortfall_t']:,.1f} t/turn** shortfall. Maintaining output at the reduced yield would require **{s['replacement_area_multiplier']:,.3f}×** productive area if all other assumptions held. Extra land cannot produce an instant replacement harvest. A separate {water_loss:g}% irrigation-source cut removes **{s['irrigation_source_shortfall_m3_per_year']/1e6:,.3f} million m³/year**; crop loss is deliberately not equated linearly to water loss.",'','## Checked scope','',f"{len(checks)} negative controls passed. Positive controls cover input-order independence, monotone irrigation response to extra effective rain, one-layer lighting equivalence and an alternative scenario rendered with its actual parameters. This checks arithmetic and obligations, not biological yields, pressure, treatment, nutrition, climate or a navigable supply route.",'']
    report='\n'.join(lines)
    svg=['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1280 1180" role="img" aria-labelledby="title desc">','<title id="title">Land behind Orison food allocations</title>',f'<desc id="desc">{len(r["rows"])} productive-canopy cases for {t["delivered_t_per_turn"]:g} tonnes per civil turn. Horizontal bars compare hectares; excluded food and assumptions are stated.</desc>','<rect width="1280" height="1180" fill="#f3efe2"/>','<style>text{font-family:Arial,sans-serif;fill:#233e47;font-size:16px}</style>']
    def text(x,y,v,extra=''):svg.append(f'<text x="{x}" y="{y}" {extra}>{html.escape(str(v))}</text>')
    text(55,52,'THE LAND BEHIND THE MEAL','style="font-size:28px;font-weight:bold"')
    text(55,84,f"Orison / {t['delivered_t_per_turn']:g} t per civil turn modeled · {r['unmodeled_food_t_per_turn']:g} t native/preserved demand outside study")
    text(55,111,'Original sizing assumptions · areas are productive canopy, not a map or proven yields','style="font-size:14px"')
    maxarea=max(row['canopy_ha'] for row in r['rows'])
    colors={'introduced_dry_staples':'#ae874b','introduced_fresh_produce':'#64846a','cultured_products':'#587f90'}
    for i,row in enumerate(r['rows']):
        y=152+i*58
        short={'introduced_dry_staples':'dry','introduced_fresh_produce':'fresh','cultured_products':'culture feed'}[row['category']]
        text(55,y,row['origin']+' / '+short,'style="font-size:14px"')
        width=row['canopy_ha']/maxarea*650
        svg.append(f'<rect data-allocation="{row["id"]}" x="440" y="{y-18}" width="{width:.3f}" height="22" fill="{colors[row["category"]]}"/>')
        text(1120,y,f'{row["canopy_ha"]:,.0f} ha','text-anchor="start" style="font-size:14px"')
    text(55,955,f'{t["canopy_ha"]/100:,.1f} km² productive canopy / {t["farm_land_ha"]/100:,.1f} km² with stated farm-land allowance','style="font-size:21px"')
    text(55,989,'Culture output consumes crop substrate. Urban fresh is a subset of Saffron delivery, not additional food.')
    text(55,1017,f'Lower-harvest case: {s["modeled_shortfall_t"]:,.0f} t/turn shortfall. Regional residents and other exports are additional.')
    text(55,1055,'Long natural nights require a crop strategy and a real source of energy.','style="font-size:17px;font-weight:bold"')
    text(55,1083,'Nacre staples use selected long-cycle lines; full artificial-light alternatives carry explicit power costs.')
    text(55,1131,'Authoritative assumptions: nacre_production.json · source allocation: Exchange/nacre_exchange.json','style="font-size:14px"')
    svg.append('</svg>')
    return report,'\n'.join(svg)+'\n'


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
    d=json.loads((OUT/'nacre_production.json').read_text(encoding='utf-8'))
    e=json.loads((WORLD/'Exchange/nacre_exchange.json').read_text(encoding='utf-8'))
    r=calculate(d,e);checks=controls(d,e);report,svg=render(d,r,checks)
    if not args.check:
        (OUT/'PRODUCTION_ARITHMETIC.md').write_text(report,encoding='utf-8')
        (OUT/'orison_productive_land.svg').write_text(svg,encoding='utf-8')
    print(json.dumps({'cases':len(r['rows']),'negative_controls':len(checks),'positive_controls':4,'totals':r['totals'],'lighting':r['lighting']},indent=2))


if __name__=='__main__':main()
