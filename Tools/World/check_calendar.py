"""Check Nacre's authored calendar and illustrative sky arithmetic; no game simulation.

Run with --json for a machine-readable report. Reads the canonical atlas only;
does not generate or change any world assets, coordinates, or saved artifacts.
"""
from __future__ import annotations

import argparse
import json
import math
from fractions import Fraction
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SECONDS_PER_TURN = 86400
REFERENCE_YEAR_TURNS = Fraction("325.19132311485833")
SOLAR_DAY_HOURS = 136.75500761617832
SEASONAL_TILT_DEG = 14.070772674237249
REFERENCE_RADIUS_KM = 5900.0
PARENT_DISTANCE_KM = 808067.0533545649
PARENT_RADIUS_KM = 70000.0
STELLAR_ECCENTRICITY = 0.010
FIRST_NORTHWARD_CROSSING_TURN = 20.0


def mean_anomaly(true_anomaly: float) -> float:
    """Signed mean anomaly for the chosen eccentric reference stellar orbit."""
    eccentric = math.atan2(math.sqrt(1-STELLAR_ECCENTRICITY**2)*math.sin(true_anomaly),
                           STELLAR_ECCENTRICITY+math.cos(true_anomaly))
    return eccentric-STELLAR_ECCENTRICITY*math.sin(eccentric)


def seasonal_events(year: int) -> list[dict]:
    """Barycentric reference seasons, with periapsis at northern solstice.

    Moon parallax, precession and perturbations are omitted. Civil timestamp
    conversion is exact at whole seconds; the astronomical model is illustrative.
    """
    first_anomaly = mean_anomaly(-math.pi/2)
    events = []
    for label, anomaly in (("Northward crossing", -math.pi/2),
                           ("Northern solstice", 0),
                           ("Southward crossing", math.pi/2),
                           ("Southern solstice", math.pi)):
        annual_fraction = ((mean_anomaly(anomaly)-first_anomaly) % (2*math.pi))/(2*math.pi)
        elapsed_turns = ((year+annual_fraction)*float(REFERENCE_YEAR_TURNS)
                         + FIRST_NORTHWARD_CROSSING_TURN)
        elapsed_seconds = round(elapsed_turns*SECONDS_PER_TURN/60)*60
        events.append(dict(name=label,
                           elapsed_turns_from_civil_year_start=round(elapsed_turns-year_start_turn(year), 6),
                           approximate_common_date=from_seconds(elapsed_seconds)))
    return events


def year_start_turn(year: int) -> int:
    if type(year) is not int:
        raise ValueError("Year must be an integer, with year zero at the Landing epoch")
    return 325 * year + (22 * year + 57) // 115


def year_length(year: int) -> int:
    return year_start_turn(year + 1) - year_start_turn(year)


def to_seconds(year: int, span: int | str, turn: int = 1,
               hour: int = 0, minute: int = 0, second: int = 0) -> int:
    start = year_start_turn(year)
    if not all(type(v) is int for v in (turn, hour, minute, second)):
        raise ValueError("Turn and clock fields must be integers")
    if not (0 <= hour < 24 and 0 <= minute < 60 and 0 <= second < 60):
        raise ValueError("Clock field outside its civil range")
    if span == "B":
        if turn != 1 or year_length(year) != 326:
            raise ValueError("Balance has one turn and exists only in intercalary years")
        ordinal = 325
    elif type(span) is int and 1 <= span <= 13 and 1 <= turn <= 25:
        ordinal = (span - 1) * 25 + turn - 1
    else:
        raise ValueError("Use spans 1–13 with turns 1–25, or B/1 for Balance")
    return ((start + ordinal) * SECONDS_PER_TURN
            + hour * 3600 + minute * 60 + second)


