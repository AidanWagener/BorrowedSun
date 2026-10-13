// Uses the configured Playwright package and a supplied browser for local SVG QA.
const fs = require('node:fs/promises');
const path = require('node:path');
const { chromium } = require('playwright');

(async () => {
  const browserPath = process.argv[2];
  if (!browserPath) throw new Error('Supply the browser executable path.');
  const root = path.resolve(__dirname, '../..');
  const output = path.join(root, 'Artifacts/world');
  await fs.mkdir(output, { recursive: true });
  const browser = await chromium.launch({ executablePath: browserPath, headless: true, args: ['--disable-gpu'] });
  try {
    const page = await browser.newPage({ viewport: { width: 1200, height: 840 } });
    const source = await fs.readFile(path.join(root, 'Docs/World/Atlas/nacre_geographic_concept.svg'), 'utf8');
    await page.setContent('<!doctype html><html><head><style>body{margin:0}</style></head><body>' + source + '</body></html>');
    const result = await page.evaluate(() => ({
      svg: !!document.querySelector('svg'),
      title: document.querySelector('title')?.textContent,
      cities: document.querySelectorAll('circle').length,
      labelsOutsideViewport: [...document.querySelectorAll('text')].filter(node => {
        const b = node.getBoundingClientRect();
        return b.left < 0 || b.top < 0 || b.right > 1200 || b.bottom > 840;
      }).map(node => node.textContent)
    }));
    if (!result.svg || result.cities !== 12 || result.labelsOutsideViewport.length) {
      throw new Error(JSON.stringify(result));
    }
    const screenshot = path.join(output, 'atlas-0.1.png');
    await page.screenshot({ path: screenshot, fullPage: true });
    await fs.writeFile(path.join(output, 'atlas-0.1-inspection.json'), JSON.stringify({ ...result, screenshot }, null, 2) + '\n');
    console.log(JSON.stringify({ ...result, screenshot }));
  } finally {
    await browser.close();
  }
})().catch(error => { console.error(error); process.exitCode = 1; });
