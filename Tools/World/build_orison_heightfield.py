"""Build Orison's first canonical macro terrain without external GIS dependencies.

Run with the pinned Python runtime. Outputs are authored source data, not Unreal
assets. Heights are altitude above the planning sea datum. The R16's first row
is SOUTH (minimum city-local Y); columns ascend east. No spherical curvature is
applied to this small, explicitly local engineering patch.
"""
from __future__ import annotations

import argparse
import copy
import hashlib
import json
import math
from pathlib import Path
import sys

import numpy as np
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "Docs/World/Cities/orison_city_plan.json"
OUTPUT = ROOT / "SourceArt/World/Orison"
STEM = "Orison_MacroTerrain_1009"
SIZE = 1009
BOUNDS = (-13500.0, 13500.0, -12000.0, 12000.0)
ORIGIN = np.array([-4754.3, -1365.3, 91.92], dtype=np.float64)
PAD_RADIUS = 100.0
PAD_TRANSITION_END = 280.0
PAD_HEIGHT = 91.52
RBF_RADIUS = 3500.0
DX = (BOUNDS[1] - BOUNDS[0]) / (SIZE - 1)
DY = (BOUNDS[3] - BOUNDS[2]) / (SIZE - 1)


def smoothstep(value):
    t = np.clip(value, 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def segment_distance(points, line):
    """Distance to the closest nondegenerate segment; no closure implied."""
    result = np.full(len(points), np.inf)
    for a, b in zip(line[:-1], line[1:]):
        direction = b - a
        norm = np.dot(direction, direction)
        if norm <= 0:
            continue
        t = np.clip(((points - a) @ direction) / norm, 0.0, 1.0)
        result = np.minimum(result, np.linalg.norm(points - a - t[:, None] * direction, axis=1))
    return result


def in_polygon(points, polygon):
    """Even/odd water classification; segments on the east edge close the bay."""
    x, y = points[:, 0], points[:, 1]
    inside = np.zeros(len(points), dtype=bool)
    for a, b in zip(polygon[:-1], polygon[1:]):
        if a[1] == b[1]:
            continue
        crosses = (a[1] > y) != (b[1] > y)
        intercept = (b[0] - a[0]) * (y - a[1]) / (b[1] - a[1]) + a[0]
        inside ^= crosses & (x <= intercept)
    return inside


def wendland(distance):
    r = np.clip(distance / RBF_RADIUS, 0.0, 1.0)
    return (1.0 - r) ** 4 * (4.0 * r + 1.0)


class Terrain:
    def __init__(self, canon):
        self.canon = canon
        self.rim = np.array(canon["crater"]["surviving_rim_trace_local_m"], dtype=np.float64)
        self.water_polygon = np.array(canon["crater"]["bay_water_display_polygon_local_m"], dtype=np.float64)[:, :2]
        coast = np.array(canon["crater"]["bay_coastline_local_m"], dtype=np.float64)[:, :2]
        # Extend the north/south shore to the boundary; the east closing line is
        # an OPEN SEA boundary, never a fabricated beach or a 5.4 km dam.
        self.coast = np.vstack(([BOUNDS[1], coast[0, 1]], coast, [BOUNDS[1], coast[-1, 1]]))
        self.dry_pois = [p for p in canon["points_of_interest"] if p.get("placement_type") != "supported_overwater_structure"]
        self.excluded_pois = [p for p in canon["points_of_interest"] if p.get("placement_type") == "supported_overwater_structure"]
        self.control_points = np.vstack((self.rim, [p["position_local_m"] for p in self.dry_pois]))
        if np.any(in_polygon(self.control_points[:, :2], self.water_polygon)):
            raise ValueError("A dry terrain control lies in canonical bay water; reconcile the source rather than filling the bay")
        self.angles = np.arctan2(self.rim[:, 1] / 11000.0, self.rim[:, 0] / 12000.0)
        order = np.argsort(self.angles)
        self.angles, self.rim_heights = self.angles[order], self.rim[order, 2]
        self.angles = np.r_[self.angles[-1] - 2 * math.pi, self.angles, self.angles[0] + 2 * math.pi]
        self.rim_heights = np.r_[self.rim_heights[-1], self.rim_heights, self.rim_heights[0]]
        xy = self.control_points[:, :2]
        baseline, _, shore_distance = self.base(xy)
        coast_gate = 1.0 - np.exp(-(shore_distance / 100.0) ** 2)
        residual = (self.control_points[:, 2] - baseline) / coast_gate
        matrix = wendland(np.linalg.norm(xy[:, None, :] - xy[None, :, :], axis=2))
        self.weights = np.linalg.solve(matrix, residual)
        self.condition_number = float(np.linalg.cond(matrix))

    def base(self, points):
        x, y = points[:, 0], points[:, 1]
        q = np.hypot(x / 12000.0, y / 11000.0)
        theta = np.arctan2(y / 11000.0, x / 12000.0)
        crest = np.interp(theta, self.angles, self.rim_heights)
        shore_distance = segment_distance(points, self.coast)
        wet = in_polygon(points, self.water_polygon)
        # Round the crest over 150 m rather than making a one-vertex sharp
        # mathematical ridge, especially beside the steep eastern breach.
        ridge_distance = np.hypot((q - 1.0) * 11500.0, 150.0) - 150.0
        fraction = shore_distance / np.maximum(shore_distance + ridge_distance, 0.001)
        inner = crest * fraction ** 1.45
        outer = crest * (0.42 + 0.58 * np.exp(-ridge_distance / 2400.0))
        terrain = np.where(q <= 1.0, inner, outer) * (1.0 - np.exp(-(shore_distance / 125.0) ** 2))
        # Bounded, deterministic sculpted folds. These imply geological form,
        # not simulated erosion, drainage correctness, roads or parcel grading.
        relief = (55 * np.sin(x / 870.0 + 0.7 * np.sin(y / 1800.0)) * np.sin(y / 1650.0)
                  + 22 * np.sin(x / 390.0 - y / 720.0)
                  + 8 * np.sin(x / 175.0 + y / 280.0))
        relief *= smoothstep(shore_distance / 950.0) * smoothstep(terrain / 350.0)
        terrain += relief
        # Sheltered bay with a broad deeper eastern exit. Explicitly authored
        # bathymetry, with no dredging, navigation or ecological certification.
        depth = 225.0 * (1.0 - np.exp(-shore_distance / 1800.0))
        depth *= 0.90 + 0.10 * np.sin(x / 2900.0) ** 2
        return np.where(wet, -depth, terrain), wet, shore_distance

    def evaluate(self, points, preserve_pad=True):
        height, wet, shore_distance = self.base(points)
        field = wendland(np.linalg.norm(points[:, None, :] - self.control_points[None, :, :2], axis=2)) @ self.weights
        height = np.where(wet, height, height + field * (1.0 - np.exp(-(shore_distance / 100.0) ** 2)))
        # Keep shallow land above water; correction residuals may not create
        # unplanned inland seas. This floor tends continuously to sea level.
        height = np.where(wet, height, np.maximum(height, 0.003 * np.minimum(shore_distance, 1000.0)))
        if preserve_pad:
            radius = np.linalg.norm(points - ORIGIN[:2], axis=1)
            factor = smoothstep((radius - PAD_RADIUS) / (PAD_TRANSITION_END - PAD_RADIUS))
            height = PAD_HEIGHT * (1.0 - factor) + height * factor
        return height


def bilinear(height, xy):
    xy = np.atleast_2d(xy)
    px = (xy[:, 0] - BOUNDS[0]) / DX
    py = (xy[:, 1] - BOUNDS[2]) / DY
    ix = np.minimum(np.floor(px).astype(int), SIZE - 2)
    iy = np.minimum(np.floor(py).astype(int), SIZE - 2)
    if np.any(ix < 0) or np.any(iy < 0) or np.any(px > SIZE - 1) or np.any(py > SIZE - 1):
        raise ValueError("Sample outside terrain grid")
    fx, fy = px - ix, py - iy
    return ((1 - fx) * (1 - fy) * height[iy, ix] + fx * (1 - fy) * height[iy, ix + 1]
            + (1 - fx) * fy * height[iy + 1, ix] + fx * fy * height[iy + 1, ix + 1])


def validate(terrain, raw, height, generated, points):
    """Independent datum/sample, placement and coastline acceptance checks."""
    assert raw.dtype == np.dtype('<u2') and raw.shape == (SIZE, SIZE)
    assert raw.nbytes == SIZE * SIZE * 2
    assert not np.any(raw == 0) and not np.any(raw == 65535), "Clipped height range"
    assert np.max(np.abs(height - generated)) <= 0.031251, "R16 conversion error exceeds half a height step"
    assert np.isfinite(height).all()
    assert np.allclose(ORIGIN + np.array([-2570, -470, 8]) / 100.0, [-4780, -1370, 92]), "Workshop anchor transform regression"
    assert np.allclose(points[0], [BOUNDS[0], BOUNDS[2]])
    assert np.allclose(points[-1], [BOUNDS[1], BOUNDS[3]])
    assert np.allclose(points[SIZE - 1], [BOUNDS[1], BOUNDS[2]]), "Row stride/orientation error"
    rim_values = bilinear(height, terrain.rim[:, :2])
    rim_error = np.abs(rim_values - terrain.rim[:, 2])
    assert max(rim_error) < 8.0, f"Rim sampling residual {max(rim_error):.2f} m"
    poi_rows = []
    for poi in terrain.dry_pois:
        xyz = np.array(poi["position_local_m"], dtype=float)
        sampled = float(bilinear(height, xyz[:2])[0])
        difference = sampled - xyz[2]
        assert abs(difference) < 2.0, f"Dry POI residual {poi['id']}: {difference:.2f} m"
        poi_rows.append({"id": poi["id"], "target_altitude_m": xyz[2], "sampled_grid_altitude_m": round(sampled, 4), "residual_m": round(difference, 4)})
    # Dense coastline samples and side probes catch reversed rows, unclosed
    # polygons, unsigned encoding and a false closing beach at the east edge.
    coast_samples, sides = [], []
    for a, b in zip(terrain.coast[:-1], terrain.coast[1:]):
        length = np.linalg.norm(b - a)
        if length < 0.01:
            continue
        count = max(2, math.ceil(length / 75.0))
        t = (np.arange(count) + 0.5) / count
        center = a + t[:, None] * (b - a)
        normal = np.array([-(b - a)[1], (b - a)[0]]) / length
        coast_samples.extend(center)
        sides.extend(center + normal * 55.0)
        sides.extend(center - normal * 55.0)
    coast_samples = np.array(coast_samples)
    coastal = bilinear(height, coast_samples)
    # A raster with ~26 m samples cannot preserve a vector shore to millimetres.
    assert max(abs(coastal)) < 9.0, f"Grid coastline residual {max(abs(coastal)):.2f} m"
    sides = np.array(sides)
    sides = sides[(sides[:, 0] >= BOUNDS[0]) & (sides[:, 0] <= BOUNDS[1]) & (sides[:, 1] >= BOUNDS[2]) & (sides[:, 1] <= BOUNDS[3])]
    sampled_sides = bilinear(height, sides)
    wet_sides = in_polygon(sides, terrain.water_polygon)
    assert np.all(sampled_sides[wet_sides] < 0), "Water-side shore probe emerges as land"
    assert np.all(sampled_sides[~wet_sides] > 0), "Land-side shore probe submerged"
    east_probe = np.array([[13499, 0], [12500, 0], [3000, 0]], dtype=float)
    assert np.all(bilinear(height, east_probe) < -50), "Open bay exit obstructed"
    for poi in terrain.excluded_pois:
        assert bilinear(height, np.array(poi["position_local_m"][:2]))[0] < 0, "Supported structure filled the bay"
    # Main quay plus margin. The 100 m flat disk leaves >35 m between this
    # test rectangle and the transition, beyond one grid diagonal (~36 m).
    pad_probes = np.array([[ORIGIN[0] + x, ORIGIN[1] + y] for x in np.linspace(-43, 43, 9) for y in np.linspace(-34, 34, 9)])
    pad_height = bilinear(height, pad_probes)
    assert np.max(abs(pad_height - PAD_HEIGHT)) < 0.032
    assert np.all(pad_height < ORIGIN[2] - 0.30), "Terrain penetrates existing quay floor"
    gradient_y, gradient_x = np.gradient(height, DY, DX)
    slope = np.degrees(np.arctan(np.hypot(gradient_x, gradient_y)))
    assert np.max(slope) < 83.0, "Unbounded terrain spike"
    return {
        "passed": True,
        "r16_byte_count": raw.nbytes,
        "height_quantization_max_error_m": round(float(np.max(abs(height - generated))), 6),
        "rim_control_max_grid_residual_m": round(float(max(rim_error)), 4),
        "coast_sample_count": len(coast_samples),
        "coast_grid_max_abs_altitude_m": round(float(max(abs(coastal))), 4),
        "coast_side_probe_count": len(sides),
        "all_coast_side_probes_have_correct_sign": True,
        "dry_poi_grid_residuals": poi_rows,
        "preserved_quay_grid_altitude_m": round(float(pad_height[0]), 4),
        "slope_degrees": {"maximum": round(float(np.max(slope)), 3), "percentile_99": round(float(np.percentile(slope, 99)), 3)},
        "unreal_import_collision_render_and_play_verified": False,
    }


def plate(height, terrain, destination):
    """Static geographic review plate with hillshade and true source markers."""
    stops = np.array([-250, -90, 0, 1, 80, 320, 650, 1000, 1500, 1800], dtype=float)
    colors = np.array([[18, 46, 62], [24, 78, 92], [63, 119, 125], [189, 185, 127], [147, 158, 95],
                       [116, 143, 91], [152, 151, 112], [169, 153, 135], [204, 191, 172], [230, 221, 207]], dtype=float)
    rgb = np.stack([np.interp(height, stops, colors[:, c]) for c in range(3)], axis=-1)
    gy, gx = np.gradient(height, DY, DX)
    normal = np.stack((-gx, -gy, np.ones_like(height)), axis=-1)
    normal /= np.linalg.norm(normal, axis=-1)[..., None]
    illumination = np.sum(normal * np.array([-0.45, 0.55, 0.704]), axis=-1)
    shade = np.clip(0.61 + 0.47 * illumination, 0.40, 1.11)
    rgb *= shade[..., None]
    # Grid samples are anisotropic. Preserve real geographic aspect ratio in
    # the review plate rather than silently depicting a 27 x 24 km square.
    map_height = round(SIZE * 24000.0 / 27000.0)
    map_image = Image.fromarray(np.uint8(np.clip(rgb[::-1], 0, 255))).resize((SIZE, map_height), Image.Resampling.BILINEAR)
    canvas = Image.new('RGB', (1330, map_height + 170), (18, 27, 33))
    canvas.paste(map_image, (65, 95))
    draw = ImageDraw.Draw(canvas)
    font_path = Path('C:/Windows/Fonts/segoeui.ttf')
    font = ImageFont.truetype(str(font_path), 19) if font_path.exists() else ImageFont.load_default()
    small = ImageFont.truetype(str(font_path), 15) if font_path.exists() else ImageFont.load_default()
    large = ImageFont.truetype(str(font_path), 28) if font_path.exists() else ImageFont.load_default()
    def pixel(xy):
        return (65 + (xy[0] - BOUNDS[0]) / (BOUNDS[1] - BOUNDS[0]) * (SIZE - 1),
                95 + (BOUNDS[3] - xy[1]) / (BOUNDS[3] - BOUNDS[2]) * (map_height - 1))
    draw.text((65, 24), 'ORISON / FIRST MACRO TERRAIN', fill='#ecede0', font=large)
    draw.text((65, 62), '27 x 24 km   |   1009 x 1009 samples   |   source terrain, not a gameplay screenshot', fill='#a5bdc6', font=small)
    draw.line([pixel(p) for p in terrain.coast], fill='#cce3e7', width=2)
    for point in terrain.rim:
        px, py = pixel(point)
        draw.ellipse((px - 3, py - 3, px + 3, py + 3), fill='#fff0c6')
    for poi in terrain.dry_pois:
        px, py = pixel(poi['position_local_m'])
        draw.ellipse((px - 3, py - 3, px + 3, py + 3), outline='#f9d25b', width=2)
    for poi in terrain.excluded_pois:
        px, py = pixel(poi['position_local_m'])
        draw.line((px - 4, py - 4, px + 4, py + 4), fill='#67dcf8', width=2)
        draw.line((px - 4, py + 4, px + 4, py - 4), fill='#67dcf8', width=2)
    px, py = pixel(ORIGIN[:2])
    draw.ellipse((px - 7, py - 7, px + 7, py + 7), outline='#ffffff', width=2)
    draw.line((px - 8, py, px - 85, py + 36), fill='#ffffff', width=1)
    draw.text((px - 225, py + 32), 'Second Dawn / preserved quay', fill='#ffffff', font=small)
    draw.text((1098, 110), 'NORTH', fill='#e9ede7', font=font)
    draw.line((1134, 192, 1134, 147), fill='#e9ede7', width=3)
    draw.polygon(((1128, 158), (1134, 145), (1140, 158)), fill='#e9ede7')
    labels = [('Sea datum', '0 m'), ('Highest ground', f'{height.max():.1f} m'), ('Deepest bay', f'{height.min():.1f} m'), ('Rim controls', str(len(terrain.rim))), ('Dry POI controls', str(len(terrain.dry_pois))), ('Decks over water', str(len(terrain.excluded_pois)))]
    y = 235
    for title, value in labels:
        draw.text((1095, y), title, fill='#a5bdc6', font=small)
        draw.text((1095, y + 22), value, fill='#ecede0', font=font)
        y += 66
    draw.text((1095, 675), 'Authored macro relief', fill='#e7cd8b', font=small)
    for i, text in enumerate(['No erosion simulation', 'No surveyed drainage', 'No built city parcels', 'Local sea datum', 'East bay stays open']):
        draw.text((1095, 710 + i * 29), text, fill='#a5bdc6', font=small)
    for km in [-10, -5, 0, 5, 10]:
        px, _ = pixel([km * 1000, 0])
        draw.text((px - 20, map_height + 105), f'{km:+} km', fill='#a5bdc6', font=small)
    for km in [-10, -5, 0, 5, 10]:
        _, py = pixel([0, km * 1000])
        draw.text((3, py - 9), f'{km:+}', fill='#a5bdc6', font=small)
    draw.text((65, map_height + 141), 'Cream dots: canonical rim  /  gold: dry POIs  /  cyan crosses: supported decks, deliberately excluded from ground controls', fill='#a5bdc6', font=small)
    canvas.save(destination)


def negative_tests(terrain, raw, height, points):
    """Corruptions model actual import/authoring mistakes, not mocked success."""
    rejected = []
    def must_reject(name, action):
        try:
            action()
        except (AssertionError, ValueError, np.linalg.LinAlgError):
            rejected.append(name)
        else:
            raise AssertionError('Negative control escaped validation: ' + name)
    def check_corrupted(name, altered):
        encoded = np.rint(altered * 16 + 32768).astype('<u2')
        decoded = (encoded.astype(float) - 32768) / 16
        must_reject(name, lambda: validate(terrain, encoded, decoded, decoded, points))
    check_corrupted('north_south_rows_reversed', height[::-1].copy())
    byte_swapped = np.frombuffer(raw.tobytes(), dtype='>u2').astype('<u2')
    wrong_height = (byte_swapped.astype(float).reshape(SIZE, SIZE) - 32768) / 16
    check_corrupted('big_endian_source_misread_as_little_endian', wrong_height)
    check_corrupted('wrong_sea_datum_plus_10m', height + 10)
    corrupted = height.copy()
    corrupted[np.linalg.norm(points - [3000, 0], axis=1).reshape(SIZE, SIZE) < 150] = 10
    check_corrupted('fabricated_land_in_open_bay', corrupted)
    corrupted = height.copy()
    corrupted[np.linalg.norm(points - ORIGIN[:2], axis=1).reshape(SIZE, SIZE) < 100] = 110
    check_corrupted('quay_pad_buried_by_terrain', corrupted)
    corrupted = height.copy()
    corrupted[0, 0] = (65535 - 32768) / 16
    check_corrupted('saturated_uint16_height', corrupted)
    corrupted = height.copy()
    corrupted[(points[:, 0] > 13450).reshape(SIZE, SIZE) & (abs(points[:, 1]) < 1000).reshape(SIZE, SIZE)] = 20
    check_corrupted('east_edge_closed_with_false_beach', corrupted)
    wrong_canon = copy.deepcopy(terrain.canon)
    for poi in wrong_canon['points_of_interest']:
        if poi['id'] == 'ORI-POI-024':
            poi.pop('placement_type', None)
    must_reject('supported_overwater_deck_used_as_ground', lambda: Terrain(wrong_canon))
    return {'passed': True, 'rejected_corruption_count': len(rejected), 'rejected_corruptions': rejected}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Recompute and validate; fail if checked-in R16 or metadata does not match')
    args = parser.parse_args()
    canon = json.loads(SOURCE.read_text(encoding='utf-8'))
    declared = canon['coordinate_frame']['planning_bounds_local_m']
    assert tuple(declared[k] for k in ['min_x', 'max_x', 'min_y', 'max_y']) == BOUNDS
    terrain = Terrain(canon)
    x, y = np.linspace(BOUNDS[0], BOUNDS[1], SIZE), np.linspace(BOUNDS[2], BOUNDS[3], SIZE)
    grid_x, grid_y = np.meshgrid(x, y)
    points = np.column_stack((grid_x.ravel(), grid_y.ravel()))
    generated = np.empty(len(points))
    for start in range(0, len(points), 8192):
        generated[start:start + 8192] = terrain.evaluate(points[start:start + 8192])
    generated = generated.reshape((SIZE, SIZE))
    encoded = np.rint(generated * 16 + 32768)
    assert encoded.min() > 0 and encoded.max() < 65535, 'Reject overflow, never silently clip'
    raw = encoded.astype('<u2')
    height = (raw.astype(np.float64) - 32768.0) / 16.0
    validation = validate(terrain, raw, height, generated, points)
    validation['negative_controls'] = negative_tests(terrain, raw, height, points)
    payload = raw.tobytes(order='C')
    sha256 = hashlib.sha256(payload).hexdigest()
    metadata = {
        'schemaVersion': 1, 'terrain_id': 'NACRE-ORISON-MACRO-001', 'edition': '0.1',
        'resolution': [SIZE, SIZE], 'engineScaleCm': [DX * 100, DY * 100, 800], 'runtimeOriginMetres': ORIGIN.tolist(),
        'status': 'Authored local macro terrain source; Unreal integration and play verification recorded separately',
        'source_city_plan': str(SOURCE.relative_to(ROOT)).replace('\\', '/'),
        'source_city_plan_sha256': hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
        'generator': 'Tools/World/build_orison_heightfield.py',
        'heightmap_file': STEM + '.r16', 'sha256': sha256,
        'encoding': {'type': 'uint16', 'endianness': 'little', 'header_bytes': 0, 'row_order': 'south to north (ascending city-local y)', 'column_order': 'west to east (ascending city-local x)', 'row_stride_bytes': SIZE * 2, 'height_m': '(raw - 32768) / 16', 'sea_level_raw': 32768, 'quantization_step_m': 0.0625},
        'grid': {'width': SIZE, 'height': SIZE, 'quads_per_component': 63, 'sections_per_component': 1, 'components_x': 16, 'components_y': 16, 'extent_m': [27000, 24000], 'bounds_local_m': declared, 'sample_spacing_m': [DX, DY]},
        'unreal': {'landscape_scale_cm': [DX * 100, DY * 100, 800], 'landscape_actor_location_cm': [(BOUNDS[0] - ORIGIN[0]) * 100, (BOUNDS[2] - ORIGIN[1]) * 100, -ORIGIN[2] * 100], 'runtime_zero_city_local_m': ORIGIN.tolist(), 'city_to_runtime': '(city_local_m - runtime_zero_city_local_m) * 100', 'sea_level_runtime_z_cm': -ORIGIN[2] * 100, 'world_axes': 'X east, Y north, Z altitude relative to runtime zero; local flat patch, not ECEF'},
        'height_range_m': [float(height.min()), float(height.max())],
        'control_counts': {'canonical_rim': len(terrain.rim), 'dry_pois': len(terrain.dry_pois), 'excluded_overwater_pois': len(terrain.excluded_pois)},
        'excluded_overwater_poi_ids': [p['id'] for p in terrain.excluded_pois],
        'surface_method': {'baseline': 'Coast-to-rim interpolation with authored exterior falloff, bounded deterministic sculpted folds and open-bay bathymetry', 'correction': 'Compact support C2 Wendland radial basis correction at rim and dry POIs, faded to zero at the coast', 'rbf_support_radius_m': RBF_RADIUS, 'rbf_matrix_condition_number': round(terrain.condition_number, 5), 'inland_water_policy': 'No new inland seas: positive floor on canonical land', 'shore_resolution_note': 'Canonical vector shore has zero source elevation; coarse R16 interpolation introduces bounded near-shore altitude residuals reported below'},
        'starting_area_preservation': {'center_city_local_m': ORIGIN[:2].tolist(), 'flat_radius_m': PAD_RADIUS, 'transition_end_radius_m': PAD_TRANSITION_END, 'target_ground_altitude_m': PAD_HEIGHT, 'encoded_ground_altitude_m': round(float(bilinear(height, ORIGIN[:2])[0]), 4), 'runtime_ground_z_cm': round(float((bilinear(height, ORIGIN[:2])[0] - ORIGIN[2]) * 100), 4), 'existing_quay_top_runtime_z_cm': 0, 'workshop_position_city_local_m': [-4780, -1370, 92], 'workshop_anchor_runtime_cm': [-2570, -470, 8], 'intentional_exception': 'Local quay pad ground is 0.48 m below workshop deck altitude; retains existing functional prototype geometry'},
        'validation': validation,
        'limitations': ['First sculpted 27 x 24 km macro surface, not a finished city or a whole planet', 'Heightfield cannot model caves, bridges, overhangs or supported water structures', 'Dry POI heights are planning placement anchors used to shape ground, not surveyed foundations', 'Existing prototype landmarks remain compressed; this file establishes only the workshop coordinate anchor', 'No hydraulic erosion, actual drainage validation, vegetation, roads, interiors, settlement parcels or navigation engineering', 'Local flat altitude patch; planet curvature and coordinate handoff need a separate verified runtime layer', 'Approximately 24-27 m horizontal samples require separate finer terrain and meshes for foot-scale detail', 'Terrain relief is bounded authored fiction, not a geological or oceanographic simulation'],
    }
    if args.check:
        if (OUTPUT / (STEM + '.r16')).read_bytes() != payload:
            raise AssertionError('Generated R16 differs from committed source')
        old = json.loads((OUTPUT / (STEM + '.json')).read_text(encoding='utf-8'))
        if old != metadata:
            raise AssertionError('Metadata or source canon has changed; regenerate explicitly')
    else:
        OUTPUT.mkdir(parents=True, exist_ok=True)
        (OUTPUT / (STEM + '.r16')).write_bytes(payload)
        (OUTPUT / (STEM + '.json')).write_text(json.dumps(metadata, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
        plate(height, terrain, OUTPUT / (STEM + '.png'))
    print(json.dumps({'check_only': args.check, 'heightmap': str(OUTPUT / (STEM + '.r16')), 'sha256': sha256, 'min_max_m': metadata['height_range_m'], 'validation': validation}, indent=2))


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    main()
