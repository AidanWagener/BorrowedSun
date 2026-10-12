"""Build the planned world library, offline catalog and family index. No 3D assets are generated.

Stable IDs derive from fixed family/form keys, never from the ordering of a whole catalog.
Construction profiles require actual geometric changes; a recolor never qualifies a second asset.
"""
from pathlib import Path
import argparse, collections, hashlib, html, json, re

ROOT = Path(__file__).resolve().parents[2]
FAMILIES = []
PROFILES = {
    "quay_masonry": {"name": "Quay masonry", "district": "Low Tides", "geometry": "Load-bearing cut sandstone with deep beds, radial voussoirs at curves, drainage falls, thick reveals and individually readable repair inserts.", "materials": "sandstone; lime mortar; brass anchors", "depthCm": [35, 70]},
    "courtyard_infill": {"name": "Courtyard infill", "district": "Low Tides / Ladder", "geometry": "Recessed plaster infill held by a visible timber frame; modeled perimeter stops, timber joinery, lime capping and replaceable inset panels.", "materials": "lime plaster; timber; indigo cloth", "depthCm": [20, 35]},
    "hanging_timber": {"name": "Hanging timber", "district": "Ladder", "geometry": "Light open framing with diagonal tension braces, interlocking timber joints, bolted metal shoes and exterior service ledges; different section profiles from masonry.", "materials": "oiled timber; oxidized copper; iron", "depthCm": [15, 30]},
    "civic_cast": {"name": "Civic castwork", "district": "Crown", "geometry": "Cast mineral shell with ribbed rear stiffeners, recessed expansion joints, cast anchoring pockets and stepped modular returns; broad calm faces.", "materials": "ivory mineral composite; pale stone; brass", "depthCm": [25, 45]},
    "pressure_shell": {"name": "Pressure shell", "district": "Orbit / service yards", "geometry": "Ribbed metal shell with welded flanges, gasket channels, rounded pressure corners, inspection covers and visible attachment points; retain external cladding as a separate layer.", "materials": "painted steel; copper; gasket rubber", "depthCm": [20, 40]},
    "workshop": {"name": "Workshop made", "district": "Low Tides / Ladder", "geometry": "Individually assembled construction: separate repairable body pieces, hand-fitted sections, exposed joinery or seams, replaceable fittings and a distinctive asymmetric maker silhouette.", "materials": "family palette with visible construction seams"},
    "concession": {"name": "Concession standard", "district": "Crown / citywide resale", "geometry": "A separately modeled manufactured design: pressed or molded main body, integrated grip or support profile, standardized closure or joint and replaceable service insert. Change silhouette and construction, not just marks or color.", "materials": "family palette with standardized manufactured sections"},
    "open_service": {"name": "Open service", "district": "Low Tides / Ladder / Crown", "geometry": "Accessible surface installation with visible brackets, drip management, removable guards and reachable manual controls.", "materials": "copper; enamel steel; iron; rubber"},
    "sealed_service": {"name": "Sealed service", "district": "Orbit / pressure rooms", "geometry": "Pressure-rated enclosed installation with a different sealed housing, bolted access flange, captive fasteners, gasket seat and protected connector geometry.", "materials": "painted alloy; gasket rubber; ceramic; glass"},
    "authored": {"name": "Authored form", "district": "Assigned by family", "geometry": "Individually authored silhouette and internal construction appropriate to the named form; neither a mirrored export nor a material swap.", "materials": "family palette"}
}

def slug(value):
    return re.sub(r"[^a-z0-9]+", "_", value.lower()).strip("_")

def group(domain, category, profiles, size, materials, interaction, destruction, source, district, rows):
    for raw in rows.strip().splitlines():
        family, forms = raw.strip().split(":", 1)
        forms = [x.strip() for x in forms.split(";")]
        assert len(forms) == 8, (family, len(forms))
        key = f"{domain}.{slug(family)}"
        assert not any(f["id"] == key for f in FAMILIES), key
        FAMILIES.append(dict(id=key, domain=domain, category=category, name=family,
            forms=forms, profiles=profiles, sizeEnvelopeCm=size, materials=materials,
            interaction=interaction, destruction=destruction, sourceRoute=source, district=district))

ARCH = ["quay_masonry", "courtyard_infill", "hanging_timber", "civic_cast", "pressure_shell"]
CRAFT = ["workshop", "concession"]
SERVICE = ["open_service", "sealed_service"]

