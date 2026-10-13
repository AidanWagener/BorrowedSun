"""Fetch the reviewed CC0 continuous wood scan, preserving original bytes and checksums."""
from pathlib import Path
import hashlib
import json
import urllib.request

ROOT = Path(__file__).resolve().parents[1]
ASSET = 'fine_grained_wood'
FILES = [
    ('diff', 'fine_grained_wood_col_2k.png', 5970519, '7762bdf53e42e7aa622140788bc5b291'),
    ('normal', 'fine_grained_wood_nor_gl_2k.png', 3210822, 'c29616d3509274d1ba7c3fa7a111045f'),
    ('rough', 'fine_grained_wood_rough_2k.png', 2103115, '470f4054f357efccd166b6683c621c06'),
]
records = []
maps = {}
for kind, filename, expected_bytes, expected_md5 in FILES:
    path = ROOT / 'SourceArt/PolyHaven' / ASSET / filename
    url = f'https://dl.polyhaven.org/file/ph-assets/Textures/png/2k/{ASSET}/{filename}'
    if path.exists():
        payload = path.read_bytes()
    else:
        request = urllib.request.Request(url, headers={
            'User-Agent': 'BorrowedSunAssetIntake/0.4 (local solo game prototype; Poly Haven)'})
        with urllib.request.urlopen(request, timeout=120) as response:
            payload = response.read(expected_bytes + 1)
    if len(payload) != expected_bytes or hashlib.md5(payload).hexdigest() != expected_md5:
        raise RuntimeError('Provider size/checksum mismatch: ' + filename)
    if not path.exists():
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(payload)
    relative = path.relative_to(ROOT).as_posix()
    maps[kind] = relative
    records.append(dict(kind=kind, path=relative, url=url, bytes=len(payload),
                        sourceMD5=expected_md5, sha256=hashlib.sha256(payload).hexdigest()))
    print('Verified', kind, filename, flush=True)

register = dict(
    schemaVersion=1, reviewedDate='2026-10-13', id=ASSET, name='Fine Grained Wood',
    author='Rob Tuytel', source=f'https://polyhaven.com/a/{ASSET}',
    fileMetadata=f'https://api.polyhaven.com/files/{ASSET}',
    infoMetadata=f'https://api.polyhaven.com/info/{ASSET}',
    license='CC0-1.0', licenseSource='https://polyhaven.com/license', costEUR=0,
    licenseDecision='CC0 permits commercial use, modification and redistribution; original scan bytes retained.',
    apiTerms='https://github.com/Poly-Haven/Public-API/blob/master/ToS.md',
    purpose='Continuous grain for original Second Dawn joinery; no baked nails or plank seams.',
    resolution='2k', physicalSizeCm=[60, 60], grainAxis='V',
    scaleEvidence='Current asset page width 0.6m and API dimensions about 600x600mm agree; legacy API scale string 30x30CM conflicts and is not used.',
    normalConvention='OpenGL', flipNormalGreen=True, maps=maps, files=records)
(ROOT / 'Docs/Registers/workshop-surface-intake.json').write_text(
    json.dumps(register, indent=2) + '\n', encoding='utf-8', newline='\n')
