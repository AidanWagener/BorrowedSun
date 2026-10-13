"""Read-only reconciliation of city demands, source mentions and existing studies.

No engineering/yield validation is implied. Prose table rows and selected JSON
fields are checked against their source; prose-only process assumptions are
checked arithmetically, not independently certified. No outputs are rewritten.
"""
from __future__ import annotations

import copy
import json
import math
from pathlib import Path
import sys

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2] / "Docs" / "World"
DATA = ROOT / "Resources" / "nacre_city_region_accounts.json"
CATEGORIES = ["dry", "fresh", "cultured", "qualified_native", "other"]
EXCHANGE_CATEGORIES = ["introduced_dry_staples", "introduced_fresh_produce", "cultured_products", "qualified_native_fractions", "other_preserved"]


def read(path):
    return json.loads(path.read_text(encoding="utf-8"))


def value_at(data, pointer):
    for part in pointer.strip("/").split("/"):
        data = data[int(part)] if isinstance(data, list) else data[part]
    return data


def number(value, label, positive=False):
    if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value):
        raise ValueError(f"{label}: finite numeric value required")
    if value < 0 or positive and value == 0:
        raise ValueError(f"{label}: invalid negative/zero value")
    return value


def same(a, b, label, absolute=0.6):
    number(a, label); number(b, label)
    if not math.isclose(a, b, rel_tol=1e-9, abs_tol=absolute):
        raise ValueError(f"{label}: {a} != {b}")


def require(condition, label):
    if not condition:
        raise ValueError(label)


def finite_tree(value, path="ledger"):
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        require(math.isfinite(value) and value >= 0, f"{path}: nonfinite or negative number")
    elif isinstance(value, dict):
        for k, v in value.items(): finite_tree(v, f"{path}/{k}")
    elif isinstance(value, list):
        for i, v in enumerate(value): finite_tree(v, f"{path}/{i}")


def keyed(rows, key):
    result = {row[key]: row for row in rows}
    require(len(result) == len(rows), f"duplicate {key}")
    return result


def prose_row(file, label):
    matches = []
    for line in (ROOT / file).read_text(encoding="utf-8").splitlines():
        cells = [s.strip() for s in line.strip("|").split("|")]
        if line.startswith("|") and cells[0] == label:
            matches.append([float(s.replace(",", "")) for s in cells[1:]])
    require(len(matches) == 1, f"source table row not unique: {file}: {label}")
    return matches[0]