group("architecture", "Envelope", ARCH, [100, 800], "profile", "FIXED", "D3_SELECTED", "CUSTOM + architectural donor", "profile", """
Wall bay: solid bay; narrow service bay; tall clerestory bay; paired window bay; recessed shop bay; open ventilation bay; angled lane bay; stepped terrain bay
Arcade support: square pier; octagonal pier; bundled column; forked capital pier; wall-engaged half pier; corner return pier; bridge bearing pier; deep alcove pier
Arch span: shallow segmental arch; tall pointed arch; round passage arch; three-center shop arch; asymmetric stair arch; paired slender arch; broad freight arch; compact blind arch
Door surround: single domestic reveal; paired shop reveal; sliding service reveal; tall civic reveal; inset airlock reveal; side-light reveal; transom reveal; low cellar reveal
Window surround: deep square reveal; tall narrow reveal; circular oculus; paired opening; projecting bay opening; clerestory strip opening; arched opening; corner light opening
Ventilation screen: vertical slat frame; radial pierced frame; staggered brick screen; diagonal lattice frame; double-depth baffle; honeycomb opening screen; rotating louver frame; perforated solid panel
Parapet: plain capped parapet; drainage notch parapet; slotted service parapet; stepped corner parapet; scalloped parapet; rail-backed parapet; planter-depth parapet; wind-baffle parapet
Cornice: shallow drip cornice; deep shade cornice; cable channel cornice; bracketed cornice; stepped civic cornice; corner mitre cornice; curved balcony cornice; vented roof cornice
Floor panel: square infill panel; long passage panel; drain-edge panel; service hatch frame; diagonal corner panel; curved edge panel; split-level transition; grated ventilation panel
Roof structure: shallow barrel segment; ribbed half dome; folded sawtooth segment; pitched ridge segment; flat service deck; glazed lantern curb; curved eave segment; collector bearing segment
Roof edge: straight eave; outside eave corner; inside eave corner; gutter outlet eave; gable rake; service cable eave; curved eave return; deep shade soffit
Balcony deck: narrow overlook; deep social balcony; clipped corner balcony; curved frontage balcony; split-level balcony; bridge landing balcony; planter-integrated balcony; service access balcony
Balustrade: solid low guard; vertical picket guard; lattice privacy guard; rope-inset guard; broad civic guard; curved return guard; hinged service guard; planter-back guard
Stair flight: four-riser short flight; eight-riser lane flight; sixteen-riser full flight; broad civic flight; tight turning flight; tapered wedge flight; split flight with landing; service open-riser flight
Stair landing: square turn landing; rectangular rest landing; half-round landing; fork landing; bridge transfer landing; recessed door landing; overlook landing; utility-access landing
Ramp: short threshold ramp; cart access ramp; broad promenade ramp; switchback ramp segment; curved climbing ramp; dock transfer ramp; grated service ramp; wedge terrain ramp
Foundation: deep quay footing; stepped slope footing; arch-bearing footing; pier cap footing; retaining corner footing; isolated plinth footing; cantilever anchor footing; raised damp-proof footing
Retaining wall: straight high retaining bay; low garden retaining bay; buttressed retaining bay; curved concave bay; curved convex bay; drainage outlet bay; stepped height bay; access recess bay
Buttress: narrow wall buttress; broad arcade buttress; flying support arm; stepped terrace buttress; angled corner buttress; fork bridge support; hollow service buttress; embedded cistern buttress
Bridge deck: short footbridge span; covered crossing span; wide goods span; curved terrace span; hinged maintenance span; split pedestrian goods span; suspended frame span; central overlook span
Bridge abutment: square bank seat; angled bank seat; stepped cliff seat; tall pier seat; narrow lane seat; broad goods seat; channel-crossing seat; recessed service seat
Colonnade junction: straight junction; outside corner junction; inside corner junction; tee junction; cross junction; slope transition junction; terminal alcove junction; offset span junction
Interior partition: plain room partition; kitchen service partition; storage niche partition; ventilated sleeping partition; folding screen support; clinic observation partition; double-depth sound partition; glazed office partition
Ceiling structure: shallow coffer bay; deep beam bay; vaulted room bay; ribbed service bay; ventilated panel bay; hanging rail bay; stepped skylight bay; removable maintenance bay
Interior alcove: sleeping recess; water jar recess; cooking recess; shrine-free remembrance recess; tool storage recess; seating recess; plant recess; meter recess
Door leaf: plank swing leaf; lattice ventilation leaf; solid reinforced leaf; paired folding leaf; sliding pocket leaf; glazed shop leaf; grated service leaf; sealed pressure leaf
Window closure: side-hinged shutter; top-hinged shade shutter; folding double shutter; sliding privacy shutter; fixed translucent inset; pivoting louver closure; glazed casement; sealed inspection port
Shopfront: fold-up counter front; shuttered goods front; deep display front; corner shop front; open workshop front; pass-through food front; narrow repair front; double-height merchant front
Canopy support: paired post frame; single cantilever frame; wall bracket frame; crossed ridge frame; stepped lane frame; freestanding four-post frame; circular umbrella frame; long market spine frame
Service hatch: square floor hatch; narrow wall hatch; double-leaf machinery hatch; circular pressure hatch; louvered inspection hatch; flush civic hatch; raised flood hatch; ceiling access hatch
Paving edge: straight kerb; rounded crossing kerb; stair nose; drain channel edge; quay mooring edge; planter kerb; curved courtyard edge; service trench edge
Courtyard basin: shallow stepped basin; long washing basin; circular collecting basin; corner catch basin; divided treatment basin; raised drinking basin; overflow cascade basin; dry planted basin
Drain opening: slot inlet; grated square inlet; broad channel inlet; wall scupper; curved kerb inlet; removable sump cover; inspection pit surround; screened overflow outlet
Facade bracket: shallow lamp corbel; deep balcony corbel; paired shade corbel; cable support corbel; sign support corbel; collector arm corbel; planter support corbel; climbing access corbel
Street portal: compact residential portal; broad dock portal; double arcade portal; elevated garden portal; customs checkpoint portal; workshop yard portal; collector maintenance portal; pressure vestibule portal
Roof lantern: square light well; octagonal light well; ridge light strip; curved clerestory well; baffled heat exhaust; screened moonlight well; stepped civic light well; sealed observatory well
Terrace corner: open right-angle corner; rounded overlook corner; chamfered market corner; recessed service corner; planted concave corner; projecting stair corner; split-height corner; bridge landing corner
Expansion connector: straight flush seam; raised waterstop seam; articulated slope seam; flexible bridge seam; wide seismic seam; sliding roof seam; pressure transition collar; removable service seam
Vertical shaft: square stair shaft; round ventilation shaft; paired lift shaft; narrow cable shaft; open goods-hoist shaft; screened exhaust shaft; glazed civic shaft; sealed service shaft
Landmark base: circular statue base; stepped sun platform; broad gathering dais; water-channel dais; cantilever overlook base; recessed memorial base; machinery-bearing base; collector root base
""")

