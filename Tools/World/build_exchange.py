"""Validate selected fictional dependencies and arithmetic; never certify production capacity."""
from __future__ import annotations

import argparse
import copy
import html
import json
import math
import textwrap
from pathlib import Path

from build_atlas import distance_km

ROOT = Path(__file__).resolve().parents[2]
WORLD = ROOT / "Docs/World"
OUT = WORLD / "Exchange"
CATEGORY_LABELS = {
    'introduced_dry_staples': 'Introduced dry staples',
    'introduced_fresh_produce': 'Introduced fresh produce',
    'cultured_products': 'Cultured products',
    'qualified_native_fractions': 'Qualified native fractions',
    'other_preserved': 'Other preserved products',
}


def nonnegative(value):
    try:
        return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value) and value >= 0
    except OverflowError:
        return False


def positive_integer(value):
    return type(value) is int and nonnegative(value) and value > 0


def check(d, atlas, orison):
    errors = []
    cities = {c['id']: c for c in atlas['cities']}
    regions = {r['id'] for r in atlas['regions']}
    for key in ('city_roles', 'selected_dependencies', 'event_cases'):
        ids = [r['id'] for r in d[key]]
        if len(ids) != len(set(ids)):
            errors.append(f'Duplicate IDs: {key}')
    if {r['id'] for r in d['city_roles']} != set(cities):
        errors.append('City role set disagrees with atlas')
    for r in d['city_roles']:
        if r['id'] in cities and r['name'] != cities[r['id']]['name']:
            errors.append(f'City role name mismatch: {r["id"]}')
        if r['id'] in cities and r['region_id'] != cities[r['id']]['region_id']:
            errors.append(f'City role region mismatch: {r["id"]}')
    for r in d['selected_dependencies']:
        if r['from_city_id'] not in cities or r['to_city_id'] not in cities or r['from_city_id'] == r['to_city_id']:
            errors.append(f'Invalid dependency endpoints: {r["id"]}')
    food = d['orison_food']
    if d['civil_turn_hours'] != 24 or food['unit'] != 'edible_product_tonnes_per_civil_turn':
        errors.append('Food ledger time/mass units differ from adopted civil basis')
    if food['city_id'] != orison['city_id'] or food['served_population'] != orison['population']['core_estimate']:
        errors.append('Food ledger population disagrees with Orison core')
    categories = food['categories']
    if len(categories) != len(set(categories)) or set(categories) != set(food['category_targets']):
        errors.append('Food category declarations disagree')
    ids = [r['id'] for r in food['deliveries']]
    if len(ids) != len(set(ids)):
        errors.append('Duplicate food origin ID')
    totals = {key: 0 for key in categories}
    for r in food['deliveries']:
        if not r['origin_region_ids'] or any(key not in regions for key in r['origin_region_ids']):
            errors.append(f'Unknown food origin region: {r["id"]}')
        values = r['category_tonnes_per_turn']
        if set(values) != set(categories) or not all(nonnegative(v) for v in values.values()) or not nonnegative(r['row_total_tonnes_per_turn']):
            errors.append(f'Invalid food row categories or quantities: {r["id"]}')
            continue
        if not math.isclose(sum(values.values()), r['row_total_tonnes_per_turn']):
            errors.append(f'Food row total mismatch: {r["id"]}')
        for key, value in values.items(): totals[key] += value
    if totals != food['category_targets'] or not math.isclose(sum(totals.values()), food['baseline_total']):
        errors.append('Food ledger category or overall totals disagree')
    low, high = orison['service_assumptions']['edible_food_product_tonnes_per_turn_range']
    if not low <= food['baseline_total'] <= high:
        errors.append('Food baseline outside Orison planning range')
    dry = food['category_targets']['introduced_dry_staples']
    if not nonnegative(dry) or dry <= 0 or not nonnegative(food['dry_reserve_tonnes']) or not nonnegative(food['nominal_dry_reserve_turns']):
        errors.append('Invalid dry reserve quantities')
    elif not math.isclose(food['dry_reserve_tonnes'] / dry, food['nominal_dry_reserve_turns']):
        errors.append('Dry reserve duration disagrees with demand')
    if dry != orison['service_assumptions']['dry_grain_pulse_tonnes_per_turn_illustrative'] or food['dry_reserve_tonnes'] != orison['service_assumptions']['dry_reserve_tonnes_illustrative']:
        errors.append('Dry food quantities disagree with city dossier')
    interrupted = next((r for r in food['deliveries'] if r['id'] == food['interruption_case']['interrupted_delivery_id']), None)
    if interrupted is None:
        errors.append('Interruption refers to missing food origin')
    else:
        deficit = interrupted['category_tonnes_per_turn']['introduced_dry_staples']
        if not nonnegative(deficit) or deficit <= 0:
            errors.append('Dry reserve interruption requires a positive finite dry deficit')
    urban = food['urban_fresh_subset']
    origin = next((r for r in food['deliveries'] if r['id'] == urban['included_in_delivery_id']), None)
    amount = urban['tonnes_per_civil_turn']
    target_low, target_high = orison['service_assumptions']['urban_share_of_fresh_edible_product_by_mass_target_range']
    fresh = food['category_targets']['introduced_fresh_produce']
    if not nonnegative(amount) or origin is None or amount > origin['category_tonnes_per_turn']['introduced_fresh_produce'] or not target_low*fresh <= amount <= target_high*fresh:
        errors.append('Urban fresh subset is absent, double-counted or outside city target')
    served_city = cities.get(food['city_id'])
    if origin is not None and (served_city is None or served_city['region_id'] not in origin['origin_region_ids']):
        errors.append('Urban fresh source excludes the served city producer region')
    household = d['household_example']
    if not positive_integer(household['people']) or not positive_integer(household['duration_civil_turns']):
        errors.append('Household people and duration must be positive finite integers')
    for r in household['income']:
        if not all(nonnegative(r[k]) for k in ('count', 'rate', 'total')) or not math.isclose(r['count'] * r['rate'], r['total']):
            errors.append(f'Household income arithmetic mismatch: {r["source"]}')
    if not all(nonnegative(r['amount']) for r in household['expenses']):
        errors.append('Household expense quantity invalid')
    elif sum(r['amount'] for r in household['expenses']) != household['total_expense']:
        errors.append('Household expense total mismatch')
    computed_income = sum(r['total'] for r in household['income'])
    computed_expense = sum(r['amount'] for r in household['expenses'])
    if not all(nonnegative(value) for value in (household['total_income'], household['total_expense'], computed_income, computed_expense)):
        errors.append('Household stated and computed totals must be finite nonnegative quantities')
    if computed_income != household['total_income']:
        errors.append('Household income total mismatch')
    if household['total_income'] != household['total_expense']:
        errors.append('Illustrative household no longer balances; revise surplus/deficit prose')
    return errors


