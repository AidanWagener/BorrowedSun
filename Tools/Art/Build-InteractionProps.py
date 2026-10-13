"""Ten original Brass Sanctuary hand props. Blender 4.5, centimetres, authored UCX hulls.

No vendor geometry is an input. Workshop material names are reused intentionally.
Run with --background --factory-startup --python Tools/Art/Build-InteractionProps.py.
"""
from pathlib import Path
import bpy, bmesh, math, json, hashlib
from mathutils import Vector

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'SourceArt/InteractionProps'
OUT.mkdir(parents=True, exist_ok=True)
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
bpy.context.scene.unit_settings.system = 'METRIC'
bpy.context.scene.unit_settings.scale_length = .01
MATS = {}
for name, color, metal in [('Wood',(.29,.17,.085,1),0),('Brass',(.55,.32,.095,1),.85),
                         ('Copper',(.10,.27,.21,1),.7),('Shadow',(.025,.035,.039,1),0),
                         ('Enamel',(.026,.12,.16,1),0),('Iron',(.075,.085,.085,1),.8),
                         ('Ivory',(.72,.67,.53,1),0),('MilkGlass',(.82,.76,.62,1),0)]:
    m=bpy.data.materials.new(name); m.diffuse_color=color; MATS[name]=m
parts=[]; hulls=[]; assets=[]; active=''

def mesh(verts, faces, mat='Brass', smooth=False, collision=False):
    d=bpy.data.meshes.new('Geometry'); d.from_pydata(verts,[],faces); d.update()
    bm=bmesh.new(); bm.from_mesh(d)
    bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001)
    bmesh.ops.dissolve_degenerate(bm,edges=list(bm.edges),dist=.00001)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces)); bm.to_mesh(d); bm.free()
    ob=bpy.data.objects.new('Piece',d); bpy.context.collection.objects.link(ob)
    if collision:
        hulls.append(ob)
    else:
        d.materials.append(MATS[mat]); parts.append(ob)
        for p in d.polygons:p.use_smooth=smooth
    return ob

def box(p,s,mat='Wood',bevel=.2,collision=False):
    v=[tuple(p[k]+a[k]*s[k]/2 for k in range(3)) for a in
       [(-1,-1,-1),(1,-1,-1),(1,1,-1),(-1,1,-1),(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]]
    ob=mesh(v,[(0,3,2,1),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)],mat,collision=collision)
    if bevel and not collision:
        mod=ob.modifiers.new('Working edge radius','BEVEL');mod.width=bevel;mod.segments=3
        mod=ob.modifiers.new('Face normals','WEIGHTED_NORMAL');mod.keep_sharp=True
    return ob

def lathe(profile,mat='Brass',p=(0,0,0),n=48,collide=False,sectors=1):
    # Profile must close or terminate on the axis. Convex sectors preserve vessel interiors.
    def coords(angle,r,z):return(p[0]+r*math.cos(angle),p[1]+r*math.sin(angle),p[2]+z)
    v=[coords(i*math.tau/n,r,z) for r,z in profile for i in range(n)]
    f=[(j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i) for j in range(len(profile)-1) for i in range(n)]
    mesh(v,f,mat,True)
    if collide:
        for sector in range(sectors):
            points=[coords((sector+i/4)*math.tau/sectors,r,z) for r,z in profile for i in range(5)] if sectors>1 else v
            ob=mesh(points,[],collision=True)
            bm=bmesh.new();bm.from_mesh(ob.data)
            bmesh.ops.convex_hull(bm,input=list(bm.verts),use_existing_faces=False)
            loose=[v for v in bm.verts if not v.link_faces]
            if loose:bmesh.ops.delete(bm,geom=loose,context='VERTS')
            bm.to_mesh(ob.data);bm.free()

def tube(points,r,mat='Brass',collide=False):
    d=bpy.data.curves.new('Bent metal','CURVE');d.dimensions='3D';d.bevel_depth=r;d.bevel_resolution=2;d.use_fill_caps=True
    s=d.splines.new('POLY');s.points.add(len(points)-1)
    for a,p in zip(s.points,points):a.co=(*p,1)
    ob=bpy.data.objects.new('Bent metal',d);bpy.context.collection.objects.link(ob);d.materials.append(MATS[mat]);parts.append(ob)
    if collide:
        for a,b in zip(points,points[1:]):
            a,b=Vector(a),Vector(b); delta=b-a
            h=box((0,0,0),(r*2,r*2,delta.length+r),collision=True)
            h.rotation_mode='QUATERNION';h.rotation_quaternion=delta.to_track_quat('Z','Y');h.location=(a+b)/2

def ring(radius,z,wire=.15,mat='Brass'):
    tube([(radius*math.cos(i*math.tau/64),radius*math.sin(i*math.tau/64),z) for i in range(65)],wire,mat)