group("props", "Containers", CRAFT, [8, 120], "timber; ceramic; copper; woven fiber", "CARRY_COLLECT", "D1_BREAKABLE", "CUSTOM + CC0 + market donor", "Low Tides / citywide", """
Storage crate: open slatted crate; partitioned bottle crate; reinforced cargo crate; shallow produce crate; vented courier crate; corner-foot crate; hinged instrument crate; tall narrow crate
Basket: shallow sorting basket; deep carrying basket; twin-handle basket; lidded storage basket; shoulder pannier; hanging bulb basket; rectangular woven tray; tapered harvest basket
Jar: broad grain jar; narrow oil jar; twin-ear water jar; squat spice jar; screw-collar reagent jar; footed ceremonial jar; handled ferment jar; clamp-lid storage jar
Bottle: long-neck oil bottle; flat travel bottle; bulbous drinking bottle; square medicine bottle; dropper bottle; wide-mouth sample bottle; siphon bottle; protected pressure bottle
Canister: latch-top provisions can; ribbed fuel can; insulated water can; tall filter can; shallow parts can; dual-compartment can; handled emergency can; compact capsule can
Case: clamshell tool case; sliding instrument case; divided medicine case; folding document case; hard transit case; cylindrical lens case; strapped sample case; nested repair case
Bucket: shallow wash bucket; tall water bucket; spouted pour bucket; divided cleaning bucket; folding frame bucket; lidded refuse bucket; hook-backed service bucket; footed heat bucket
Tray: low food tray; compartment parts tray; deep draining tray; handled service tray; circular tea tray; hanging drying tray; cushioned instrument tray; narrow desk tray
Box: hinged keepsake box; sliding money box; tiered sewing box; perforated insect box; latch-front letter box; stacked lunch box; locking permit box; fitted calibration box
Tank vessel: squat refill vessel; tall domestic reservoir; wheeled service vessel; saddle cargo vessel; vented fermentation vessel; pressure accumulator vessel; insulated clinic vessel; double-wall cooling vessel
""")
group("props", "Cooking and dining", CRAFT, [5, 90], "ceramic; enamel; copper; timber; iron", "CARRY_COLLECT", "D1_BREAKABLE", "CUSTOM + CC0 + market donor", "Low Tides / Ladder", """
Bowl: shallow eating bowl; deep broth bowl; footed serving bowl; lipped mixing bowl; divided condiment bowl; handled warming bowl; perforated rinse bowl; nested measuring bowl
Plate: broad dinner plate; deep rim plate; rectangular street-food plate; divided meal plate; footed display plate; small condiment plate; heatproof cooking plate; drain-slot plate
Cup: loop-handle mug; flared tea cup; narrow travel cup; double-wall insulated cup; footed tasting cup; lidded infuser cup; measuring cup; collapsible bellows cup
Pitcher: tall water pitcher; short milk pitcher; long-spout oil pitcher; double-handle serving pitcher; lidded cooling pitcher; strainer-neck pitcher; insulated communal pitcher; narrow shelf pitcher
Cooking pot: wide stew pot; tall steam pot; handled skillet pot; pressure-lid pot; divided hot pot; hanging cauldron pot; shallow grain pot; insulated slow-cook pot
Pan: shallow frying pan; deep wok pan; ribbed griddle pan; covered baking pan; perforated roasting pan; long fish pan; divided snack pan; handled warming pan
Kitchen utensil: ladle; slotted serving spoon; long stirring paddle; hinged tongs; wire skimmer; pastry roller; measuring scoop; perforated turner
Cutting implement: broad chopping blade; curved herb blade; narrow fillet blade; bread saw; folding preparation knife; rotary dough wheel; palm peeler; straight scraping blade
Food preparation tool: mortar and pestle unit; hand grain grinder; lever fruit press; folding drying rack; dough kneading trough; nested steamer insert; hand whisk frame; strainer stand
Table accessory: three-leg trivet; folding cup stand; napkin clamp; utensil rail; oil warmer; salt cellar; covered condiment stand; food cover frame
""")
group("props", "Furniture", CRAFT, [35, 240], "timber; iron; woven fiber; enamel", "HEAVY_MOVE", "D2_ASSEMBLY", "CUSTOM + market donor", "Low Tides / Ladder / Crown", """
Chair: straight dining chair; curved reading chair; folding dock chair; low woven chair; tall counter chair; wheeled workshop chair; broad civic chair; narrow transit chair
Stool: three-leg stool; four-leg square stool; stepped work stool; circular swivel stool; folding stool; low kneeling stool; storage stool; narrow leaning stool
Bench: backless communal bench; backed garden bench; curved gathering bench; folding dock bench; storage bench; high workshop bench; padded clinic bench; split-seat transit bench
Table: square meal table; round tea table; long communal table; folding market table; nested side table; height-adjustable work table; wall-drop table; recessed board-game table
Cabinet: shallow tool cabinet; tall food cabinet; glazed display cabinet; divided medicine cabinet; louvered drying cabinet; wall document cabinet; drawer parts cabinet; double-door storage cabinet
Shelf: ladder shelf; wall bracket shelf; corner shelf; hanging rope shelf; modular crate shelf; deep workshop shelf; narrow spice shelf; enclosed service shelf
Bed frame: low domestic frame; folding clinic cot; stacked bunk frame; curtained alcove frame; wheeled treatment frame; suspended berth frame; compact crew berth; communal rest platform
Counter: straight market counter; corner trade counter; curved reception counter; fold-out food counter; shutter-integrated counter; glass display counter; tool vise counter; wheeled vendor counter
Screen divider: three-leaf woven divider; perforated rigid divider; sliding cloth divider; freestanding glazed divider; hinged timber divider; plant-support divider; privacy louver divider; acoustic padded divider
Display stand: stepped produce stand; rotating smallwares stand; hanging utensil stand; tiered lantern stand; inclined book stand; long cloth stand; secure instrument stand; nesting gift stand
""")
group("props", "Tools", CRAFT, [8, 110], "steel; brass; timber; grip rubber", "CARRY_COLLECT", "D0_DURABLE", "CUSTOM + workshop donor", "Low Tides / service yards", """
Hammer: cross-peen hammer; ball-peen hammer; soft-face mallet; long forging hammer; nail claw hammer; stone mason hammer; compact riveting hammer; dead-blow service hammer
Wrench: open-ended wrench; ring wrench; adjustable wrench; pipe wrench; long torque wrench; pin spanner; strap wrench; socket ratchet
Pliers: needle-nose pliers; broad gripping pliers; diagonal cutters; locking pliers; insulated crimper; circlip pliers; cable shear; bent-nose service pliers
Saw: frame bow saw; narrow coping saw; folding hand saw; broad crosscut saw; flush trim saw; metal hacksaw; keyhole saw; compact reciprocating tool
Driver: straight blade driver; cross-head driver; offset driver; ratchet driver; insulated precision driver; torque-limited driver; long-reach driver; powered hand driver
Clamping tool: screw C clamp; quick bar clamp; corner clamp; spring clamp; bench vise; strap clamp; pipe clamp; suction alignment clamp
Measuring tool: folding rule; vernier caliper; dial gauge; plumb bob housing; spirit level; angle square; contour gauge; laser alignment tool
Shaping tool: broad wood plane; fine cabinet plane; curved rasp; flat metal file; stone chisel; curved gouge; hand scraper; edge deburring tool
Welding tool: soldering iron; brazing torch; arc welding handle; heat shield paddle; flux applicator; solder reel holder; electrode clamp; hot-air repair handle
Portable machine: hand drill; compact grinder; palm sander; cable tester; pressure test pump; thermal inspection scanner; filter cleaner; portable vacuum extractor
""")
group("props", "Lamp making and optics", CRAFT, [3, 95], "copper; brass; milk glass; enamel; ceramic", "CARRY_COLLECT", "D1_BREAKABLE", "CUSTOM signature", "Second Dawn / citywide", """
Lantern body: petal cage lantern; ribbed drum lantern; folded hood lantern; shielded dock lantern; faceted street lantern; perforated table lantern; concentric ring lantern; compact travel lantern
Desk lamp: gooseneck lamp; pivot-arm lamp; weighted dome lamp; twin-arm inspection lamp; folding travel lamp; clamped bench lamp; low reading lamp; magnifier lamp
Lamp shade: conical reflector; petal reflector; fluted bowl reflector; slotted drum shade; asymmetric hood shade; layered ring shade; faceted bell shade; louvered square shade
Lamp pedestal: three-foot pedestal; weighted circular pedestal; clamp pedestal; wall-return pedestal; gimbal pedestal; folding pedestal; magnetic service pedestal; short articulated pedestal
Lens assembly: convex reading lens; fluted diffuser lens; segmented Fresnel lens; twin-lens focus unit; rectangular prism unit; protective lens cage; pivoting signal lens; shuttered inspection lens
Light cell: squat charge cell; long instrument cell; paired reserve cell; ribbed thermal cell; handle-top utility cell; flat portable cell; sealed clinic cell; keyed public lamp cell
Optical instrument: hand viewing scope; folding rangefinder; mirror alignment scope; beam splitter unit; inspection periscope; calibration light meter; pocket spectroscope; lens polishing spindle
Signal light: hooded amber beacon; twin shutter signal; rotating dock lamp; directional line lamp; folding emergency lamp; protected stair lamp; transit stop lamp; service warning lamp
Reflector: shallow oval reflector; deep dish reflector; segmented ring reflector; angular folded reflector; narrow trough reflector; handheld inspection mirror; twin pivot reflector; collector calibration target
Lamp service part: threaded lamp collar; bayonet lamp socket; ceramic terminal block; finned heat sink; adjustable pivot yoke; hanging chain anchor; enclosed switch pod; quick-release battery shoe
""")
group("props", "Textiles and personal equipment", CRAFT, [10, 220], "woven fiber; leather-like plant laminate; brass fittings", "CARRY_COLLECT", "D1_STATE_SWAP", "CUSTOM + cloth donor", "Ladder / Low Tides", """
Carry bag: shoulder satchel; roll-top courier bag; twin-pannier bag; framed tool bag; drawstring provisions bag; narrow document bag; padded instrument bag; folding shopping bag
Protective cover: tied vessel cover; buckled equipment cover; hinged padded cover; drawcord lens cover; fitted chair cover; folded rain cover; peaked stall cover; strapped transit cover
Cushion: square seat cushion; long bench cushion; round floor cushion; wedge back cushion; folded berth cushion; tubular bolster; quilted clinic cushion; narrow kneeling cushion
Floor textile: rectangular woven mat; round braided mat; long runner mat; threshold rib mat; layered sleeping mat; folded cargo blanket; rolled awning fabric; bound-edge prayer-neutral gathering mat
Work apron: short tool apron; full metalworker apron; crossed-back cook apron; pocketed lampmaker apron; waterproof wash apron; padded cargo apron; split gardener apron; insulated service apron
Glove pair: reinforced cargo gloves; fine repair gloves; heat-resistant gloves; sealed service gloves; gardening gloves; padded flight gloves; long cleaning gloves; fingerless market gloves
Headwear prop: soft courier cap; broad sun hat; folded head wrap; protective service hood; rigid dock helmet; cloth flight helmet; clinic fabric hood; collapsible rain hood
Footwear prop: low market shoes; tall work boots; soft indoor slippers; sealed service boots; strapped climbing shoes; reinforced cargo shoes; insulated crew boots; waterproof wash boots
Belt equipment: broad tool belt; looped cable belt; satchel harness; climbing safety harness; flight restraint harness; compact utility belt; padded lifting belt; medical carrying harness
Rope equipment: coiled mooring rope; braided lifting sling; knotted climbing rope; wrapped handline; net cargo harness; rolled safety net; adjustable tie-down strap; hanging drying line spool
""")
group("props", "Trade and navigation", CRAFT, [3, 100], "paper; timber; brass; ceramic; glass", "CARRY_COLLECT", "D1_STATE_SWAP", "CUSTOM signature + CC0 donor", "Low Tides / Crown / Orbit", """
Trade scale: twin-pan scale; hanging spring scale; platform grain scale; fine reagent scale; beam cargo scale; folding travel scale; balance weight stand; sealed customs scale
Accounting device: sliding bead ledger; geared tally counter; stamped receipt press; mechanical calculator; pocket transaction terminal; queue ticket dispenser; permit validation stamp; security seal applicator
Document object: folded cargo manifest; bound repair ledger; rolled city chart; hinged permit wallet; pocket route booklet; ring-bound service manual; sealed dispatch envelope; rigid identity folio
Navigation instrument: brass compass; folding sextant; horizon level; star sight tube; route plotting frame; pocket beacon receiver; orbital timing dial; magnetic anomaly probe
Writing tool: refill pen; thick marking stylus; chalk holder; stamping brush; engraving stylus; seal press handle; drafting compass; ink reservoir stand
Map support: folding chart board; rolled chart tube; clamped route easel; circular plotting table top; hinged navigation slate; wall map rail; portable map case; projection alignment frame
Courier equipment: keyed parcel clip; locking cargo strap; delivery proof stamp; insulated parcel pod; fold-out address board; priority marker frame; tamper seal roll; portable dispatch buzzer
Communication device: hand radio; horn-backed announcer; portable recorder; signal whistle; wired field handset; directional receiver; folding antenna pack; short-range message terminal
Clock: wall pendulum clock; compact pocket clock; mechanical sun schedule dial; shift alarm clock; navigation chronometer; kitchen timer; service interval counter; queue countdown clock
Permit token: pierced daylight token; notched water token; embossed transit token; keyed maintenance token; laminated crew token; ceramic clinic token; folding visitor token; sealed cargo token
""")
group("props", "Medicine and household care", CRAFT, [3, 150], "ceramic; steel; glass; cloth; rubber", "CARRY_COLLECT", "D1_BREAKABLE", "CUSTOM + domestic donor", "Clinic / Ladder", """
Medical instrument: diagnostic hand scanner; protected thermometer; pressure cuff unit; folding splint; examination light handle; compact otoscope; sample pipette unit; sealed stethoscope unit
Medical vessel: narrow sample vial; insulated medicine flask; wide rinse basin; covered instrument dish; divided reagent cup; sealed specimen box; dispensing pump bottle; portable oxygen cylinder
First aid object: bandage roll; rigid field dressing case; burn compress packet; fold-out emergency kit; tourniquet case; wound wash capsule; thermal blanket pouch; sling packet
Cleaning implement: long floor broom; hand brush; curved pipe brush; squeegee; mop head and handle; scrubbing block; dustpan; folded cleaning cloth holder
Washing object: deep laundry tub; ribbed washboard; hand wringer; drying peg rack; soap dish; suspended drying hoop; covered wash pail; folded drying frame
Heating object: portable radiant heater; shielded foot warmer; insulated hot-water vessel; ceramic heat brick case; compact room stove; duct booster unit; heated cushion base; thermal storage cassette
Cooling object: folding hand fan; pivoting desk fan; insulated food chest; evaporative cooling jar; window fan unit; portable mist head; cold-pack carrier; cooling coil basket
Plant care tool: narrow watering can; hand trowel; pruning shears; seed dibber; soil sieve; grafting tool case; mist sprayer; moisture probe
Personal care object: handled mirror; folding comb; shaving kit box; narrow wash cup; hair clip tray; garment brush; nail tool case; refill soap dispenser
Repair consumable: threaded fastener tin; gasket sample roll; fuse strip box; wire offcut reel; adhesive cartridge; patch plate bundle; cloth repair spool; replacement clip strip
""")
group("props", "Small machinery and salvage", CRAFT, [4, 140], "steel; copper; brass; rubber; enamel", "CARRY_COLLECT", "D1_STATE_SWAP", "CUSTOM + Minerva / Neo City donor", "Service yards / citywide", """
Motor: compact spindle motor; finned pump motor; belt-drive motor; sealed fan motor; flat wheel motor; articulated servo motor; twin-shaft motor; compact generator motor
Pump: manual lever pump; rotary water pump; diaphragm pump; piston service pump; compact vacuum pump; dual-chamber transfer pump; filter-backwash pump; hand bulb pump
Valve: wheel-operated gate valve; quarter-turn ball valve; pressure relief valve; flap check valve; needle metering valve; three-way selector valve; foot pedal valve; quick shutoff valve
Coupling: threaded pipe coupling; flanged pipe coupling; flexible bellows coupling; quick-release hose coupling; shaft jaw coupling; keyed cable coupling; bayonet optical coupling; sealed pressure coupling
Filter: pleated air cartridge; mesh water strainer; ceramic filter candle; charcoal filter box; spiral dust separator; washable screen drum; oil trap cartridge; sealed medical filter
Gauge: round pressure gauge; rectangular voltage meter; tube flow gauge; folding angle gauge; dial temperature gauge; optical alignment gauge; rotary torque gauge; paired system gauge
Switch unit: covered toggle unit; large rotary selector; guarded pushbutton; lever disconnect; foot switch; keyed authorization switch; three-position slide switch; emergency pull switch
Reel: broad cable reel; narrow solder reel; hose reel; flat strap reel; twin-wire signal reel; enclosed spring reel; folding handline reel; compact inspection reel
Drive component: spoke belt pulley; toothed timing pulley; large handwheel; worm gear unit; exposed sprocket; folding crank; eccentric cam unit; tensioning roller
Salvage component: dented heat exchanger; detached thruster liner; split sensor housing; obsolete control keypad; cut support bracket; recovered collector rib; cracked insulator body; stripped circuit chassis
""")
group("props", "Civic life and leisure", CRAFT, [4, 180], "timber; brass; cloth; ceramic; paper", "CARRY_COLLECT", "D1_STATE_SWAP", "CUSTOM signature", "Citywide", """
Musical object: bowed string instrument; plucked box instrument; hand drum; reed wind instrument; bell frame; folding concertina; resonant metal bowl; portable street amplifier
Game object: folding tile board; nesting strategy pieces; carved puzzle cage; weighted throwing ring; spinning tabletop toy; miniature sailing craft; mechanical walking toy; stackable balance blocks
Memory object: hinged portrait frame; engraved remembrance plate; bound family album; miniature house model; repaired childhood toy; pressed flower frame; voyage medal case; carved keeper figurine
Ceremony object: floating light bowl; folded festival crown; hanging wind chime; framed civic ribbon; portable sun reflector; remembrance ribbon spool; illuminated procession staff; shared meal token stand
Shop sign object: hanging lantern emblem; bread oven emblem; tailor spool emblem; water refill emblem; coil repair emblem; clinic hand emblem; courier wing emblem; garden sprout emblem
Public service object: queue barrier stand; folding notice easel; lost-property box; portable voting box; community donation vessel; public book crate; reusable festival cup rack; shared umbrella stand
Education object: slate writing board; mechanical orbit model; lens teaching box; seed sorting board; geometry model frame; letter tile case; simple circuit teaching panel; folded learning chart
Sports equipment: weighted practice ball; climbing grip block; padded sparring target; balance pole; flexible practice hoop; compact exercise weight; folded court net; hand-driven training wheel
Travel comfort: folding meal kit; portable seat frame; rolled sleeping bag; padded neck rest; compact wash kit; collapsible travel basin; shoe drying frame; insulated drink carrier
Household keepsake: geometric mantel ornament; carved moth wing; hinged music box; miniature mirror tower; engraved voyage cup; family recipe tin; woven wall token; repaired mechanical bird
""")

