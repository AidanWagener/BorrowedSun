const fs = require('node:fs/promises');
const path = require('node:path');
const { chromium } = require('playwright');

(async () => {
  const root = path.resolve(__dirname, '../..');
  if (!process.argv[2]) throw new Error('Supply the browser executable path.');
  const source = await fs.readFile(path.join(root,'Docs/World/Cities/orison_city_plan.svg'),'utf8');
  const browser = await chromium.launch({executablePath:process.argv[2], headless:true, args:['--disable-gpu']});
  try {
    const page = await browser.newPage({viewport:{width:1440,height:1450}});
    await page.setContent('<html><head><meta charset="utf-8"></head><body style="margin:0">'+source+'</body></html>');
    const result = await page.evaluate(() => {
      const bounds = document.querySelector('svg').viewBox.baseVal;
      const textOutside = [...document.querySelectorAll('text')].filter(e=>{
        const r=e.getBoundingClientRect();
        return r.left < 0 || r.top < 0 || r.right > bounds.width || r.bottom > bounds.height;
      }).map(e=>e.textContent);
      const labels = [...document.querySelectorAll('.map-label-box')].map(e=>e.getBoundingClientRect());
      let labelOverlaps=0;
      labels.forEach((a,i)=>labels.slice(i+1).forEach(b=>{
        if(a.left < b.right && a.right > b.left && a.top < b.bottom && a.bottom > b.top) labelOverlaps++;
      }));
      return {places:document.querySelectorAll('[data-poi]').length,districts:document.querySelectorAll('[data-district]').length,textOutside,labelOverlaps};
    });
    const output = path.join(root,'Artifacts/world');
    await fs.mkdir(output,{recursive:true});
    await page.screenshot({path:path.join(output,'orison_city_plan.png')});
    await fs.writeFile(path.join(output,'orison-plan-inspection.json'),JSON.stringify(result,null,2)+'\n');
    console.log(JSON.stringify(result));
    if(result.places !== 24 || result.districts !== 11 || result.textOutside.length || result.labelOverlaps) throw new Error('City plan layout check failed');
  } finally { await browser.close(); }
})().catch(error=>{console.error(error);process.exitCode=1;});