def negative_controls(d, atlas, orison):
    cases = []
    x = copy.deepcopy(d); x['orison_food']['served_population'] = 1200000
    cases.append(('core/service population confusion', x, 'population disagrees'))
    x = copy.deepcopy(d); x['orison_food']['deliveries'][0]['category_tonnes_per_turn']['introduced_dry_staples'] = -1
    cases.append(('negative food quantity', x, 'Invalid food row'))
    x = copy.deepcopy(d); x['orison_food']['deliveries'][0]['row_total_tonnes_per_turn'] += 1
    cases.append(('row mismatch', x, 'row total mismatch'))
    x = copy.deepcopy(d); x['orison_food']['deliveries'][0]['origin_region_ids'] = ['REG-999']
    cases.append(('missing productive region', x, 'Unknown food origin'))
    x = copy.deepcopy(d); x['orison_food']['nominal_dry_reserve_turns'] = 365
    cases.append(('incorrect reserve interval', x, 'reserve duration'))
    x = copy.deepcopy(d); x['household_example']['income'][0]['rate'] = 25
    cases.append(('unreconciled pay change', x, 'income arithmetic'))
    x = copy.deepcopy(d); x['selected_dependencies'][0]['to_city_id'] = 'CITY-999'
    cases.append(('missing trade endpoint', x, 'Invalid dependency'))
    x = copy.deepcopy(d); x['civil_turn_hours'] = 136.755
    cases.append(('natural day substituted for civil turn', x, 'time/mass units'))
    x = copy.deepcopy(d); x['orison_food']['urban_fresh_subset']['tonnes_per_civil_turn'] = 100
    cases.append(('urban growing exceeds its source row or target', x, 'Urban fresh subset'))
    x = copy.deepcopy(d); x['orison_food']['urban_fresh_subset']['included_in_delivery_id'] = 'SUP-002'
    cases.append(('urban growing assigned to another producer region', x, 'Urban fresh source excludes'))
    x = copy.deepcopy(d); x['orison_food']['interruption_case']['interrupted_delivery_id'] = 'SUP-005'
    cases.append(('interruption has no dry deficit', x, 'positive finite dry deficit'))
    for field, value, label in (
        ('people', float('nan'), 'nonfinite household population'),
        ('duration_civil_turns', float('nan'), 'nonfinite household duration'),
        ('people', 0, 'zero household population'),
        ('duration_civil_turns', 25.5, 'fractional household duration'),
        ('people', True, 'boolean household population'),
    ):
        x = copy.deepcopy(d); x['household_example'][field] = value
        cases.append((label, x, 'positive finite integers'))
    x = copy.deepcopy(d)
    x['household_example']['total_income'] = x['household_example']['total_expense'] = float('inf')
    cases.append(('nonfinite stated household totals', x, 'totals must be finite'))
    x = copy.deepcopy(d)
    for index, row in enumerate(x['household_example']['income']):
        value = 1e308 if index < 2 else 0
        row.update(count=1, rate=value, total=value)
    for index, row in enumerate(x['household_example']['expenses']):
        row['amount'] = 1e308 if index < 2 else 0
    x['household_example']['total_income'] = x['household_example']['total_expense'] = float('inf')
    cases.append(('finite household rows overflow computed totals', x, 'totals must be finite'))
    for value, label in ((float('nan'), 'NaN food quantity'), (float('inf'), 'infinite food quantity')):
        x = copy.deepcopy(d); x['orison_food']['deliveries'][0]['category_tonnes_per_turn']['introduced_fresh_produce'] = value
        cases.append((label, x, 'Invalid food row'))
    for name, case, expected in cases:
        if not any(expected in error for error in check(case, atlas, orison)):
            raise ValueError(f'Negative control not rejected: {name}')
    return [name for name, _, _ in cases]