group("infrastructure", "Utility networks", SERVICE, [30, 600], "profile", "FIXED_USE", "D2_SELECTED", "CUSTOM + Minerva / Neo City donor", "profile", """
Pipe run: straight one-meter run; straight three-meter run; right-angle elbow; shallow offset; tee junction; cross junction; expansion loop; flanged termination
Cable route: wall conduit; overhead tray; floor trench; vertical riser; curved turning tray; sealed wall penetration; branch distribution box; service slack loop frame
Water station: public refill head; household metering box; filter service column; wash tap bank; fire water outlet; portable hose station; pressure reduction station; overflow monitoring station
Electrical cabinet: small fuse cabinet; wide distribution cabinet; dual-meter cabinet; battery change cabinet; public lighting cabinet; emergency isolation cabinet; collector control cabinet; clinic priority cabinet
Heat exchanger: wall radiator bank; roof fin array; tube bundle exchanger; counterflow box exchanger; service cooling tower; compact recuperator; heated bench core; water-loop exchanger
Ventilation unit: wall axial fan; roof exhaust fan; baffled intake box; large duct elbow; adjustable damper box; filter plenum; sound-lined exhaust; smoke extraction head
Collector mechanism: narrow tracking yoke; broad mirror yoke; edge drive carriage; tilt bearing pedestal; focus alignment head; maintenance brake assembly; counterweight frame; storm stow cradle
Dock service: mooring clamp; retractable bollard; hose supply pedestal; cargo scan arch; refueling mast; tow winch; docking guidance beacon; shore-power cabinet
Lift mechanism: small goods platform; broad cargo platform; passenger cabin frame; counterweight guide; rope traction head; rack-drive carriage; telescopic lifting column; safety brake housing
Transit equipment: stop marker pedestal; ticket validation gate; platform barrier; guideway switch; track brake block; overhead pickup assembly; maintenance transfer table; route display housing
Hoist: wall jib hoist; overhead trolley hoist; tripod field hoist; dock derrick head; folding workshop hoist; cable capstan; cargo sling beam; service lifting cradle
Waste equipment: sorted refuse station; sealed medical waste unit; kitchen compost drum; filter waste hopper; street suction inlet; dock scrap skip; glass recovery bin; textile recovery cage
Garden infrastructure: drip manifold; raised growing frame; shade tracking frame; seedling warming bed; irrigation timer housing; hydroponic channel; mist distribution rail; pollinator shelter frame
Clinic equipment: treatment bed base; sterilization cabinet; diagnostic cart; reserve-power station; oxygen manifold; medicine cooling cabinet; clean-water station; accessible wash station
Workshop machine: lathe bed; pillar drill; polishing wheel; bending press; parts washer; bench furnace; balancing rig; lens grinding rig
Food equipment: communal oven; grain mill stand; refrigerated counter core; steam cooker bank; spice grinding station; bread proofing cabinet; dish washing station; public cooking hearth
Communication installation: rooftop receiver frame; public message kiosk; broadcast horn bank; hidden relay box; signal repeater cabinet; archive terminal; civic announcement mast; emergency call column
Security fixture: customs scan pedestal; lockable checkpoint gate; document inspection booth frame; cargo seal camera; public alarm box; flood barrier control; secure parcel hatch; perimeter observation mast
Light distribution: arcade light rail; roadway lantern mast; stair edge light strip; civic floodlight yoke; roof reflector stand; dock warning light frame; clinic emergency light bank; collector washlight frame
Flood management: removable flood gate; sump pump station; backflow valve chamber; emergency spillway gate; quay drain trap; water-level sensor mast; sediment separation box; reservoir isolation wheel
Pressure boundary: single vestibule frame; double transfer lock frame; emergency seal iris; inspection viewport frame; bulkhead service plate; equipment transfer drawer; suit service cradle; pressure equalization panel
Landing equipment: compact pad support; wide pad edge frame; retractable guidance arm; ground power pod; wheel restraint cradle; cargo lift interface; engine blast screen; maintenance access gantry
Fuel service: refill hose drum; vented storage cage; metered pump cabinet; emergency cutoff post; spill collection tray frame; filtered transfer skid; pressure bottle bank; service grounding mast
Solar timing device: street schedule dial; civic mirror calendar; household timer terminal; maintenance alignment clock; public allocation board; navigation timing beacon; shift bell mechanism; daylight queue counter
Market infrastructure: stall fold mechanism; canopy tensioning head; goods unloading rail; public scale base; shared refrigeration header; stall washdown inlet; suspended sign rail; removable closing barrier
Structural machinery: adjustable bearing jack; foundation monitoring station; tension rod anchor; bridge expansion bearing; cable tensioning drum; cliff tieback head; roof maintenance cradle; articulated support foot
Emergency service: rescue line cabinet; fold-down stretcher station; emergency lamp rack; flood pump connection; evacuation route sign frame; portable barrier store; emergency charge cabinet; muster point beacon
Maintenance access: fold-down ladder; sliding access panel; roof step frame; removable grating panel; service crawl hatch; cable climb rail; protected inspection platform; ceiling service cradle
Research equipment: sampling station; sky observation mount; material testing press; water analysis cabinet; portable survey tripod; climate sensor mast; plant growth chamber; mirror surface inspection arm
Ship servicing: thruster alignment stand; detachable cargo cradle; wing maintenance brace; hull patching scaffold; navigation calibration frame; flight cell exchange rack; landing strut test rig; cockpit access stair
""")

