"""Fetch the deliberately small Poly Haven material set, preserving source bytes and hashes.

Uses the public Poly Haven API with an identifying user agent. No runtime network use.
Run with Python 3 from any directory. Existing verified files are reused.
"""
from pathlib import Path
import hashlib
import json
import urllib.request

ROOT = Path(__file__).resolve().parents[1]
DEST = ROOT / 'SourceArt' / 'PolyHaven'
REGISTER = ROOT / 'Docs' / 'Registers' / 'city-material-intake.json'
ASSETS = ['plastered_wall_02', 'seaworn_sandstone_brick', 'cobblestone_floor_08', 'denmin_fabric_02']
HEADERS = {'User-Agent': 'BorrowedSunAssetIntake/0.3 (local solo game prototype; Poly Haven)'}

def read(url):
    with urllib.request.urlopen(urllib.request.Request(url, headers=HEADERS), timeout=90) as response:
        return response.read()

records = []
for asset in ASSETS:
    files = json.loads(read('https://api.polyhaven.com/files/' + asset))
    info = json.loads(read('https://api.polyhaven.com/info/' + asset))
    folder = DEST / asset
    folder.mkdir(parents=True, exist_ok=True)
    record = {'id': asset, 'name': info.get('name'), 'authors': info.get('authors'),
              'source': 'https://polyhaven.com/a/' + asset, 'license': 'CC0-1.0',
              'licenseSource': 'https://polyhaven.com/license', 'resolution': '2k',
              'costEUR': 0, 'maps': []}
    for kind in ['Diffuse', 'nor_dx', 'Rough']:
        entry = files[kind]['2k']['jpg']
        filename = entry['url'].rsplit('/', 1)[1]
        path = folder / filename
        if not path.exists():
            payload = read(entry['url'])
            if hashlib.md5(payload).hexdigest() != entry['md5']:
                raise RuntimeError('Source hash mismatch: ' + filename)
            path.write_bytes(payload)
        payload = path.read_bytes()
        assert hashlib.md5(payload).hexdigest() == entry['md5'], filename
        record['maps'].append({'kind': kind, 'path': path.relative_to(ROOT).as_posix(),
                               'url': entry['url'], 'bytes': len(payload),
                               'sha256': hashlib.sha256(payload).hexdigest(), 'sourceMD5': entry['md5']})
    records.append(record)
    print('Verified Poly Haven:', asset, flush=True)
REGISTER.write_text(json.dumps({'schemaVersion': 1, 'reviewedDate': '2026-10-12',
    'provider': 'Poly Haven', 'licenseDecision': 'CC0 permits commercial use, modification and redistribution; imported source maps retain original bytes.',
    'apiTerms': 'https://github.com/Poly-Haven/Public-API/blob/master/ToS.md',
    'assets': records}, indent=2) + '\n', encoding='utf-8')
print('Total source bytes:', sum(m['bytes'] for a in records for m in a['maps']))
