"""Original Second Dawn repair-shop kit; pinned Blender, centimetres, front -Y.

FBX import converts the front to Unreal +Y. The recipe uses Unreal coordinates.
Run: blender --background --python Tools/Art/Build-LanternWorkshop.py
"""
from pathlib import Path
import bpy, bmesh, math, json, hashlib
from mathutils import Vector

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'SourceArt/LanternWorkshop'
OUT.mkdir(parents=True, exist_ok=True)
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
bpy.context.scene.unit_settings.system = 'METRIC'
bpy.context.scene.unit_settings.scale_length = .01
MATS = {}
for name, color, metal, rough in [
    ('Wood', (.19,.10,.045,1),0,.65), ('Brass',(.55,.32,.095,1),.85,.32),
    ('Copper',(.10,.27,.21,1),.7,.47), ('Shadow',(.025,.035,.039,1),0,.96),
    ('Enamel',(.026,.12,.16,1),.25,.31), ('Iron',(.075,.085,.085,1),.8,.52),
    ('Ivory',(.72,.60,.36,1),0,.57), ('MilkGlass',(.82,.68,.39,1),0,.25)]:
    m=bpy.data.materials.new(name); m.diffuse_color=color; m.use_nodes=True
    p=m.node_tree.nodes.get('Principled BSDF'); p.inputs['Base Color'].default_value=color
    p.inputs['Metallic'].default_value=metal; p.inputs['Roughness'].default_value=rough
    MATS[name]=m
parts=[]; assets=[]

def geom(name, verts, faces, mat, smooth=False):
    d=bpy.data.meshes.new(name); d.from_pydata(verts,[],faces); d.update()
    bm=bmesh.new(); bm.from_mesh(d); bmesh.ops.recalc_face_normals(bm,faces=bm.faces); bm.to_mesh(d); bm.free()
    ob=bpy.data.objects.new(name,d); bpy.context.collection.objects.link(ob); d.materials.append(MATS[mat])
    for p in d.polygons:p.use_smooth=smooth
    parts.append(ob); return ob

def box(p,s,mat='Wood',edge=.6):
    v=[tuple(p[k]+sign[k]*s[k]/2 for k in range(3)) for sign in [(-1,-1,-1),(1,-1,-1),(1,1,-1),(-1,1,-1),(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]]
    ob=geom('Joinery',v,[(0,3,2,1),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)],mat)
    if edge:
        b=ob.modifiers.new('Rounded working edges','BEVEL'); b.width=edge; b.segments=3
        n=ob.modifiers.new('Face normals','WEIGHTED_NORMAL'); n.keep_sharp=True
    return ob

def lathe(profile,mat='Brass',p=(0,0,0),n=48):
    v=[(p[0]+r*math.cos(i*math.tau/n),p[1]+r*math.sin(i*math.tau/n),p[2]+z) for r,z in profile for i in range(n)]
    f=[(j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i) for j in range(len(profile)-1) for i in range(n)]
    return geom('Turned metal',v,f,mat,True)

def tube(points,r,mat='Brass',closed=False):
    d=bpy.data.curves.new('Bent tube','CURVE'); d.dimensions='3D'; d.bevel_depth=r; d.bevel_resolution=2
    s=d.splines.new('POLY'); s.points.add(len(points)-1)
    for v,p in zip(s.points,points):v.co=(*p,1)
    s.use_cyclic_u=closed
    ob=bpy.data.objects.new('Bent tube',d); bpy.context.collection.objects.link(ob); d.materials.append(MATS[mat]); parts.append(ob); return ob

def circle(x,y,z,r,mat='Brass',wire=.7):
    return tube([(x+r*math.cos(i*math.tau/64),y,z+r*math.sin(i*math.tau/64)) for i in range(64)],wire,mat,True)

def rivet(x,y,z):
    # Flattened, chamfered head on a front-facing fitting.
    box((x,y,z),(2.4,1.1,2.4),'Brass',.5)

def lettering(text,p,size,mat='Ivory'):
    d=bpy.data.curves.new('Cast lettering','FONT'); d.body=text; d.size=size; d.align_x='CENTER'; d.align_y='CENTER'
    d.extrude=.22; d.bevel_depth=.07; d.bevel_resolution=1; d.resolution_u=4
    ob=bpy.data.objects.new('Cast lettering',d); bpy.context.collection.objects.link(ob)
    ob.location=p; ob.rotation_euler=(math.pi/2,0,0); d.materials.append(MATS[mat]); parts.append(ob)