group("landscape", "Geology and planted city", ["authored"], [5, 1500], "stone; soil; bark; leaves; food surfaces", "FIXED", "D0_FIXED", "CUSTOM + CC0 nature donor", "Cliff / gardens / Low Tides", """
Cliff face: planar fractured face; concave eroded face; convex ledge face; undercut quay face; stepped bedding face; hanging cleft face; narrow vertical fissure; rounded weathered face
Cliff cap: flat planted cap; projecting dry cap; broken edge cap; sloped soil cap; rounded lookout cap; split cleft cap; overhanging root cap; terraced construction cap
Rock arch: narrow footpath arch; low water arch; tall cliff arch; fractured twin arch; broad sheltered arch; tilted bedding arch; root-bound arch; half-collapsed arch
Rock ledge: narrow dry ledge; broad planted ledge; sloping ledge; stepped double ledge; fractured terminal ledge; concave catchment ledge; convex lookout ledge; overhanging shaded ledge
Boulder: squat layered boulder; tall split boulder; rounded water-worn boulder; angular fallen boulder; flattened ledge boulder; clustered fused boulder; porous weathered boulder; cleft-root boulder
Stone rubble: large block fragment; thin slab fragment; wedge arch fragment; rounded cobble group; splintered pillar fragment; layered cliff shard; mortar-backed block fragment; porous aggregate fragment
Ground transition: paving-to-soil edge; stair-to-rock edge; drain-to-mud edge; quay-to-stone edge; root-to-wall edge; gravel-to-terrace edge; dry-silt channel edge; planted-kerb edge
Soil mound: low garden mound; deep excavation mound; narrow root mound; compact potting mound; stepped runoff mound; dry compost mound; wet channel mound; gravel mixed mound
Erosion channel: shallow runoff channel; deep split channel; branching channel; stepped cascade channel; under-wall channel; curved terrace channel; sediment fan channel; dry abandoned channel
Cave module: narrow walking passage; broad maintenance chamber; low water passage; vertical light shaft; forked passage; stepped entry; column-supported chamber; blind storage alcove
Root structure: exposed surface root; cliff-gripping root; wall-entering root; hanging fine roots; buttress root; split paving root; planter root ball; entwined bridge root
Trunk: straight young trunk; leaning mature trunk; forked mature trunk; hollow old trunk; knotted pollarded trunk; wind-shaped trunk; coppiced multi-stem trunk; low branching trunk
Branch: straight scaffold branch; forked branch; curved hanging branch; broken hollow branch; flower-bearing branch; fruit-bearing branch; exposed dead branch; dense twig branch
Tree canopy: narrow upright canopy; broad shade canopy; wind-combed canopy; twin-crown canopy; sparse stressed canopy; pruned courtyard canopy; young open canopy; dense mature canopy
Palm form: juvenile fan palm; tall narrow palm; low spreading palm; forked growth palm; wind-bent palm; fruiting mature palm; trimmed courtyard palm; clustered terrace palm
Shrub: low cushion shrub; upright hedge shrub; spreading flower shrub; sparse dry shrub; hanging ledge shrub; dense rounded shrub; clipped civic shrub; young nursery shrub
Climber: narrow wall vine; broad lattice vine; hanging balcony vine; flowering arch vine; fruiting trellis vine; root-exposed vine; sparse service-yard vine; dense garden vine
Grass clump: low dry grass; tall seed grass; short courtyard grass; wide fountain grass; sparse rock grass; dense drainage grass; wind-bent terrace grass; cut garden grass
Reed clump: short water reed; tall straight reed; flowering reed; bent overflow reed; dense bank reed; sparse dry reed; cut harvested reed; young regrowth reed
Fern: small crevice fern; broad shade fern; hanging basket fern; tall damp fern; curled juvenile fern; sparse dry fern; dense root fern; divided-leaf fern
Succulent: flat rosette succulent; tall branching succulent; small clustered succulent; hanging succulent; ribbed column succulent; flowering rosette succulent; broad paddle succulent; low cushion succulent
Flower cluster: upright bell flowers; broad cup flowers; tiny trailing flowers; star-shaped flowers; dense spherical flowers; long trumpet flowers; flat layered flowers; nocturnal folded flowers
Moss geometry: thin stone-edge tuft; deep damp cushion; root-wrapping tuft; channel lip growth; hanging wet strands; dry brittle clump; dense crevice pad; soft planter carpet
Fungal growth: shelf fungus cluster; narrow stem cluster; broad cap cluster; root-thread growth; cup fungus group; crust fungus patch; hanging damp cluster; branching coral fungus
Garden crop: low leafy rosette; tall grain stalk; climbing bean stem; broad root-crop leaves; clustered herb plant; fruiting bush; hanging gourd vine; compact bulb plant
Harvested fruit: lobed sunfruit; long crescent fruit; round stonefruit; clustered berry stem; ribbed cooling melon; segmented citrus-like fruit; small seeded pod; broad flat seedfruit
Prepared fruit: halved sunfruit; sliced crescent fruit fan; pitted stonefruit half; separated berry dish mesh; cut melon wedge; peeled segmented fruit; opened seed pod; split seedfruit plate mesh
Harvested vegetable: knotted root; long tapered root; layered bulb; ribbed green pod; broad leaf head; clustered tuber; round squash; narrow seed cucumber
Prepared vegetable: sliced root rounds; split roasted root; quartered layered bulb; opened green pod; cut leaf head; peeled tuber; hollowed squash; sliced seed cucumber
Bread and grain food: scored round loaf; long twisted loaf; layered flatbread; folded filled bread; ring bread; small steamed bun; pressed grain cake; woven dough roll
Preserved food: tied dried fruit string; stacked dried grain cakes; sealed leaf parcel; pressed seed block; threaded vegetable slices; curled dried strips; wrapped fermentation block; compact travel ration
Market herbs: broad cooking leaf bundle; narrow tea leaf bundle; flowering medicine bundle; long aromatic stalk bundle; knotted root spice bundle; seed head spice bundle; dried hanging herb bundle; fresh clipped herb bundle
Seed and nut goods: ridged shell nut; smooth round nut; winged dry seed; split shell seed; large oil seed; clustered seed cone; fluted spice seed; flat roasting seed
Planter fill: dense herb fill; young seedling fill; trailing flower fill; dry succulent fill; tall grass fill; layered edible crop fill; bare prepared soil fill; shaded fern fill
Leaf debris: curled broad leaves; narrow dried leaves; torn palm pieces; clustered seed husks; clipped branch debris; flower petal drift; swept leaf pile; water-soaked leaf clump
Shoreline organic: driftwood branch; tangled washed roots; compact reed bundle; smooth water-worn log; layered algae clump; porous dried growth; stranded seed raft; tangled fiber mat
Water boundary: shallow basin bed; stepped overflow bed; narrow canal bed; broad quay edge bed; planted wet margin; sluice outfall bed; fountain splash bed; drained maintenance bed
Landscape retaining insert: root-split stone insert; mossed drainage insert; planted crack insert; eroded cap insert; soil-pocket insert; tree-foot surround insert; watercut ledge insert; garden-step insert
Nursery specimen: seedling tray geometry; young tree root ball; tied climbing plant; potted herb starter; bundled bare-root plants; transplanted shrub; supported young palm; wrapped grafting specimen
Hanging garden body: shallow trough planting; tiered shelf planting; circular suspended planting; long balcony planting; lattice climbing planting; drip-fed pouch planting; narrow shade planting; broad collector-side planting
""")

