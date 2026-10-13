"""Read-only continuity checks for the authored Nacre chronology.

Run from any directory: python Tools/World/check_chronology.py
Outputs a compact JSON report and returns nonzero on validation/control failure.
Does not write a report, rewrite canon, or infer historical truth from a source.
"""

from __future__ import annotations

import argparse
from collections import Counter
from copy import deepcopy
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
REGISTER = Path("Docs/World/History/nacre_chronology.json")
MASTER = Path("Docs/World/04_HISTORY_OF_NACRE.md")
ATLAS = Path("Docs/World/Atlas/nacre_atlas.json")
STATUSES = {"established", "adopted", "adopted_revised", "retained_proposal"}
PRECISIONS = {"year", "interval", "earliest_attestation", "context_window",
              "onset_only", "onset_marker", "reference_epoch"}
RELATIONS = {"response", "influence", "precedes", "institutional_ancestor",
             "historical_context", "local_expression", "distinct_from", "compatible_overlap"}
# These pre-existing authorial anchors must not move when city material is added.
PROTECTED = {
    f"HIST-{index:04}": dates
    for index, dates in enumerate([
        (0, 0), (9, 9), (17, 17), (43, 43), (62, 128), (76, 76),
        (84, 91), (113, 113), (129, 216), (147, 147), (163, 163),
        (188, 188), (198, 198), (231, 231), (248, 248), (279, 279),
        (318, 318), (337, 337), (354, 354), (367, 372), (392, None),
        (392, 468), (397, 397), (481, 481), (506, 506), (563, 563),
        (571, 589), (549, 624), (612, 612), (641, 641), (681, 686),
        (698, 698), (716, 716), (742, 742), (742, 742), (305, 391),
    ], 1)
}
ARCHIVES = [
    "The first lesson was not how to open the door. It was how to be sure that the person outside meant the same door.",
    "Sent again with the blue boat. Do not mark delivered because the boat returned. Ask whether my sister opened it.",
]


def normalized(text: str) -> str:
    return " ".join(text.casefold().split())


def validate(data: dict, root: Path = ROOT, master_text: str | None = None) -> list[str]:
    errors: list[str] = []
    if data.get("schema_version") != 1:
        errors.append("schema: unsupported version")
    if data.get("present_year_AL") != 742 or data.get("local_year_earth_days") != 325.191:
        errors.append("calendar: reference epoch/year changed")
    atlas = json.loads((root / ATLAS).read_text(encoding="utf-8"))
    cities = {item["id"]: item for item in atlas["cities"]}
    regions = {item["id"] for item in atlas["regions"]}
    city_region = {key: item["region_id"] for key, item in cities.items()}
    cache: dict[Path, str] = {}

    def source_check(path_string: str, anchor: str, label: str) -> None:
        path = Path(path_string)
        resolved = (root / path).resolve()
        if path.is_absolute() or not resolved.is_relative_to(root.resolve()):
            errors.append(f"{label}: source path escapes repository")
            return
        if not resolved.is_file():
            errors.append(f"{label}: source file absent")
            return
        if not anchor or not anchor.strip():
            errors.append(f"{label}: source anchor empty")
            return
        if resolved not in cache:
            cache[resolved] = normalized(resolved.read_text(encoding="utf-8"))
        if normalized(anchor) not in cache[resolved]:
            errors.append(f"{label}: source anchor absent: {anchor}")

    entries = data.get("events", [])
    ids = [entry.get("id") for entry in entries]
    if len(ids) != len(set(ids)):
        errors.append("identity: duplicate event ID")
    by_id = {entry.get("id"): entry for entry in entries}
    coverage: set[str] = set()
    for entry in entries:
        eid = entry.get("id", "missing")
        if not re.fullmatch(r"HIST-(?:\d{4}|[A-Z]{3}-\d{3})", eid):
            errors.append(f"{eid}: invalid stable ID")
        if entry.get("status") not in STATUSES:
            errors.append(f"{eid}: unknown status")
        precision = entry.get("date_precision")
        if precision not in PRECISIONS:
            errors.append(f"{eid}: unknown date precision")
        start, end = entry.get("start_AL"), entry.get("end_AL")
        valid_start = type(start) is int and 0 <= start <= 742
        valid_end = end is None or (type(end) is int and 0 <= end <= 742)
        if not valid_start or not valid_end:
            errors.append(f"{eid}: date outside authored calendar")
        if valid_start and valid_end and end is not None and end < start:
            errors.append(f"{eid}: reversed date interval")
        if (end is None) != (precision == "onset_only"):
            errors.append(f"{eid}: null endpoint/precision mismatch")
        if precision in {"year", "earliest_attestation", "onset_marker", "reference_epoch"} and start != end:
            errors.append(f"{eid}: point-date precision has interval")
        if not entry.get("title") or not entry.get("summary") or not entry.get("adoption_rationale"):
            errors.append(f"{eid}: missing authored meaning or adoption rationale")
        places = entry.get("locations", {})
        city_ids, region_ids = places.get("city_ids", []), places.get("region_ids", [])
        if not set(city_ids) <= set(cities) or not set(region_ids) <= regions:
            errors.append(f"{eid}: unknown atlas location")
        if any(city_region.get(city) not in region_ids for city in city_ids if city in cities):
            errors.append(f"{eid}: city/region mismatch")
        coverage.update(city_ids)
        sources = entry.get("sources", [])
        if not sources:
            errors.append(f"{eid}: missing provenance")
        for source in sources:
            source_check(source.get("path", ""), source.get("anchor", ""), eid)
            if not source.get("source_role"):
                errors.append(f"{eid}: source role absent")
    if coverage != set(cities):
        errors.append("coverage: chronology does not cover all atlas cities")
    for eid, dates in PROTECTED.items():
        entry = by_id.get(eid, {})
        if (entry.get("start_AL"), entry.get("end_AL")) != dates or entry.get("status") != "established":
            errors.append(f"{eid}: protected master anchor changed or absent")
    if by_id.get("HIST-0028", {}).get("date_precision") != "context_window":
        errors.append("Passage cases: era window became proceedings duration")
    if by_id.get("HIST-0013", {}).get("date_precision") != "earliest_attestation":
        errors.append("First Wrong House: evidence date became invention date")
    eras = data.get("eras", [])
    if not eras or eras[0].get("start_AL") != 0 or eras[-1].get("end_AL") != 742:
        errors.append("eras: reference span incomplete")
    if len({era.get("id") for era in eras}) != len(eras):
        errors.append("eras: duplicate identity")
    for earlier, later in zip(eras, eras[1:]):
        if earlier.get("end_AL", -2) + 1 != later.get("start_AL"):
            errors.append("eras: gap or overlap")
    for relation in data.get("relations", []):
        left, right = relation.get("from_event_id"), relation.get("to_event_id")
        kind = relation.get("kind")
        if left not in by_id or right not in by_id or left == right:
            errors.append("relation: invalid event reference")
            continue
        if kind not in RELATIONS or not relation.get("note"):
            errors.append("relation: unknown kind or missing meaning")
        if kind in {"precedes", "influence", "institutional_ancestor", "response"}:
            end = by_id[left].get("end_AL")
            if end is not None and end > by_id[right]["start_AL"]:
                errors.append("relation: asserted temporal order reversed")
    contexts = data.get("undated_contexts", [])
    for context in contexts:
        if context.get("date_AL") is not None:
            errors.append(f"{context.get('id')}: undated context acquired an unsupported date")
        source_check(context.get("source_path", ""), context.get("anchor", ""), context.get("id", "context"))
    if not any(item.get("id") == "HIST-U001" for item in contexts):
        errors.append("archive: undated teaching-copy context absent")
    master_text = master_text if master_text is not None else (root / MASTER).read_text(encoding="utf-8")
    for passage in ARCHIVES:
        if f"> {passage}" not in master_text:
            errors.append("archive: original passage changed or absent")
    return errors


