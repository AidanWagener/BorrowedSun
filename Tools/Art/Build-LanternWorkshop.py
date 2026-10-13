"""Original Second Dawn repair-shop kit; pinned Blender, centimetres, front -Y.

FBX import converts the front to Unreal +Y. The recipe uses Unreal coordinates.
Run: blender --background --factory-startup --python Tools/Art/Build-LanternWorkshop.py
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
    ('Wood', (.29,.17,.085,1),0,.65), ('Brass',(.55,.32,.095,1),.85,.32),
    ('Copper',(.10,.27,.21,1),.7,.47), ('Shadow',(.025,.035,.039,1),0,.96),
    ('Enamel',(.026,.12,.16,1),0,.31), ('Iron',(.075,.085,.085,1),.8,.52),
    ('Ivory',(.72,.67,.53,1),0,.57), ('MilkGlass',(.82,.76,.62,1),0,.25)]:
    m=bpy.data.materials.new(name); m.diffuse_color=color; m.use_nodes=True
    p=m.node_tree.nodes.get('Principled BSDF'); p.inputs['Base Color'].default_value=color
    p.inputs['Metallic'].default_value=metal; p.inputs['Roughness'].default_value=rough
    MATS[name]=m
parts=[]; assets=[]
UV_TILE_CM=60.0

def bump(value,center,radius):
    """Compact smooth authored influence; exactly zero outside the contact region."""
    t=max(0.0,1.0-abs(value-center)/radius)
    return t*t*(3-2*t)

def patch(x,y,cx,cy,rx,ry):
    return bump(math.sqrt(((x-cx)/rx)**2+((y-cy)/ry)**2),0,1)

def contact_masks(asset,co,normal,material):
    """G=handled/contact polish, B=localized grime; coordinates are authored cm.

    These regions describe use and construction, not randomized material noise.
    R remains a separate unsigned angular-edge mask; A is always one.
    """
    x,y,z=co; polish=grime=0.0
    if material not in {'Wood','Brass','Copper','Enamel','Iron'}:return 0.0,0.0
    if asset=='SM_WorkshopBench':
        if material=='Wood' and z>92:
            top=max(0.0,normal.z)
            # Hands work in front of the tool well and beside the vise.
            work=max(.65*patch(x,y,-25,-24,70,26),.48*patch(x,y,103,-8,35,24))
            lip=bump(y,-44.5,8)*max(bump(x,-36,95),.7*bump(x,102,40))
            polish=top*work+.55*lip*bump(z,99,7)
            # Oily servicing spot, rear tool-well corner, and narrow plank joints.
            grime=top*(.40*patch(x,y,109,12,19,14)+.28*patch(x,y,-113,32,30,12))
            seam=max(bump(y,joint,1.3) for joint in [-28,-9.5,9,27.5])
            grime=max(grime,.22*seam*bump(z,101,4)*bump(x,112,35))
        if z<20:
            grime=max(grime,.72*bump(z,0,20))
        for drawer in [-101,0,101]:
            if material=='Brass':
                polish=max(polish,.90*bump(x,drawer,12)*bump(y,-47,7)*bump(z,76,7))
                grime=max(grime,.35*max(bump(x,drawer-10,4),bump(x,drawer+10,4))*bump(y,-44,3)*bump(z,79,4))
            elif material=='Enamel':
                front=bump(y,-42,3)*max(0.0,-normal.y)
                polish=max(polish,.32*patch(x,z,drawer,76,23,10)*front)
                grime=max(grime,.30*bump(x,drawer+42,8)*bump(z,65,5)*front)
        if material=='Iron' and z>96:
            polish=max(polish,.35*patch(x,y,100,-40,22,19)*bump(z,111,16))
    elif asset=='SM_WorkshopRack':
        grime=.65*bump(z,0,24)
        for shelf in [12,73,136,201,267]:
            lip=bump(y,-30.5,7)*bump(z,shelf+2.5,6)
            hand=max(bump(x,bay,31) for bay in [-115,-38,38,115])
            polish=max(polish,.56*lip*hand)
            back=bump(y,23,9)*bump(z,shelf+4,8)
            corner=max(bump(x,divider,12) for divider in [-151,-78,0,78,151])
            grime=max(grime,.44*back*corner)
        if material=='Brass':
            polish=max(polish,.62*bump(y,-32,5)*bump(z,46,5))
    elif asset=='SM_WorkshopToolTray':
        if material=='Wood':
            polish=.46*bump(z,5,4)*max(bump(x,-37,4),bump(x,37,4),bump(y,-20,4))
            grime=.34*bump(z,1,2)*max(patch(x,y,-31,15,10,8),patch(x,y,29,14,10,8))
        else:polish=.44*bump(y,-12,9)
    return min(1.0,polish),min(1.0,grime)

def author_uv(ob,asset,index):
    """UV0: V follows each board's long axis; end caps have a transverse chart.

    The texture is assumed to have vertical grain. Both axes are 60 cm per
    tile. Stable component offsets avoid identical grain on adjacent boards.
    """
    for existing in list(ob.data.uv_layers):ob.data.uv_layers.remove(existing)
    layer=ob.data.uv_layers.new(name='UVMap')
    lo=[min(v.co[k] for v in ob.data.vertices) for k in range(3)]
    hi=[max(v.co[k] for v in ob.data.vertices) for k in range(3)]
    grain=ob.get('grain_axis',max(range(3),key=lambda k:hi[k]-lo[k]))
    digest=hashlib.sha256(json.dumps([asset,index,lo,hi],sort_keys=True).encode()).digest()
    offset=(int.from_bytes(digest[:4],'little')/2**32,int.from_bytes(digest[4:8],'little')/2**32)
    for face in ob.data.polygons:
        dominant=max(range(3),key=lambda k:abs(face.normal[k]))
        axes=[k for k in range(3) if k!=dominant]
        wood=ob.data.materials[face.material_index].name=='Wood'
        if wood and dominant!=grain:axes=[next(k for k in axes if k!=grain),grain]
        for li in face.loop_indices:
            co=ob.data.vertices[ob.data.loops[li].vertex_index].co
            layer.data[li].uv=(co[axes[0]]/UV_TILE_CM+(offset[0] if wood else 0),
                               co[axes[1]]/UV_TILE_CM+(offset[1] if wood else 0))

def geom(name, verts, faces, mat, smooth=False):
    d=bpy.data.meshes.new(name); d.from_pydata(verts,[],faces); d.update()
    bm=bmesh.new(); bm.from_mesh(d); bmesh.ops.recalc_face_normals(bm,faces=bm.faces); bm.to_mesh(d); bm.free()
    ob=bpy.data.objects.new(name,d); bpy.context.collection.objects.link(ob); d.materials.append(MATS[mat])
    for p in d.polygons:p.use_smooth=smooth
    parts.append(ob); return ob

def box(p,s,mat='Wood',edge=.6,mask_step=None):
    if mask_step:
        # Matching anisotropic grids on the six sides preserve a closed surface.
        # Only components carrying authored spatial masks opt into this density.
        steps=[max(1,math.ceil(d/mask_step)) for d in s];v=[];f=[];indices={}
        def vertex(grid):
            key=tuple(grid)
            if key not in indices:
                indices[key]=len(v)
                v.append([p[k]+(grid[k]/steps[k]-.5)*s[k] for k in range(3)])
            return indices[key]
        for fixed in range(3):
            a,b=[k for k in range(3) if k!=fixed]
            for side in [-1,1]:
                for ia in range(steps[a]):
                    for ib in range(steps[b]):
                        corners=[]
                        for da,db in [(0,0),(0,1),(1,1),(1,0)]:
                            grid=[0,0,0];grid[fixed]=0 if side<0 else steps[fixed]
                            grid[a]=ia+da;grid[b]=ib+db;corners.append(vertex(grid))
                        f.append(corners)
        ob=geom('Joinery',v,f,mat)
        ob['mask_grid']=True
    else:
        # Conventional eight-corner box for pieces without spatial wear regions.
        v=[tuple(p[k]+sign[k]*s[k]/2 for k in range(3)) for sign in [(-1,-1,-1),(1,-1,-1),(1,1,-1),(-1,1,-1),(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]]
        ob=geom('Joinery',v,[(0,3,2,1),(4,5,6,7),(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7)],mat)
    if mat=='Wood':ob['grain_axis']=max(range(3),key=lambda k:s[k])
    if edge:
        b=ob.modifiers.new('Rounded working edges','BEVEL'); b.width=edge; b.segments=3;b.limit_method='ANGLE'
        n=ob.modifiers.new('Face normals','WEIGHTED_NORMAL'); n.keep_sharp=True
    return ob

def lathe(profile,mat='Brass',p=(0,0,0),n=48):
    v=[(p[0]+r*math.cos(i*math.tau/n),p[1]+r*math.sin(i*math.tau/n),p[2]+z) for r,z in profile for i in range(n)]
    f=[(j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i) for j in range(len(profile)-1) for i in range(n)]
    ob=geom('Turned metal',v,f,mat,True)
    # Weld the axis and closed profile seam instead of exporting zero-area caps.
    bm=bmesh.new(); bm.from_mesh(ob.data)
    bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001)
    bmesh.ops.dissolve_degenerate(bm,edges=list(bm.edges),dist=.00001)
    bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces))
    bm.to_mesh(ob.data); bm.free(); ob.data.update()
    return ob

def tube(points,r,mat='Brass',closed=False,mask_step=None):
    if mask_step:
        sampled=[]
        for a,b in zip(points,points[1:]):
            a,b=Vector(a),Vector(b);count=max(1,math.ceil((b-a).length/mask_step))
            sampled.extend(tuple(a.lerp(b,i/count)) for i in range(count))
        points=sampled+[points[-1]]
    d=bpy.data.curves.new('Bent tube','CURVE'); d.dimensions='3D'; d.bevel_depth=r; d.bevel_resolution=2; d.use_fill_caps=True
    s=d.splines.new('POLY'); s.points.add(len(points)-1)
    for v,p in zip(s.points,points):v.co=(*p,1)
    s.use_cyclic_u=closed
    ob=bpy.data.objects.new('Bent tube',d); bpy.context.collection.objects.link(ob); d.materials.append(MATS[mat]); parts.append(ob); return ob

def circle(x,y,z,r,mat='Brass',wire=.7,sideways=False):
    return tube([(x if sideways else x+r*math.cos(i*math.tau/64),
                  y+r*math.cos(i*math.tau/64) if sideways else y,
                  z+r*math.sin(i*math.tau/64)) for i in range(64)],wire,mat,True)

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
    for index,ob in enumerate(parts):
        ob.select_set(True); bpy.context.view_layer.objects.active=ob
        was_tube=ob.type=='CURVE'
        if ob.type!='MESH':bpy.ops.object.convert(target='MESH')
        if was_tube:
            # Curve conversion emits separate cap vertices; weld each tube before joining.
            bm=bmesh.new(); bm.from_mesh(ob.data)
            bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001)
            bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces)); bm.to_mesh(ob.data); bm.free()
        for m in list(ob.modifiers):bpy.ops.object.modifier_apply(modifier=m.name)
        if ob.get('mask_grid'):
            # Bevel can duplicate a vertex where a straight grid edge meets its
            # rim. Weld only this one component, before UVs, never adjacent boards.
            bm=bmesh.new();bm.from_mesh(ob.data)
            bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=.00001)
            bmesh.ops.dissolve_degenerate(bm,edges=list(bm.edges),dist=.00001)
            assert all(e.is_manifold for e in bm.edges),(name,index,'grid component must remain closed')
            bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(ob.data);bm.free()
        author_uv(ob,name,index)
        ob.select_set(False)
    for ob in parts:ob.select_set(True)
    bpy.context.view_layer.objects.active=parts[0]; bpy.ops.object.join(); ob=bpy.context.object; ob.name=name
    bpy.context.scene.cursor.location=(0,0,0); bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
    # Converted lettering can retain zero-area faces. Remove only those faces;
    # separate fittings must not be globally welded into the surrounding joinery.
    bm=bmesh.new(); bm.from_mesh(ob.data)
    def stable_area(face):
        if len(face.verts)==3:
            a,b,c=[v.co for v in face.verts]
            return .5*(b-a).cross(c-a).length
        return face.calc_area()
    bmesh.ops.delete(bm,geom=[f for f in bm.faces if stable_area(f)<1e-8],context='FACES_ONLY')
    bm.to_mesh(ob.data); bm.free(); ob.data.update()
    # Each component already has its full UV0; joining preserves the grain charts.
    assert len(ob.data.uv_layers)==1
    # An angular edge mask drives restrained finish changes in the Unreal materials.
    # This is geometric normal variation, not baked occlusion or signed curvature.
    normals=[Vector((0,0,0)) for _ in ob.data.vertices]; counts=[0]*len(normals)
    for face in ob.data.polygons:
        for vi in face.vertices:normals[vi]+=face.normal; counts[vi]+=1
    wear=[min(1,max(0,(1-n.length/max(1,c))*9)) for n,c in zip(normals,counts)]
    color=ob.data.color_attributes.new(name='SurfaceWear',type='FLOAT_COLOR',domain='CORNER')
    for face in ob.data.polygons:
        material=ob.data.materials[face.material_index].name
        for li in face.loop_indices:
            loop=ob.data.loops[li];polish,grime=contact_masks(name,ob.data.vertices[loop.vertex_index].co,face.normal,material)
            color.data[li].color=(wear[loop.vertex_index],polish,grime,1)
    ob.data.color_attributes.active_color=color
    ob.data.calc_loop_triangles()
    uv=ob.data.uv_layers[0].data
    for tri in ob.data.loop_triangles:
        a,b,c=[uv[li].uv for li in tri.loops]
        assert abs((b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x))>1e-14,(name,tri.polygon_index,'degenerate UV')
    path=OUT/(name+'.fbx')
    bpy.ops.export_scene.fbx(filepath=str(path),use_selection=True,object_types={'MESH'},global_scale=1,
        apply_unit_scale=True,apply_scale_options='FBX_SCALE_NONE',axis_forward='-Y',axis_up='Z',
        mesh_smooth_type='FACE',use_mesh_modifiers=True,add_leaf_bones=False,bake_anim=False,path_mode='STRIP',
        colors_type='LINEAR') # SurfaceWear is numeric data, never display-color sRGB.
    assets.append(dict(name=name,path=path.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
        dimensionsCm=list(ob.dimensions),triangles=len(ob.data.loop_triangles),collision=collision,
        surfaceWearRange={channel:[min(c.color[i] for c in color.data),max(c.color[i] for c in color.data)] for i,channel in enumerate('RGB')}))
    ob.location=((len(assets)-1)%4*500,((len(assets)-1)//4)*500,0)
    parts.clear(); print('EXPORTED',name,assets[-1]['triangles'],flush=True)

# A heavy repair bench: separate boards, pegged joints, drawers, tool well and vice.
for x in [-137,137]:
    for y in [-37,37]:
        box((x,y,47),(13,13,94),edge=1.1,mask_step=9)
        box((x,y,7),(15,15,14),'Iron',.7,mask_step=4)
    box((x,0,30),(9,75,9))
box((0,0,28),(278,7,12))
for i in range(5):box((0,-37+i*18.5,98),(302,18,8),edge=1,mask_step=6)
for x in [-101,0,101]:
    box((x,0,76),(95,75,30),'Wood',.8)
    box((x,-40,76),(94,4,26),'Enamel',.8,mask_step=4)
    for z in [65,87]:box((x,-43,z),(88,2,1.5),'Brass',.2)
    tube([(x-10,-44,79),(x-10,-48,76),(x+10,-48,76),(x+10,-44,79)],1.3,'Brass',mask_step=3)
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
for x in [-156,156]:box((x,0,132),(10,51,264),edge=1.2,mask_step=12)
box((0,24,135),(304,5,255),'Enamel',.6,mask_step=12)
for z in [12,73,136,201,267]:box((0,-2,z),(320,57,7),edge=.7,mask_step=9)
for x in [-78,0,78]:box((x,0,171),(5,48,188),mask_step=12)
for x in [-115,-38,38,115]:
    box((x,-27,43),(72,4,47),'Enamel',.6)
    for i in range(4):box((x,-30,27+i*10),(61,3,4),'Wood',.6)
    rivet(x+26,-32,46)
for x in [-147,147]:box((x,-30,140),(3,2,255),'Brass',.3,mask_step=12)
for x in [-143,143]:box((x,26,342),(10,10,150),'Wood',.9,mask_step=12)
box((0,26,410),(310,10,9),'Wood',.8)
for side in [-1,1]:
    tube([(side*143,26,352),(side*118,26,352),(side*118,-16,352)],1.8,'Iron')
lettering('CELLS   /   FITTINGS   /   REPAIRS',(0,-34,281),9)
box((0,-1,282),(320,54,24),'Enamel',1)
finish('SM_WorkshopRack',True)

# Portable repair lamp: enamel bell, ceramic light body and an articulated spine.
lathe([(0,0),(22,0),(24,3),(23,6),(17,9),(0,9)],'Iron')
tube([(0,0,8),(0,0,45),(8,0,63),(12,0,72)],3.2,'Brass')
lathe([(0,42),(8,42),(9,52),(7,59),(0,60)],'MilkGlass',p=(12,-11,0))
lathe([(32,58),(31,62),(21,72),(9,80),(0,80),(0,78.6),(8.4,78.6),
       (20,70.8),(29.7,61.3),(30.5,58.6),(32,58)],'Enamel',p=(12,-11,0))
lathe([(31.5,58),(32.5,59),(32.5,61),(31.5,62),(30.8,61),(30.8,59),(31.5,58)],'Brass',p=(12,-11,0))
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
    panel=geom('Formed copper petal',vs,fs,'Copper',True)
    shell=panel.modifiers.new('Folded copper sheet thickness','SOLIDIFY'); shell.thickness=.7; shell.offset=0
    rim=panel.modifiers.new('Rolled panel edges','BEVEL'); rim.width=.18; rim.segments=2
    tube([(r*math.cos(a),r*math.sin(a),z+1) for r,z in [(3,70),(10,66),(19,59),(25,51)]],.65,'Brass')
circle(0,0,77,6,'Brass',1.2)
finish('SM_WorkshopPetalLamp')

# Replaceable sealed solar cell, ceramic casing and brass terminal posts.
lathe([(0,0),(7,0),(8,2),(8,5),(6,7),(6,28),(8,30),(8,33),(0,33)],'Ivory')
for z in [4,29]:lathe([(7,z),(8.5,z),(8.5,z+2),(7,z+2)],'Brass')
for x in [-3,3]:lathe([(0,0),(1.5,0),(1.5,5),(0,5)],'Brass',p=(x,0,33),n=16)
box((0,-6.2,17),(6,1,10),'Enamel',.3)
finish('SM_WorkshopSunCell')

# A service-open cell: ceramic wall volume, visible coil and a removed terminal cap.
# The two pieces share a display mesh; future interaction can split their source islands.
lathe([(0,0),(7,0),(8,2),(8,5),(6,7),(6,21),(4.8,21),(4.8,7),(0,7)],'Ivory')
lathe([(0,7),(3.7,7),(3.7,27),(0,27)],'Iron',n=32)
tube([(4.25*math.cos(t*math.tau),4.25*math.sin(t*math.tau),8+t*1.5)
      for t in [i/32 for i in range(353)]],.45,'Copper')
lathe([(0,0),(8,0),(8,3),(0,3)],'Ivory',p=(21,0,0))
for x in [-3,3]:lathe([(0,3),(1.5,3),(1.5,8),(0,8)],'Brass',p=(21+x,0,0),n=16)
for x in [-6,6]:box((x,-11,1),(1.8,1.8,1.2),'Brass',.2)
box((0,-6.2,15),(6,1,8),'Enamel',.3)
finish('SM_WorkshopCellRepair')

# Reels of real coiled cable; rope follows a continuous helix between timber flanges.
for z in [0,27]:lathe([(0,z),(18,z),(19,z+2),(19,z+4),(0,z+4)],'Wood')
lathe([(7,4),(7,27)],'Iron')
for layer in range(3):
    r=9+layer*3
    tube([(r*math.cos(t*math.tau),r*math.sin(t*math.tau),5+t*2.7) for t in [i/30 for i in range(241)]],1.35,'Shadow')
tube([(15,0,8),(25,6,3),(38,-8,2),(48,-4,2)],1.3,'Shadow')
finish('SM_WorkshopCableReel')

# A tray of repair tools, measured to rest on the bench rather than float above it.
box((0,0,1),(76,43,2),'Wood',.5,mask_step=5)
for x in [-37,37]:box((x,0,4),(2,43,7),'Wood',.5,mask_step=5)
for y in [-20,20]:box((0,y,4),(76,2,7),'Wood',.5,mask_step=5)
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
box((0,0,0),(92,6,112),'Enamel',2.7)
for x in [-42,42]:box((x,-4,0),(2,2,103),'Brass',.4)
for z in [-51,51]:box((0,-4,z),(86,2,2),'Brass',.4)
circle(0,-5,9,20,'Brass',1.7)
for side in [-1,1]:
    tube([(0,-5,10),(side*31,-5,34),(side*25,-5,9),(side*35,-5,-10),(0,-5,0)],1.4,'Ivory')
for i in range(12):
    a=i*math.tau/12;tube([(23*math.cos(a),-5,9+23*math.sin(a)),(29*math.cos(a),-5,9+29*math.sin(a))],.8,'Brass')
lettering('REPAIR',(0,-5,-33),10)
for x in [-29,29]:
    # Alternating planes interlock; the terminal link encloses the support bar.
    for i in range(8):circle(x,-1,59+i*5.5,3,'Iron',.6,sideways=bool(i%2))
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
put('shop-name','SM_WorkshopNameplate',(-60,-137,352))
put('trade-sign','SM_WorkshopTradeSign',(302,110,315),yaw=-20)
put('hanging-rail','SM_WorkshopHangingRail',(-60,-157,387))
put('bench-lamp','SM_WorkshopDeskLamp',(-167,55,103),yaw=-18)
put('repair-tray','SM_WorkshopToolTray',(-98,64,103),yaw=8)
put('bench-reel','SM_WorkshopCableReel',(10,70,103),yaw=27)
for i,x,scale,z in [(0,-185,1.0,230),(1,-123,.7,252),(4,65,.93,235)]:
    top=z+82*scale
    put(f'hanging-lamp-{i}','SM_WorkshopPetalLamp',(x,-63,z),s=scale,yaw=i*17)
    put(f'suspension-{i}','SM_WorkshopSuspension',(x,-63,top),s=[1,1,(373-top)/100])
for shelf,z in enumerate([77,140,205]):
    for col,x in enumerate([-176,-99,-22,55]):
        if shelf==1 and col==2:continue # A working shop has a reserved repair bay.
        if (shelf+col)%3==0:
            put(f'stock-reel-{shelf}-{col}','SM_WorkshopCableReel',(x,-147,z),s=.85,yaw=col*33)
        elif (shelf+col)%3==1:
            put(f'stock-lamp-{shelf}-{col}','SM_WorkshopPetalLamp',(x,-146,z),s=.64,yaw=col*19)
        else:
            for j in range(2 if shelf==2 else 3):put(f'stock-cell-{shelf}-{col}-{j}','SM_WorkshopSunCell',(x-19+j*19,-137,z),s=.82)
for i in range(2):put(f'bench-cell-{i}','SM_WorkshopSunCell',(-41+i*19,92,103),yaw=i*14)
put('cell-under-repair','SM_WorkshopCellRepair',(54,61,103),yaw=-11)
put('display-crate','SM_wooden_crate_02',(242,10,0),s=1.65,yaw=-7,collision=True)
put('display-lamp','SM_WorkshopPetalLamp',(242,10,76),s=1.6,yaw=16)
put('floor-reel','SM_WorkshopCableReel',(-270,-30,0),s=1.9,yaw=38)
put('spare-lamp','SM_WorkshopDeskLamp',(-289,45,0),s=1.15,yaw=34)
put('mechanic-stool','SM_wooden_stool_01',(-274,-117,0),s=1.2,yaw=21,collision=True)
definition=OUT/'SecondDawn.assembly.json'
definition.write_text(json.dumps(dict(schemaVersion=1,id='orison.second-dawn',name='DA_SecondDawnWorkshop',parts=recipe),indent=2)+'\n',encoding='utf-8',newline='\n')
bpy.context.preferences.filepaths.save_version=0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'LanternWorkshop.blend'))
files=[dict(path=p.relative_to(ROOT).as_posix(),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [definition,OUT/'LanternWorkshop.blend']]
(OUT/'workshop-manifest.json').write_text(json.dumps(dict(schemaVersion=1,author='Original Borrowed Sun Second Dawn workshop',
    blenderVersion=bpy.app.version_string,units='centimetres',assetCount=len(assets),materialCount=7,assets=assets,files=files,
    uv0=dict(tileSizeCm=UV_TILE_CM,woodGrainAxis='V follows each board longest axis; end caps use transverse projection',boardOffsets='deterministic per construction component'),
    vertexColor=dict(name='SurfaceWear',domain='CORNER',encoding='linear numeric masks; FBX colors_type=LINEAR',R='unsigned angular-edge variation; not ambient occlusion or signed curvature',G='authored contact polish: hands, work zones, drawer handles and shelf lips',B='authored localized grime: servicing spot, joints, rear shelf corners and feet',A='constant one'),
    assembly=definition.relative_to(ROOT).as_posix(),assemblyPartCount=len(recipe)),indent=2)+'\n',encoding='utf-8',newline='\n')
print('WORKSHOP COMPLETE',len(assets),'meshes',len(recipe),'parts',flush=True)