def begin(name):
    global active
    active='SM_Hand'+name

def finish(label,mass):
    global parts,hulls
    for ob in parts+hulls:
        bpy.ops.object.select_all(action='DESELECT');ob.select_set(True);bpy.context.view_layer.objects.active=ob
        if ob.type=='CURVE':bpy.ops.object.convert(target='MESH')
        bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
        for mod in list(ob.modifiers):bpy.ops.object.modifier_apply(modifier=mod.name)
    for index,ob in enumerate(parts):
        uv=ob.data.uv_layers.new(name='UVMap')
        colors=ob.data.color_attributes.new(name='Color',type='FLOAT_COLOR',domain='CORNER')
        for face in ob.data.polygons:
            dominant=max(range(3),key=lambda k:abs(face.normal[k]));axes=[k for k in range(3) if k!=dominant]
            for li in face.loop_indices:
                co=ob.data.vertices[ob.data.loops[li].vertex_index].co
                uv.data[li].uv=(co[axes[0]]/60+index*.17,co[axes[1]]/60+index*.29)
                # Low localized contact variation; no uniform polished-edge mask.
                colors.data[li].color=(.15,.15 if abs(co.x)<3 else .02,.12 if co.z<2 else .02,1)
    bpy.ops.object.select_all(action='DESELECT')
    for ob in parts:ob.select_set(True)
    bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();visual=bpy.context.object;visual.name=active
    # Origin at authored base, separate from center of mass used by the carry grip.
    bpy.context.scene.cursor.location=(0,0,0);bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    visual.data.calc_loop_triangles()
    triangles=len(visual.data.loop_triangles)
    for i,ob in enumerate(hulls):ob.name=f'UCX_{active}_{i:02d}';ob.select_set(True)
    fbx=OUT/(active+'.fbx')
    bpy.ops.export_scene.fbx(filepath=str(fbx),use_selection=True,object_types={'MESH'},global_scale=1,
        apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        mesh_smooth_type='FACE',use_tspace=True,colors_type='LINEAR',add_leaf_bones=False,bake_anim=False)
    assets.append(dict(name=active,label=label,massKg=mass,path=str(fbx.relative_to(ROOT)).replace('\\','/'),
        triangles=triangles,collisionHulls=len(hulls),dimensionsCm=list(visual.dimensions),
        materials=[m.name for m in visual.data.materials],sha256=hashlib.sha256(fbx.read_bytes()).hexdigest()))
    for ob in [visual]+hulls:ob.hide_set(True)
    parts=[];hulls=[]

begin('Cup')
lathe([(0,0),(3.1,0),(3.5,.6),(4,8),(3.9,9),(3.3,9),(3.25,1),(0,1),(0,0)],'Enamel',collide=True,sectors=10)
ring(3.7,8.7,.23);ring(3.25,.8,.18)
tube([(3.6,0,7.5),(6.1,0,7),(6.6,0,5),(6.1,0,3),(3.5,0,2.8)],.5,collide=True)
finish('Enamel tea cup',.28)

begin('Bowl')
lathe([(0,0),(4,0),(4,.7),(6.3,1.5),(9.8,5.6),(10,6.2),(9.3,6.5),(5.9,2.3),(0,1.2),(0,0)],'Ivory',collide=True,sectors=12)
ring(9.7,6,.18,'Copper');ring(6.5,2.1,.15,'Enamel');finish('Glazed sharing bowl',.65)

begin('Lantern')
lathe([(0,0),(6,0),(6.7,.6),(6.7,2),(5,3),(0,3),(0,0)],collide=True)
lathe([(0,3),(4.2,3),(4.5,4),(4.5,16),(4,17),(0,17),(0,3)],'MilkGlass',collide=True)
lathe([(0,17),(5.2,17),(5.2,18),(3,19.5),(2,20),(0,20),(0,17)],'Enamel',collide=True)
for i in range(6):
    a=i*math.tau/6;x,y=5*math.cos(a),5*math.sin(a)
    tube([(x,y,2),(x,y,18)],.25)
tube([(-3,0,20),(-3,0,24),(-2,0,26),(2,0,26),(3,0,24),(3,0,20)],.35,collide=True)
ring(5,10,.15);finish('Caged service lantern',1.1)

begin('Tool')
box((0,0,9),(2.2,1.3,17),'Iron',.35);box((0,0,9),(2.3,1.4,8),'Enamel',.35)
for x in [-2.3,2.3]:box((x,0,19),(1.4,1.8,6),'Iron',.15)
box((0,0,16.6),(5.5,1.8,1.5),'Iron',.15)
box((0,0,9),(2.4,1.5,18),collision=True)
for x in [-2.3,2.3]:box((x,0,19),(1.4,1.8,6),collision=True)
box((0,0,16.6),(5.5,1.8,1.5),collision=True)
for z in [6,8,10,12]:box((0,-.8,z),(2.3,.2,.25),'Brass',.05)
finish('Insulated service spanner',.55)

