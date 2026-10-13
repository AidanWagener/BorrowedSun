"""Run with pinned Blender --background --python. Original centimetre-scale market kit.

Exports individual FBX meshes, editable Blender source, and dimensions/material slots.
Unreal imports and validates these outputs; this script does not manufacture uassets.
"""
from pathlib import Path
import bpy,bmesh,math,json,random,hashlib
from mathutils import Vector

ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'SourceArt/MarketKit';OUT.mkdir(parents=True,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.context.scene.unit_settings.system='METRIC'
bpy.context.scene.unit_settings.scale_length=.01
MATS={}
for name,color,metal,rough in [
    ('Stone',(.62,.46,.29,1),0,.78),('Plaster',(.81,.73,.55,1),0,.84),
    ('Wood',(.15,.073,.026,1),0,.68),('Brass',(.48,.26,.055,1),.8,.32),
    ('Copper',(.10,.26,.20,1),.75,.47),('Cloth',(.018,.041,.10,1),0,.9),
    ('Shadow',(.017,.027,.035,1),0,.9),('Ceramic',(.38,.12,.026,1),0,.3)]:
    m=bpy.data.materials.new(name);m.diffuse_color=color;m.use_nodes=True
    bsdf=m.node_tree.nodes.get('Principled BSDF');bsdf.inputs['Base Color'].default_value=color
    bsdf.inputs['Metallic'].default_value=metal;bsdf.inputs['Roughness'].default_value=rough
    MATS[name]=m
parts=[];assets=[]

def mesh(name,verts,faces,mat,uv=None,smooth=False):
    d=bpy.data.meshes.new(name);d.from_pydata(verts,[],faces);d.update()
    bm=bmesh.new();bm.from_mesh(d);bmesh.ops.recalc_face_normals(bm,faces=bm.faces);bm.to_mesh(d);bm.free()
    ob=bpy.data.objects.new(name,d);bpy.context.collection.objects.link(ob)
    d.materials.append(MATS[mat])
    for p in d.polygons:p.use_smooth=smooth
    layer=d.uv_layers.new(name='UVMap')
    for loop in d.loops:
        v=d.vertices[loop.vertex_index].co
        layer.data[loop.index].uv=uv[loop.vertex_index] if uv else (v.x/160,v.z/160)
    parts.append(ob);return ob

def bevel(ob,width=1,segments=3):
    mod=ob.modifiers.new('Edge craftsmanship','BEVEL');mod.width=width;mod.segments=segments;mod.limit_method='ANGLE';mod.harden_normals=True
    for p in ob.data.polygons:p.use_smooth=True
    norm=ob.modifiers.new('Weighted face normals','WEIGHTED_NORMAL');norm.keep_sharp=True
    return ob

def box(p,size,mat='Stone',edge=1,rot=None):
    x,y,z=p;a,b,c=(v/2 for v in size)
    verts=[(x+sx*a,y+sy*b,z+sz*c) for sx,sy,sz in [(-1,-1,-1),(1,-1,-1),(1,1,-1),(-1,1,-1),(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]]
    ob=mesh('Joined block',verts,[(0,3,2,1),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)],mat)
    if rot:
        from mathutils import Matrix
        axis,angle=rot
        matrix=Matrix.Rotation(angle,4,axis)
        for v in ob.data.vertices:v.co=Vector(p)+matrix@(v.co-Vector(p))
    if edge:bevel(ob,edge)
    return ob

def masonry(p,size,seed,edge=1.1):
    """Bounded hand dressing, not noisy displacement of the whole building."""
    ob=box(p,size,'Stone',0)
    rng=random.Random(seed)
    for vertex in ob.data.vertices:
        vertex.co.x+=rng.uniform(-.32,.32)
        vertex.co.y+=rng.uniform(-.22,.22)
        vertex.co.z+=rng.uniform(-.28,.28)
    bevel(ob,edge,3)
    return ob

def extrusion(poly,y,depth,mat='Stone',edge=1):
    n=len(poly);verts=[(x,y+dy,z) for dy in [-depth/2,depth/2] for x,z in poly]
    faces=[tuple(reversed(range(n))),tuple(range(n,2*n))]+[(i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n)]
    ob=mesh('Profile',verts,faces,mat)
    if edge:bevel(ob,edge)
    return ob

def lathe(profile,mat='Stone',p=(0,0,0),sides=64,flutes=0):
    verts=[];faces=[]
    for r,z in profile:
        for i in range(sides):
            a=i*math.tau/sides;rr=r*(1+flutes*math.cos(a*16))
            verts.append((p[0]+rr*math.cos(a),p[1]+rr*math.sin(a),p[2]+z))
    for j in range(len(profile)-1):
        for i in range(sides):a=j*sides+i;b=j*sides+(i+1)%sides;faces.append((a,b,b+sides,a+sides))
    return mesh('Turned profile',verts,faces,mat,smooth=True)

def tube(points,radius,mat='Brass',sides=8,cyclic=False):
    data=bpy.data.curves.new('Swept ornament','CURVE');data.dimensions='3D';data.resolution_u=1
    spline=data.splines.new('POLY');spline.points.add(len(points)-1)
    for a,p in zip(spline.points,points):a.co=(*p,1)
    spline.use_cyclic_u=cyclic;data.bevel_depth=radius;data.bevel_resolution=2;data.resolution_u=1
    ob=bpy.data.objects.new('Swept ornament',data);bpy.context.collection.objects.link(ob);data.materials.append(MATS[mat]);parts.append(ob)
    return ob

def rosette(x,y,z,r,mat='Brass',petals=8):
    points=[]
    for i in range(129):
        a=i*math.tau/128;rr=r*(.73+.27*math.cos(a*petals));points.append((x+rr*math.cos(a),y,z+rr*math.sin(a)))
    tube(points,max(.4,r*.027),mat)
    tube([(x+r*.26*math.cos(a*math.tau/40),y-1,z+r*.26*math.sin(a*math.tau/40)) for a in range(41)],max(.5,r*.022),mat)
    for i in range(petals):
        a=i*math.tau/petals
        tube([(x+rr*math.cos(a),y-1,z+rr*math.sin(a)) for rr in [r*.30,r*.68]],max(.4,r*.014),mat)

def finish(name):
    global parts
    bpy.ops.object.select_all(action='DESELECT')
    for ob in parts:
        bpy.context.view_layer.objects.active=ob;ob.select_set(True)
        if ob.type=='CURVE':bpy.ops.object.convert(target='MESH')
        for mod in list(ob.modifiers):
            bpy.ops.object.modifier_apply(modifier=mod.name)
        ob.select_set(False)
    for ob in parts:ob.select_set(True)
    bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join()
    ob=bpy.context.object;ob.name=name
    bpy.context.scene.cursor.location=(0,0,0);bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
    if name.startswith('SM_FlagstonePatch'):
        up=sum(p.normal.z*p.area for p in ob.data.polygons)/sum(p.area for p in ob.data.polygons)
        if up<.9:raise RuntimeError('Walkable paving must face upward: '+name)
    # World-projected materials ignore these UVs, but nondegenerate UVs are still needed
    # for reliable imported tangent bases. Preserve the cloth's deliberately sewn layout.
    uv=ob.data.uv_layers.active
    for polygon in ob.data.polygons:
        if ob.data.materials[polygon.material_index].name=='Cloth':continue
        axis=max(range(3),key=lambda a:abs(polygon.normal[a]));axes=[a for a in range(3) if a!=axis]
        for index in polygon.loop_indices:
            co=ob.data.vertices[ob.data.loops[index].vertex_index].co
            uv.data[index].uv=(co[axes[0]]/160,co[axes[1]]/160)
    # The fabric shader uses linear vertex R as the amount of printed gold motif.
    # Sewn border strips retain the motif; broad indigo panels provide visual rest.
    # This is material data, not a baked lighting/AO mask. G/B/A remain one.
    color=ob.data.color_attributes.new(name='ArtMasks',type='FLOAT_COLOR',domain='CORNER')
    for polygon in ob.data.polygons:
        for index in polygon.loop_indices:
            co=ob.data.vertices[ob.data.loops[index].vertex_index].co
            amount=1.0
            if ob.data.materials[polygon.material_index].name=='Cloth':
                if name=='SM_MarketCanopy':
                    border=max(abs(co.x)/250,abs(co.y)/165)
                    band=max(0.0,min(1.0,(border-.78)/.09))
                    seam=min(abs(co.x-83.333),abs(co.x+83.333))
                    amount=max(.12+.08*(co.x>0),band,.72*max(0.0,1-seam/5))
                elif name=='SM_WovenSackCluster':amount=.02
                elif name=='SM_MarketDrape':amount=1.0 if abs(co.x)>158 or co.z<-32 else .22
            color.data[index].color=(amount,1,1,1)
    bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},
        global_scale=1,apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        mesh_smooth_type='FACE',use_mesh_modifiers=True,add_leaf_bones=False,bake_anim=False,path_mode='STRIP',colors_type='LINEAR')
    ob.data.calc_loop_triangles()
    assets.append({'name':name,'path':(OUT/(name+'.fbx')).relative_to(ROOT).as_posix(),
        'sha256':hashlib.sha256((OUT/(name+'.fbx')).read_bytes()).hexdigest(),
        'dimensionsCm':list(ob.dimensions),'triangles':len(ob.data.loop_triangles),
        'materials':[m.name for m in ob.data.materials],
        'vertexColorContract':'Linear ArtMasks: R=cloth motif amount; G/B/A=1',
        'boundsCm':{'min':[min(v.co[a] for v in ob.data.vertices) for a in range(3)],
                    'max':[max(v.co[a] for v in ob.data.vertices) for a in range(3)]}})
    ob.location=((len(assets)-1)%5*850,((len(assets)-1)//5)*900,0)
    parts=[];print('EXPORTED',name,assets[-1],flush=True)

# Segmented voussoirs, layered mouldings and small inset solar reliefs.
for variant in [0,1]:
    inner=140;outer=196;spring=285;stretch=1.0 if variant==0 else 1.25
    for side in [-1,1]:
        for row in range(6):masonry((side*168,0,23.5+row*47.5),(55,83,46),1100+variant*100+side*10+row,1.3)
        for radius,height in [(33,0),(29,20),(23,45)]:
            box((side*204,-42,height+10),(radius*2,62,20),'Stone',1.5)
        lathe([(0,55),(25,55),(25,70),(19,84),(16,92),(16,270),(23,285),(28,292),(28,307),(0,307)],p=(side*204,-37,0),flutes=.07)
        box((side*204,-40,319),(70,74,24),'Stone',2)
        rosette(side*204,-83,330,18,'Brass')
    for i in range(28):
        a=i*math.pi/28+.0025;b=(i+1)*math.pi/28-.0025
        poly=[(r*math.cos(t),spring+r*math.sin(t)*stretch) for r,t in [(inner,a),(outer,a),(outer,b),(inner,b)]]
        extrusion(poly,-3+math.sin(i*2.39+variant)*.35,86,'Stone',1.1)
    for radius,yy,rr in [(137,-52,2.2),(153,-51,1.4),(201,-48,4.0),(214,-43,2.5)]:
        tube([(radius*math.cos(t*math.pi/96),yy,spring+radius*math.sin(t*math.pi/96)*stretch) for t in range(97)],rr,'Stone' if rr>2 else 'Brass')
    for side in [-1,1]:
        rosette(side*178,-53,470 if variant==0 else 505,24,'Stone')
    extrusion([(-22,455 if variant==0 else 489),(22,455 if variant==0 else 489),(28,507 if variant==0 else 562),(-28,507 if variant==0 else 562)],-13,100,'Stone',1.5)
    # Recessed, separately bedded threshold slabs with a worn central walking line.
    for slab in range(3):
        masonry((-91+slab*91,-5,3),(89,108,6),1800+variant*10+slab,.9)
    for side in [-1,1]:
        # A concave shoe at each jamb makes the dressed stone meet the pavement.
        extrusion([(side*141,6),(side*191,6),(side*191,19),(side*147,19)],-45,19,'Stone',1)
    finish('SM_MarketArch_'+('Round' if variant==0 else 'Tall'))

# Double timber door: boards, inset panels, rim mouldings, hinges and pull rings.
for side in [-1,1]:
    x=side*69
    box((x,8,143),(136,15,282),'Wood',1.0)
    for xx in [-125,-87,-49,-11] if side<0 else [11,49,87,125]:box((xx,-2,146),(35,4,277),'Wood',.6)
    for zz in [60,181]:
        box((x,-6,zz),(99,4,91),'Shadow',.8)
        box((x,-9,zz),(84,4,77),'Wood',1)
        for dx in [-50,50]:box((x+dx,-12,zz),(5,8,99),'Wood',.5)
        for dz in [-48,48]:box((x,-12,zz+dz),(105,8,5),'Wood',.5)
        rosette(x,-16,zz,23,'Brass')
    for zz in [30,145,263]:
        box((side*123,-15,zz),(28,7,8),'Brass',1)
        for dx in [-8,8]:lathe([(0,0),(2.1,0),(2.1,2),(0,2)],'Brass',(side*123+dx,-19,zz),sides=12)
    tube([(side*18+7*math.cos(t*math.tau/32),-25,153+7*math.sin(t*math.tau/32)) for t in range(33)],1.8,'Brass')
for r,y in [(139,3),(149,-5)]:tube([(r*math.cos(t*math.pi/64),y,284+r*math.sin(t*math.pi/64)) for t in range(65)],4,'Wood')
extrusion([(-139,284),(139,284)]+[(139*math.cos(t*math.pi/32),284+139*math.sin(t*math.pi/32)) for t in range(33)],10,5,'Shadow',0)
for i in range(1,12):
    a=i*math.pi/12;tube([(0,-4,284),(136*math.cos(a),-4,284+136*math.sin(a))],2.2,'Wood')
rosette(0,-12,323,28,'Brass')
finish('SM_MarketDoor')

# Recessed ornamental window screen, curved octagonal tracery with actual openings.
for x in [-121,121]:box((x,0,166),(20,30,352),'Wood',1.4)
for z in [0,332]:box((0,0,z),(260,30,20),'Wood',1.4)
for x in [-103,103]:box((x,-9,166),(5,12,310),'Brass',.6)
for z in [18,314]:box((0,-9,z),(210,12,5),'Brass',.6)
for row in range(5):
    for col in range(3):
        x=-68+68*col;z=42+60*row
        points=[(x+(25 if i%2==0 else 18)*math.cos(i*math.pi/8),-5,z+(25 if i%2==0 else 18)*math.sin(i*math.pi/8)) for i in range(16)]
        tube(points,1.6,'Copper',cyclic=True)
        rosette(x,-8,z,9,'Brass',4)
box((0,16,165),(222,3,312),'Shadow',0)
for zz in [-18,-29]:box((0,-10,zz),(280 if zz==-29 else 269,65,12),'Stone',1.6)
finish('SM_MarketWindow')

# Carved support bracket with a curved load-bearing profile and ribbed face.
poly=[(-45,0),(48,0),(48,-20),(38,-28),(28,-38),(12,-59),(-4,-84),(-20,-103),(-38,-106),(-45,-90)]
extrusion(poly,0,45,'Stone',2)
for y in [-24,24]:tube([(x,y,z) for x,z in poly],2,'Stone',cyclic=True)
rosette(-14,-27,-62,20,'Brass')
finish('SM_MarketCorbel')

# Balcony: three enclosed sides, a stone lattice and a continuous load-bearing rail.
# The old front-only balusters left the floor edges visibly unfinished at oblique angles.
for z,width,depth in [(0,380,120),(18,395,132),(28,385,128)]:box((0,0,z),(width,depth,18),'Stone',1.5)
for x in [-177,0,177]:
    masonry((x,-46,83),(22,28,102),2100+int(x),1)
    box((x,-46,132),(30,36,10),'Stone',1.1)
    box((x,-46,141),(25,31,8),'Stone',1)

def lattice(x,y,z,width,height):
    # Chamfered octagonal rings have real open apertures and 11 cm of depth.
    for row in range(2):
        for col in range(4):
            cx=x+(col-1.5)*width/4;cz=z+(row-.5)*height/2
            outer=width/8;inner=outer-4.6
            for segment in range(8):
                a=(segment+.5)*math.tau/8;b=(segment+1.5)*math.tau/8
                poly=[(cx+r*math.cos(t),cz+r*math.sin(t)) for r,t in [(inner,a),(outer,a),(outer,b),(inner,b)]]
                extrusion(poly,y,11,'Stone',.45)
            if col<3:box((cx+width/8,y,cz),(5.8,11,6),'Stone',.45)
            box((cx,y,z),(6,11,5),'Stone',.45)
    for zz in [z-height*.5-3,z+height*.5+3]:box((x,y,zz),(width+13,16,6),'Stone',.7)
    for xx in [x-width*.5-2,x+width*.5+2]:box((xx,y,z),(7,16,height+12),'Stone',.7)

for x in [-87,87]:lattice(x,-46,83,148,70)
for z in [125,139]:box((0,-46,z),(390,36 if z==139 else 28,12),'Stone',1.4)
for side in [-1,1]:
    # Returns close the open sides without changing the floor footprint or pivots.
    for y in [-29,5,39]:
        lathe([(0,36),(7,36),(7,44),(4,54),(6,65),(5,83),(4,102),(7,114),(7,124),(0,124)],p=(side*177,y,0),sides=20)
    for z in [124,139]:box((side*177,3,z),(28,108,12),'Stone',1.2)
    box((side*177,53,84),(22,24,101),'Stone',1)
# A narrow soffit drip-groove is geometry, preventing the balcony reading as stacked boxes.
for y in [-62,62]:box((0,y,-7),(367,5,4),'Shadow',.4)
finish('SM_MarketBalcony')

# Cloth has authored UVs, scalloped valance, sewn hems and visible suspension ropes.
verts=[];uv=[];faces=[];nx=64;ny=48
def clothpoint(u,v):
    belly=-46*math.sin(u*math.pi)*math.sin(v*math.pi)
    selvage=-11*math.sin(u*math.pi)*(1-.35*v)-6*math.sin(v*math.pi)
    gathered=2.2*math.sin(u*12*math.pi+.8*v)*math.sin(v*math.pi)
    diagonal=1.7*math.sin((u+v)*18)*math.sin(u*math.pi)*math.sin(v*math.pi)
    return ((u-.5)*500,(v-.5)*330,belly+selvage+gathered+diagonal)
for j in range(ny+1):
    for i in range(nx+1):
        u=i/nx;v=j/ny;verts.append(clothpoint(u,v));uv.append((u*2.4,v*1.6))
for j in range(ny):
    for i in range(nx):a=j*(nx+1)+i;faces.append((a,a+1,a+nx+2,a+nx+1))
mesh('Draped fabric',verts,faces,'Cloth',uv,True)
for v in [0,1]:
    tube([clothpoint(i/64,v) for i in range(65)],1.3,'Cloth')
for u in [0,1/3,2/3,1]:tube([clothpoint(u,j/48) for j in range(49)],.55,'Cloth')
verts=[];uv=[];faces=[]
for i in range(81):
    u=i/80;x=(u-.5)*500
    top=clothpoint(u,0)[2]
    for row in [0,1]:verts.append((x,-165-2*math.sin(u*math.pi*12),top-row*(24+6*(1-math.cos(u*math.pi*20)))));uv.append((u*2.4,-row*.2))
for i in range(80):faces.append((i*2,i*2+1,i*2+3,i*2+2))
mesh('Scalloped valance',verts,faces,'Cloth',uv,True)
for i in range(11):
    u=i/10;x=(u-.5)*500;z=clothpoint(u,0)[2]-25
    tube([(x,-165,z),(x+1,-165,z-8),(x,-165,z-16)],.9,'Wood')
    for dx in [-1.3,0,1.3]:tube([(x+dx,-165,z-15),(x+dx,-165,z-26)],.4,'Cloth')
for x in [-250,250]:
    for y in [-165,165]:
        tube([(x+4*math.cos(i*math.tau/20),y+4*math.sin(i*math.tau/20),0) for i in range(21)],.8,'Brass')
        tube([(x,y,0),(x*1.045,y*1.04,6),(x*1.12,y*1.18,20)],1.25,'Wood')
        for offset in [-3,0,3]:tube([(x+offset,y,0),(x*1.045+offset,y*1.04,6)],.45,'Wood')
finish('SM_MarketCanopy')

# Vertical cloth banner with a pointed foot, brass rods and tassels.
verts=[];uv=[];faces=[];nx=24;ny=64
for j in range(ny+1):
    for i in range(nx+1):
        u=i/nx;v=j/ny
        verts.append(((u-.5)*135,8*math.sin(u*math.pi*6)+v*v*12*math.sin(v*9+u*4),-v*390-(1-abs(u-.5)*2)*v**12*53));uv.append((u*.66,v*1.9))
for j in range(ny):
    for i in range(nx):a=j*(nx+1)+i;faces.append((a,a+1,a+nx+2,a+nx+1))
mesh('Hanging banner',verts,faces,'Cloth',uv,True)
tube([(-85,0,8),(85,0,8)],3.3,'Brass')
for x in [-73,73]:
    tube([(x,0,8),(x,0,45),(0,20,72)],1.5,'Wood')
    for dx in [-4,0,4]:tube([(x+dx,0,0),(x+dx,0,-22),(x+dx+2,1,-40)],.65,'Brass')
finish('SM_MarketBanner')

# Functional joinery: four legs, braces, planked top, shelf and forged corner straps.
for x in [-126,126]:
    for y in [-42,42]:box((x,y,46),(12,12,92),'Wood',1)
for x in [-128,128]:box((x,0,70),(10,94,24),'Wood',.8)
for y in [-44,44]:box((0,y,72),(260,10,22),'Wood',.8)
for i in range(7):box((-117+i*39,0,95),(37,115,10),'Wood',.65)
for i in range(3):box((0,-30+i*30,22),(269,28,6),'Wood',.6)
for x in [-130,130]:
    for y in [-48,48]:
        box((x,y,80),(14,2,23),'Brass',.4)
        for z in [73,87]:lathe([(0,0),(1.4,0),(1.4,1),(0,1)],'Brass',(x,y-2,z),sides=12)
finish('SM_MarketTable')

# Octagonal reliquary with banding, shallow decorative niches and repeated solar motifs.
lathe([(0,0),(101,0),(103,7),(98,13),(89,18),(89,66),(97,72),(98,82),(104,86),(104,92),(0,92)],'Stone',sides=8)
for z,r in [(17,90),(69,95),(84,104)]:lathe([(r-2,z),(r,z),(r,z+3),(r-2,z+3)],'Brass',sides=64)
for i in range(8):
    a=(i+.5)*math.tau/8
    for z in [29,59]:
        lathe([(0,0),(2,0),(2,2),(0,2)],'Brass',(90*math.cos(a),90*math.sin(a),z),sides=12)
    x=89*math.cos(a);y=89*math.sin(a)
    # Relief is on each facade plane, constructed in that plane.
    pts=[]
    for k in range(65):
        t=k*math.tau/64;rr=13*(.73+.27*math.cos(t*8));d=rr*math.cos(t)
        pts.append((x-d*math.sin(a),y+d*math.cos(a),44+rr*math.sin(t)))
    tube(pts,1,'Brass')
lathe([(0,92),(65,92),(65,94),(0,94)],'Copper',sides=64)
for r in [32,59,75]:lathe([(r-1,94),(r,94),(r,96),(r-1,96)],'Brass',sides=96)
finish('SM_SunseedReliquary')

# Paving relief follows the reviewed scan's height data, so physical joints match the PBR maps.
from array import array
height_image=bpy.data.images.load(str(ROOT/'SourceArt/PolyHaven/monastery_stone_floor/monastery_stone_floor_disp_2k.png'))
height_image.colorspace_settings.name='Non-Color'
iw,ih=height_image.size;pixels=array('f',[0.0])*(iw*ih*4);height_image.pixels.foreach_get(pixels)
def paving_height(x,y):
    # Unreal's FBX coordinate conversion flips Y; sample the eventual world orientation.
    u=(x/200)%1;v=(-y/200)%1
    xx=u*(iw-1);yy=(1-v)*(ih-1);ix=int(xx);iy=int(yy);fx=xx-ix;fy=yy-iy
    def p(a,b):return pixels[(min(ih-1,b)*iw+min(iw-1,a))*4]
    return ((p(ix,iy)*(1-fx)+p(ix+1,iy)*fx)*(1-fy)+(p(ix,iy+1)*(1-fx)+p(ix+1,iy+1)*fx)*fy)*3.8
for variant in range(3):
    verts=[];faces=[];n=240
    for j in range(n+1):
        for i in range(n+1):
            x=-200+i*400/n;y=-200+j*400/n;verts.append((x,y,paving_height(x,y)))
    for j in range(n):
        for i in range(n):a=j*(n+1)+i;faces.append((a,a+1,a+n+2,a+n+1))
    relief=mesh('Scanned stone relief',verts,faces,'Stone',smooth=True)
    # Generic closed-volume normal recalculation can invert an open height field.
    # Its playable surface must face +Z, otherwise the old quay shows through.
    if sum(p.normal.z*p.area for p in relief.data.polygons)<0:
        bm=bmesh.new();bm.from_mesh(relief.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(relief.data);bm.free();relief.data.update()
    finish('SM_FlagstonePatch_'+str(variant))

# Downpipe assembly with bend, collars, stand-offs and flange bolts.
tube([(0,0,0),(0,0,350),(0,-2,366),(0,-10,380),(0,-24,392),(0,-40,399),(0,-65,400)],7,'Copper')
for z in [25,175,325]:
    lathe([(7,z),(10,z),(10,z+7),(7,z+7)],'Brass',sides=32)
    box((0,11,z+3),(25,7,27),'Copper',1)
    for x in [-9,9]:box((x,16,z+3),(4,4,4),'Brass',.4)
finish('SM_CopperDownpipe')

# Drain: separate dressed curb blocks, recessed patinated grates and finite end frames.
# Dark aperture, not a bright continuous guide stripe, should dominate the street view.
for side in [-1,1]:
    for course in range(4):masonry((side*13,-75+course*50,0),(4.8,49.1,7),3000+side*20+course,.55)
box((0,0,-3),(23,200,4),'Shadow',0)
for x in [-10.5,10.5]:box((x,0,-.3),(1.6,198,2),'Copper',.3)
for y in [-98,0,98]:box((0,y,-.1),(22,2,2.5),'Copper',.3)
for i in range(24):box((0,-94+i*8,-.6),(21,1.4,2),'Copper',.25)
for y in [-51,47]:box((0,y,-1.5),(2,85,1.7),'Copper',.25)
finish('SM_DrainStrip')

# Large turned planter, layered stone lip and deliberate fluting.
lathe([(0,0),(45,0),(48,8),(41,15),(42,22),(58,91),(65,96),(65,110),(56,113),(51,102),(36,20),(0,20)],'Stone',sides=96,flutes=.025)
for z,r in [(9,47),(94,62),(106,65)]:lathe([(r-2,z),(r,z),(r,z+3),(r-2,z+3)],'Brass',sides=64)
lathe([(0,90),(49,90),(0,92)],'Shadow',sides=48)
finish('SM_CarvedPlanter')

# Woven sacks have uneven shoulders, folded necks and irregular but restrained silhouettes.
for sack in range(3):
    verts=[];faces=[];rings=28;segs=48
    ox=[-27,26,4][sack];oy=[0,5,42][sack];height=[75,63,81][sack];rr=[27,25,22][sack]
    for j in range(rings+1):
        v=j/rings
        profile=math.sin(v*math.pi)**.36*(.82+.18*math.sin(v*math.pi))
        if v>.8:profile*=.9-(v-.8)*2
        for i in range(segs):
            a=i*math.tau/segs;r=rr*profile*(1+.045*math.sin(a*7+v*13)+.025*math.sin(a*13-v*19))
            verts.append((ox+r*math.cos(a),oy+r*.76*math.sin(a),height*v))
    for j in range(rings):
        for i in range(segs):a=j*segs+i;b=j*segs+(i+1)%segs;faces.append((a,b,b+segs,a+segs))
    mesh('Folded sack',verts,faces,'Cloth',smooth=True)
    lathe([(0,height*.86),(8,height*.86),(8,height*.89),(0,height*.89)],'Wood',(ox,oy,0),sides=24)
finish('SM_WovenSackCluster')

# The Keeper is an original ceremonial moth figure. Continuous folded surfaces and
# curved limbs replace the earlier cylinder study; fine relief is visible on approach.
def ellipsoid(center,scale,mat='Brass',rings=40,segs=64):
    verts=[];faces=[]
    for j in range(rings+1):
        t=math.pi*j/rings
        for i in range(segs):
            a=math.tau*i/segs
            verts.append((center[0]+scale[0]*math.sin(t)*math.cos(a),center[1]+scale[1]*math.sin(t)*math.sin(a),center[2]+scale[2]*math.cos(t)))
    for j in range(rings):
        for i in range(segs):a=j*segs+i;b=j*segs+(i+1)%segs;faces.append((a,b,b+segs,a+segs))
    return mesh('Sculpted volume',verts,faces,mat,smooth=True)

def limb(points,radii,mat='Copper'):
    # Catmull-Rom skin, with continuously changing cross section.
    path=[Vector(points[0])]+[Vector(p) for p in points]+[Vector(points[-1])];samples=[];rs=[]
    for j in range(len(points)-1):
        p0,p1,p2,p3=path[j:j+4]
        for k in range(12):
            t=k/12;t2=t*t;t3=t2*t
            samples.append((2*p1+(-p0+p2)*t+(2*p0-5*p1+4*p2-p3)*t2+(-p0+3*p1-3*p2+p3)*t3)*.5)
            rs.append(radii[j]*(1-t)+radii[j+1]*t)
    samples.append(Vector(points[-1]));rs.append(radii[-1]);verts=[];faces=[];n=32
    for j,p in enumerate(samples):
        tangent=(samples[min(j+1,len(samples)-1)]-samples[max(j-1,0)]).normalized()
        side=tangent.cross(Vector((0,1,0))).normalized();other=tangent.cross(side).normalized()
        for i in range(n):
            a=i*math.tau/n;v=p+rs[j]*(side*math.cos(a)+other*math.sin(a));verts.append(tuple(v))
    for j in range(len(samples)-1):
        for i in range(n):a=j*n+i;b=j*n+(i+1)%n;faces.append((a,b,b+n,a+n))
    mesh('Sculpted limb',verts,faces,mat,smooth=True)

verts=[];faces=[];nr=96;ns=144
for j in range(nr+1):
    z=j*640/nr;t=z/640
    shoulder=max(0,min(1,(t-.86)/.14));shoulder=shoulder*shoulder*(3-2*shoulder)
    width=190-110*math.sin(t*math.pi*.72)+75*math.exp(-((t-.88)/.16)**2)-94*shoulder
    for i in range(ns):
        a=i*math.tau/ns;fold=1+.052*math.cos(a*22+z*.008)+.035*math.sin(a*7-z*.006)
        verts.append((width*math.cos(a)*fold,width*.60*math.sin(a)*fold,z+6*math.sin(a*11)*(1-t)**4))
for j in range(nr):
    for i in range(ns):a=j*ns+i;b=j*ns+(i+1)%ns;faces.append((a,b,b+ns,a+ns))
mesh('Folded keeper robe',verts,faces,'Copper',smooth=True)
for side in [-1,1]:
    # A narrow embroidered edge follows the robe all the way to the shoulder.
    tube([(side*(20+65*t),-58-50*(1-t)**2,35+t*570) for t in [k/80 for k in range(81)]],4,'Brass')
    limb([(side*110,0,586),(side*181,-23,474),(side*156,-97,446),(side*70,-183,483)],[59,41,32,22])
    ellipsoid((side*65,-190,487),(32,23,48),'Brass',24,40)
    for f in range(4):
        x=side*(43+f*14)
        limb([(x,-211,470),(x,-230,492),(x-side*5,-224,516-f*5)],[9,8,5],'Brass')
    limb([(side*85,-191,484),(side*108,-218,510),(side*84,-223,526)],[12,10,6],'Brass')
    for band in [0,1]:
        # Shoulder medallions give the robe a designed attachment point.
        rosette(side*98,-76-band*2,586-band*38,26-band*6,'Brass')
ellipsoid((0,0,654),(28,27,42),'Brass')
head_parts=len(parts)
ellipsoid((0,-5,758),(77,56,108),'Brass',64,96)
for side in [-1,1]:
    ellipsoid((side*38,-54,773),(26,12,42),'Shadow',32,48)
    tube([(side*38+28*math.cos(t*math.tau/80),-64,773+44*math.sin(t*math.tau/80)) for t in range(81)],3,'Copper')
    for lens in range(7):
        z=746+lens*8;span=19*math.sin((lens+1)*math.pi/8)
        tube([(side*38-span,-68,z),(side*38+span,-68,z)],.8,'Brass')
    tube([(side*8,-62,799),(side*15,-72,760),(side*19,-70,722),(side*9,-51,685)],3,'Copper')
    tube([(side*27,-39,690),(side*41,-73,688),(side*30,-94,709),(side*14,-101,731)],7,'Brass')
    antenna=[(side*(40+110*t+20*t*t),0,838+190*t) for t in [i/50 for i in range(51)]]
    tube(antenna,5,'Brass')
    for i in range(18):
        t=i/20;x=side*(40+110*t+20*t*t);z=838+190*t
        length=49*math.sin((i+2)*math.pi/22)
        for branch in [-1,1]:tube([(x,0,z),(x+side*length*branch,-3,z+18),(x+side*length*branch*1.12,-1,z+38)],1.4,'Brass')
rosette(0,-61,817,17,'Copper')
for ob in parts[head_parts:]:
    ob.scale=(.75,.9,.88);ob.location.z=745*(1-.88)
for z in [285,343,401,554]:rosette(0,-75,z,22,'Brass')
finish('SM_KeeperSculpt')

# Broken solar halo: framed collector panels with individual inset cells and cables.
cz=565;yy=110
for side in [-1,1]:
    for panel in range(7):
        a0=math.radians(-73+panel*21);a1=a0+math.radians(17)
        def hp(r,a,y=yy):return (side*r*math.cos(a),y,cz+r*math.sin(a))
        poly=[(hp(r,a)[0],hp(r,a)[2]) for r,a in [(285,a0),(430,a0),(430,a1),(285,a1)]]
        extrusion(poly,yy,9,'Copper',1)
        for r in [285,430]:tube([hp(r,a0+(a1-a0)*i/24,yy-7) for i in range(25)],3,'Brass')
        for a in [a0,a1]:tube([hp(282,a,yy-7),hp(434,a,yy-7)],3,'Brass')
        for r in [318,354,391]:tube([hp(r,a0+(a1-a0)*i/24,yy-8) for i in range(25)],1.1,'Brass')
        for c in range(1,5):
            a=a0+(a1-a0)*c/5;tube([hp(287,a,yy-9),hp(428,a,yy-9)],1,'Brass')
        a=(a0+a1)/2;tube([hp(95,a,yy+20),hp(434,a,yy+20)],4,'Brass')
    for r in [272,445]:tube([(side*r*math.cos(a),yy+15,cz+r*math.sin(a)) for a in [math.radians(-79+k*164/100) for k in range(101)]],4,'Brass')
finish('SM_SolarHalo')

# A draped shop textile follows the table top, then folds down over its two long edges.
verts=[];faces=[];uv=[];nx=64;ny=56
for j in range(ny+1):
    v=j/ny;t=(v-.5)*280
    for i in range(nx+1):
        u=i/nx;fall=max(0,abs(t)-73)
        y=max(-73,min(73,t))+math.copysign(3*math.sin(u*math.pi*16)*min(1,fall/25),t)
        z=-fall*1.15+1.4*math.sin(u*math.pi*14)+.6*math.sin(v*23)
        verts.append(((u-.5)*380,y,z));uv.append((u*1.0,v*.85))
for j in range(ny):
    for i in range(nx):a=j*(nx+1)+i;faces.append((a,a+1,a+nx+2,a+nx+1))
mesh('Shop display cloth',verts,faces,'Cloth',uv,True)
finish('SM_MarketDrape')

# A hollow six-sided street lantern: cast metal caps, cage, pierced tracery and hanging eye.
lathe([(0,0),(17,0),(23,4),(24,7),(19,10),(18,12)],'Brass',sides=96)
lathe([(18,57),(24,60),(25,64),(19,67),(13,76),(6,80),(0,81)],'Brass',sides=96,flutes=.025)
for k in range(6):
    a=k*math.tau/6
    tube([(19*math.cos(a),19*math.sin(a),z) for z in [8,20,42,60]],1.1,'Brass')
    b=(k+1)*math.tau/6
    for z in [14,53]:
        tube([(19*math.cos(a),19*math.sin(a),z),(19*math.cos(b),19*math.sin(b),z)],.7,'Brass')
    # Inset pointed arch in each facet; open metal lets the real inner light show through.
    A=Vector((19*math.cos(a),19*math.sin(a),0));B=Vector((19*math.cos(b),19*math.sin(b),0))
    pts=[]
    for t,z in [(0.1,18),(0.1,43),(.25,48),(.5,53),(.75,48),(.9,43),(.9,18)]:
        p=A.lerp(B,t);pts.append((p.x,p.y,z))
    tube(pts,.7,'Brass')
tube([(5*math.cos(a*math.tau/32),0,86+5*math.sin(a*math.tau/32)) for a in range(33)],1.2,'Brass')
finish('SM_MarketLantern')

# Retain editable source and the original export pivots.
bpy.context.preferences.filepaths.save_version=0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MarketKit.blend'))
(OUT/'market-kit-manifest.json').write_text(json.dumps({'schemaVersion':1,'author':'Original Borrowed Sun architecture and sculpture; CC0 scan-derived paving',
    'blenderVersion':bpy.app.version_string,'units':'centimetres','assetCount':len(assets),
    'pavingMaterialPeriodCm':200,'pavingReliefCm':3.8,
    'sourceRevision':'starting-area-polish-2026-10-13',
    'assets':assets},indent=2)+'\n',encoding='utf-8')
print('MARKET KIT COMPLETE',len(assets),sum(a['triangles'] for a in assets),'triangles',flush=True)