def validate(data):
    finite_tree(data)
    require(data["civil_turn_hours"] == 24, "civil-turn duration")
    require(data["categories"] == CATEGORIES, "category definitions")
    cities = keyed(data["cities"], "city_id")
    exchange = read(ROOT / "Exchange/nacre_exchange.json")
    roles = keyed(exchange["city_roles"], "id")
    require(set(cities) == set(roles), "all twelve existing cities required")
    for cid, c in cities.items():
        src = read(ROOT / c["source_json"])
        require(c["name"] == roles[cid]["name"] and c["host_region_id"] == roles[cid]["region_id"], "city identity")
        pop = c["core_population"]
        require(type(pop) is int and pop > 0, "core population must be a positive integer")
        w, f, e = c["water"], c["food"], c["electricity"]
        if cid == "CITY-001":
            s = src["service_assumptions"]
            same(pop, src["population"]["core_estimate"], "Orison core population", 0)
            same(w["total_delivered_m3_per_turn"], s["potable_delivered_total_m3_per_turn"], "Orison water source")
            same(w["treated_network_input_m3_per_turn"], s["potable_treatment_output_m3_per_turn_approx"], "Orison rounded input")
            same(f["resident_edible_product_tonnes_per_turn"], exchange["orison_food"]["baseline_total"], "Orison food baseline", 1e-6)
            same(e["mean_end_use_mw"], s["electricity_average_MW"], "Orison mean", 1e-6)
            same(e["peak_case_mw"], s["electricity_peak_target_MW"], "Orison peak", 1e-6)
        elif cid == "CITY-005":
            s = src["service_assumptions"]
            same(pop, src["population"]["core_resident_estimate"], "Talweir population", 0)
            same(w["total_delivered_m3_per_turn"], s["water"]["total_delivery_m3_per_turn"], "Talweir delivery")
            same(w["combined_raw_abstraction_m3_per_turn"], s["water"]["raw_abstraction_m3_per_turn"], "Talweir raw")
            require(w["treated_network_input_m3_per_turn"] is None and w["distribution_loss_fraction"] is None, "Talweir cannot silently treat combined raw loss as distribution loss")
            same(w["total_delivered_m3_per_turn"], w["combined_raw_abstraction_m3_per_turn"] * (1-w["combined_raw_to_delivery_loss_fraction"]), "Talweir combined boundary")
            same(f["resident_edible_product_tonnes_per_turn"], s["food"]["edible_mass_tonnes_per_turn"], "Talweir food", 1e-6)
            same(e["mean_end_use_mw"], s["electricity"]["average_demand_study_MW"], "Talweir mean", 1e-6)
            same(e["peak_case_mw"], s["electricity"]["peak_demand_study_MW"], "Talweir peak", 1e-6)
        else:
            same(pop, src["population"], "population source", 0)
            s = src["service_cases"]
            for own, original in [("domestic_liters_per_core_resident_per_turn", "domestic_liters_per_person_per_turn"), ("other_delivered_m3_per_turn", "other_potable_m3_per_turn"), ("total_delivered_m3_per_turn", "delivered_potable_m3_per_turn"), ("distribution_loss_fraction", "network_loss_fraction"), ("treated_network_input_m3_per_turn", "network_input_m3_per_turn")]:
                same(w[own], s[original], f"{cid} water source {own}", 1e-5)
            same(f["resident_edible_product_tonnes_per_turn"], s["edible_product_tonnes_per_turn"], "food source", 1e-6)
            same(e["mean_end_use_mw"], s["electric_average_mw"], "mean demand source", 1e-6)
            same(e["peak_case_mw"], s["electric_peak_mw"], "peak demand source", 1e-6)
        same(pop*w["domestic_liters_per_core_resident_per_turn"]/1000+w["other_delivered_m3_per_turn"], w["total_delivered_m3_per_turn"], "delivered boundary")
        if w["treated_network_input_m3_per_turn"] is not None:
            loss = number(w["distribution_loss_fraction"], "network loss")
            require(loss < 1, "network loss < 1")
            same(w["treated_network_input_m3_per_turn"]*(1-loss), w["total_delivered_m3_per_turn"], "treated to delivered")
        if w["raw_source_streams"]:
            same(sum(r["treated_product_m3_per_turn"] for r in w["raw_source_streams"]), w["treated_network_input_m3_per_turn"], "source product sum")
        for r in w["raw_source_streams"]:
            require(r["capacity_verified"] is False, "raw source capacity unverified")
            recovery = r["treatment_recovery"]
            if recovery is None:
                require(r["raw_abstraction_m3_per_turn"] is None and r["source_process_residual_m3_per_turn"] is None, "unknown recovery cannot establish raw flow")
            else:
                require(0 < number(recovery, "recovery") <= 1, "recovery range")
                same(r["raw_abstraction_m3_per_turn"]*recovery, r["treated_product_m3_per_turn"], "raw recovery")
                same(r["raw_abstraction_m3_per_turn"]-r["treated_product_m3_per_turn"], r["source_process_residual_m3_per_turn"], "raw residual")
        same(w["gross_end_use_m3_per_turn"], w["total_delivered_m3_per_turn"]+w["reuse_internal_loop_m3_per_turn"], "reuse loop")
        if w["sewer_collection_m3_per_turn"] is not None:
            same(w["sewer_collection_m3_per_turn"]-w["reuse_internal_loop_m3_per_turn"], w["final_residual_m3_per_turn"], "sewer reuse split")
            require(w["gross_end_use_m3_per_turn"] >= w["sewer_collection_m3_per_turn"], "unallocated negative end-use consumption")
        same(f["resident_edible_product_tonnes_per_turn"], pop*f["kg_per_resident_per_turn"]/1000, "resident edible mass", 1e-6)
        if f["basket_tonnes_per_turn"] is not None:
            require(set(f["basket_tonnes_per_turn"]) == set(CATEGORIES), "basket categories")
            same(sum(f["basket_tonnes_per_turn"].values()), f["resident_edible_product_tonnes_per_turn"], "resident basket", 1e-6)
        if f["kcal_per_resident_per_turn"] is not None:
            same(f["resident_kcal_per_turn"], pop*f["kcal_per_resident_per_turn"], "calorie target", 1e-6)
            same(f["protein_tonnes_per_turn"], pop*f["protein_g_per_resident_per_turn"]/1e6, "protein target", 1e-6)
        if f["calorie_origin_shares"] is not None:
            require(f["calorie_origin_shares"] == value_at(src, f["calorie_origin_source_pointer"]), "calorie source fractions must retain their source basis")
            same(sum(f["calorie_origin_shares"].values()), 1, "calorie shares", 1e-9)
        require(e["peak_case_mw"] >= e["mean_end_use_mw"], "peak below mean")
        require(e["fuel_or_regional_supply_verified"] is False, "no verified generation inferred")
    commitments = keyed(data["food_commitments"], "id")
    anchors = [(r["source_file"], r.get("source_record_id", r.get("source_table_row"))) for r in commitments.values()]
    require(len(set(anchors)) == len(anchors), "same source procurement row entered twice")
    exchange_rows = keyed(exchange["orison_food"]["deliveries"], "id")
    recipient_totals = {}
    for row in commitments.values():
        require(row["recipient_city_id"] in cities and row["recipient_scope"] == "core_food", "recipient core boundary")
        require(set(row["basket_tonnes_per_turn"]) == set(CATEGORIES), "commitment categories")
        same(sum(row["basket_tonnes_per_turn"].values()), row["total_tonnes_per_turn"], "commitment sum", 1e-6)
        require(row["capacity_verified"] is False, "commitment is not capacity")
        if row.get("source_record_id"):
            original = exchange_rows[row["source_record_id"]]
            expected = [original["category_tonnes_per_turn"][k] for k in EXCHANGE_CATEGORIES]
            require(row["source_region_ids"] == original["origin_region_ids"], "original regional pool must stay intact")
            require(row["origin_resolution"] == ("single_region" if len(row["source_region_ids"]) == 1 else "unsplit_multi_region_pool"), "regional pool attribution")
        else:
            raw = prose_row(row["source_file"], row["source_table_row"])
            expected = raw[:-1] if len(raw) == 6 else [*raw[:3], 0, raw[3]]
            same(raw[-1], row["total_tonnes_per_turn"], "source table total", 1e-6)
            if row["recipient_city_id"] == "CITY-011" and row["id"] in ("FOOD-BRI-02", "FOOD-BRI-04"):
                require(row["origin_resolution"] == "serving_network_not_exclusive_origin", "Brin network cannot be silently assigned exclusively to a region")
        for key, original in zip(CATEGORIES, expected):
            same(row["basket_tonnes_per_turn"][key], original, "source category allocation", 1e-6)
        recipient_totals.setdefault(row["recipient_city_id"], []).append(row)
    require(set(recipient_totals) == {"CITY-001", "CITY-004", "CITY-010", "CITY-011", "CITY-012"}, "five fully mass-allocated recipients")
    for cid, rows in recipient_totals.items():
        same(sum(r["total_tonnes_per_turn"] for r in rows), cities[cid]["food"]["resident_edible_product_tonnes_per_turn"], "recipient commitments are within demand", 1e-6)
        for key in CATEGORIES:
            same(sum(r["basket_tonnes_per_turn"][key] for r in rows), cities[cid]["food"]["basket_tonnes_per_turn"][key], "recipient category", 1e-6)
    for alias in data["duplicate_source_mentions"]:
        same(alias["amount_tonnes_per_turn"], value_at(read(ROOT / alias["source_file"]), alias["source_pointer"]), "alias source", 1e-6)
        same(alias["amount_tonnes_per_turn"], commitments[alias["canonical_commitment_id"]]["total_tonnes_per_turn"], "alias target", 1e-6)
        require(alias["additional_demand_tonnes_per_turn"] == 0, "duplicate source mention cannot add demand")
    for subset in data["nested_subsets"]:
        parent = commitments[subset["parent_commitment_id"]]
        same(subset["tonnes_per_turn"], value_at(read(ROOT/subset["source_file"]), subset["source_pointer"]), "urban subset source", 1e-6)
        require(parent["id"] == "FOOD-ORI-SUP-001" and subset["category"] == "fresh", "urban subset producer/category")
        require(subset["tonnes_per_turn"] <= parent["basket_tonnes_per_turn"]["fresh"] and subset["additional_demand_tonnes_per_turn"] == 0, "nested subset is not additional output")
    for guest in data["guest_cases"]:
        c = cities[guest["city_id"]]
        require(type(guest["people"]) is int and guest["people"] > 0, "guest count")
        same(guest["food_tonnes_per_turn"], guest["people"]*c["food"]["kg_per_resident_per_turn"]/1000, "guest food", 1e-6)
        same(guest["domestic_water_m3_per_turn"], guest["people"]*guest["domestic_liters_per_guest_per_turn"]/1000, "guest domestic water", 1e-6)
        expected_relation = "additional_domestic_only" if guest["city_id"] == "CITY-011" else "replaces_ordinary_guest_domestic_component" if guest["scenario"] == "event_ceiling" else "already_in_baseline"
        require(guest["water_relation_to_baseline"] == expected_relation, "guest water inclusion")
        require(guest["food_relation_to_baseline"] == "additional_to_resident_food", "guest food scope")
    stock = data["separate_viable_stock_case"]
    same(stock["net_biological_tonnes_per_turn"], sum(stock[k] for k in ["dry_seed_tonnes_per_turn", "living_plant_tonnes_per_turn", "cultures_tissue_tonnes_per_turn"]), "biological stock", 1e-6)
    same(stock["gross_dispatched_tonnes_per_turn"], stock["net_biological_tonnes_per_turn"]+stock["packaging_conditioning_tonnes_per_turn"], "stock packaging", 1e-6)
    require(stock["additional_edible_food_tonnes_per_turn"] == 0, "viable stocks are not bulk meals")
    study = data["existing_production_study"]
    require(study["source_capacity_tonnes_per_turn"] is None, "sizing study does not establish productive capacity")
    require(study["included_categories"] == ["dry", "fresh", "cultured"] and study["excluded_categories"] == ["qualified_native", "other"], "production category scope")
    require(set(study["included_commitment_ids"]) == {"FOOD-ORI-"+o["delivery_id"] for o in read(ROOT/study["source_file"])["origins"]}, "production origin scope")
    deps = keyed(data["existing_unquantified_dependencies"], "id")
    require(set(deps) == {d["id"] for d in exchange["selected_dependencies"]}, "selected dependency completeness")
    require(all(d["additional_quantified_food_tonnes_per_turn"] is None for d in deps.values()), "qualitative dependency is not quantified food")
    light = data["closure_light_screens"]
    prod = read(ROOT/light["source_model"])
    crop, lamp = prod["crop_cases"]["introduced_fresh_produce"], prod["lighting_case"]
    retained_yield = crop["gross_harvest_t_per_canopy_ha_per_civil_year"]*crop["retained_fraction"]
    area_per_t = prod["civil_year_turns"]/retained_yield
    wm = lamp["canopy_ppfd_umol_m2_s"]/lamp["effective_canopy_efficacy_umol_per_joule"]
    fraction, dark, layers = lamp["scheduled_light_hours_per_turn"]/24, lamp["scheduled_light_in_dark_fraction"], lamp["stack_layers"]
    target_refs = {"CITY-001": ("FOOD-ORI-SUP-001", 24), "CITY-004": ("FOOD-SER-01", None), "CITY-012": ("FOOD-SOL-03", None), "CITY-011": ("FOOD-BRI-01", None)}
    for case in light["comparison_cases"]:
        parent, override = target_refs[case["city_id"]]
        same(case["fresh_target_tonnes_per_turn"], override if override is not None else commitments[parent]["basket_tonnes_per_turn"]["fresh"], "lighting target source", 1e-6)
        canopy = case["fresh_target_tonnes_per_turn"]*area_per_t
        peak = canopy*10000*wm/1e6
        same(case["canopy_ha_under_shared_case"], canopy, "lighting canopy", 1e-6)
        same(case["daylit_supplement_mean_lamp_mw"], peak*fraction*dark, "daylit mean lamps", 1e-6)
        same(case["four_layer_mean_lamp_mw"], peak*fraction*(layers-1+dark)/layers, "lower layers require full light", 1e-6)
        same(case["four_layer_canopy_footprint_ha"], canopy/layers, "stack footprint", 1e-6)
        require(case["adopted"] is False, "comparison is not adopted new capacity")
    vel = light["velis_separate_case"]
    same(vel["mean_lamp_mw"], vel["active_lit_canopy_m2"]*vel["lamp_watts_per_m2_when_on"]*vel["light_hours_per_turn"]/24/1e6, "Velis selected lamp arithmetic", 1e-6)
    same(vel["implied_retained_fresh_tonnes_per_canopy_ha_per_civil_year"], vel["fresh_output_target_tonnes_per_turn"]*prod["civil_year_turns"]/(vel["active_lit_canopy_m2"]/10000), "Velis implied throughput", 1e-6)
    same(vel["comparison_generic_retained_fresh_tonnes_per_ha_per_civil_year"], retained_yield, "retained yield comparison", 1e-6)
    require(vel["adopted_new_capacity"] is False, "Velis screen adds no capacity")
    return summary(data)