def from_seconds(seconds: int) -> dict:
    if type(seconds) is not int:
        raise ValueError("Arithmetic authority uses whole SI seconds, not float timestamps")
    elapsed_turn, clock = divmod(seconds, SECONDS_PER_TURN)
    # Mean-cycle estimate followed by boundary checks, also valid before year zero.
    year = elapsed_turn * 115 // 37397
    while year_start_turn(year) > elapsed_turn:
        year -= 1
    while year_start_turn(year + 1) <= elapsed_turn:
        year += 1
    ordinal = elapsed_turn - year_start_turn(year)
    span, turn = ("B", 1) if ordinal == 325 else (ordinal // 25 + 1, ordinal % 25 + 1)
    hour, remainder = divmod(clock, 3600)
    minute, second = divmod(remainder, 60)
    return dict(year=year, span=span, turn=turn,
                hour=hour, minute=minute, second=second)


def daylight_hours(latitude_deg: float, declination_deg: float) -> float:
    """Frozen-declination, geometric center above unobstructed horizon duration."""
    latitude, declination = map(math.radians, (latitude_deg, declination_deg))
    crossing = -math.tan(latitude) * math.tan(declination)
    if crossing <= -1:
        return SOLAR_DAY_HOURS
    if crossing >= 1:
        return 0.0
    return SOLAR_DAY_HOURS * math.acos(crossing) / math.pi


def parent_geometry(latitude_deg: float, longitude_deg: float) -> tuple[float, float]:
    """Topocentric center elevation and angular radius on the reference sphere."""
    latitude, longitude = map(math.radians, (latitude_deg, longitude_deg))
    dot = math.cos(latitude) * math.cos(longitude)
    distance = math.sqrt(PARENT_DISTANCE_KM**2 + REFERENCE_RADIUS_KM**2
                         - 2*PARENT_DISTANCE_KM*REFERENCE_RADIUS_KM*dot)
    elevation = math.asin((PARENT_DISTANCE_KM*dot - REFERENCE_RADIUS_KM)/distance)
    radius = math.asin(PARENT_RADIUS_KM / distance)
    return math.degrees(elevation), math.degrees(radius)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def verify() -> dict:
    checks = 0
    for year in range(-1000, 2001):
        require(year_length(year) in (325, 326), f"Invalid year length: {year}")
        require(year_start_turn(year + 115) - year_start_turn(year) == 37397,
                f"Measure-cycle drift: {year}")
        for offset in (-1, 0, 1, year_length(year)*SECONDS_PER_TURN-1):
            seconds = year_start_turn(year)*SECONDS_PER_TURN + offset
            require(to_seconds(**from_seconds(seconds)) == seconds,
                    f"Boundary roundtrip failed: {seconds}")
        checks += 6
    for year in (0, 2, 114, 115, 739, 742, 743, 10000, -1, -115):
        for ordinal in range(year_length(year)):
            for clock in (0, 28799, 28800, 57599, 57600, 86399):
                seconds = (year_start_turn(year)+ordinal)*SECONDS_PER_TURN + clock
                require(to_seconds(**from_seconds(seconds)) == seconds,
                        "Turn/watch roundtrip failed")
                checks += 1
    require(to_seconds(0, 1, 1) == 0, "Landing epoch changed")
    require(year_start_turn(742) == 241292, "Year 742 epoch changed")
    require(to_seconds(**from_seconds(-1)) == -1, "Pre-epoch boundary failed")
    checks += 3
    invalid = [dict(year=742, span="B"), dict(year=742, span=14),
               dict(year=742, span=1, turn=26), dict(year=742, span=0),
               dict(year=742, span=1, hour=24), dict(year=742, span=1, second=60),
               dict(year=True, span=1), dict(year=742, span=1, turn=True)]
    for arguments in invalid:
        try:
            to_seconds(**arguments)
        except ValueError:
            checks += 1
        else:
            raise RuntimeError(f"Invalid civil date accepted: {arguments}")

    for latitude in (-82, -61, -49, -17, 0, 17, 49, 61, 82):
        north = daylight_hours(latitude, SEASONAL_TILT_DEG)
        south = daylight_hours(latitude, -SEASONAL_TILT_DEG)
        require(abs(north+south-SOLAR_DAY_HOURS) < 1e-10, "Hemisphere symmetry failed")
        require(abs(daylight_hours(latitude, 0)-SOLAR_DAY_HOURS/2) < 1e-10,
                "Equinox geometric identity failed")
        checks += 2
    require(daylight_hours(82, SEASONAL_TILT_DEG) == SOLAR_DAY_HOURS,
            "Northern polar summer missing")
    require(daylight_hours(82, -SEASONAL_TILT_DEG) == 0,
            "Northern polar winter missing")
    require(abs(parent_geometry(0, 0)[0]-90) < 1e-10, "Sub-parent zenith failed")
    require(abs(parent_geometry(0, 180)[0]+90) < 1e-10, "Anti-parent nadir failed")
    checks += 4
    phases = seasonal_events(742)
    phase_turns = [event["elapsed_turns_from_civil_year_start"] for event in phases]
    require(all(a < b for a, b in zip(phase_turns, phase_turns[1:])),
            "Season markers out of order")
    require(phase_turns[2]-phase_turns[0] < float(REFERENCE_YEAR_TURNS)/2,
            "Northern season should be shorter with this periapsis convention")
    require(abs(mean_anomaly(0)) < 1e-14, "Periapsis mean anomaly incorrect")
    checks += 3

    atlas = json.loads((ROOT / "Docs/World/Atlas/nacre_atlas.json").read_text(encoding="utf-8-sig"))
    require(atlas["radius_km"] == REFERENCE_RADIUS_KM, "Atlas/model radius mismatch")
    require(len(atlas["cities"]) == 12, "City list changed: review table before publication")
    require(len({city["id"] for city in atlas["cities"]}) == 12, "Duplicate city ID")
    checks += 3
    cities = []
    for city in atlas["cities"]:
        latitude, longitude = city["lat_deg"], city["lon_deg"]
        parent_elevation, parent_radius = parent_geometry(latitude, longitude)
        cities.append(dict(
            id=city["id"], name=city["name"], latitude=latitude, longitude=longitude,
            northern_solstice_light_h=round(daylight_hours(latitude, SEASONAL_TILT_DEG), 3),
            equinox_light_h=round(SOLAR_DAY_HOURS/2, 3),
            southern_solstice_light_h=round(daylight_hours(latitude, -SEASONAL_TILT_DEG), 3),
            rauth_center_elevation_deg=round(parent_elevation, 3),
            rauth_upper_limb_deg=round(parent_elevation+parent_radius, 3),
            rauth_status=("whole disc above geometric horizon" if parent_elevation > parent_radius else
                          "whole disc below geometric horizon" if parent_elevation < -parent_radius else
                          "disc intersects geometric horizon")))
    rational_year = Fraction(37397, 115)
    error742 = Fraction(year_start_turn(742)) - 742*REFERENCE_YEAR_TURNS
    max_error = max(abs(Fraction(year_start_turn(y))-y*REFERENCE_YEAR_TURNS)
                    for y in range(1001))
    require(max_error < Fraction(53, 100), "Calendar phase error exceeds documented bound")
    checks += 1
    return dict(
        status="passed", assertion_count=checks,
        scope="civil arithmetic and idealized sky geometry; no climate or orbital integration",
        civil_epoch="AL 0.01.01 00:00:00 Common Time = adopted Landing timestamp",
        second_minus_one=from_seconds(-1),
        year742_start_turn=year_start_turn(742),
        year742_length_turns=year_length(742),
        intercalary_years_first_cycle=[y for y in range(115) if year_length(y)==326],
        intercalary_years_near_present=[y for y in range(730, 751) if year_length(y)==326],
        mean_calendar_year_turns=float(rational_year),
        mean_calendar_minus_reference_seconds_per_year=float((rational_year-REFERENCE_YEAR_TURNS)*86400),
        year742_start_minus_reference_return_minutes=float(error742*1440),
        maximum_absolute_year_boundary_error_turns_0_to_1000=float(max_error),
        example_date=dict(year=742, span=4, turn=12, hour=17, minute=20, second=0),
        example_elapsed_seconds=to_seconds(742, 4, 12, 17, 20),
        epoch742_standard_years=year_start_turn(742)/365.25,
        reference_orbital742_standard_years=float(742*REFERENCE_YEAR_TURNS/Fraction("365.25")),
        solar_cycle_civil_turns=SOLAR_DAY_HOURS/24,
        approximate_reference_seasons_year742=phases,
        city_sky_table=cities)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", action="store_true", help="emit the complete machine-readable report")
    arguments = parser.parse_args()
    report = verify()
    if arguments.json:
        print(json.dumps(report, ensure_ascii=False, indent=2))
    else:
        print(f"PASS: {report['assertion_count']} civil/sky arithmetic assertions")
        print(f"742 AL starts at elapsed civil turn {report['year742_start_turn']}; "
              f"length {report['year742_length_turns']} turns")
        print(f"Reference-return offset: {report['year742_start_minus_reference_return_minutes']:.3f} minutes")
        print("Scope: civil arithmetic and illustrative sky geometry; not climate or orbital validation.")