def render_outputs(d, atlas, controls, positives=()):
    """Build artifacts in memory so regression checks cannot overwrite deliverables."""
    food = d['orison_food']
    household = d['household_example']
    failed = next(r for r in food['deliveries'] if r['id'] == food['interruption_case']['interrupted_delivery_id'])
    deficit = failed['category_tonnes_per_turn']['introduced_dry_staples']
    interval = food['dry_reserve_tonnes'] / deficit
    cities = {c['id']: c for c in atlas['cities']}
    lines = [f'# Exchange checks — working edition {d["edition"]}', '', 'Authored demand and relationships only. Productive capacity, crop yield, nutritional adequacy, navigable routes and runtime behavior have not been validated.', '',
             f'Core edible delivery allowance: **{food["baseline_total"]:,} t per 24-hour civil turn** for **{food["served_population"]:,} residents**, approximately **{food["baseline_total"]*1000/food["served_population"]:.3f} kg per resident per turn**.', '',
             f'Dry reserve: **{food["dry_reserve_tonnes"]:,} t**, equivalent to **{food["nominal_dry_reserve_turns"]} turns** at the full **{food["category_targets"]["introduced_dry_staples"]} t/turn** dry demand. Losing only **{failed["label"]} ({failed["id"]})** removes **{deficit} t/turn** of dry inflow; using all reserve against that deficit gives an upper-bound arithmetic interval of **{interval:.2f} turns**. Other supplies, losses, accessibility and obligations are deliberately omitted. This is not a food-security guarantee.', '',
             f'The {household["people"]}-person household example totals **{household["total_income"]:,} {household["currency"]} income and {household["total_expense"]:,} {household["currency"]} spending over {household["duration_civil_turns"]} turns**. It does not measure a median income or universal affordability.', '',
             '## Selected dependency distances', '', 'Great-circle lower bounds are calculated only between existing city anchors. These are not navigable alignments, sea routes, journey times or production capacities.', '', '| ID | From → to | Minimum km | Goods/service |', '|---|---|---:|---|']
    for r in d['selected_dependencies']:
        a, b = cities[r['from_city_id']], cities[r['to_city_id']]
        dist = distance_km((a['lon_deg'], a['lat_deg']), (b['lon_deg'], b['lat_deg']), atlas['radius_km'])
        lines.append(f'| {r["id"]} | {a["name"]} → {b["name"]} | {dist:,.0f} | {r["goods"]} |')
    lines += ['', '## Scope of checks', '', 'Checked city/region references, unique identities, declared civil units, nonnegative finite food rows, category and grand totals, Orison population and reserve agreement, urban-subset amount and producer-region compatibility, a positive dry interruption deficit, and finite household arithmetic with positive integer people and duration.', '', 'Embedded negative controls rejected: ' + '; '.join(controls) + '.', '', 'Positive regression controls passed: ' + '; '.join(positives) + '.', '', 'No planetary economic balance, spare capacity, service viability, actual prices, crop productivity or construction is certified.', '']
    report = '\n'.join(lines)

    # Two common-scale bar charts make origin and composition distinguishable.
    svg = ['<svg xmlns="http://www.w3.org/2000/svg" width="1440" height="690" viewBox="0 0 1440 690" role="img" aria-labelledby="title desc">', '<title id="title">Orison core food allowance: origin and composition</title>', f'<desc id="desc">Two bar charts show the same {food["baseline_total"]:,} tonnes per civil turn, once by productive origin and once by product category. These totals must not be added together.</desc>', '<rect width="1440" height="690" fill="#f1eddf"/>', '<style>text{font-family:Arial,sans-serif;fill:#273e43;font-size:16px}.small{font-size:13px}.grid{stroke:#b6c4be;stroke-width:.7}</style>']
    def text(x,y,value,extra=''):
        svg.append(f'<text x="{x}" y="{y}" {extra}>{html.escape(str(value))}</text>')
    text(50,44,'ORISON / WHAT FEEDS THE CORE', 'style="font-size:27px"')
    text(50,77,f'{food["baseline_total"]:,} tonnes of edible product per {d["civil_turn_hours"]}-hour civil turn · {food["served_population"]:,} residents · authored demand allowance')
    text(50,103,'Both panels describe the same total. Production capacity, nutrition and transport feasibility remain unverified.', 'class="small"')
    origins = [(r['label'], r['row_total_tonnes_per_turn']) for r in sorted(food['deliveries'], key=lambda row: row['id'])]
    category_order = [key for key in CATEGORY_LABELS if key in food['category_targets']]
    category_order += sorted(set(food['category_targets']) - set(CATEGORY_LABELS))
    products = [(CATEGORY_LABELS.get(key, key.replace('_', ' ').capitalize()), food['category_targets'][key]) for key in category_order]
    panels = [(50,'BY PRODUCTIVE ORIGIN',origins, '#356d7a'),(755,'BY PRODUCT CATEGORY',products, '#936749')]
    axis_max = max(100, math.ceil(max(value for _, value in origins + products) / 100) * 100)
    scale = 320 / axis_max
    for x,title,rows,color in panels:
        text(x,150,title,'style="font-size:18px;font-weight:bold"')
        start=x+235
        for tick in range(0, axis_max + 1, max(1, axis_max // 4)):
            px=start+tick*scale
            svg.append(f'<line x1="{px}" y1="170" x2="{px}" y2="553" class="grid"/>')
            text(px,576,tick,'class="small" text-anchor="middle"')
        for i,(label,value) in enumerate(rows):
            y=190+i*62
            for j,line in enumerate(textwrap.wrap(label,26)):
                text(x,y+16*j,line)
            svg.append(f'<rect data-value="{value}" x="{start}" y="{y-17}" width="{value*scale}" height="27" rx="2" fill="{color}"/>')
            text(start+value*scale+8,y+3,f'{value:g}', 'class="small"')
        text(start+320,601,'tonnes / civil turn', 'class="small" text-anchor="end"')
    text(50,640,f'The dry reserve is a separate stock: {food["dry_reserve_tonnes"]:,} tonnes, or {food["nominal_dry_reserve_turns"]} turns of full dry demand before losses and other obligations.','class="small"')
    text(50,663,f'Source: nacre_exchange.json · Working edition {d["edition"]} · Core demand only; wider service-region requirements are additional.','class="small"')
    svg.append('</svg>')
    return {'EXCHANGE_CHECKS.md': report, 'orison_food_allowance.svg': '\n'.join(svg)+'\n'}


def positive_controls(d, atlas, orison):
    results = []
    baseline = render_outputs(d, atlas, [])
    x = copy.deepcopy(d)
    x['orison_food']['deliveries'].reverse()
    x['orison_food']['categories'].reverse()
    x['orison_food']['category_targets'] = dict(reversed(list(x['orison_food']['category_targets'].items())))
    if check(x, atlas, orison) or render_outputs(x, atlas, []) != baseline:
        raise ValueError('Reordering identified origins or keyed categories changed the rendered artifacts')
    results.append('origin and category reordering preserves both rendered artifacts')

    x = copy.deepcopy(d)
    household = x['household_example']
    household.update(people=4, currency='M-TL', duration_civil_turns=26, total_income=736, total_expense=736)
    household['income'][0].update(rate=25, total=400)
    household['expenses'][-1]['amount'] += 16
    rendered = render_outputs(x, atlas, [])['EXCHANGE_CHECKS.md']
    if check(x, atlas, orison) or not all(term in rendered for term in ('4-person household', '736 M-TL income', '736 M-TL spending over 26 turns')):
        raise ValueError('A balanced household change did not propagate into its rendered summary')
    results.append('revised household quantities and denomination reach the report')

    x = copy.deepcopy(d)
    food = x['orison_food']
    origin = next(row for row in food['deliveries'] if row['id'] == 'SUP-001')
    origin['category_tonnes_per_turn']['introduced_fresh_produce'] += 20
    origin['row_total_tonnes_per_turn'] += 20
    food['category_targets']['introduced_fresh_produce'] += 20
    food['baseline_total'] += 20
    food['interruption_case']['interrupted_delivery_id'] = 'SUP-003'
    rendered = render_outputs(x, atlas, [])
    if check(x, atlas, orison) or '720 tonnes of edible product' not in rendered['orison_food_allowance.svg'] or 'Glassback foothill valleys (SUP-003)' not in rendered['EXCHANGE_CHECKS.md'] or '**345.00 turns**' not in rendered['EXCHANGE_CHECKS.md']:
        raise ValueError('Revised food totals or interruption source did not propagate into rendered outputs')
    results.append('revised food total and interruption source reach the artifacts')
    return results


def render(d, atlas, controls, positives=()):
    for filename, content in render_outputs(d, atlas, controls, positives).items():
        (OUT / filename).write_text(content, encoding='utf8')


if __name__ == '__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
    data=json.loads((OUT/'nacre_exchange.json').read_text(encoding='utf8'))
    atlas=json.loads((WORLD/'Atlas/nacre_atlas.json').read_text(encoding='utf8'))
    orison=json.loads((WORLD/'Cities/orison_city_plan.json').read_text(encoding='utf8'))
    errors=check(data,atlas,orison)
    if errors:raise SystemExit('\n'.join(errors))
    controls=negative_controls(data,atlas,orison)
    positives=positive_controls(data,atlas,orison)
    if not args.check:render(data,atlas,controls,positives)
    print(json.dumps({'passed':True,'cities':len(data['city_roles']),'dependencies':len(data['selected_dependencies']),'negative_controls':controls,'positive_controls':positives,'scope':'Authored demand arithmetic and references only'}))
