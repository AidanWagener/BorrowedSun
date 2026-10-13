"""Check the authored people register against its local evidence and place records.

This validates continuity contracts, not literary truth, simulated people, legal
relationships, travel feasibility, or every name in the world bible. Source
hash drift is a warning: exact evidence and references still have to resolve.
"""

from __future__ import annotations

import argparse
import copy
import hashlib
import json
import math
import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTER = ROOT / "Docs/World/People/nacre_people.json"
VISIBILITY = "author_knowledge_not_automatic_npc_knowledge"
NUMBER_TOKEN = re.compile(r"(?<![\w.,])[+-]?(?:\d+(?:\.\d+)?|\.\d+)(?![\w.,])")


def finite_number(value: object) -> bool:
    """JSON booleans, strings, NaN and infinities are not authored ages."""
    if type(value) not in (int, float):
        return False
    try:
        return math.isfinite(value)
    except OverflowError:
        return False


def check(data: dict, root: Path = ROOT) -> tuple[list[str], list[str]]:
    errors: list[str] = []
    warnings: list[str] = []
    atlas = json.loads((root / "Docs/World/Atlas/nacre_atlas.json").read_text(encoding="utf-8-sig"))
    city_ids = {item["id"] for item in atlas["cities"]}
    city_names = {item["id"]: item["name"] for item in atlas["cities"]}
    places: dict[str, str] = {}
    source_cities: dict[Path, str] = {}
    plans = list((root / "Docs/World/Cities").glob("*_settlement.json"))
    plans += [root / "Docs/World/Cities/orison_city_plan.json", root / "Docs/World/Cities/talweir_city_plan.json"]
    for path in plans:
        plan = json.loads(path.read_text(encoding="utf-8-sig"))
        source_cities[(path.parent / plan["source_bible"]).resolve()] = plan["city_id"]
        for place in plan.get("points_of_interest", plan.get("places", [])):
            places[place["id"]] = plan["city_id"]

    def require(condition: bool, message: str) -> None:
        if not condition:
            errors.append(message)

    def unique(items: list, key: str, kind: str) -> None:
        for value, count in Counter(item.get(key) for item in items).items():
            require(count == 1, f"duplicate {kind} {key}: {value}")

    unique(data.get("sources", []), "id", "source")
    unique(data.get("sources", []), "path", "source")
    unique(data.get("people", []), "id", "person")
    unique(data.get("people", []), "display_name", "public identity")
    unique(data.get("relationships", []), "id", "relationship")
    sources = {source["id"]: source for source in data.get("sources", [])}
    texts: dict[str, str] = {}
    for sid, source in sources.items():
        path = (root / source["path"]).resolve()
        require(path.is_relative_to(root.resolve()), f"source path escapes repository: {sid}")
        if not path.is_relative_to(root.resolve()):
            continue
        require(path.is_file(), f"source missing: {sid}: {source['path']}")
        if not path.is_file():
            continue
        raw = path.read_bytes()
        texts[sid] = raw.decode("utf-8-sig")
        if hashlib.sha256(raw).hexdigest() != source.get("audit_sha256"):
            warnings.append(f"source changed since audit; review evidence freshness: {source['path']}")

    def evidence(items: list, context: str) -> None:
        require(bool(items), f"missing evidence: {context}")
        for item in items:
            sid = item.get("source_id")
            require(sid in sources, f"unknown evidence source: {context}: {sid}")
            quote = item.get("quote", "")
            require(bool(quote.strip()) and quote in texts.get(sid, ""), f"evidence quote absent: {context}: {sid}")
            require(item.get("visibility") == VISIBILITY, f"evidence implies automatic world knowledge: {context}")
            require(isinstance(item.get("line_hint"), int) and item["line_hint"] > 0, f"invalid evidence line hint: {context}")

    people = {person["id"]: person for person in data.get("people", [])}
    local_ids: list[str] = []
    current = data.get("present_year_al")
    require(current == atlas.get("present_year_AL"), "present year disagrees with atlas")
    require(data.get("calendar", {}).get("local_year_standard_days") == 325.191, "local year differs from adopted canon")
    require(data.get("calendar", {}).get("standard_year_days") == 365.25, "standard age year differs from adopted canon")
    association_counts = Counter()
    for pid, person in people.items():
        require(bool(re.fullmatch(r"PER-\d{4}", pid)), f"invalid person ID: {pid}")
        require(person.get("status") in {"original_anchor", "historical_established", "inherited_world_character", "proposed_city_character"}, f"invalid person status: {pid}")
        association = person.get("primary_city_association", {}).get("city_id")
        require(association is None or association in city_ids, f"unknown associated city: {pid}")
        if association:
            association_counts[association] += 1
        evidence(person.get("profile_evidence", []), pid)
        require(bool(person.get("knowledge_limits")), f"missing knowledge limits: {pid}")
        local_ids.extend(person.get("source_local_ids", []))
        for local_id in person.get("source_local_ids", []):
            require(any(local_id in line and person["display_name"] in line for text in texts.values() for line in text.splitlines()), f"local person ID lacks same-line name evidence: {pid}: {local_id}")
        home = person.get("home", {})
        hcity, place = home.get("city_id"), home.get("place_id")
        require(hcity is None or hcity in city_ids, f"unknown home city: {pid}")
        require(place is None or places.get(place) == hcity, f"home place/city mismatch: {pid}: {place}")
        if hcity is not None or place is not None:
            evidence(home.get("evidence", []), f"{pid} home")
        # A chapter's local "here" or "nearby flat" belongs to that city's
        # setting even when no exact POI was assigned. Explicit statements about
        # a different city require a separately authored, evidenced exception.
        exceptions = home.get("city_evidence_exceptions", [])
        valid_exceptions: set[str] = set()
        for exception in exceptions:
            sid = exception.get("source_id")
            items = exception.get("evidence", [])
            evidence(items, f"{pid} cross-city home exception")
            target = exception.get("asserted_city_id")
            basis_ok = (target == hcity and target in city_ids
                        and exception.get("basis") == "explicit_cross_city_home_statement"
                        and bool(str(exception.get("rationale", "")).strip()))
            require(basis_ok, f"invalid cross-city home exception: {pid}")
            same_source = bool(items) and all(item.get("source_id") == sid for item in items)
            require(same_source, f"cross-city home exception source mismatch: {pid}")
            named_city = target in city_names and any(
                re.search(rf"(?<!\w)(?:{re.escape(city_names[target])}|{re.escape(target)})(?!\w)",
                          item.get("quote", ""), flags=re.I) for item in items)
            require(named_city, f"cross-city home exception lacks explicit target city: {pid}")
            if basis_ok and same_source and named_city:
                valid_exceptions.add(sid)
        if hcity is not None:
            for item in home.get("evidence", []):
                sid = item.get("source_id")
                source_path = (root / sources.get(sid, {}).get("path", "")).resolve()
                require(source_cities.get(source_path) == hcity or sid in valid_exceptions,
                        f"home city not supported by source setting or explicit exception: {pid}")
        if place is not None:
            # A valid POI in the wrong chapter section is still a bad home address.
            for item in home.get("evidence", []):
                text = texts.get(item.get("source_id"), "")
                position = text.find(item.get("quote", ""))
                prior_headings = re.findall(r"^#{1,6} .*", text[:position], flags=re.M)
                require(bool(prior_headings) and place in prior_headings[-1], f"home evidence not in declared place section: {pid}: {place}")
        birth = person.get("birth")
        if birth is not None:
            year = birth.get("year_al")
            require(type(year) is int and 0 <= year <= current, f"invalid or future birth: {pid}")
            evidence(birth.get("evidence", []), f"{pid} birth")
            require(any(re.search(rf"(?<!\d){year}(?!\d)", item.get("quote", "")) for item in birth.get("evidence", [])), f"birth number absent from evidence: {pid}")
        for age in person.get("reported_ages", []):
            require(age.get("unit") == "Julian Earth years", f"wrong biological-age unit: {pid}")
            require(age.get("as_of_al") == current, f"wrong reported-age epoch: {pid}")
            require(age.get("precision") == "approximate_as_reported", f"invented age precision: {pid}")
            evidence(age.get("evidence", []), f"{pid} age")
            value = age.get("value")
            valid_age = finite_number(value) and value >= 0
            require(valid_age, f"invalid age: {pid}")
            if valid_age:
                require(any(float(token) == value
                            for item in age.get("evidence", [])
                            for token in NUMBER_TOKEN.findall(item.get("quote", ""))),
                        f"reported age number absent from evidence: {pid}")
            if birth is not None and type(birth.get("year_al")) is int and valid_age:
                estimate = (current - birth["year_al"]) * 325.191 / 365.25
                # Broad tolerance allows an unknown position within the birth year
                # and whole-year reporting. It does not derive a missing age.
                require(abs(value - estimate) <= 1.4, f"age/calendar inconsistency: {pid}")
        for affiliation in person.get("affiliations", []):
            evidence(affiliation.get("evidence", []), f"{pid} affiliation")
        require(bool(person.get("appearances")), f"no source appearance: {pid}")
        for appearance in person.get("appearances", []):
            evidence([appearance.get("evidence", {})], f"{pid} appearance")
            require(appearance.get("source_id") == appearance.get("evidence", {}).get("source_id"), f"appearance/evidence source mismatch: {pid}")
            require(appearance.get("mode") == "textual_reference", f"appearance falsely implies physical presence: {pid}")
            if appearance.get("place_id"):
                require(appearance["place_id"] in places, f"unknown appearance POI: {pid}: {appearance['place_id']}")
    require(len(local_ids) == len(set(local_ids)), "local person IDs assigned to multiple identities")
    require(set(data.get("scope", {}).get("city_ids", [])) == city_ids, "audit scope does not cover all atlas cities")
    require(set(association_counts) == city_ids, "one or more cities lack a curated person")
    seen_edges = set()
    for rel in data.get("relationships", []):
        rid = rel.get("id")
        origin, target = rel.get("from_person_id"), rel.get("to_person_id")
        require(origin in people and target in people, f"dangling relationship endpoint: {rid}")
        require(origin != target, f"self relationship: {rid}")
        key = (origin, target, rel.get("kind"))
        require(key not in seen_edges, f"duplicate directed relationship: {rid}")
        seen_edges.add(key)
        evidence(rel.get("evidence", []), rid)
        require(bool(rel.get("knowledge_limits")), f"unbounded relationship knowledge: {rid}")
        require(rel.get("publicity") == "not_specified_by_source", f"unsupported global relationship publicity: {rid}")
        require(rel.get("status") == "stated_in_city_proposal", f"relationship status inflation: {rid}")
    for item in data.get("identity_resolutions", []):
        require(all(pid in people for pid in item.get("people", [])), f"unknown identity-resolution person: {item.get('id')}")
    for item in data.get("social_design_cases", []):
        require(all(pid in people for pid in item.get("person_ids", [])), f"unknown social-case person: {item.get('id')}")
        require(item.get("status") == "new_behavioral_design_proposal", f"social proposal presented as inherited fact: {item.get('id')}")
    return errors, warnings


