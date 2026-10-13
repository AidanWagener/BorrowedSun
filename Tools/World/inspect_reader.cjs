const fs = require('node:fs/promises');
const path = require('node:path');
const { pathToFileURL, fileURLToPath } = require('node:url');
const { chromium } = require('playwright');

(async () => {
  const root = path.resolve(__dirname, '../..');
  const output = path.join(root, 'Artifacts/world');
  await fs.mkdir(output, {recursive:true});
  if (!process.argv[2]) throw new Error('Supply the browser executable path.');
  const browser = await chromium.launch({executablePath:process.argv[2], headless:true, args:['--disable-gpu']});
  const evidence = [];
  try {
    for (const width of [1000, 390]) {
      const page = await browser.newPage({viewport:{width, height:1000}});
      const errors = [];
      page.on('pageerror', error => errors.push(error.message));
      await page.goto(pathToFileURL(path.join(root,'Docs/World/Living_Planet_Reader.html')).href);
      const check = await page.evaluate(() => ({
        articles:document.querySelectorAll('article').length,
        overflow:document.documentElement.scrollWidth > innerWidth + 1,
        brokenImages:[...document.images].filter(i => !i.complete || !i.naturalWidth).map(i=>i.src),
        missingAnchors:[...document.querySelectorAll('a[href^="#"]')].filter(a=>!document.getElementById(a.hash.slice(1))).map(a=>a.hash),
        localFiles:[...new Set([...document.querySelectorAll('a[href],img[src]')].map(e=>e.href || e.src).filter(u=>u.startsWith('file:') && !u.includes('#')))]
      }));
      const missingFiles = [];
      for (const url of check.localFiles) {
        try { await fs.access(fileURLToPath(url)); } catch { missingFiles.push(url); }
      }
      await page.locator('nav a[href="#atlas"]').click();
      await page.waitForFunction(() => location.hash === '#atlas');
      await page.locator('#atlas').scrollIntoViewIfNeeded();
      await page.screenshot({path:path.join(output,`reader-atlas-${width}.png`)});
      await page.goto(pathToFileURL(path.join(root,'Docs/World/Living_Planet_Reader.html')).href);
      await page.screenshot({path:path.join(output,`reader-contents-${width}.png`)});
      const result = {width,...check, missingFiles, errors, atlasNavigation:true};
      delete result.localFiles;
      evidence.push(result);
      if (check.articles !== 7 || check.overflow || check.brokenImages.length || check.missingAnchors.length || missingFiles.length || errors.length) {
        throw new Error(JSON.stringify(result));
      }
      await page.close();
    }
    await fs.writeFile(path.join(output,'reader-inspection.json'),JSON.stringify(evidence,null,2)+'\n');
    console.log(JSON.stringify(evidence));
  } finally { await browser.close(); }
})().catch(error=>{console.error(error);process.exitCode=1;});
