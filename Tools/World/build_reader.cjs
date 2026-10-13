// Compile the current authoring edition; source Markdown remains authoritative.
const fs = require('node:fs/promises');
const path = require('node:path');
const { pathToFileURL } = require('node:url');

(async () => {
  const { marked } = await import(pathToFileURL(require.resolve('marked')).href);
  const root = path.resolve(__dirname, '../../Docs/World');
  const chapters = [
    ['orientation', '00_CANON_AND_ORIENTATION.md', '00 · Canon and orientation', 'Working canon'],
    ['physics', 'Drafts/PHYSICAL_FOUNDATION_REVIEW.md', '01a · Physical foundation', 'Checked design candidate; open physical questions remain'],
    ['landwater', '01_LAND_WATER_AND_DEEP_TIME.md', '01b · Land, water and deep time', 'Working geological canon and authored relief controls; not surveyed terrain'],
    ['sky', '02_SKY_WATER_AND_SEASONS.md', '02 · Sky, water and seasons', 'Adopted calendar; idealized sky and regional weather proposals'],
    ['ecology', 'Drafts/ECOLOGY_FOUNDATION.md', '03a · Ecology foundation', 'Draft; limited food compatibility adopted, regional ranges conditional'],
    ['morrow', 'Drafts/MORROW_ECOSYSTEMS.md', '03b · Morrow ecosystems', '24 additional species and six regional networks; climate and carrying capacities unresolved'],
    ['serepale', 'Drafts/SERE_AND_PALE_ECOSYSTEMS.md', '03c · Sere and Pale ecosystems', '24 additional species, six regional webs and Velis stock requirements; qualification and capacities open'],
    ['history', '04_HISTORY_OF_NACRE.md', '04 · History', 'Working canon'],
    ['chronology', '04b_CHRONOLOGY_AND_MEMORY.md', '04b · Chronology and memory', '105 reconciled dated entries; retained proposals and undated contexts remain explicit'],
    ['societies', 'Drafts/SOCIETIES_FOUNDATION.md', '05 · Societies', 'Draft reconciled to the historical spine'],
    ['pleasures', 'Drafts/PLEASURES_AND_EVERYDAY_LIFE.md', '05b · Everyday pleasures', 'Draft'],
    ['languages', '05_LANGUAGES_NAMES_AND_BELONGING.md', '05c · Languages, names and belonging', 'Four public varieties, a bounded grammar and starter lexicon; not complete conlangs'],
    ['exchange', '06_POWER_AND_EXCHANGE.md', '06 · Power and exchange', 'Working institutions and selected demand/dependency records; not a simulated economy'],
    ['production', '06b_PRODUCTIVE_LANDSCAPES.md', '06b · Productive landscapes', 'Cultivation canon and traceable land, water and light sizing; regional source capacity remains unfinished'],
    ['accounts', '06c_CITY_AND_REGION_ACCOUNTS.md', '06c · City and region accounts', 'Twelve demand accounts and attributed procurement; source capacity remains unresolved'],
    ['atlas', '07_REGIONAL_ATLAS.md', '07 · Regional atlas', 'Geographic concept; terrain and climate development pending'],
    ['settlements', '07b_SETTLEMENT_BELTS_AND_DISTANCE.md', '07b · Settlement belts and distance', 'Population allocation, 48 community proposals and two route chains; no constructed settlements'],
    ['orison', 'Cities/ORISON.md', '08a · Orison', 'Authored city proposal: eleven districts, 24 places and service dependencies; not constructed'],
    ['marroway', 'Cities/MARROWAY.md', '08b · Marroway', 'Authored inland river city: nine districts and 24 places; sources and service capacity remain open'],
    ['talweir', 'Cities/TALWEIR.md', '08c · Talweir', 'Authored lake city proposal: nine districts, 24 places and level-controlled navigation; not constructed'],
    ['aven', 'Cities/AVEN.md', '08d · Aven', 'Authored upland city: broad terraces, nine districts and 24 places; no terrain or services implemented'],
    ['sola', 'Cities/SOLA.md', '08e · Sola', 'Authored island city: five islands, nine districts and 24 places; no navigation or source yield proved'],
    ['hesh', 'Cities/HESH.md', '08f · Hesh', 'Authored closed-basin city: nine districts and 24 places; lake geography preserved'],
    ['serein', 'Cities/SEREIN.md', '08g · Serein', 'Authored fog-coast city: nine districts and 24 places; local marine and service assumptions'],
    ['dhal', 'Cities/DHAL.md', '08h · Dhal', 'Authored inland forest river city: nine districts and 24 places; not a coastal port'],
    ['merrow', 'Cities/MERROW.md', '08i · Merrow', 'Authored lower-river city: ten districts and 24 places; downstream estuary remains distinct'],
    ['tern', 'Cities/TERN.md', '08j · Tern', 'Authored industrial harbor city: ten districts and 24 places; coastal and service capacities remain proposals'],
    ['velis', 'Cities/VELIS.md', '08k · Velis', 'Authored river-to-coast city: eight districts and 24 places; stock qualification and source capacities remain open'],
    ['brin', 'Cities/BRIN.md', '08l · Brin', 'Authored northern service city: eight districts and 24 places; winter daylight and absent Rauth preserved'],
    ['journeys', '09_ROADS_SKIES_AND_EXPLORATION.md', '09 · Roads, skies and exploration', 'Travel culture, thirteen route intentions and adventure requirements; no new movement or time systems implemented'],
    ['people', '11_PEOPLE_AND_RELATIONSHIPS.md', '11 · People and relationships', '261 evidenced identities and 158 directed relationships; behavioral cases remain proposals']
  ];
  const escape = s => s.replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const sections = [], metrics = [];
  for (const [id, relative, title, status] of chapters) {
    const absolute = path.join(root, relative);
    const markdown = await fs.readFile(absolute, 'utf8');
    const words = (markdown.match(/\b[\p{L}\p{N}][\p{L}\p{N}'’−-]*\b/gu) || []).length;
    let body = await marked.parse(markdown, { gfm: true });
    body = body.replace(/(href|src)="([^"]+)"/g, (all, attr, target) => {
      if (/^(?:[a-z]+:|#|\/\/)/i.test(target)) return all;
      const resolved = path.resolve(path.dirname(absolute), target.split('#')[0]);
      const chapter = chapters.find(c => path.join(root, c[1]) === resolved);
      if (attr === 'href' && chapter && !target.includes('#')) return `href="#${chapter[0]}"`;
      return `${attr}="${escape(path.relative(root, resolved).replace(/\\/g, '/'))}${target.includes('#') ? '#' + target.split('#')[1] : ''}"`;
    });
    body = body.replace(/<table>/g, '<div class="table-wrap"><table>').replace(/<\/table>/g, '</table></div>');
    sections.push(`<article id="${id}"><p class="status">${escape(status)} · ${words.toLocaleString('en-US')} source words · <a href="${escape(relative)}">Source</a></p>${body}<p><a href="#contents">Contents ↑</a></p></article>`);
    metrics.push({ id, source: relative, title, status, words });
  }
  const count = metrics.reduce((s, c) => s + c.words, 0);
  const output = `<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>Borrowed Sun · The living planet · Authoring edition 0.7</title>
<style>
:root{color-scheme:light dark;--paper:#f3efdf;--ink:#203743;--muted:#526068;--line:#c6c6b5;--link:#235f78}
@media(prefers-color-scheme:dark){:root{--paper:#142a31;--ink:#e9e4d5;--muted:#b5c5c7;--line:#456069;--link:#9ccee0}}
*{box-sizing:border-box}body{margin:0;background:var(--paper);color:var(--ink);font:18px/1.65 Georgia,serif}main{max-width:940px;margin:auto;padding:48px 28px}header{padding-bottom:40px;border-bottom:1px solid var(--line)}h1,h2,h3,h4,nav,.status{font-family:Arial,sans-serif;line-height:1.25}h1{font-size:2rem;margin:20px 0}h2{font-size:1.6rem;margin:48px 0 18px}h3{font-size:1.25rem;margin:32px 0 15px}h4{font-size:1.05rem}a{color:var(--link);overflow-wrap:anywhere}p,li{overflow-wrap:anywhere}nav ol{padding-left:24px;line-height:1.6}nav li{margin:10px 0}.status{color:var(--muted);font-size:.85rem;line-height:1.5}article{padding:44px 0;border-bottom:1px solid var(--line)}article>h1{font-size:1.9rem}blockquote{margin:24px 0;padding:4px 24px;border-left:3px solid var(--line);font-style:italic}.table-wrap{max-width:100%;overflow:auto}table{border-collapse:collapse;font:14px/1.5 Arial,sans-serif;width:100%;min-width:520px}th,td{border-bottom:1px solid var(--line);text-align:left;vertical-align:top;padding:12px 10px}img,svg{max-width:100%;height:auto}pre{overflow:auto;font-size:13px;padding:18px;border:1px solid var(--line)}code{font-size:.87em}html{scroll-behavior:smooth}@media(prefers-reduced-motion:reduce){html{scroll-behavior:auto}}@media(max-width:540px){body{font-size:17px}main{padding:26px 18px}h1{font-size:1.7rem}article>h1{font-size:1.6rem}}@media print{body{font-size:10.5pt;background:white;color:black}main{max-width:none;padding:0}article{break-before:page}.table-wrap{overflow:visible}table{min-width:0}a{color:inherit}pre{white-space:pre-wrap}h2,h3,h4{break-after:avoid}img{break-inside:avoid}.status{color:#444}}
</style></head><body><main><header id="contents"><p class="status">BORROWED SUN · AUTHORING EDITION 0.7 · 13 OCTOBER 2026</p><h1>The living planet</h1><p>A world of long days, distant homes and inherited promises.</p><p>This reading edition collects the current foundations. The complete fifteen-volume bible is still in progress. Draft proposals and unresolved questions are labeled within each chapter. No playable planet is claimed.</p><p class="status">${count.toLocaleString('en-US')} source words across ${chapters.length} chapters. This count includes research and review notes; it is not a page count or a quality score. Historical specifications are excluded.</p><nav aria-label="Contents"><ol>${chapters.map(c => `<li><a href="#${c[0]}">${escape(c[2])}</a></li>`).join('')}</ol></nav><p><a href="README.md">Project index and completion gates</a> · <a href="Atlas/nacre_geographic_concept.svg">Geographic concept map</a> · <a href="Atlas/nacre_relief_and_water.svg">Relief and water</a> · <a href="Cities/orison_city_plan.svg">Orison city plan</a></p></header>${sections.join('\n')}<footer><p class="status">Generated from repository Markdown. The source files, canon decisions and research provenance remain authoritative.</p></footer></main></body></html>`;
  await fs.writeFile(path.join(root, 'Living_Planet_Reader.html'), output);
  await fs.writeFile(path.join(root, 'AUTHORING_COUNTS.json'), JSON.stringify({edition:'0.7', scope:'Current new foundations including research/review notes; not pages, completion or quality certification', words:count, chapters:metrics}, null, 2) + '\n');
  console.log(JSON.stringify({chapters:chapters.length, words:count, output:path.join(root,'Living_Planet_Reader.html')}));
})().catch(error => { console.error(error); process.exitCode = 1; });
