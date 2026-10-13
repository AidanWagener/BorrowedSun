# Orison macro terrain source

This is the first **27 × 24 km** authored terrain surface for Orison, derived from the city plan. It gives the current starting block a canonical geographic anchor, a coastal basin, a surviving crater rim and underwater bay relief. It is a source heightfield, not a completed city or a whole-planet terrain system.

`Orison_MacroTerrain_1009.r16` is a headerless **1009 × 1009 little-endian unsigned 16-bit** heightfield. Rows ascend city-local **Y from south to north**; columns ascend **X from west to east**. Decode altitude in metres as `(raw − 32768) / 16`. Do not flip rows when importing into Unreal. The PNG is a north-up, geographically proportioned review plate; it intentionally reverses the R16 row presentation for reading.

The adjacent JSON contains the hash, exact transform, dimensions, elevation bounds, control residuals and validation evidence. Its top-level `resolution`, `engineScaleCm`, `runtimeOriginMetres` and `sha256` fields are the importer contract. Native Landscape configuration is 16 × 16 components, one section per component, 63 quads per section. The bottom-left Landscape actor location is **[−874570, −1063470, −9192] cm** with scale **[2678.5714285714, 2380.9523809524, 800]**.

The runtime origin is city-local **[−4754.3, −1365.3, 91.92] m**. Therefore the existing workshop anchor **[−2570, −470, 8] cm** maps to **[−4780, −1370, 92] m**. A 100 m radius preservation pad is flat at an encoded **91.5 m**, 42 cm below the existing quay floor; a smooth transition ends at 280 m. This is an intentional local foundation allowance. It does not move the workshop or reinterpret the whole compressed prototype as a finished canonical city.

The source city plan supplies 42 rim controls and 21 dry POI controls. Three POIs—ORI-POI-012, ORI-POI-021 and ORI-POI-024—are explicitly supported above water, so their deck altitudes are **excluded from ground controls**. The east boundary is an open bay exit. Its polygon-closing line is not a beach or dam.

The generator uses a coast-to-rim macro profile, bounded sculpted folds, authored bathymetry and a compact-support C2 radial-basis correction through the canonical controls. It requires NumPy and Pillow, both available in the pinned workspace Python runtime; it does not require SciPy or external terrain services.

```powershell
& 'C:\Users\aidan\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' Tools/World/build_orison_heightfield.py
& 'C:\Users\aidan\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' Tools/World/build_orison_heightfield.py --check
```

The check regenerates the numeric surface in memory, verifies the bytes and metadata against disk, and exercises eight deliberate corruption cases: row reversal, wrong byte order, wrong sea datum, bay filling, buried quay, saturated height, a false eastern beach and an overwater deck mistaken for ground. The source checks cover coast-side classification, control elevations, quantization, open-sea continuity and the preserved starting footprint. They do not establish Unreal collision, rendering or play correctness; those are separate integration checks.

At approximately 24–27 m between samples, this surface provides **macro terrain only**. Streets, foundations, foot-scale rock detail, caves, supported decks, overhangs, drainage, coastal defenses and city assets require finer authored geometry. The landforms are bounded fiction, not the output of geological erosion or validated hydrology. It is a local flat-altitude patch and has no verified planet-curvature or inter-region coordinate handoff. The sourced coastline is still schematic; the imported grid's near-shore altitude residual is measured in the JSON rather than described as millimetre-accurate.