group("hero", "Signature parts and vehicles", ["authored"], [10, 2500], "Brass Sanctuary signature palette", "FIXED_USE", "D0_PROTECTED", "CUSTOM signature", "Assigned by family", """
Keeper head component: forehead crest; left eye housing; right eye housing; cheek fan; lower face shield; antenna root; crown vent; rear access cowl
Keeper torso component: front breast shell; dorsal spine; shoulder hinge shell; waist ring; layered abdomen shell; lower robe panel; service hatch surround; internal light cradle
Keeper arm component: upper arm shell; elbow yoke; forearm shell; wrist ring; open palm frame; cupped palm frame; finger assembly; sun support saddle
Keeper wing component: primary leading rib; secondary support rib; collector feather root; collector feather tip; wing hinge cover; light channel fin; trailing trim segment; maintenance walkway bracket
Keeper sun component: central globe cage; inner reflector bowl; focus lens crown; equatorial support ring; lower heat exchanger; service iris; power conduit collar; protective outer petal
Kestrel hull component: cockpit nose shell; central cabin shell; belly service shell; rear engine spine; upper cargo fairing; side access surround; underside landing housing; aft cargo door frame
Kestrel wing component: wing root; inner spar cover; outer spar cover; collector surface frame; leading edge shell; trailing control surface; wing tip housing; folded wing hinge fairing
Kestrel propulsion component: intake bell; compressor casing; thruster throat; nozzle petal ring; vectoring yoke; heat shield; exhaust vane; service line manifold
Kestrel interior component: pilot console; navigation console; crew berth shell; cabin storage wall; floor service plate; canopy latch frame; overhead utility panel; fold-down crew seat
Kestrel landing component: front strut; rear strut; landing foot; compression collar; deployment hinge; brake link; wheel fork; shock reservoir
Courier craft shell: needle single-seat hull; broad parcel-skiff hull; twin-boom courier hull; enclosed messenger hull; folding-wing courier hull; canal-edge runner hull; short-hop clinic hull; salvage courier hull
Work craft shell: flatbed cargo hull; deep utility tug hull; water-service hull; mirror-maintenance hull; rescue launch hull; refuse collection hull; garden supply hull; mobile workshop hull
Civic vehicle shell: narrow transit cabin; broad public ferry cabin; accessible lift cabin; customs inspection rover body; clinic transport body; emergency response body; maintenance tram cabin; archive delivery body
Dock cart body: two-wheel parcel cart; four-wheel goods cart; articulated barrel cart; long timber cart; insulated clinic cart; folding vendor cart; heavy scrap cart; narrow stair-climbing cart
Sunseed component: authorization core body; keyed mounting collar; protective transport cage; contact terminal ring; charge indication window; tamper-evident latch; reserve heat sink; custody tag frame
Relay apparatus: docking authorization console; mirror alignment terminal; signal focus ring; transfer contact cradle; maintenance access iris; emergency manual control; custody scan frame; directional beacon housing
Clinic installation: sunseed receiver housing; priority distribution console; public charge-status board; treatment-light canopy; reserve heat storage bank; emergency transfer cradle; queue shelter emblem; service shutoff station
Evacuation installation: departure allocation console; route authorization cradle; public boarding marker; passenger shelter module; medical boarding platform; cargo restraint station; guidance beacon array; emergency override pedestal
Common Table landmark: shared meal canopy crown; water-table centerpiece; neighborhood notice tree; repair cooperative emblem; seed-exchange cabinet shell; communal cooking hood; public tool-lending frame; memorial light pedestal
Concession landmark: mirror allocation tower crown; administrative entrance seal; schedule display drum; inspection bridge console; priority queue canopy; archival vault surround; civic clock housing; collector dedication plinth
Open Wake landmark: captains gathering table body; voyage register cabinet; docking bell frame; cooperative mooring emblem; chart archive shell; rescue pledge board frame; shared tow-winching stand; departure ritual light
Night Choir landmark: concealed broadcast console; folding street performance shell; archive recorder pedestal; rooftop signal sculpture; listening booth shell; testimony collection cabinet; portable projection cart; hidden antenna sculpture
Market hero sign: Second Dawn repair sign; Bent Spoon food sign; Copper Thread tailor sign; Three Drops refill sign; Last Light book sign; Open Palm clinic sign; High Step courier sign; Root and Coil garden sign
Waterworks landmark: old cistern valve crown; reservoir level tower head; public washing pavilion core; bridge overflow sculpture; flood memorial marker; filtration hall entrance; emergency spillway control; garden water clock
Mirror ring component: primary mirror segment frame; inspection rail carriage; maintenance capsule shell; radiator petal frame; docking collar; orbital timing head; reserve power housing; collector alignment beacon
""")