def summary(data):
    cities = data["cities"]
    rows = data["food_commitments"]
    by_id = {r["id"]: r for r in rows}
    study = data["existing_production_study"]
    modeled = sum(by_id[k]["basket_tonnes_per_turn"][cat] for k in study["included_commitment_ids"] for cat in study["included_categories"])
    result = {
        "core_population": sum(c["core_population"] for c in cities),
        "resident_food_t_per_turn": sum(c["food"]["resident_edible_product_tonnes_per_turn"] for c in cities),
        "recipient_mass_allocated_t_per_turn": sum(r["total_tonnes_per_turn"] for r in rows),
        "delivered_water_m3_per_turn_mixed_city_service_scopes": sum(c["water"]["total_delivered_m3_per_turn"] for c in cities),
        "known_network_input_excluding_talweir_m3_per_turn": sum(c["water"]["treated_network_input_m3_per_turn"] or 0 for c in cities),
        "internal_reuse_m3_per_turn_not_extra_external_supply": sum(c["water"]["reuse_internal_loop_m3_per_turn"] for c in cities),
        "city_mean_demand_mw": sum(c["electricity"]["mean_end_use_mw"] for c in cities),
        "sum_city_peak_cases_mw_not_a_simultaneous_grid_forecast": sum(c["electricity"]["peak_case_mw"] for c in cities),
        "orison_existing_production_study_t_per_turn": modeled,
        "strict_single_region_commitments_t_per_turn": {},
    }
    for r in rows:
        if len(r["source_region_ids"]) == 1 and r["origin_resolution"] in ("single_region", "city_internal", "region_outside_city", "local_and_regional_group"):
            reg = r["source_region_ids"][0]
            result["strict_single_region_commitments_t_per_turn"][reg] = result["strict_single_region_commitments_t_per_turn"].get(reg, 0)+r["total_tonnes_per_turn"]
    # Summaries derive from IDs, not row order, and are compared structurally.
    return result


