"""Selective, hash-verified CC0 asset intake for the approved starting-market benchmark."""
from pathlib import Path
import hashlib,json,urllib.request

ROOT=Path(__file__).resolve().parents[1]
REGISTER=ROOT/'Docs/Registers/market-asset-intake.json'
HEADERS={'User-Agent':'BorrowedSunAssetIntake/0.4 (local solo game prototype; Poly Haven)'}

def read(url):
    with urllib.request.urlopen(urllib.request.Request(url,headers=HEADERS),timeout=120) as response:return response.read()

def download(asset,kind,entry):
    name=entry['url'].rsplit('/',1)[1]
    path=ROOT/'SourceArt/PolyHaven'/asset/name
    path.parent.mkdir(parents=True,exist_ok=True)
    if not path.exists():
        payload=read(entry['url'])
        if hashlib.md5(payload).hexdigest()!=entry['md5']:raise RuntimeError('Provider checksum failed: '+name)
        path.write_bytes(payload)
    payload=path.read_bytes()
    if hashlib.md5(payload).hexdigest()!=entry['md5']:raise RuntimeError('Existing file checksum failed: '+name)
    return {'kind':kind,'path':path.relative_to(ROOT).as_posix(),'url':entry['url'],'bytes':len(payload),
            'sha256':hashlib.sha256(payload).hexdigest(),'sourceMD5':entry['md5']}

texture_ids=['monastery_stone_floor','medieval_wood','worn_cracked_plaster','sandstone_cracks']
prop_ids=['wooden_crate_02','wicker_basket_02','food_pomegranate_01','potted_plant_01',
          'ceramic_vase_02','wooden_stool_01','wooden_bowl_01','brass_pan_01','tree_small_02']
records=[]
for asset in texture_ids+prop_ids:
    files=json.loads(read('https://api.polyhaven.com/files/'+asset))
    info=json.loads(read('https://api.polyhaven.com/info/'+asset))
    record={'id':asset,'name':info['name'],'authors':info.get('authors'),'source':'https://polyhaven.com/a/'+asset,
            'license':'CC0-1.0','licenseSource':'https://polyhaven.com/license','costEUR':0,
            'type':'texture' if asset in texture_ids else 'model','resolution':'2k','files':[],'materials':[]}
    if asset in prop_ids:record['files'].append(download(asset,'mesh',files['fbx']['2k']['fbx']))
    if asset=='monastery_stone_floor':record['files'].append(download(asset,'displacement',files['Displacement']['2k']['png']))
    if asset=='potted_plant_01':
        definitions=[('pot',{'diff':'pot_diff','normal':'pot_nor_dx','rough':'pot_rough','ao':'pot_ao'},False,False),
                     ('leaves',{'diff':'leaves_diff','normal':'leaves_nor_gl','rough':'leaves_rough','ao':'leaves_ao','opacity':'leaves_alpha'},True,True)]
    elif asset=='tree_small_02':
        definitions=[('trunk',{'diff':'Diffuse','normal':'nor_dx','rough':'Rough','ao':'AO'},False,False),
            ('branches',{'diff':'branch_diff','normal':'branch_nor_dx','rough':'branch_rough','ao':'branch_ao'},False,False),
            ('leaves',{'diff':'leaves_diff','normal':'leaves_nor_dx','rough':'leaves_rough','ao':'leaves_ao','opacity':'leaves_alpha'},False,True)]
    else:
        maps={'diff':'Diffuse','normal':'nor_dx','rough':'Rough'}
        if 'AO' in files:maps['ao']='AO'
        if 'Metal' in files:maps['metal']='Metal'
        definitions=[('surface',maps,False,False)]
    for mat_name,maps,flip_green,masked in definitions:
        material={'name':mat_name,'flipNormalGreen':flip_green,'masked':masked,'maps':{}}
        for semantic,key in maps.items():
            formats=files[key]['2k'];ext='png' if semantic=='opacity' and 'png' in formats else 'jpg'
            if ext not in formats:ext=next(iter(formats))
            source=download(asset,mat_name+'.'+semantic,formats[ext])
            record['files'].append(source)
            material['maps'][semantic]=source['path']
        record['materials'].append(material)
    records.append(record)
    print('Verified',asset,len(record['files']),'source files',flush=True)
REGISTER.write_text(json.dumps({'schemaVersion':1,'reviewedDate':'2026-10-12','purpose':'Starting market visual benchmark',
    'provider':'Poly Haven','apiTerms':'https://github.com/Poly-Haven/Public-API/blob/master/ToS.md',
    'licenseDecision':'CC0 permits commercial use, modification and redistribution. Original source bytes and hashes retained.',
    'assets':records},indent=2)+'\n',encoding='utf-8')
print('Total source bytes',sum(f['bytes'] for a in records for f in a['files']),flush=True)