EXPECTED = {"architecture": 1600, "props": 1600, "infrastructure": 480, "landscape": 320, "hero": 200}

def build():
    assets = []
    domain_family_counts = collections.Counter()
    wave0_names = {"Wall bay", "Arcade support", "Arch span", "Door surround", "Storage crate", "Basket", "Cup", "Bench", "Cabinet", "Lantern body", "Desk lamp", "Light cell", "Hammer", "Shop sign object", "Dock service", "Water station", "Planter fill", "Harvested fruit", "Market hero sign", "Keeper head component"}
    wave1_families = {"architecture": 14, "props": 35, "infrastructure": 10, "landscape": 12, "hero": 8}
    for family in FAMILIES:
        domain = family["domain"]
        ordinal = domain_family_counts[domain]
        domain_family_counts[domain] += 1
        for pi, profile_id in enumerate(family["profiles"]):
            profile = PROFILES[profile_id]
            for form in family["forms"]:
                key = f"bs.{family['id']}.{slug(form)}.{profile_id}"
                phase = "W0 First block" if pi == 0 and family["name"] in wave0_names else "W1 Neighborhood" if pi <= 1 and ordinal < wave1_families[domain] else "W2 District library" if pi <= 2 else "W3 Extended library"
                interaction = family["interaction"]
                destruction = family["destruction"]
                if domain == "landscape" and family["category"] == "Geology and planted city" and any(word in family["name"].lower() for word in ["fruit", "vegetable", "bread", "food", "herbs", "seed and nut"]):
                    interaction, destruction = "CARRY_COLLECT", "D1_STATE_SWAP"
                if family["name"] in ["Door leaf", "Window closure", "Service hatch"]:
                    interaction, destruction = "HINGED_USE", "D2_SELECTED"
                district = profile["district"] if family["district"] == "profile" else family["district"]
                brief = f"Author {form.lower()} for the {family['name'].lower()} family. {profile['geometry']}"
                assets.append(dict(id=key, familyId=family["id"], domain=domain, category=family["category"],
                    family=family["name"], name=form, constructionProfile=profile_id, constructionName=profile["name"],
                    district=district, geometryBrief=brief, sizeEnvelopeCm=family["sizeEnvelopeCm"],
                    dimensionsStatus="Family envelope only; lock per-axis dimensions in blockout before authoring",
                    materials=profile["materials"] if family["materials"] == "profile" else family["materials"],
                    interactionProfile=interaction, destructionProfile=destruction,
                    sourceRoute=family["sourceRoute"], productionWave=phase, status="Planned",
                    countedAsFinished=False, uniquenessStatus="Requires silhouette and construction review",
                    dependencies=["approved family exemplar", "material profile", "interaction profile", "placement test"],
                    delivery=["editable source", "runtime mesh", "collision", "material references", "thumbnail", "provenance", "validation record"],
                    acceptance=["distinct geometry at intended camera distance", "construction profile visible in geometry", "no scale or material-only duplicate", "correct pivot and snap sockets", "packaged route validation"],
                    replacementFor=None, mappedRuntimeAsset=None))
    counts = collections.Counter(a["domain"] for a in assets)
    assert dict(counts) == EXPECTED, counts
    assert len(assets) == len({a["id"] for a in assets}) == 4200
    assert len(FAMILIES) == 235, len(FAMILIES)
    return {"schemaVersion": 1, "title": "Borrowed Sun world asset catalog", "date": "2026-10-13",
        "scope": "4200 planned geometry briefs across 235 families; no claim of completed assets or automatic geometry uniqueness",
        "counts": dict(counts), "finishedFromThisCatalog": 0, "families": FAMILIES, "constructionProfiles": PROFILES,
        "countingPolicy": "Only accepted geometric forms count as built. LODs, recolors, mirrored copies, placements, assemblies and destruction debris do not inflate this total.",
        "assets": assets}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=ROOT.parent)
    args = parser.parse_args()
    data = build()
    register = ROOT / "Docs/Registers/world-asset-catalog.json"
    if register.exists():
        previous = json.loads(register.read_text(encoding="utf-8"))
        if any(a.get("status") != "Planned" or a.get("mappedRuntimeAsset") or a.get("countedAsFinished") for a in previous.get("assets", [])):
            raise RuntimeError("Catalog contains production progress. Reconcile it before regenerating; refusing to erase accepted or mapped records.")
    register.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8", newline="\n")
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "Borrowed_Sun_Asset_Catalog.json").write_bytes(register.read_bytes())
    template = (Path(__file__).with_name("World-Catalog.template.html")).read_text(encoding="utf-8")
    compact = json.dumps(data, ensure_ascii=False, separators=(",", ":")).replace("<", "\\u003c")
    (args.output / "Borrowed_Sun_Asset_Catalog.html").write_text(template.replace("__CATALOG_DATA__", compact), encoding="utf-8", newline="\n")
    lines = ["# Borrowed Sun asset family index", "", "235 families and 4,200 planned geometry briefs. All records start as Planned. This index lists form coverage; the JSON and offline catalog add construction, interaction and production fields.", ""]
    for domain in EXPECTED:
        lines += [f"## {domain.title()}", ""]
        for f in [f for f in FAMILIES if f["domain"] == domain]:
            lines += [f"### {f['name']}", "", f"{8*len(f['profiles'])} planned records. Profiles: {', '.join(f['profiles'])}. Interaction: {f['interaction']}. Destruction: {f['destruction']}.", "", "; ".join(f["forms"]) + ".", ""]
    (ROOT / "Docs/Art/WORLD_ASSET_FAMILIES.md").write_text("\n".join(lines), encoding="utf-8", newline="\n")
    print(json.dumps({"planned":len(data["assets"]), "families":len(FAMILIES), "counts":data["counts"], "waves":dict(collections.Counter(a["productionWave"] for a in data["assets"])), "sha256":hashlib.sha256(register.read_bytes()).hexdigest()}))

if __name__ == "__main__":
    main()
