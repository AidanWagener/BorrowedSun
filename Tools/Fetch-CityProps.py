"""Fetch two small CC0 Poly Haven props and their 1K maps for the first city block."""
from pathlib import Path
import hashlib, json, urllib.request
ROOT=Path(__file__).resolve().parents[1]
def read(url):
    req=urllib.request.Request(url,headers={'User-Agent':'BorrowedSunAssetIntake/0.3 (local solo game prototype; Poly Haven)'})
    with urllib.request.urlopen(req,timeout=90) as response:return response.read()
assets=[]
for name in ['brass_pot_01','brass_diya_lantern']:
    files=json.loads(read('https://api.polyhaven.com/files/'+name))
    info=json.loads(read('https://api.polyhaven.com/info/'+name))
    folder=ROOT/'SourceArt/PolyHaven'/name;folder.mkdir(parents=True,exist_ok=True)
    record={'id':name,'name':info['name'],'authors':info.get('authors'),
            'source':'https://polyhaven.com/a/'+name,'license':'CC0-1.0','licenseSource':'https://polyhaven.com/license',
            'costEUR':0,'textureResolution':'1k','files':[]}
    entries=[('mesh',files['fbx']['1k']['fbx'])]+[(kind,files[kind]['1k']['jpg']) for kind in ['Diffuse','nor_dx','Rough','Metal']]
    if name=='brass_diya_lantern':entries.append(('opacity',files['opacity']['1k']['png']))
    for kind,entry in entries:
        filename=entry['url'].rsplit('/',1)[1]; path=folder/filename
        if not path.exists():
            payload=read(entry['url'])
            assert hashlib.md5(payload).hexdigest()==entry['md5'],filename
            path.write_bytes(payload)
        payload=path.read_bytes();assert hashlib.md5(payload).hexdigest()==entry['md5'],filename
        record['files'].append({'kind':kind,'path':path.relative_to(ROOT).as_posix(),'url':entry['url'],
            'bytes':len(payload),'sha256':hashlib.sha256(payload).hexdigest(),'sourceMD5':entry['md5']})
    assets.append(record);print('Verified Poly Haven prop:',name,flush=True)
(ROOT/'Docs/Registers/city-prop-intake.json').write_text(json.dumps({'schemaVersion':1,'reviewedDate':'2026-10-12','provider':'Poly Haven','assets':assets},indent=2)+'\n',encoding='utf-8')