def negative_controls(data: dict) -> list[str]:
    """Mutate copies into continuity errors; the saved source is never changed."""
    cases = []
    def run(name, mutate, expected):
        candidate = copy.deepcopy(data)
        mutate(candidate)
        failures, _ = check(candidate)
        if not any(expected in failure for failure in failures):
            raise AssertionError(f"negative control failed to detect {name}: {failures}")
        cases.append(name)
    run("duplicate persistent identity", lambda d: d["people"].append(copy.deepcopy(d["people"][0])), "duplicate person")
    run("dangling relationship endpoint", lambda d: d["relationships"][0].update(to_person_id="PER-9999"), "dangling relationship")
    run("fabricated source quotation", lambda d: d["people"][0]["profile_evidence"][0].update(quote="THIS SENTENCE WAS NOT AUTHORED IN THE SOURCE."), "evidence quote absent")
    run("unknown source", lambda d: d["people"][0]["profile_evidence"][0].update(source_id="PSRC-999"), "unknown evidence source")
    run("repository escape", lambda d: d["sources"][0].update(path="../outside-source.md"), "source path escapes")
    run("unknown city", lambda d: d["people"][0]["primary_city_association"].update(city_id="CITY-999"), "unknown associated city")
    def wrong_home(d):
        person = next(p for p in d["people"] if p["display_name"] == "Mara Venn")
        person["home"]["place_id"] = "VEL-POI-002"
    run("real POI but wrong home section", wrong_home, "home evidence not in declared place section")
    def wrong_city_only_home(d):
        person = next(p for p in d["people"] if p["display_name"] == "Vessa Dren")
        person["home"]["city_id"] = "CITY-004"
    run("city-only home moved with unchanged local evidence", wrong_city_only_home, "home city not supported")
    def invented_home_exception(d):
        wrong_city_only_home(d)
        person = next(p for p in d["people"] if p["display_name"] == "Vessa Dren")
        item = person["home"]["evidence"][0]
        person["home"]["city_evidence_exceptions"] = [{
            "source_id": item["source_id"], "asserted_city_id": "CITY-004",
            "basis": "explicit_cross_city_home_statement",
            "rationale": "An exception cannot supply a city absent from the quotation.",
            "evidence": [copy.deepcopy(item)]}]
    run("unsupported cross-city exception", invented_home_exception, "cross-city home exception lacks explicit target city")
    run("same local alias on different people", lambda d: d["people"][0]["source_local_ids"].append("VEL-PER-02"), "local person IDs assigned")
    def wrong_birth(d):
        next(p for p in d["people"] if p["birth"])["birth"]["year_al"] = 743
    run("future birth", wrong_birth, "invalid or future birth")
    def wrong_age_unit(d):
        next(p for p in d["people"] if p["reported_ages"])["reported_ages"][0]["unit"] = "AL years"
    run("local years mislabeled biological age", wrong_age_unit, "wrong biological-age unit")
    def wrong_age(d):
        person = next(p for p in d["people"] if p["reported_ages"])
        person["reported_ages"][0]["value"] = 742 - person["birth"]["year_al"]
    run("AL difference mistaken for standard age", wrong_age, "age/calendar inconsistency")
    def no_birth_age(d, value):
        person = next(p for p in d["people"] if p["id"] == "PER-0009")
        assert person["display_name"] == "Neris Tamm" and person["birth"] is None
        item = copy.deepcopy(person["profile_evidence"][0])
        assert "informal" in item["quote"]
        person["reported_ages"] = [{"value": value, "unit": "Julian Earth years",
            "as_of_al": 742, "precision": "approximate_as_reported", "evidence": [item]}]
    run("no-birth infinite age matching informal", lambda d: no_birth_age(d, float("inf")), "invalid age")
    run("no-birth NaN age", lambda d: no_birth_age(d, float("nan")), "invalid age")
    run("no-birth boolean age", lambda d: no_birth_age(d, True), "invalid age")
    run("no-birth numeric string age", lambda d: no_birth_age(d, "41"), "invalid age")
    run("text citation becomes physical visit", lambda d: d["people"][0]["appearances"][0].update(mode="physically_present"), "appearance falsely implies")
    run("omniscient evidence visibility", lambda d: d["people"][0]["profile_evidence"][0].update(visibility="known_by_everyone"), "automatic world knowledge")
    run("planet-wide relationship publicity", lambda d: d["relationships"][0].update(publicity="globally_known"), "unsupported global relationship publicity")
    run("missing city audit scope", lambda d: d["scope"]["city_ids"].pop(), "audit scope does not cover")
    return cases


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--negative-controls", action="store_true")
    args = parser.parse_args()
    data = json.loads(REGISTER.read_text(encoding="utf-8"))
    errors, warnings = check(data)
    for warning in warnings:
        print("WARNING:", warning)
    for error in errors:
        print("ERROR:", error)
    if errors:
        return 1
    if args.negative_controls:
        controls = negative_controls(data)
        print(f"PASS: {len(controls)} negative controls")
        for name in controls:
            print("  rejected:", name)
    print(f"PASS: {len(data['people'])} identities; {len(data['relationships'])} directed relationships; {len(data['sources'])} sources; 12 city audit scope")
    print("Authored references and continuity checks only; not a population simulation or exhaustive-name guarantee.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