def negative_controls(data: dict) -> dict[str, bool]:
    outcomes: dict[str, bool] = {}

    def event(d, eid):
        return next(item for item in d["events"] if item["id"] == eid)

    def probe(name, mutate, expected):
        candidate = deepcopy(data)
        mutate(candidate)
        outcomes[name] = any(expected in error for error in validate(candidate))

    probe("shift_existing_arrival", lambda d: event(d, "HIST-0001").update(start_AL=1, end_AL=1), "protected master anchor")
    probe("duplicate_identity", lambda d: d["events"].append(deepcopy(event(d, "HIST-0001"))), "duplicate event ID")
    probe("reverse_interval", lambda d: event(d, "HIST-0005").update(end_AL=50), "reversed date interval")
    probe("unknown_city", lambda d: event(d, "HIST-0002")["locations"]["city_ids"].append("CITY-999"), "unknown atlas location")
    probe("wrong_city_region", lambda d: event(d, "HIST-0002")["locations"].update(region_ids=["REG-002"]), "city/region mismatch")
    probe("remove_provenance", lambda d: event(d, "HIST-0001").update(sources=[]), "missing provenance")
    probe("source_path_escape", lambda d: event(d, "HIST-0001")["sources"][0].update(path="../not-a-source.md"), "escapes repository")
    probe("invent_source_anchor", lambda d: event(d, "HIST-0001")["sources"][0].update(anchor="Unwritten historical passage 98347"), "source anchor absent")
    probe("unknown_relation", lambda d: d["relations"][0].update(to_event_id="HIST-9999"), "invalid event reference")
    probe("invent_relay_closure", lambda d: event(d, "HIST-0021").update(end_AL=468, date_precision="interval"), "protected master anchor")
    probe("turn_case_window_into_trial", lambda d: event(d, "HIST-0028").update(date_precision="interval"), "proceedings duration")
    probe("turn_script_into_invention", lambda d: event(d, "HIST-0013").update(date_precision="year"), "invention date")
    probe("date_undated_copy", lambda d: d["undated_contexts"][0].update(date_AL=0), "unsupported date")
    altered = (ROOT / MASTER).read_text(encoding="utf-8").replace(ARCHIVES[1], "The message was delivered.")
    outcomes["rewrite_archival_voice"] = any("original passage" in error for error in validate(data, master_text=altered))
    return outcomes


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--register", type=Path, default=ROOT / REGISTER)
    args = parser.parse_args()
    try:
        data = json.loads(args.register.read_text(encoding="utf-8"))
        errors = validate(data)
        controls = negative_controls(data) if not errors else {}
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print(json.dumps({"valid": False, "errors": [str(exc)]}, indent=2))
        return 1
    passed = not errors and bool(controls) and all(controls.values())
    print(json.dumps({"valid": passed, "event_count": len(data["events"]),
                      "statuses": dict(Counter(item["status"] for item in data["events"])),
                      "era_count": len(data["eras"]), "relation_count": len(data["relations"]),
                      "negative_controls": controls, "errors": errors}, indent=2))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