begin('Cell')
lathe([(0,0),(3,0),(3.6,.6),(3.6,11),(3,12),(0,12),(0,0)],'Enamel',collide=True)
for z in [1,2,10,11]:ring(3.6,z,.25)
lathe([(0,12),(1.6,12),(1.6,13),(0,13),(0,12)],'Copper',collide=True)
box((0,-3.55,6),(2.7,.3,4),'Ivory',.15);finish('Ordinary workshop power cell',.8)

begin('Crate')
for z in [2.5,7.5,12.5,17.5]:
    for y in [-10,10]:box((0,y,z),(28,1.8,4.8),'Wood',.18)
    for x in [-13,13]:box((x,0,z),(1.8,20,4.8),'Wood',.18)
for x in [-10,-5,0,5,10]:
    for z in [1,20]:box((x,0,z),(4.8,20,1.8),'Wood',.13)
for x in [-13,13]:
    for y in [-10,10]:box((x,y,10),(2.6,2.6,21),'Copper',.25)
box((0,-11,11),(10,.6,6),'Enamel',.35);box((0,-11.4,11),(7,.1,3.4),'Ivory',.1)
box((0,0,10.5),(29,23,21),collision=True);finish('Small strapped parts crate',3.2)

begin('Basket')
lathe([(0,0),(6,0),(6,1),(0,1),(0,0)],'Wood',collide=True)
for i in range(20):
    a=i*math.tau/20
    tube([(6*math.cos(a),6*math.sin(a),1),(9.5*math.cos(a),9.5*math.sin(a),12)],.34,'Wood')
for z in range(2,13):ring(6+(z-1)*3.5/11,z,.28,'Wood')
# Eight closed convex sectors support the basket while leaving its hollow center usable.
lathe([(6,1),(9.6,12),(8.7,12),(5.4,1),(6,1)],'Wood',n=40,collide=True,sectors=10)
tube([(-9,0,11),(-9,0,17),(-6,0,20),(6,0,20),(9,0,17),(9,0,11)],.55,'Wood',collide=True)
finish('Woven parts basket',.45)

begin('Bottle')
lathe([(0,0),(3.5,0),(4.2,1),(4.2,11),(3.8,13),(1.8,15),(1.8,19),(0,19),(0,0)],'Enamel',collide=True)
lathe([(0,19),(2,19),(2,21),(0,21),(0,19)],'Wood',collide=True)
ring(1.9,18.5,.2);ring(4.2,3,.2,'Copper');ring(4.2,10,.2,'Copper')
box((0,-4.1,7),(3,.3,4),'Ivory',.18);finish('Stoppered ceramic flask',.9)

begin('Book')
box((0,0,2.3),(12,17,3.7),'Ivory',.2)
for z in [.25,4.5]:box((0,0,z),(13,18,.5),'Enamel',.18)
box((-6.2,0,2.3),(.8,18,4.6),'Enamel',.3)
for z in [1,1.5,2,2.5,3,3.5]:box((6,0,z),(.12,16,.055),'Shadow',.01)
for x in [-5.5,5.5]:
    for y in [-7.8,7.8]:box((x,y,4.83),(1.5,1.5,.18),'Brass',.15)
box((6.6,0,2.3),(1,2.5,4.4),'Brass',.15)
box((0,0,4.87),(6,7,.14),'Copper',.4)
box((0,0,2.4),(14,18,4.9),collision=True);finish('Repair ledger',.7)

begin('Bundle')
box((0,0,6),(23,16,12),'Enamel',2)
for y in [-5,5]:box((0,y,6),(23.6,1.3,12.5),'Ivory',.6)
box((0,0,6),(1.4,16.5,12.6),'Ivory',.6)
for x in [-8,-4,4,8]:tube([(x,-7,11),(x+.4,0,12.2),(x,7,11)],.12,'Copper')
box((0,0,12.6),(3,2,.6),'Brass',.4)
box((0,0,6),(23,16,12),collision=True);finish('Tied cloth parcel (rigid)',1.4)

blend=OUT/'InteractionProps.blend';bpy.ops.wm.save_as_mainfile(filepath=str(blend))
manifest=dict(schemaVersion=1,units='centimetres',definitionRevision=1,origin='Original procedural geometry authored for Borrowed Sun; no external geometry inputs.',
    generator='Tools/Art/Build-InteractionProps.py',blend='SourceArt/InteractionProps/InteractionProps.blend',
    blendSha256=hashlib.sha256(blend.read_bytes()).hexdigest(),assets=assets)
(OUT/'interaction-props-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
print(json.dumps(dict(status='Exported',assets=len(assets),triangles=sum(a['triangles'] for a in assets),collisionHulls=sum(a['collisionHulls'] for a in assets))))