def controls(data):
    mutations = [
        ("double resident food", lambda d: d["cities"][0]["food"].update(resident_edible_product_tonnes_per_turn=1400)),
        ("Talweir raw becomes treated", lambda d: d["cities"][4]["water"].update(treated_network_input_m3_per_turn=225000)),
        ("reuse becomes external product", lambda d: d["cities"][3]["water"]["raw_source_streams"][0].update(treated_product_m3_per_turn=120000)),
        ("duplicate source adds demand", lambda d: d["duplicate_source_mentions"][0].update(additional_demand_tonnes_per_turn=360)),
        ("urban subset counted twice", lambda d: d["nested_subsets"][0].update(additional_demand_tonnes_per_turn=24)),
        ("urban subset moved to another region", lambda d: d["nested_subsets"][0].update(parent_commitment_id="FOOD-ORI-SUP-002")),
        ("unsplit distant pool assigned to Dhal", lambda d: d["food_commitments"][5].update(source_region_ids=["REG-011"])),
        ("calorie fractions replaced by tonnes", lambda d: d["cities"][4]["food"]["calorie_origin_shares"].update(Lake_Country_regional=936)),
        ("source allocation changed", lambda d: d["food_commitments"][6]["basket_tonnes_per_turn"].update(fresh=31)),
        ("guest water charged again", lambda d: d["guest_cases"][0].update(water_relation_to_baseline="additional_domestic_only")),
        ("study mislabeled capacity", lambda d: d["existing_production_study"].update(source_capacity_tonnes_per_turn=620)),
        ("nonfinite raw flow", lambda d: d["cities"][1]["water"]["raw_source_streams"][0].update(raw_abstraction_m3_per_turn=float("nan"))),
        ("nonfinite peak", lambda d: d["cities"][1]["electricity"].update(peak_case_mw=float("inf"))),
        ("fractional population", lambda d: d["cities"][1].update(core_population=670000.5)),
        ("stock treated as food", lambda d: d["separate_viable_stock_case"].update(additional_edible_food_tonnes_per_turn=.8)),
        ("unquantified route becomes extra food", lambda d: d["existing_unquantified_dependencies"][0].update(additional_quantified_food_tonnes_per_turn=360)),
        ("stack lower layers treated as daylit", lambda d: d["closure_light_screens"]["comparison_cases"][0].update(four_layer_mean_lamp_mw=90.33091787439612)),
        ("counterfactual silently adopted", lambda d: d["closure_light_screens"]["comparison_cases"][1].update(adopted=True)),
        ("Velis canopy hectares confused with square meters", lambda d: d["closure_light_screens"]["velis_separate_case"].update(active_lit_canopy_m2=18)),
        ("negative ship allowance", lambda d: d["ship_provision_cases"][0].update(food_tonnes_per_turn=-20)),
        ("guest water arithmetic", lambda d: d["guest_cases"][0].update(domestic_water_m3_per_turn=2400)),
    ]
    rejected = []
    for label, mutation in mutations:
        changed = copy.deepcopy(data); mutation(changed)
        try: validate(changed)
        except (ValueError, KeyError, TypeError): rejected.append(label)
        else: raise ValueError(f"negative control falsely passed: {label}")
    changed = copy.deepcopy(data)
    for key in ("cities", "food_commitments", "duplicate_source_mentions", "guest_cases", "existing_unquantified_dependencies"):
        changed[key].reverse()
    for row in changed["food_commitments"]:
        row["basket_tonnes_per_turn"] = dict(reversed(list(row["basket_tonnes_per_turn"].items())))
    require(validate(changed) == validate(data), "positive order-independence control")
    return len(rejected), 1


if __name__ == "__main__":
    data = read(DATA)
    result = validate(data)
    negative, positive = controls(data)
    print(json.dumps(result, indent=2))
    print(f"PASS: 12 city accounts; 26 attributed food commitments; {negative} negative controls rejected; {positive} positive order-independence control passed.")
    print("Checks cover authored relationships and arithmetic only, not yields, engineering, supply rights or runtime behavior.")