def finish(name,collision=False):
    bpy.ops.object.select_all(action='DESELECT')
    for ob in parts:
        ob.select_set(True); bpy.context.view_layer.objects.active=ob
        if ob.type!='MESH':bpy.ops.object.convert(target='MESH')
        for m in list(ob.modifiers):bpy.ops.object.modifier_apply(modifier=m.name)
        ob.select_set(False)
    for ob in parts:ob.select_set(True)
    bpy.context.view_layer.objects.active=parts[0]; bpy.ops.object.join(); ob=bpy.context.object; ob.name=name
    bpy.context.scene.cursor.location=(0,0,0); bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
    layer=ob.data.uv_layers.new(name='UVMap')
    for p in ob.data.polygons:
        axes=[i for i in range(3) if i!=max(range(3),key=lambda k:abs(p.normal[k]))]
        for j in p.loop_indices:
            v=ob.data.vertices[ob.data.loops[j].vertex_index].co; layer.data[j].uv=(v[axes[0]]/120,v[axes[1]]/120)
    path=OUT/(name+'.fbx')
    bpy.ops.export_scene.fbx(filepath=str(path),use_selection=True,object_types={'MESH'},global_scale=1,
        apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        mesh_smooth_type='FACE',use_mesh_modifiers=True,add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
    ob.data.calc_loop_triangles()
    assets.append(dict(name=name,path=path.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
        dimensionsCm=list(ob.dimensions),triangles=len(ob.data.loop_triangles),collision=collision))
    ob.location=((len(assets)-1)%4*500,((len(assets)-1)//4)*500,0)
    parts.clear(); print('EXPORTED',name,assets[-1]['triangles'],flush=True)

# A heavy repair bench: separate boards, pegged joints, drawers, tool well and vice.
for x in [-137,137]:
    for y in [-37,37]:
        box((x,y,47),(13,13,94),edge=1.1)
        box((x,y,7),(15,15,14),'Iron',.7)
    box((x,0,30),(9,75,9))
box((0,0,28),(278,7,12))
for i in range(5):box((0,-37+i*18.5,98),(302,18,8),edge=1)
for x in [-101,0,101]:
    box((x,0,76),(95,75,30),'Wood',.8)
    box((x,-40,76),(94,4,26),'Enamel',.8)
    for z in [65,87]:box((x,-43,z),(88,2,1.5),'Brass',.2)
    tube([(x-10,-44,79),(x-10,-48,76),(x+10,-48,76),(x+10,-44,79)],1.3,'Brass')
    for dx in [-41,41]:
        for z in [66,86]:rivet(x+dx,-44,z)
for x in [-145,145]:
    box((x,0,103),(10,90,1.4),'Iron',.3)
    for y in [-34,34]:box((x,y,104),(2.6,2.6,1.3),'Brass',.4)
# Vice and its threaded adjustment screw.
for x in [89,110]:box((x,-40,109),(9,26,21),'Iron',.7)
tube([(82,-41,107),(121,-41,107)],2,'Iron')
for i in range(17):
    x=83+i*2; tube([(x,-41+2.5*math.cos(j*math.tau/12),107+2.5*math.sin(j*math.tau/12)) for j in range(13)],.35,'Brass')
tube([(125,-42,93),(125,-42,120)],1.3,'Brass')
finish('SM_WorkshopBench',True)

# Freestanding stock cabinet. Each shelf and divider has thickness and visible feet.
for x in [-156,156]:box((x,0,132),(10,51,264),edge=1.2)
box((0,24,135),(304,5,255),'Enamel',.6)
for z in [12,73,136,201,267]:box((0,-2,z),(320,57,7),edge=.7)
for x in [-78,0,78]:box((x,0,171),(5,48,188))
for x in [-115,-38,38,115]:
    box((x,-27,43),(72,4,47),'Enamel',.6)
    for i in range(4):box((x,-30,27+i*10),(61,3,4),'Wood',.6)
    rivet(x+26,-32,46)
for x in [-147,147]:box((x,-30,140),(3,2,255),'Brass',.3)
for x in [-143,143]:box((x,26,342),(10,10,150),'Wood',.9)
box((0,26,410),(310,10,9),'Wood',.8)
lettering('CELLS   /   FITTINGS   /   REPAIRS',(0,-34,281),9)
box((0,-1,282),(320,54,24),'Enamel',1)
finish('SM_WorkshopRack',True)

# Portable repair lamp: enamel bell, ceramic light body and an articulated spine.
lathe([(0,0),(22,0),(24,3),(23,6),(17,9),(0,9)],'Iron')
tube([(0,0,8),(0,0,45),(8,0,63),(12,0,72)],3.2,'Brass')
lathe([(0,42),(8,42),(9,52),(7,59),(0,60)],'MilkGlass',p=(12,-11,0))
lathe([(11,53),(28,56),(32,59),(31,62),(21,72),(9,80),(0,80)],'Enamel',p=(12,-11,0))
lathe([(31.5,58),(32.5,59),(32.5,61),(31.5,62)],'Brass',p=(12,-11,0))
circle(0,-4,46,5,'Brass',1.5)
tube([(0,8,8),(9,17,7),(24,26,3),(48,17,3),(57,-5,3)],1.1,'Shadow')
finish('SM_WorkshopDeskLamp')

# Product lamp with a fluted diffuser and split petal roof; clear, distinctive silhouette.
lathe([(0,0),(15,0),(19,3),(19,7),(14,11),(0,11)],'Brass')
lathe([(0,12),(11,12),(13,18),(13,42),(10,47),(0,47)],'MilkGlass',n=64)
for k in range(8):
    a=k*math.tau/8
    tube([(15*math.cos(a),15*math.sin(a),10),(17*math.cos(a),17*math.sin(a),29),(15*math.cos(a),15*math.sin(a),49)],.85,'Brass')
    # Raised seams and individually curved panels on the copper canopy.
    vs=[];fs=[]
    for j in range(7):
        t=j/6;r=3+23*math.sin(t*math.pi*.62);z=70-20*t
        for i in range(7):
            aa=a+(i/6-.5)*math.tau/8*.93;vs.append((r*math.cos(aa),r*math.sin(aa),z+math.sin(i*math.pi/6)))
    for j in range(6):
        for i in range(6):b=j*7+i;fs.append((b,b+1,b+8,b+7))
    geom('Formed copper petal',vs,fs,'Copper',True)
    tube([(r*math.cos(a),r*math.sin(a),z+1) for r,z in [(3,70),(10,66),(19,59),(25,51)]],.65,'Brass')
circle(0,0,77,6,'Brass',1.2)
finish('SM_WorkshopPetalLamp')

# Replaceable sealed solar cell, ceramic casing and brass terminal posts.
lathe([(0,0),(7,0),(8,2),(8,5),(6,7),(6,28),(8,30),(8,33),(0,33)],'Ivory')
for z in [4,29]:lathe([(7,z),(8.5,z),(8.5,z+2),(7,z+2)],'Brass')
for x in [-3,3]:lathe([(0,0),(1.5,0),(1.5,5),(0,5)],'Brass',p=(x,0,33),n=16)
box((0,-6.2,17),(6,1,10),'Enamel',.3)
finish('SM_WorkshopSunCell')

# Reels of real coiled cable; rope follows a continuous helix between timber flanges.
for z in [0,27]:lathe([(0,z),(18,z),(19,z+2),(19,z+4),(0,z+4)],'Wood')
lathe([(7,4),(7,27)],'Iron')
for layer in range(3):
    r=9+layer*3
    tube([(r*math.cos(t*math.tau),r*math.sin(t*math.tau),5+t*2.7) for t in [i/30 for i in range(241)]],1.35,'Shadow')
tube([(15,0,8),(25,6,3),(38,-8,2),(48,-4,2)],1.3,'Shadow')
finish('SM_WorkshopCableReel')

# A tray of repair tools, measured to rest on the bench rather than float above it.
box((0,0,1),(76,43,2),'Wood',.5)
for x in [-37,37]:box((x,0,4),(2,43,7),'Wood',.5)
for y in [-20,20]:box((0,y,4),(76,2,7),'Wood',.5)
for i in range(4):
    x=-24+i*16;box((x,5,4),(5,17,5),'Wood',1)
    tube([(x,-4,4),(x,-16,4)],1.15,'Iron')
    box((x,-17,4),(4,4,1.5),'Iron',.3)
for x in [-11,11]:tube([(x,13,5),(x*.6,0,5),(x*.4,-9,5)],1.4,'Brass')
finish('SM_WorkshopToolTray')

# Trade sign: real cast lettering, repaired corners and a sun-in-moth emblem.
box((0,0,0),(252,7,67),'Enamel',2)
for x in [-122,122]:box((x,-5,0),(3,3,61),'Brass',.5)
for z in [-30,30]:box((0,-5,z),(245,3,3),'Brass',.5)
lettering('SECOND DAWN',(0,-5.4,10),24)
lettering('L A M P S   &   R E P A I R S',(0,-5.4,-17),8.8)
for x in [-113,113]:
    for z in [-24,24]:rivet(x,-7,z)
finish('SM_WorkshopNameplate')

# Hanging blade sign. The bracket returns to the rear wall and the two chains carry the board.
box((0,0,0),(92,6,112),'Enamel',3)
for x in [-42,42]:box((x,-4,0),(2,2,103),'Brass',.4)
for z in [-51,51]:box((0,-4,z),(86,2,2),'Brass',.4)
circle(0,-5,9,20,'Brass',1.7)
for side in [-1,1]:
    tube([(0,-5,10),(side*31,-5,34),(side*25,-5,9),(side*35,-5,-10),(0,-5,0)],1.4,'Ivory')
for i in range(12):
    a=i*math.tau/12;tube([(23*math.cos(a),-5,9+23*math.sin(a)),(29*math.cos(a),-5,9+29*math.sin(a))],.8,'Brass')
lettering('REPAIR',(0,-5,-33),10)
for x in [-29,29]:
    for i in range(7):circle(x,-1,60+i*5.5,3,'Iron',.6)
tube([(-42,0,101),(42,0,101)],2.4,'Iron')
finish('SM_WorkshopTradeSign')

# Wall-mounted hanging rack includes brackets and individually bent hooks.
for x in [-143,143]:
    box((x,26,0),(12,5,48),'Iron',.8)
    tube([(x,24,-15),(x,-94,0),(x,24,15)],2.3,'Iron')
tube([(-156,-94,0),(156,-94,0)],2.4,'Brass')
for x in [-125,-63,0,63,125]:tube([(x,-94,2),(x,-97,-8),(x,-92,-15),(x,-85,-11)],1.2,'Iron')
finish('SM_WorkshopHangingRail')
tube([(0,0,0),(0,0,100)],.8,'Iron')
finish('SM_WorkshopSuspension')

# Explicit, stable part keys make art changes reviewable. Transform values are UE centimetres.
recipe=[]
def put(key,mesh,p,s=1,yaw=0,collision=False):
    recipe.append(dict(key=key,mesh=mesh,position=list(p),scale=s if isinstance(s,list) else [s]*3,rotation=[0,yaw,0],collision=collision))
put('repair-bench','SM_WorkshopBench',(-70,65,0),collision=True)
put('stock-cabinet','SM_WorkshopRack',(-60,-157,0),collision=True)
put('shop-name','SM_WorkshopNameplate',(-60,-177,328))
put('trade-sign','SM_WorkshopTradeSign',(302,110,315),yaw=-20)
put('hanging-rail','SM_WorkshopHangingRail',(-60,-157,387))
put('bench-lamp','SM_WorkshopDeskLamp',(-167,55,103),yaw=-18)
put('repair-tray','SM_WorkshopToolTray',(-98,64,103),yaw=8)
put('bench-reel','SM_WorkshopCableReel',(10,70,103),yaw=27)
for i,x in enumerate([-185,-123,-60,3,65]):
    scale=.95 if i%2 else 1.15; z=252+(i%2)*24; top=z+82*scale
    put(f'hanging-lamp-{i}','SM_WorkshopPetalLamp',(x,-63,z),s=scale,yaw=i*17)
    put(f'suspension-{i}','SM_WorkshopSuspension',(x,-63,top),s=[1,1,(373-top)/100])
for shelf,z in enumerate([77,140,205]):
    for col,x in enumerate([-176,-99,-22,55]):
        if (shelf+col)%3==0:
            put(f'stock-reel-{shelf}-{col}','SM_WorkshopCableReel',(x,-147,z),s=.85,yaw=col*33)
        elif (shelf+col)%3==1:
            put(f'stock-lamp-{shelf}-{col}','SM_WorkshopPetalLamp',(x,-146,z),s=.7,yaw=col*19)
        else:
            for j in range(3):put(f'stock-cell-{shelf}-{col}-{j}','SM_WorkshopSunCell',(x-19+j*19,-137,z),s=.82)
for i in range(3):put(f'bench-cell-{i}','SM_WorkshopSunCell',(-41+i*19,92,103),yaw=i*14)
put('display-crate','SM_wooden_crate_02',(242,10,0),s=1.65,yaw=-7,collision=True)
put('display-lamp','SM_WorkshopPetalLamp',(242,10,76),s=1.6,yaw=16)
put('floor-reel','SM_WorkshopCableReel',(-270,-30,0),s=1.9,yaw=38)
put('spare-lamp','SM_WorkshopDeskLamp',(-289,45,0),s=1.15,yaw=34)
put('mechanic-stool','SM_wooden_stool_01',(-274,-117,0),s=1.2,yaw=21,collision=True)
definition=OUT/'SecondDawn.assembly.json'
definition.write_text(json.dumps(dict(schemaVersion=1,id='orison.second-dawn',name='DA_SecondDawnWorkshop',parts=recipe),indent=2)+'\n',encoding='utf-8')
bpy.context.preferences.filepaths.save_version=0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'LanternWorkshop.blend'))
files=[dict(path=p.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [definition,OUT/'LanternWorkshop.blend']]
(OUT/'workshop-manifest.json').write_text(json.dumps(dict(schemaVersion=1,author='Original Borrowed Sun Second Dawn workshop',
    blenderVersion=bpy.app.version_string,units='centimetres',assetCount=len(assets),assets=assets,files=files,
    assembly=definition.relative_to(ROOT).as_posix(),assemblyPartCount=len(recipe)),indent=2)+'\n',encoding='utf-8')
print('WORKSHOP COMPLETE',len(assets),'meshes',len(recipe),'parts',flush=True)
