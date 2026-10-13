// Render selected authored SVG plates and check their visible contents.
const fs = require('node:fs/promises');
const path = require('node:path');
const { chromium } = require('playwright');

(async () => {
  const root=path.resolve(__dirname,'../..');
  const sources={
    food:{file:'Exchange/orison_food_allowance.svg',counts:{'[data-value]':11}},
    talweir:{file:'Cities/talweir_city_plan.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    marroway:{file:'Cities/marroway_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    aven:{file:'Cities/aven_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    sola:{file:'Cities/sola_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    hesh:{file:'Cities/hesh_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    serein:{file:'Cities/serein_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    dhal:{file:'Cities/dhal_settlement.svg',counts:{'[data-poi]':24,'[data-district]':9}},
    merrow:{file:'Cities/merrow_settlement.svg',counts:{'[data-poi]':24,'[data-district]':10}},
    tern:{file:'Cities/tern_settlement.svg',counts:{'[data-poi]':24,'[data-district]':10}},
    velis:{file:'Cities/velis_settlement.svg',counts:{'[data-poi]':24,'[data-district]':8}},
    brin:{file:'Cities/brin_settlement.svg',counts:{'[data-poi]':24,'[data-district]':8}},
    production:{file:'Resources/orison_productive_land.svg',counts:{'[data-allocation]':13}}
  };
  const config=sources[process.argv[3]];
  if(!process.argv[2] || !config) throw new Error('Supply browser executable and a known plate key: '+Object.keys(sources).join(', '));
  const source=await fs.readFile(path.join(root,'Docs/World',config.file),'utf8');
  const browser=await chromium.launch({executablePath:process.argv[2],headless:true,args:['--disable-gpu']});
  try {
    const page=await browser.newPage();
    await page.setContent('<!doctype html><meta charset="utf-8"><body style="margin:0">'+source+'</body>');
    const size=await page.evaluate(()=>{const b=document.querySelector('svg').viewBox.baseVal;return {width:Math.ceil(b.width),height:Math.ceil(b.height)};});
    await page.setViewportSize(size);
    const result=await page.evaluate((counts)=>{
      const textOutside=[...document.querySelectorAll('text')].filter(e=>{const b=e.getBoundingClientRect();return b.left < 0 || b.top < 0 || b.right > innerWidth || b.bottom > innerHeight;}).map(e=>e.textContent);
      const actual=Object.fromEntries(Object.keys(counts).map(selector=>[selector,document.querySelectorAll(selector).length]));
      const rects=[...document.querySelectorAll('.map-label-box')].map(e=>e.getBoundingClientRect());
      let labelOverlaps=0;
      rects.forEach((a,i)=>rects.slice(i+1).forEach(b=>{if(a.left<b.right&&a.right>b.left&&a.top<b.bottom&&a.bottom>b.top)labelOverlaps++;}));
      const sidebar=[...document.querySelectorAll('text')].map(e=>({text:e.textContent,rect:e.getBoundingClientRect()})).filter(e=>e.rect.left>=1120 && e.rect.top>=140 && e.rect.bottom<=970);
      const sidebarOverlaps=[];
      sidebar.forEach((a,i)=>sidebar.slice(i+1).forEach(b=>{if(a.rect.left<b.rect.right&&a.rect.right>b.rect.left&&a.rect.top<b.rect.bottom&&a.rect.bottom>b.rect.top)sidebarOverlaps.push([a.text,b.text]);}));
      return {textOutside,actual,labelOverlaps,sidebarOverlaps};
    },config.counts);
    const out=path.join(root,'Artifacts/world');await fs.mkdir(out,{recursive:true});
    const stem=path.basename(config.file,'.svg');
    await page.screenshot({path:path.join(out,stem+'.png')});
    await fs.writeFile(path.join(out,stem+'-inspection.json'),JSON.stringify({plate:config.file,...size,...result},null,2)+'\n');
    console.log(JSON.stringify(result));
    if(result.textOutside.length || result.labelOverlaps || result.sidebarOverlaps.length || Object.entries(config.counts).some(([k,v])=>result.actual[k]!==v))throw new Error('Plate inspection failed');
  } finally {await browser.close();}
})().catch(error=>{console.error(error);process.exitCode=1;});
