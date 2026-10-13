# Tripo Studio asset intake

13 October 2026

The official Tripo CLI 0.6.1 is installed and authenticated. The API account reports zero credits. The user's existing credits belong to Studio, whose subscription balance is independent of the API and cannot fund CLI generation. The first candidates therefore use the signed-in Studio interface. [Official credit explanation](https://www.tripo3d.ai/help/api-plugins/tripo-studiotripo-api).

Four original Brass Sanctuary briefs were submitted privately at 55 credits each: 220 existing Studio credits used, 2,980 remaining, no new purchase. Eight authored briefs are available in `SourceArt/Tripo/Briefs`. The [intake register](../Registers/tripo-asset-intake.json) is the authoritative record of submitted IDs, observed status, settings and counts. These candidates are not accepted production assets or additions to the completed world catalog.

| Candidate | Saved Studio model | Requested triangle budget | Purpose |
|---|---|---:|---|
| Moth service lantern | [Open](https://studio.tripo3d.ai/workspace/generate/35622150-77b5-44b2-87df-8592bc71de3b) | 50,000 | Portable workshop hero prop |
| Solar charging cabinet | [Open](https://studio.tripo3d.ai/workspace/generate/e9349668-6acc-47d8-b406-1fbf3e0f40bf) | 50,000 | Wall utility and repair-shop storytelling |
| Repair meter | [Open](https://studio.tripo3d.ai/workspace/generate/b520cd00-c6b0-4153-b0ae-eb715cc0f396) | 35,000 | Collectible instrument candidate |
| Moth civic corbel | [Open](https://studio.tripo3d.ai/workspace/generate/8547a1c0-e5ec-4d08-b7d7-a2e640214d2e) | 60,000 | Original architectural ornament |

Studio settings were H3.1, Ultra Mesh Quality, 4K textures, Remove Lighting, PBR, triangles and Private. Studio exposed no seed field; the seeds and exact API model version in the briefs must not be attributed to these Studio jobs. Model generation does not guarantee the requested count, scale, solid geometry, separate moving parts or correct material response.

## Export and production gate

The lantern's Studio export reported success, but the embedded browser did not deliver a local download after GLB and FBX attempts. No local model bytes or texture maps have been verified. A request is pending for the user to export the saved lantern as a 4K GLB into Downloads using their ordinary browser. Resume that saved model; do not regenerate it to fix a download failure. Other completed models remain recoverable by their saved URLs above.

After an export arrives, retain the original bytes under `SourceArt/Tripo/Originals/<id>/`, hash them, and add the filenames and export settings to the register. Preserve any signed download URL only transiently; never commit account credentials or signed URLs. Inspect the actual mesh and embedded PBR maps in Blender, normalize units and pivot to the authored dimensions, and retain an editable derivative. Review closed surfaces and thin parts, texture seams, normal orientation, material ranges and silhouette. Author simple collision for movable props separately. Import into Unreal through editor APIs and review the same workshop cameras before accepting a candidate.

The first screen review suggests useful custom silhouettes and detailing. The blue enamel and gold are too saturated for the established game materials, and the meter's bezel/controls need geometric cleanup. Correct these during local asset preparation. A Studio preview is not an in-engine quality or performance result.

![Generated lantern in Studio](../Art/Reviews/tripo-2026-10-13/lantern-studio.jpg)

![Generated cabinet in Studio](../Art/Reviews/tripo-2026-10-13/cabinet-studio.jpg)

![Generated meter in Studio](../Art/Reviews/tripo-2026-10-13/meter-studio.jpg)

![Generated architectural corbel in Studio](../Art/Reviews/tripo-2026-10-13/corbel-studio.jpg)

## Rights and retention record

The user identifies this as a paid Studio subscription. Exact tier evidence at generation remains to be recorded privately before production acceptance. Under the current [Terms](https://www.tripo3d.ai/terms), last updated 11 July 2025, section 5.2.2 grants paid users broad commercial rights subject to applicable law and input rights; the [commercial-use help](https://www.tripo3d.ai/help/privacy-policy/can-i-use-models-commercially) specifically permits paid membership outputs in commercial uses. This contractual permission does not prove exclusivity, copyrightability or clearance of every generated detail. Inputs for this batch are original authored text only.

The paid-user terms restrict AI training use of inputs and outputs. The live [Privacy Policy](https://www.tripo3d.ai/privacy), effective 30 September 2026, also describes authorized personnel access for operational purposes; private generation does not imply that nobody at the provider can access the content. There is no guaranteed permanent storage or fixed retention period for all content. Keep local originals once export succeeds. These observations were checked against official pages on 13 October 2026.

## API adapter verification

`Tools/Tripo.ps1` provides local Doctor/Plan and explicit Generate/Resume operations for future API credit use. It records each submission before the network call, preserves actual task IDs across failures and restores missing or corrupt completed downloads by retrieving the same task. It rejects existing reparse entries before writing the request or invoking a download. It never substitutes a Studio ID for an API task ID. No API generation was submitted. See [commands and recovery](../../Tools/README.md#tripo-api-adapter).

The final offline suite passed 37 checks on both [Windows PowerShell 5.1](../../Artifacts/tripo-tests/20261013T053905Z-71842fcf/tripo-test-result.json) and [PowerShell 7](../../Artifacts/tripo-tests/20261013T053905Z-fff6bd2d/tripo-test-result.json). Three file-symlink groups were explicitly skipped because Windows denied fixture creation; these are not passing assertions. The nested directory-junction case executed and verified rejection before any CLI query while retaining the task ID. Installed CLI command definitions were also checked offline. The suite uses a fake CLI and makes no network or paid generation calls.
