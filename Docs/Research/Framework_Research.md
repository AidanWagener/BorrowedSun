# Borrowed Sun — Framework and Repository Research

Version 1.0 | Research snapshot: 12 October 2026 | Solo creator, Unreal 5.8.2, initial budget below EUR 500

## 1 The recommendation

Keep the verified Borrowed Sun C++ foundation and add capabilities in the order that they remove actual production bottlenecks. The strongest opportunities are editor automation through Unreal's own MCP tools, repeatable Blender-to-Unreal asset intake, procedural district dressing, and a compact mission-authoring solution. AI workflow frameworks can help with retrieval and critique, but our existing plans are already sufficient to begin building the first playable mission.

This report follows the completed development handbook. It researches repositories and their implementation implications; it does not replace the original game concept or the engine qualification. No third-party repository from this sprint was installed, executed as a plugin, or integrated into the game. Native editor-tool descriptors were inspected without starting a server. All proposed experiment durations are focused-hour planning limits, not measured estimates or guarantees.

The current local baseline is concrete: Launcher Unreal 5.8.2, Editor and Game targets compiled, five foundation tests passed, and 27 operational regression checks passed in both Windows PowerShell 5.1 and PowerShell 7.6.5. There is still no project-owned playable map or packaged game. New tooling should help cross those milestones. It should not conceal that they remain unfinished.

### Shortlist by bottleneck

| Bottleneck | First candidate | Adoption boundary |
|---|---|---|
| Repetitive editor actions | Native Unreal MCP and narrowly scoped toolsets | Inspect and validate in a throwaway editor project; preserve a manual path |
| Repeated Blender exports | Blender For Unreal Engine or Send to Unreal | Compare on one representative model; retain exactly one routine exporter |
| Repetitive district dressing | Built-in PCG first, then PCG Extended Toolkit for a demonstrated missing operation | Prove one deterministic alley with exclusion zones and collision checks |
| Mission iteration is dominated by graph glue | FlowGraph or a smaller quest solution | Keep persistent mission truth in owned game contracts; test save/reload and failure |
| Dialogue iteration is the bottleneck | DlgSystem or Inkpot | Choose the authoring model with one scene; do not install both speculatively |
| Native code is becoming difficult to navigate | Serena language-server trial | Qualify Unreal symbols and Blueprint-reference limitations, measure memory |
| Large build logs waste debugging time | OpenUnrealAutomationTools | Evaluate focused log analysis against the existing tested build scripts |
| Need an independent game-design critique | BMAD Game Dev Studio | Critique the current mission; do not restart the whole project specification |

This is a sequence of choices, not a combined dependency list. The detailed cards below explain evidence, gaps, integration costs and alternatives. Repository and license links are pinned where public access permits. Native Unreal features and Epic samples use Epic documentation and engine terms, rather than being mislabeled as MIT open-source repositories.

## 2 How to read the evidence

Five evidence levels prevent accidental overclaiming. **Inspected** means the upstream repository or official documentation was read. **Declared** means a descriptor or maintainer states a version or capability. **Built locally** means our actual pinned project compiled with it. **Packaged** means a cooked/staged executable ran successfully with it. **Production-qualified** means its specific game behavior survived relevant saves, performance checks, failures and playtests. An inspected HEAD never automatically advances to the later levels.

The full machine-readable register accompanies this report. A commit SHA fixes the source snapshot we examined. The commit date records the observed commit timestamp, not a promise of active support, a release date, or a security audit. Some recent HEADs are documentation or dependency updates. Conversely, a stable small library may not require frequent commits. We inspect what changed and which engine versions the actual files target.

Read mismatches as useful evidence. A README may lag its plugin descriptor. A plugin descriptor can declare a target without proving a clean Shipping build. An example project may enable other plugins that are omitted from its marketing description. A GitHub topic named ue5 does not establish UE 5.8 compatibility. A successful build with the wrong engine patch does not qualify our pinned 5.8.2 environment.

Use these decision labels consistently. **Reference** means learn from or consult the project without adopting it. **Trial** means there is a bounded experiment worth running when its prerequisite exists. **Defer** means it could be valuable later but adds cost now. **Avoid for this baseline** means it conflicts with the current engine, scope, budget or workflow; it is not a judgment that the upstream project is bad. **Retain** means an existing capability is already serving the project. No new runtime dependency earns an unconditional adoption recommendation merely from this desk research.

### The actual cost of a free framework

Assess more than the purchase price. For each dependency, count configuration, learning, data migration, adapters, additional modules, test fixtures, binary asset conversion, cook failures, update work and removal cost. Ask which game system owns authoritative state. If three frameworks each want to own the player, input routing, save data and session flow, their integration costs will not be independent.

The starting budget remains the handbook's EUR 480 envelope with EUR 0 spent by this task. This research allocates no new subscription. Keep the existing allowance for memory, storage and independent recovery ahead of speculative purchases. Approximately 65 GiB was free on C: when this sprint began; no uninstall or removal was needed. The user's optional cleanup permission remains available for future storage pressure, but this delivery does not delete Satisfactory, No Man's Sky or Hold My Ladder worktrees.

## 3 Dependency architecture and exit paths

Use the game-owned domain as the stable center. Stable IDs, cargo ownership, mission facts, progression, save migrations and sector transfer should have documented contracts that do not expose a particular narrative plugin's internal classes throughout the project. A graph node can request an action; it should not become the only place where durable mission progress exists. A dialogue choice can emit an intent; the game applies it and records the result.

For example, the sunseed pickup should call a game-owned transaction such as TryTransferCargo with a stable item ID and explicit source/destination. The exact API is a future design proposal. FlowGraph, dialogue, a debug command and a direct interaction can all reach that contract through small adapters. Saving an in-progress graph by serializing arbitrary UObject pointers is not an acceptable substitute for a versioned save model. If the graph plugin is removed, a simpler mission runner should be able to consume the same facts.

Keep content-generation tools outside the shipped runtime when the result is meant to be baked. Record the generator version, seed, input manifest, output package paths and whether regeneration overwrites artist edits. A deterministic rebuild is valuable only if it preserves deliberate exceptions such as landing-pad clearance and mission-critical sightlines. Do not regenerate a district at game load unless that runtime behavior is actually required and profiled.

Use one exclusive writer for binary assets and one coordinated build process per checkout. Parallel agents can research different plugins, write independent C++ helpers or review code. They should not concurrently mutate the same map, project configuration or Blueprint. Shared-memory collisions and build output races are not an acceptable price for apparent parallelism.

Every promoted dependency gets an owner, a pinned revision or release, its exact license record, required engine patch, direct and optional dependencies, runtime/editor classification, relevant tests, and a removal plan. Preserve third-party notices separately from project-authored code. Do not silently copy a few convenient source files while losing their origin. A public repository with an unclear license is a research candidate until its usage terms are established.

For a runtime plugin, a promotion check includes Editor and Game compilation, a minimal Shipping cook, launch without the editor, save/reload if it holds state, and a comparison against the unmodified baseline. For an editor-only tool, demonstrate the resulting assets still cook and function with the tool disabled. Some generation systems intentionally require runtime components; record that explicitly instead of assuming all generated results are independent.

The first experiment should be small enough to discard in an afternoon. Preserve the starting commit, isolate new configuration and use project-owned test assets. If the tool cannot demonstrate its intended benefit by the end of its timebox, record the failure and continue the game with the simpler path. Extending a trial can be justified by a concrete new finding; extending it because the repository promises enormous future leverage is not evidence.



Research date: 12 October 2026. Target: the installed Windows Unreal Engine 5.8.2, changelist 56702186, and the existing Borrowed Sun C++ project. This is a read-only qualification study, not an installation record. No repository plug-in, downloaded script, installer, MCP endpoint, or remote-execution service was run. Ten public repositories were checked at exact default-branch commits through GitHub's API. The delivered `Docs/Registers/repository-candidates.json` records complete commit IDs, UTC dates, pinned license files, source URLs, decisions, and estimated effort.

## 4 Recommendation

Use the editor facilities already shipped with Unreal, one qualified Blender exporter, and a small set of project-owned validation rules. Reserve procedural extensions for a demonstrated content bottleneck. The three serious trials are **Epic's native Unreal MCP**, **Blender for Unreal Engine**, and **PCGEx**, in that order of conditional admission: MCP can be evaluated against a disposable map; the exporter becomes useful when original props actually enter production; PCGEx waits for a hand-built street segment and a repeated assembly recipe. These are candidates to test, not three mandatory installations.

The strongest immediate gain is reliable repetition. A script that imports a measured door, checks its collision and scale, creates a material instance, and reports the changed packages is more valuable here than an agent generating an impressive but unmaintainable city. Borrowed Sun's important work is the clinic-to-dock journey, sunseed interaction, ship departure, orbital destination, and meaningful return. Every automation should shorten one of those production tasks or make its result easier to verify.

Keep the current **EUR 480 planning envelope**: EUR 240 for memory, storage and independent recovery; EUR 120 for qualified assets or production aids; EUR 40 for operations or testing; EUR 80 uncommitted. Spending remains zero. The proposed repository trials have no newly quoted purchase price; their main cost is the creator's time and existing AI usage. They do not justify a hosted-agent subscription or displacing recovery funding. Any paid service would require a concrete gross cost, including tax, exchange effects, and renewal obligations, before changing the budget.

## 5 What the installed engine already contains

Epic documents Unreal MCP as an experimental HTTP server inside the editor, with serial game-thread tool calls, selectable toolsets, and no authentication layer. It supports local use and is incomplete. The documented default endpoint is loopback port 8000. Individual toolsets can be enabled instead of the broad All Toolsets aggregator. This is the correct first-party comparator for third-party bridges in 5.8. [EB01: Epic Unreal MCP documentation](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor)

Local inspection confirms the plug-in is installed, rather than merely announced. `ModelContextProtocol.uplugin` declares version 1.0, experimental status and `EnabledByDefault=false`. `AllToolsets.uplugin` is also experimental and disabled by default. The settings header sets autostart false, port 8000 and tool search true. Tool search changes discovery; it is not a permission boundary. The existing `.uproject` does not declare either plug-in enabled, and the inspected project configuration contains no matching override. This is a static configuration finding, not a network scan or proof of the state of every running process. Local evidence and file hashes are preserved in the editor baseline assessment in the register.

The installed HTTPServer configuration defaults its bind address to `localhost`; the server source also checks Origin headers, allowing requests without Origin. The practical implication is precise: do not describe origin validation as authentication, and verify the effective listener address during a future test rather than assuming documentation or a plug-in name enforces isolation. We did not open that listener. A local service still has the editor's access to project files and assets.

Actual installed editor toolset declarations cover selected actors, visible actors, content-browser selections, viewport capture, open assets and PIE control. The registry and All Toolsets descriptor include automation, PCG, animation, gameplay tags, Slate, UMG and other domains. Those declarations demonstrate available code; they do not establish that every operation succeeds against this project. Enable only the subset needed for a trial and inspect the discovered tools before use. An inspection-only first session is an operating restriction, not a claimed sandbox.

The native route also removes one misleading comparison: a third-party README that says Unreal 5.8 ships only an empty server framework is not an accurate description of this installed build. Conversely, Epic ownership does not guarantee reversible edits. Transactions, saving, external file writes and recovery still depend on the tool being called. Keep engine tools out of the packaged offline game; the transport includes runtime modules, so “MCP” does not itself mean editor-only.

Three further native facilities cover much of the proposed workflow:

- **Data Validation** is installed and enabled by default. Epic supports project rules through `IsDataValid` or `UEditorValidatorBase`, and a commandlet suitable for automation. C++ rules run by default; Python validators need registration. Put critical asset checks in the owned editor module so unattended runs do not depend on someone opening a utility widget. [EB02: Data Validation](https://dev.epicgames.com/documentation/unreal-engine/data-validation-in-unreal-engine)
- **Remote Control** is an existing HTTP interface to exposed UObject properties and functions. Its reference describes transactional writes, including `WRITE_TRANSACTION_ACCESS`; that is a useful capability, not general undo for arbitrary script effects. For Borrowed Sun it is an alternative for a narrow control surface, not another service to enable alongside MCP by default. [EB03: Remote Control HTTP reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/remote-control-api-http-reference-for-unreal-engine)
- **Editor Utility Widgets** provide a dockable UMG-based editor interface. Use one only after a repeated task has a stable function underneath it. A polished control panel for an unstable pipeline consumes time without stabilizing the result. [EB04: Editor Utility Widgets](https://dev.epicgames.com/documentation/en-us/unreal-engine/editor-utility-widgets-in-unreal-engine)

## 6 Ten repositories assessed for this project

### ER01 — EpicGames/BlenderTools: reference and fallback

Canonical repository: [EpicGames/BlenderTools](https://github.com/EpicGames/BlenderTools). The requested EpicGamesExt address redirects here. Observed HEAD is `04cae45d71874a37d75522f4a694a5d4aae32147`, dated 3 September 2026; its change updates documentation URLs. The pinned [MIT license](https://github.com/EpicGames/BlenderTools/blob/04cae45d71874a37d75522f4a694a5d4aae32147/LICENSE.md) was read and hashed. Latest displayed Send to Unreal release is 2.4.3, dated 9 November 2023, with reported tests on Blender 3.3/3.6 and Unreal 5.3. Recent repository activity therefore does not establish a recent exporter release or 5.8 qualification. [Release evidence](https://github.com/EpicGames/BlenderTools/releases/tag/20231109043947)

Its quick start uses Unreal's Python remote execution and editor scripting plug-ins. That makes it a live editor pipeline with project-selection and import-setting risks, not simply a file converter. [Pinned setup](https://github.com/EpicGames/BlenderTools/blob/04cae45d71874a37d75522f4a694a5d4aae32147/docs/send2ue/introduction/quickstart.md)

Decision: keep as a reference and fallback, with no immediate integration allocation. Prefer the exporter with current explicit 5.8 coverage below. If revisited, allow 3–6 hours for a measured export/reimport test. Reject a setup that requires unrelated rendering changes for simple static props. Preserve conventional FBX/glTF intermediates and the source `.blend` so abandoning the add-on does not strand the asset.

### ER02 — poly-hammer/BlenderTools: stronger Send to Unreal fallback

[poly-hammer/BlenderTools](https://github.com/poly-hammer/BlenderTools) is an independently maintained community fork, explicitly unaffiliated with Epic. HEAD `470b0218f6cc4578b79ea367c508211b02b41ba8`, 27 May 2026, fixes combined-mesh transforms and child selection. Its pinned [MIT file](https://github.com/poly-hammer/BlenderTools/blob/470b0218f6cc4578b79ea367c508211b02b41ba8/LICENSE.md) is present. Release 2.6.7, dated 23 November 2025, fixes Blender 5.0 preferences; its reported Unreal test matrix is still 5.3/5.4. [Release evidence](https://github.com/poly-hammer/BlenderTools/releases/tag/20251123002929)

The pinned setup recommends the legacy FBX importer for Unreal 5.5 onward and uses Python remote execution. [Setup evidence](https://github.com/poly-hammer/BlenderTools/blob/470b0218f6cc4578b79ea367c508211b02b41ba8/docs/send2ue/introduction/quickstart.md)

Decision: first alternate if BFU fails the asset test, not a second concurrent exporter. The legacy-importer requirement is a meaningful project-wide compatibility choice; do not silently apply it to the production project. Test it only in intake, record the configuration difference, then decide whether the saved time outweighs maintaining a second import route. Its 3–6 hour qualification estimate includes restoring the baseline configuration. Successful operation on an older engine is useful evidence but not proof for 5.8.2.

### ER03 — Blender for Unreal Engine: recommended exporter trial

[xavier150/Blender-For-UnrealEngine-Addons](https://github.com/xavier150/Blender-For-UnrealEngine-Addons) has HEAD `780d9217fc8271f946f358559dbbb2ebbf989175`, 8 October 2026. Use released **v4.4.8**, pin `ea415063a014ac519e9f9f80e5e7a4458b2356a2`, as the trial reference rather than development HEAD. The [pinned license](https://github.com/xavier150/Blender-For-UnrealEngine-Addons/blob/ea415063a014ac519e9f9f80e5e7a4458b2356a2/LICENSE) contains GPL version 3. Do not describe it as MIT or apply its license indiscriminately to unrelated assets.

The release history adds UE 5.8 support in 4.4.6 and fixes child transforms and socket transforms in 4.4.8, released 22 July 2026. The project supports batch export and generated Unreal import scripts. These are particularly relevant to doors, modular dock pieces and ship attachment points. [Release evidence](https://github.com/xavier150/Blender-For-UnrealEngine-Addons/releases/tag/v4.4.8), [pinned feature description](https://github.com/xavier150/Blender-For-UnrealEngine-Addons/blob/780d9217fc8271f946f358559dbbb2ebbf989175/README.md)

Decision: allocate 3–5 hours when the first custom asset exists. Use explicit export files and review the generated import script before running it in intake. This avoids making a persistent remote listener a prerequisite for ordinary prop production. Generated Python still has editor privileges; file-based execution is not a sandbox. The exit asset is an ordinary mesh with reviewed import settings. If the add-on becomes a maintenance burden, the source and export manifest support manual import without redesigning gameplay.

### ER04 — PCGEx: conditional environment-production trial

The former Nebukam path now resolves to [PCGEx/PCGExtendedToolkit](https://github.com/PCGEx/PCGExtendedToolkit). Default HEAD `5bcfbbf5fb0bfe1a3de65421e9cc8c27be3d69e5`, 11 October 2026, concerns collection schema migration. The project explicitly calls `main` unstable and distinguishes engine-version branches. Its README claims 5.8/5.7 support and graph, path and spatial operations. [Pinned README](https://github.com/PCGEx/PCGExtendedToolkit/blob/5bcfbbf5fb0bfe1a3de65421e9cc8c27be3d69e5/README.md)

The observed 5.8 branch pin is `8072735a81eca04e4cee61d0a06a5c3f6a3f6da5`; its descriptor says 0.79.2 and EngineVersion 5.8.0. Both branch and HEAD have an [MIT license](https://github.com/PCGEx/PCGExtendedToolkit/blob/8072735a81eca04e4cee61d0a06a5c3f6a3f6da5/LICENSE). A separate Delaunator notice also exists. The latest-marked release is titled “PCGEx 0.79.1 hotfix” but uses tag `v0.79.4-hotfix.1`; preserve that discrepancy rather than inventing a clean version sequence. [Release evidence](https://github.com/PCGEx/PCGExtendedToolkit/releases)

Decision: reserve a 4–6 hour trial only after ordinary splines and native PCG prove insufficient for a repeated structure. Start with service conduits or collector supports, not the mission layout. The descriptor contains runtime modules: generated content can retain a plug-in dependency. A bake-and-remove test must prove independence before calling it an editor-only cost. Do not add its MCP branch, paid extensions or a general city generator during this trial. The creator needs controllable authored routes more than maximum procedural variety.

### ER05 — chongdashu/unreal-mcp: reject for this baseline

[chongdashu/unreal-mcp](https://github.com/chongdashu/unreal-mcp) HEAD `4e5f00da50733190481311e254d16d137a84ef33` is dated 22 April 2025 and is a revert. Its own README labels the project experimental and advises against production use. It advertises a Python MCP server connected to a native TCP plug-in and a UE 5.5 sample. [Pinned README](https://github.com/chongdashu/unreal-mcp/blob/4e5f00da50733190481311e254d16d137a84ef33/README.md)

The inspected bridge starts during subsystem initialization and specifies loopback port 55557. It dispatches named commands rather than proving a universal Python interpreter. [Pinned bridge](https://github.com/chongdashu/unreal-mcp/blob/4e5f00da50733190481311e254d16d137a84ef33/MCPGameProject/Plugins/UnrealMCP/Source/UnrealMCP/Private/UnrealMCPBridge.cpp)

The README says MIT, but the complete pinned tree contains no LICENSE/COPYING file. Record that as an unresolved notice/permission-documentation gap, not a definitive legal conclusion. No 5.8.2 build, comprehensive undo, authentication, or durable action audit was established here. Recreating an older bridge's basic actor operations adds little over the engine's current toolsets. Estimate 4–8 hours to qualify and repair if someone had a specific reason; allocate zero now. Keep the existing project scaffold rather than adopting its demo project.

### ER06 — flopperam/unreal-engine-mcp: reject overlapping local bridge

[flopperam/unreal-engine-mcp](https://github.com/flopperam/unreal-engine-mcp) HEAD `b1b495b7c396002108ecf5fed7853e5799be834c`, 26 June 2026, merges an Aura banner change. The README distinguishes the downloadable local server from a separate hosted service that shares no code with it. Its larger hosted-tool claims cannot be credited to the repository. It also advertises a separate Flop AI product with 5.5–5.7 coverage, not local evidence for 5.8.2. [Pinned README](https://github.com/flopperam/unreal-engine-mcp/blob/b1b495b7c396002108ecf5fed7853e5799be834c/README.md)

The inspected local C++ bridge uses loopback port 55557 and logs dispatched commands. A full action audit, transaction recovery contract and current-engine test were not demonstrated. The README declares MIT, but the complete pinned tree has no LICENSE/COPYING file. [Pinned bridge](https://github.com/flopperam/unreal-engine-mcp/blob/b1b495b7c396002108ecf5fed7853e5799be834c/UnrealMCP/Source/UnrealMCP/Private/EpicUnrealMCPBridge.cpp)

Decision: no installation or subscription. Allowing this beside ER05 would also introduce a port collision unless reconfigured. The local world-building demonstrations do not solve Borrowed Sun's mission persistence or playable return. Qualification would plausibly consume 4–8 hours, with no measured benefit over the first-party route. Reconsider only a specific missing operation, with rights documentation and an independently scoped trial of the exact local or hosted product under consideration.

### ER07 — Claireon: credible alternate when native tools hit a gap

[believer-oss/Claireon](https://github.com/believer-oss/Claireon) HEAD `89bee2f4c62d1df625cca71ae66e7c81a415a009` is the 2.2.0 release; committer date is 15 September 2026, while the changelog dates the release 14 September. The pinned [MIT notice](https://github.com/believer-oss/Claireon/blob/89bee2f4c62d1df625cca71ae66e7c81a415a009/LICENSE) is present, with a separate MIT notice for the included BAAI model.

This is a serious maintained candidate: its changelog describes transaction identity/recovery fixes, PCG correctness work, stock-engine 5.5–5.8 builds and some UE 5.8.2 regression observations. Those remain maintainer-reported tests. [Pinned changelog](https://github.com/believer-oss/Claireon/blob/89bee2f4c62d1df625cca71ae66e7c81a415a009/CHANGELOG.md)

Source inspection finds Python script/output audit persistence and explicit rollback failure reporting. Its security policy describes unrestricted Python and unauthenticated local access, and warns that header checks are not a sufficient network boundary. That policy's bind warning must be verified on our engine, not treated as an observed local listener. [Audit source](https://github.com/believer-oss/Claireon/blob/89bee2f4c62d1df625cca71ae66e7c81a415a009/Source/Claireon/Private/ClaireonPythonAuditLog.cpp), [security policy](https://github.com/believer-oss/Claireon/blob/89bee2f4c62d1df625cca71ae66e7c81a415a009/SECURITY.md)

Decision: strongest reserve MCP alternative. Allocate 4–8 hours only if native toolsets fail a written Blueprint or asset-authoring task that Claireon addresses. Do not confuse audit logs with recovery: saved files and external Python effects require their own restoration proof. Its broader system and optional embedded assistant add maintenance the initial slice may not need. Replace the MCP trial route if it wins; do not keep two broad editors under simultaneous agent control.

### ER08 — db-lyon/ue-mcp: capable, broad, and currently too much infrastructure

[db-lyon/ue-mcp](https://github.com/db-lyon/ue-mcp) HEAD `f4433cb8e1070a99cac2d0f03edd8c7768b2c9da`, 11 October 2026, is `1.4.0-beta.3`. Its pinned [MIT file](https://github.com/db-lyon/ue-mcp/blob/f4433cb8e1070a99cac2d0f03edd8c7768b2c9da/LICENSE) is present. The README claims Windows 5.4–5.8 support, a Node service plus C++ bridge, repeatable flows, and routing into Epic toolsets. None was locally compiled. [Pinned README](https://github.com/db-lyon/ue-mcp/blob/f4433cb8e1070a99cac2d0f03edd8c7768b2c9da/README.md)

Its guard documentation distinguishes existing-file writes from all mutations and unknown arbitrary-code operations; guard hooks can veto before a call or audit afterward. These are useful mechanisms, not proof of a configured restrictive default. The inspected bridge binds loopback without authentication. Feedback can create external GitHub issues, so that capability must remain off unless sending is specifically requested. [Guard contract](https://github.com/db-lyon/ue-mcp/blob/f4433cb8e1070a99cac2d0f03edd8c7768b2c9da/docs/plugins-guards.md), [security policy](https://github.com/db-lyon/ue-mcp/blob/f4433cb8e1070a99cac2d0f03edd8c7768b2c9da/SECURITY.md)

Decision: defer. It could become attractive when multiple stable workflows need orchestration, but adding another package ecosystem, policy configuration and bridge before a playable mission is a poor trade. Estimate 5–10 hours for initial qualification, excluding repairing upstream incompatibilities. A future test should target one flow with injected failure and recovery, and use pinned packages rather than an automatic `latest` installation. Shell and arbitrary-Python actions must be evaluated separately from routine actor tools.

### ER09 — UnrealMCPToolkit: small extension, stale justification

[timargv/UnrealMCPToolkit](https://github.com/timargv/UnrealMCPToolkit) HEAD `2d2d9e63d4d2defa19693bf79a56e1b738d45318`, 28 June 2026, updates download documentation. The pinned [MIT license](https://github.com/timargv/UnrealMCPToolkit/blob/2d2d9e63d4d2defa19693bf79a56e1b738d45318/LICENSE) exists. It claims UE 5.8/Win64 support and extends Epic MCP with Python, console execution and editor conveniences. Its README explicitly acknowledges arbitrary code with editor privileges. [Pinned README](https://github.com/timargv/UnrealMCPToolkit/blob/2d2d9e63d4d2defa19693bf79a56e1b738d45318/README.md)

The same README says the native engine cannot place actors or create assets without custom tools. Current Epic documentation and the installed toolset inventory invalidate that general premise for this environment. That does not prove this extension is broken; it means its incremental value must be established tool by tool. A binary described as “5.8” is also not evidence of binary compatibility with this exact installed build.

Decision: no baseline installation. A reviewed local Python commandlet or a narrow owned tool can cover a missing batch operation without permanently exposing a universal interpreter. Estimate 2–4 hours only if a specific missing function survives that comparison. Do not use `save_all` as the default completion step; it can persist unrelated dirty work. Universal access is convenient but offers no intrinsic transaction, asset-scope or output-quality guarantee.

### ER10 — Allar/ue5-style-guide: reference, not an automation dependency

[Allar/ue5-style-guide](https://github.com/Allar/ue5-style-guide) default HEAD `198a8550947de91f19362dabf57fbd1f1cdf0904` is dated 7 March 2022. Its pinned [MIT license](https://github.com/Allar/ue5-style-guide/blob/198a8550947de91f19362dabf57fbd1f1cdf0904/LICENSE) exists. Despite the repository name, default README identifies itself as UE4 guidance and points to `v2`; observed v2 pin `2127e9ab3ef740d5df3e2546205f2fcce7bc430e` explicitly calls itself a draft. [Default reference](https://github.com/Allar/ue5-style-guide/blob/198a8550947de91f19362dabf57fbd1f1cdf0904/README.md), [v2 reference](https://github.com/Allar/ue5-style-guide/blob/2127e9ab3ef740d5df3e2546205f2fcce7bc430e/README.md)

Decision: use selected principles as a documented reference, with attribution where adapted; do not install an assumed linter or inherit every convention. The repository is not a UE 5.8 compiler-tested plug-in. Spend at most 0.5–1 hour reconciling useful naming ideas with the existing project. Then implement only checks that prevent an actual failure. Renaming all purchased content for aesthetic consistency can break references and consume more time than it saves. Owned game content and vendor intake should have different validation scopes.

## 7 Trial A: native MCP as a controlled editor assistant

Budget 2–4 hours, after confirming a recoverable project copy or branch with the required binary content. Use the existing two-module project as the production authority. A throwaway test map or intake copy is the target, not a new example game that silently becomes the foundation. Do not enable every toolset simply because it is convenient. Keep autostart off for this trial and document the tools intentionally available.

Begin with five bounded observations: engine/build identity, project path, current map, selected actors, and a compact viewport or asset report. Confirm that the tool result points to the intended project. If the exposed native tools cannot provide project identity directly, verify it through a separate trusted local check before mutating anything. A response claiming success is weaker evidence than a re-read of the changed asset.

Next test one ordinary edit at a time: duplicate a disposable actor, set a transform, create a material instance if the selected toolset supports it, assign that instance to the duplicate, then reverse the changes. Save only the intentionally changed package. Record command, relevant parameters, before/after state, package list, duration and outcome. No new broad Python escape hatch should be necessary merely to make the demonstration impressive.

Require three properties before keeping the connection. First, the same request must be scoped to the same map and asset on a repeat run. Second, an invalid asset path must fail without creating unrelated packages. Third, recovery must work both before save through the actual applicable undo mechanism and after save through the recorded project snapshot. An editor transaction does not undo an arbitrary filesystem write. A successful undo does not prove recovery after a crash.

The acceptance result is ten successful bounded operations across at least two editor launches, a known list of dirty packages, no unexplained edits, and a measured time comparison against manual execution. A screenshot alone cannot establish material-slot correctness, collision setup or persistence. Read back those properties. Reject the trial if time spent interpreting failures exceeds the manual task time, or if preventing unrelated writes requires more infrastructure than the task itself.

Use this assistant for inspection, repetitive placement from explicit coordinates, small material-instance batches and collecting logs. Keep design decisions, level readability, mission logic and final visual acceptance with the creator. Save reproducible operations as versioned functions or data-driven recipes once they recur. Asking an agent to re-invent the same ten-step procedure every day creates variance instead of a workflow.

## 8 Trial B: one Blender export pipeline

Budget 3–5 hours using BFU v4.4.8 when a first custom prop is needed. The candidate reference is pinned in the register, but its downloaded release archive is not yet verified or installed. A future intake must hash the actual archive and match its source/version to the chosen reference. Keep the qualified Blender version fixed during the test; do not simultaneously upgrade Blender, change the Unreal importer, and swap an exporter.

Use a tiny representative scene: a one-metre calibration object, a hinged service door, a dock railing with simple collision, and a ship hardpoint socket. These exercise Borrowed Sun's actual needs. Export to a task-owned intermediate directory, review generated scripts, and import under a disposable content root. Preserve source-unit settings, axes, origin/pivot, scale, material-slot order, collision names, socket transforms and import options in the manifest.

The test is two complete round trips. On the first, confirm measurements and visibility in a playable map. On the second, modify dimensions, a socket position and one material assignment in Blender, then re-export and reimport into the same asset paths. Existing scene references must survive; the exporter must not create duplicate assets or silently reorder material slots. Finally close and reopen the editor, load the map and repeat the collision and socket checks.

Treat source edits and imported package writes as separate recovery units. Blender undo cannot restore an overwritten Unreal package; editor undo cannot reconstruct a lost `.blend`. Use staged source copies and package snapshots. The console must identify errors and the destination, and the manifest must list what should have changed. A successful export button with an empty or stale asset is a failed test.

Keep the workflow only if the creator can repeat it from the short written recipe without rediscovering settings. The target is fewer decisions per asset, not a magical button that conceals them. Once accepted, migrate only reviewed content and dependencies from intake into the owned scaffold. The Third Person template remains a separately evaluated movement donor; none of these exporters supplies an already-integrated character or gameplay foundation.

If BFU fails inside the timebox, record the failing case and return to native file import. Test the poly-hammer route later only if its documented importer requirements fit the same assets. Do not maintain BFU, original Send to Unreal and a community fork simultaneously. Three exporters multiply the number of settings combinations and failure explanations without increasing the number of finished props.

## 9 Trial C: PCGEx for repeatable support geometry

Budget 4–6 hours, postponed until the creator has hand-built one readable segment and can point to a repeated placement cost. Use ordinary native PCG and splines first. PCGEx earns its place only when graph connectivity, path processing or spatial selection materially simplifies that recipe. A market district's landmark placement, clinic entrance and return route remain authored constraints.

Choose one problem, such as running utility conduits along a supplied route while preserving door clearances. Provide a fixed input spline or graph, a small approved mesh collection, an explicit seed, and exclusion volumes around gameplay. Output ordinary supports and conduits with recorded counts. Avoid generating interactive actors, mission triggers or save identifiers through an unqualified random process.

Test the same input twice, then change the route once. Record generation time, peak working set where measurable, output count, bounding boxes and visual differences. A fixed seed is necessary but not sufficient for reproducibility: plug-in version, engine version, input order and settings are part of the recipe. Determine whether an unrelated asset update changes the result. Regeneration must not accumulate duplicate objects or overwrite handcrafted work.

The final gate is packaging and exit. If the production decision is to bake generated output, verify which graph, settings, actor and material references still depend on PCGEx. Remove the experimental recipe only in a disposable copy, then reopen and package the representative map. If dependencies remain, record PCGEx as a runtime/content dependency and include its maintenance cost honestly; do not claim it has been eliminated just because the meshes appear in the viewport.

Stop if authoring the graph takes longer than placing the current set of supports by hand and the next ten repetitions are not scheduled. Procedural tools pay back through reuse. Borrowed Sun's first slice may have too little repeated environment work to recover even a modest setup cost. PCGEx can remain a well-researched deferred option without weakening the game.

## 10 Asset QA and a small owned editor layer

Build a minimal rule set in `BorrowedSunEditor` as real assets arrive. Initial rules should answer concrete questions: is owned content under the agreed root; does an interactable's identifier remain unique; does an intended solid mesh have the required collision; does a material instance reference an approved parent; does a mission definition contain valid payload and destination references; and does the shipping map accidentally reference an intake or development-only package? Choose thresholds from measured content, rather than importing arbitrary triangle and texture limits from a guide.

Name rules are useful when they communicate asset type or avoid collisions. They should not automatically rename vendor packages. Keep the intake provenance record linked to the vendor version and original path. For adapted assets, create owned instances or derivatives with a clear source relationship. Bulk renaming should produce a preview and explicit mapping, and reference integrity should be checked afterward. This is an implementation recommendation, not an existing validator claimed to be present.

A useful first utility has three actions: inspect the selected assets, validate them, and export a compact report. Add a mutation button only after the equivalent function is reliable. The report should identify the asset path, rule, observed value, expected condition and actionable fix. Distinguish blocking failures from authoring notes; a validator that floods the creator with low-value warnings will stop being used.

Prefer data passed into a reviewed function over generating new code for every batch. For example, a JSON list of asset paths and intended material parameters can be reviewed, applied to a bounded selection and re-read. The same recipe can run without an AI connection. This keeps production moving when a model, MCP bridge or documentation endpoint changes. The agent is an interface to the workflow; it should not be the only place where the workflow exists.

## 11 Time economics and admission rules

All hours below are planning hypotheses, not measured savings or vendor promises. “Net” includes setup, verification and likely troubleshooting. No financial value has been assigned to the creator's time.

| Trial | Qualification cost | Example repeated workload | Illustrative net result |
|---|---:|---|---|
| Native MCP | 2–4 hours | 100 operations save 1 minute each | Lose 0.3–2.3 hours initially |
| BFU exporter | 3–5 hours | 100 exports save 3 minutes each | Break even to save 2 hours |
| PCGEx recipe | 4–6 hours | 12 repeated assemblies save 30 minutes each | Break even to save 2 hours |

These examples deliberately include the first setup cost. At a one-minute saving, native MCP needs roughly 120–240 successful repetitions to recover 2–4 hours; troubleshooting moves the threshold upward. BFU needs roughly 60–100 three-minute savings. PCGEx needs eight to twelve half-hour savings. Measure manual and automated runs on the same content before trusting these assumptions. Faster execution that increases rework is not a saving.

Do not spend all three timeboxes before making more game. Take one trial, record keep/defer/reject, and return to the mission. A positive result should include a runnable short recipe, exact qualified versions, a small regression fixture, a restoration method and a named gameplay or content task that will reuse it. A rejected trial should leave ordinary source files and assets behind, not force a project migration.

New versions enter a disposable qualification copy first. The register separates observed default HEAD from the release or engine branch selected for testing. An upstream README, badge or successful CI run does not replace a local compile, import, packaged-map test or recovery drill on the installed 5.8.2 build. The present study establishes evidence and a decision order; every repository remains locally unqualified until those gates are actually executed.



Observed 12 October 2026. Target: the existing Windows Launcher UE 5.8.2 project, a solo developer, an initial budget below €500, and a playable street-to-orbit mission before architectural expansion. This memo covers eleven public repositories plus Epic's official Lyra sample. It recommends adoption decisions; it does not claim that any candidate was locally compiled, installed, cooked, packaged, profiled or played during this research sprint.

## 12 Decision

Keep the working native C++ project and its small gameplay modules. Use built-in character movement, Enhanced Input, owned campaign/save data and a compact NPC controller initially. The strongest optional additions are **ALS Refactored for ground locomotion**, **one of DlgSystem or Inkpot for dialogue**, and **FlowGraph 2.3 for mission orchestration when repeated authoring demonstrates the need**. Each enters through a bounded trial, not through an automatic bulk installation.

The intended game needs reliable possession, collision, flight, consequence persistence and scene transitions more urgently than another general framework. None of these repositories implements Borrowed Sun's Sunseed custody rules, safe sector transfer, campaign journal or artistic direction. Framework adoption earns its place by reducing the time needed to deliver those things.

Epic GAS becomes a reasonable built-in choice if the design acquires numerous interacting abilities, cooldowns, status effects and cancellation rules. Epic describes these as supported GAS concerns; multiplayer is a capability, not a requirement. For today's small interaction set, simple owned components are a defensible starting choice. Do not adopt three GAS examples as three dependencies. [Epic GAS overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-the-unreal-engine-gameplay-ability-system).

StateTree is a possible later controller for named NPC schedules and reactions. It is a hierarchical state machine with tasks and transitions; it does not replace mission facts or a dialogue authoring system. Pick a small state machine first, or StateTree when the task graph saves real implementation work. Avoid introducing both Behavior Trees and StateTree for the same NPC role without a concrete need. [Epic StateTree quick start](https://dev.epicgames.com/documentation/unreal-engine/statetree-quick-start-guide).

## 13 Evidence and estimate policy

Default branches and their actual HEAD commit SHA/date were queried from GitHub on the observation date. Full forty-character SHAs, exact root license URLs, descriptor evidence and limitations are included in the delivered `Docs/Registers/repository-candidates.json`. Dates in the candidate cards are commit dates, not release dates. Root licenses were read at those immutable SHAs. All eleven public repositories have an MIT root license; this is not a recursive license audit of bundled art, dependencies or Epic materials. Retain the actual notices with any adopted code.

An engine declaration, a maintainer's tested-version statement and our own successful packaged-game test are different evidence levels. This research establishes the first two where available. No candidate has reached the third. A descriptor with no `EngineVersion`, or one saying `ANY_VERSION`, does not prove universal compatibility. The observed default HEAD is recorded for reproducibility even when a different release pin is recommended.

All effort ranges below are original planning estimates for a programmer working with AI assistance. They are neither vendor promises nor measured performance. They exclude full character art, a complete narrative, platform certification and extensive production hardening. Integration ranges overlap and must not be added into a schedule as if every candidate should be installed.

| Candidate | Decision for this project | Engine evidence | Evaluation / bounded integration |
|---|---|---|---|
| G01 Lyra | Reference | Official 5.8 documentation; package unqualified | 4–8 h / 40–100 h foundation transplant |
| G02 ALS Refactored | Trial if walking polish blocks progress | Descriptor 4.18 / UE 5.8.0 | 8–16 h / 24–60 h |
| G03 GASDocumentation | Reference | Sample and docs UE 5.3 | 2–6 h / 8–20 h selected adaptation |
| G04 GASShooter | Reference; avoid as foundation | UE 4.27 | 2–4 h / 40–100+ h uncertain port |
| G05 Narxim GAS Example | Reference | Project UE 5.6; AGRPRO enabled | 4–8 h / 16–40 h |
| G06 SimpleGAS | Defer | Maintainer minimum UE 5.2; no 5.8 qualification | 6–12 h / 20–50 h |
| G07 FlowGraph | Conditional trial at 2.3 tag | 2.3 explicitly UE 5.8; HEAD 2.4 targets 5.9 | 8–12 h / 20–40 h |
| G08 DlgSystem | First graph-dialogue trial | Maintainer lists UE 5.6–5.8 | 6–10 h / 12–28 h |
| G09 Inkpot | Alternative text-dialogue trial | Changelog explicitly adds UE 5.8 | 6–12 h / 16–36 h |
| G10 SimpleQuest + SimpleCore | Defer pending authoring need | Maintainer claims 5.6–5.8 | 8–16 h / 24–60 h |
| G11 UnrealSharp | Defer | README 5.6–5.8; beta descriptor | 8–16 h / 30–80 h |
| G12 MassSample | Reference; defer runtime adoption | Project UE 5.8; beta sample plugin | 12–24 h / 30–80 h crowd prototype |

## 14 G01 — Lyra: study selected systems, preserve the small project

Epic distributes Lyra through Fab and the Launcher. Its official documentation describes a modular sample with Game Feature plugins, experiences, customized GAS, input, UI, animation and multiplayer systems. No canonical publicly readable Epic Lyra Git repository, default branch SHA or exact downloaded license file was verified. The JSON intentionally contains nulls rather than attributing an unrelated public mirror to Epic. A documentation selector showing 5.8 is not proof that a downloaded sample built on 5.8.2. [Official Lyra documentation and distribution link](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine).

**Fit and cost.** Read pawn/input initialization, settings and possession patterns when the project reaches those problems. Rebasing on the whole sample now would exchange a small known project for an extensive unfamiliar one, then require removing or adapting game-specific experience and shooter conventions. Budget 4–8 hours for focused study. A 40–100 hour foundation transplant is a low-confidence allowance, not an attractive next sprint.

**Shipping boundary.** If extracting assets or code, obtain the official package and record its actual notices first. Do not label it MIT because a mirror does. Epic distinguishes engine licensed technology from separately distributed marketplace content; the applicable package terms remain to be checked. [Epic EULA, section 1](https://www.unrealengine.com/eula/unreal). No Lyra download is required to continue the existing project.

## 15 G02 — ALS Refactored: a credible locomotion trial

Observed `main` at `b754d6f0`, 12 August 2026. The exact [MIT license](https://raw.githubusercontent.com/Sixze/ALS-Refactored/b754d6f0f2bb03741d301f8fb88077ebfe561e17/LICENSE.md) is verified. The pinned [ALS.uplugin](https://github.com/Sixze/ALS-Refactored/blob/b754d6f0f2bb03741d301f8fb88077ebfe561e17/ALS.uplugin) declares plugin 4.18 and UE 5.8.0. Its README table stops at 4.17/5.7, so quoting only the README would incorrectly exclude current 5.8 evidence. The source provides locomotion, camera and optional extras modules, with Control Rig and Enhanced Input among the dependencies. [Pinned README](https://github.com/Sixze/ALS-Refactored/blob/b754d6f0f2bb03741d301f8fb88077ebfe561e17/README.md).

**Fit and cost.** This can save time if convincing walking, turning and camera response are the remaining obstacle to a presentable street scene. It is not a ship controller, character art generator or complete traversal game. Start with its stock character, then one licensed production skeleton; do not debug retargeting, flight and a custom animation graph simultaneously. Allow 8–16 hours to evaluate and 24–60 hours for a bounded ground-character integration.

**Ownership.** The project controls possession and input-context changes. ALS controls the possessed walking pawn's movement/animation, while the flight pawn stays independent. Avoid merging ALS and a complete Lyra pawn hierarchy. Require clean unpossession, camera handoff, restored movement after loading, and a cooked build before adoption. Future upstream fixes should be deliberate pin changes, not branch pulls during content production.

## 16 G03 — GASDocumentation: useful knowledge, historical implementation

Observed `master` at `8f76c578`, 6 April 2024; [MIT license](https://raw.githubusercontent.com/tranek/GASDocumentation/8f76c5780bdea69e0ea2cc161ab2f435f04182eb/LICENSE). The author explicitly identifies this as unofficial documentation and a sample current with UE 5.3; the [project descriptor](https://github.com/tranek/GASDocumentation/blob/8f76c5780bdea69e0ea2cc161ab2f435f04182eb/GASDocumentation.uproject) agrees. [Pinned README](https://github.com/tranek/GASDocumentation/blob/8f76c5780bdea69e0ea2cc161ab2f435f04182eb/README.md).

**Fit and cost.** Use it to understand ability ownership, activation, attributes and effects, then inspect the installed 5.8 engine headers and current Epic docs before writing code. Two to six hours spent on the relevant chapters is productive; migrating the sample wholesale is unnecessary. An 8–20 hour allowance only covers adapting a small selected example.

**Shipping boundary.** Reading creates no runtime dependency. If GAS is chosen, Borrowed Sun should own its ability classes and an explicit owner/avatar policy for walking versus flying. Decide which attributes belong to the pilot, which belong to the ship, and what survives possession changes. A generic ability system must never become the authority for campaign custody merely because an action consumes a charge. Keep that transaction in campaign logic and let the ability display its accepted result.

## 17 G04 — GASShooter: inspect weapon patterns, reject a foundation port

Observed `master` at `26295c54`, 3 December 2021; [MIT license](https://raw.githubusercontent.com/tranek/GASShooter/26295c548a19f221917e4a7232c12e763db64cb1/LICENSE). Both the [README](https://github.com/tranek/GASShooter/blob/26295c548a19f221917e4a7232c12e763db64cb1/README.md) and [project descriptor](https://github.com/tranek/GASShooter/blob/26295c548a19f221917e4a7232c12e763db64cb1/GASShooter.uproject) target UE 4.27. It is an advanced FPS/TPS GAS sample. No current UE 5.8 build evidence was found in the inspected source.

**Fit and cost.** The useful outcome is a two-to-four-hour study of one targeting or weapon interaction when needed. Its networked shooter focus and engine age make a project transplant a poor match for this prototype. Forty to one hundred hours or more is a highly uncertain migration allowance, before Borrowed Sun-specific work.

**Shipping boundary.** Do not add the entire sample to the project to gain one mechanic. Reimplement the smallest needed behavior with a clear test and appropriate attribution for copied code. Age alone does not establish that a repository is abandoned; it does establish that the observed code's engine target is far from the current project. Keep this distinction in procurement and compatibility notes.

## 18 G05 — Narxim GAS Example: concrete patterns with a dependency caveat

Observed `master` at `6a676a3d`, 26 October 2025; [MIT license](https://raw.githubusercontent.com/Narxim/Narxim-GAS-Example/6a676a3d0436c55e4ba8dc83b12ab86ac61eae87/LICENSE.md). The pinned [GAS_Example.uproject](https://github.com/Narxim/Narxim-GAS-Example/blob/6a676a3d0436c55e4ba8dc83b12ab86ac61eae87/GAS_Example.uproject) declares UE 5.6 and enables GameplayAbilities, CommonUI, ModelViewViewModel and **AGRPRO**, the latter with a Marketplace reference. The README includes a 5.6 migration entry and notes separate icon licensing. [Pinned README](https://github.com/Narxim/Narxim-GAS-Example/blob/6a676a3d0436c55e4ba8dc83b12ab86ac61eae87/README.md).

**Fit and cost.** Compare one input-binding or attribute implementation with current Epic behavior if GAS is selected. Allow 4–8 hours of study; 16–40 hours for adapting a bounded subset. The descriptor makes it unsafe to advertise the whole project as a self-contained, zero-dependency starting point. AGRPRO's availability, inclusion and applicable rights were not audited here; neither a required purchase nor free availability is assumed.

**Shipping boundary.** Resolve each enabled dependency before a sample build. Avoid importing unrelated UI, roguelite or turn-based systems into the core project. Project-owned tests must cover ability cancellation on possession, repeated input and reloading. A past commit mentioning a packaging fix is not proof that this SHA packages correctly with the user's engine and toolchain.

## 19 G06 — SimpleGAS: an alternative ability system, not free simplification

Observed `main` at `953f8c2b`, 21 May 2025; [MIT license](https://raw.githubusercontent.com/strayTrain/SimpleGameplayAbilitySystem/953f8c2bc8dad3598d210ccefab7f79c4e388b5f/LICENSE). This is the canonical `strayTrain/SimpleGameplayAbilitySystem`, not similarly named tutorials. The maintainer presents a Blueprint/C++ ability, attribute and event framework as an alternative to Epic GAS. [Pinned README](https://github.com/strayTrain/SimpleGameplayAbilitySystem/blob/953f8c2bc8dad3598d210ccefab7f79c4e388b5f/README.md). Installation documentation states UE 5.2 or later; the [descriptor](https://github.com/strayTrain/SimpleGameplayAbilitySystem/blob/953f8c2bc8dad3598d210ccefab7f79c4e388b5f/SimpleGameplayAbilitySystem.uplugin) has no engine version and enables StructUtils. [Maintainer installation requirements](https://straytrain.github.io/SimpleGameplayAbilitySystem/pages/installation/installation.html).

**Fit and cost.** Defer. A simpler public API can still create another dependency whose lifecycle, save semantics and upgrades the solo developer must understand. If custom components become cumbersome, a 6–12 hour comparison can evaluate precisely two abilities and one effect against built-in GAS. Budget 20–50 hours for a bounded integration, without assuming the smaller framework costs less over the game's life.

**Shipping boundary.** Choose SimpleGAS or Epic GAS for a particular actor's action model; do not run parallel attribute authorities. Prove cancellation, repeated activation, destruction, save/load and packaged execution. Do not infer specific 5.8.2 qualification from a broad minimum-version claim.

## 20 G07 — FlowGraph: useful orchestration, but pin the right release

Observed default `5.x` at `8c20e8f6`, 7 October 2026; [HEAD MIT license](https://raw.githubusercontent.com/MothCocoon/FlowGraph/8c20e8f6cd28ce32d4a56f13a2cdeaa67b415061/LICENSE). Its descriptor says 2.4. Official notes call **2.4 work in progress and the first UE 5.9 release**. The appropriate first trial is **`v2.3-5.8`, SHA `8211b25999068407cb7b40b8c97e18b52d4832ca`**, not default HEAD. Git tag resolution and its exact [MIT license](https://raw.githubusercontent.com/MothCocoon/FlowGraph/8211b25999068407cb7b40b8c97e18b52d4832ca/LICENSE) were verified. Version 2.3's release page is dated 11 July 2026 and explicitly introduces UE 5.8 support. [2.3 notes](https://mothcocoon.github.io/FlowGraph/Releases/Version23), [2.4 notes](https://mothcocoon.github.io/FlowGraph/Releases/Version24).

**Fit and cost.** Trial when sequences such as approach, speak, choose, operate and depart repeat across missions. Keep custom nodes thin: submit a command, await an accepted result, show presentation. Allow 8–12 hours to evaluate and 20–40 hours for a bounded adapter. Flow and SimpleQuest overlap as mission-authoring choices; starting with both would dilute the productivity test.

**Persistence.** Flow provides save hooks; integration remains the game's responsibility. Its docs specifically flag loading into an already active world and custom graph owners as cases requiring explicit restoration. Version 2.3 adds save-data array overloads suitable for an owned container. Save graph position in the same campaign checkpoint, with graph/content versioning. Never let a replayed node grant a reward directly. [Save support](https://mothcocoon.github.io/FlowGraph/Features/SaveGameSupport).

## 21 G08 — DlgSystem: the focused graph-dialogue candidate

Observed `master` at `d7052312`, 22 June 2026; [MIT license](https://raw.githubusercontent.com/NotYetGames/DlgSystem/d705231216bd64558d437b87a57928689b1e7fef/LICENSE.txt). The [README](https://github.com/NotYetGames/DlgSystem/blob/d705231216bd64558d437b87a57928689b1e7fef/README.md) lists UE 5.6, 5.7 and 5.8. The [descriptor](https://github.com/NotYetGames/DlgSystem/blob/d705231216bd64558d437b87a57928689b1e7fef/DlgSystem.uplugin) identifies 18.0.8, a runtime module and uncooked editor module; `ANY_VERSION` is a placeholder, not universal support evidence. GitHub source and paid binary/support distribution are distinct options.

**Fit and cost.** This is the first trial when the creator prefers authoring conversations inside Unreal. Its bounded dialogue responsibility is easier to place behind an adapter than a replacement campaign framework. Allow 6–10 hours for a working conversation and 12–28 for a narrow production integration. Those estimates do not include all UI polish, localization, voice recording or cinematic direction.

**Ownership.** Participants resolve through stable Borrowed Sun IDs. Conditions query the campaign; events submit requests. A dialogue graph may propose spending a Sunseed charge but cannot decrement a separate variable as its own authority. Decide whether interruption restarts at a safe line or restores exact progress, then test that documented behavior. Compare it with Inkpot only if authoring preference remains unresolved; choose one before writing significant content.

## 22 G09 — Inkpot: text-first narrative with an import-tool gate

Observed `release` at `6ae21952`, 2 October 2026; [MIT license](https://raw.githubusercontent.com/The-Chinese-Room/Inkpot/6ae21952f4510106c76b5d6b513a92e1873b2925/LICENSE). Version 1.41.21's README says UE 5.7 or later, explicitly records a 5.8 build update, and warns that HEAD contains work in progress. It implements the Ink runtime in C++ but still documents **.NET 5 for Inklecate source import**. [Pinned README](https://github.com/The-Chinese-Room/Inkpot/blob/6ae21952f4510106c76b5d6b513a92e1873b2925/README.md), [runtime/editor descriptor](https://github.com/The-Chinese-Room/Inkpot/blob/6ae21952f4510106c76b5d6b513a92e1873b2925/Inkpot.uplugin).

**Fit and cost.** Favor this trial if writing, reviewing and revising dialogue as text substantially improves the creator's workflow. Allow 6–12 hours for evaluation and 16–36 hours for a bounded integration. Resolve the compiler prerequisite deliberately: the UE toolchain's .NET installation does not establish that the documented importer works. This research neither installed a legacy runtime nor qualified an alternative compiler configuration.

**Shipping boundary.** C++ runtime modules do not imply a packaged game needs the old import runtime. Prove that by cooking imported content and launching without editor-only import tools. Story variables should hold narrative-local state; money, custody and mutually exclusive world consequences come from campaign queries. Lock stable story/choice identifiers and test a content revision against a saved conversation before committing to large-scale writing.

## 23 G10 — SimpleQuest and SimpleCore: substantial scope, substantial overlap

Observed `main` at `9536c7de`, 11 October 2026; [MIT license](https://raw.githubusercontent.com/TheGeebus/SimpleQuest/9536c7de39cf6c8484f7ee334a9194905989eae7/LICENSE). The README states a 5.6 target and maintainer-verified 5.7/5.8 compatibility. This is a two-plugin progression system: SimpleCore supplies coordination/facts; SimpleQuest supplies quest state and graph authoring. It describes generated tags, serializable snapshots and catch-up notifications. [Pinned README](https://github.com/TheGeebus/SimpleQuest/blob/9536c7de39cf6c8484f7ee334a9194905989eae7/README.md). Both descriptors are 0.9.0; ElectronicNodes is explicitly optional and disabled. [SimpleQuest descriptor](https://github.com/TheGeebus/SimpleQuest/blob/9536c7de39cf6c8484f7ee334a9194905989eae7/Plugins/SimpleQuest/SimpleQuest.uplugin).

**Fit and cost.** This is more than a quest-marker widget and deserves a real evaluation if extensive progression authoring becomes central. It also reaches directly into responsibilities already assigned to Borrowed Sun's campaign module. Defer until that tradeoff is worth 8–16 hours of testing and a potential 24–60 hour integration.

**Ownership gate.** Either keep plugin facts as a read model rebuilt from authoritative campaign events, or explicitly migrate a well-defined responsibility into the plugin. Never independently increment both stores. Generated tags are not automatically safe permanent save identities: pin naming conventions and prove rename/reimport migration. Treat snapshot and catch-up behavior as testable maintainer claims, particularly when a restored event could duplicate a reward. Avoid installing SimpleQuest and Flow simultaneously for the same first mission.

## 24 G11 — UnrealSharp: defer a language migration

Observed `main` at `51a1397a`, 11 October 2026; [MIT license](https://raw.githubusercontent.com/UnrealSharp/UnrealSharp/51a1397aaf56319f390d7431d23001af11dae0d2/LICENSE). Current README requirements are UE 5.6–5.8 and .NET 10.0.5 or later. [Pinned README](https://github.com/UnrealSharp/UnrealSharp/blob/51a1397aaf56319f390d7431d23001af11dae0d2/README.md). The [descriptor](https://github.com/UnrealSharp/UnrealSharp/blob/51a1397aaf56319f390d7431d23001af11dae0d2/UnrealSharp.uplugin) is marked beta and declares Windows/Mac support. It adds runtime bindings and editor generation. Current documentation gives a custom UAT `PackageProject` path for packaging. [Command-line packaging](https://www.unrealsharp.com/getting-started-and-fundamentals/packaging/packaging-via-ci-cd-command-line).

**Fit and cost.** C# and hot reload can be valuable, but the project already has a working native C++ toolchain and no demonstrated C# productivity advantage. AI assistance alone does not justify a second runtime boundary. Defer. A future 8–16 hour experiment should compare the same feature implemented in both languages; a 30–80 hour allowance is only a bounded migration/qualification range.

**Shipping boundary.** Test generated bindings, assembly staging, garbage collection boundaries, save reflection and the packaging command on a clean machine profile. Do not assume an editor hot-reload success proves a standalone game can launch. Do not silently replace the existing package scripts merely to accommodate a language experiment. A version string of 0.1 is insufficient for reproducibility; the exact source SHA is the pin.

## 25 G12 — MassSample: learning material for a measured crowd problem

Observed `main` at `ca982586`, 27 June 2026; [MIT license](https://raw.githubusercontent.com/Megafunk/MassSample/ca9825861f35ab4f8e152351de2adb893b51ca70/LICENSE). Its [project descriptor](https://github.com/Megafunk/MassSample/blob/ca9825861f35ab4f8e152351de2adb893b51ca70/MassSampleProject.uproject) says 5.8. The README retains older engine text and warns some examples are outdated; it also requires Git LFS. [Pinned README](https://github.com/Megafunk/MassSample/blob/ca9825861f35ab4f8e152351de2adb893b51ca70/README.md). The beta MassCommunitySample runtime enables MassGameplay, MassAI and MassCrowd. [Plugin descriptor](https://github.com/Megafunk/MassSample/blob/ca9825861f35ab4f8e152351de2adb893b51ca70/Plugins/MassCommunitySample/MassCommunitySample.uplugin).

**Fit and cost.** This is a community sample, not an official turnkey pedestrian population. Keep it as reference until profiling proves the intended crowd exceeds a simpler actor/schedule budget. Allow 12–24 hours for a controlled representation experiment and 30–80 for a bounded crowd prototype. Named NPC behavior, doors, avoidance, schedules and persistence remain design and integration work.

Epic's MassGameplay documentation describes representation choices including actors, instanced meshes and no representation. That is useful for crowd distance tiers, but no crowd count or performance gain is claimed here. [Epic MassGameplay overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-gameplay-in-unreal-engine). Keep persistent named-character IDs independent of transient entity handles; promote a visible proxy to an interactable actor through a tested adapter. Sector unload must cancel entities and their callbacks without deleting campaign records.

## 26 Contracts required before adopting any runtime framework

These are proposed Borrowed Sun integration rules, not claims that any repository implements them already.

1. **One campaign writer.** Dialogue, quest nodes and abilities call `SubmitCampaignRequest(CampaignId, RequestSequence, ExpectedRevision, Command)`. The campaign dispatcher owns validation, the bounded outcome journal, permanent reward keys and accepted revision. Plugins consume the accepted result. They do not mutate Sunseed custody, remaining charges or exclusive-choice outcomes independently.
2. **Stable content identity.** Store project-owned IDs for participants, objectives and story checkpoints. Maintain a versioned mapping to plugin graph/node paths. Moving a `.uasset`, renaming a generated tag or unloading an actor must not silently create a new persistent identity. A rename is a reviewed migration with a fixture from the previous content version.
3. **One checkpoint boundary.** Capture plugin narrative/progression state and campaign state for the same revision. Only the completed save revision is durable. A plugin callback cannot show a permanent-success message merely because an in-memory transition occurred. Restore the newest intact compatible generation and never mix old graph state with a newer reward ledger.
4. **Lifecycle cancellation.** Every latent graph node, ability and conversation owns cancellation/unsubscribe handles. Sector unload, pawn unpossession and world teardown terminate or safely suspend them. Late completions carry a request/scene generation and are ignored when stale. A restored notification updates presentation without replaying an irreversible action.
5. **Dependency containment.** Put adopted source plugins under project `Plugins`, keep original notices and record SHA, engine pin, local patch list and asset provenance. Project-owned adapter classes live outside vendor source. Upgrade one dependency per review, with rollback data and smoke/cook/package evidence. Do not upgrade the engine merely because a repository's HEAD moved.

## 27 Three bounded experiments

Run these only when their trigger appears. They are proposed future work; no plugin was installed for this research. Work in an isolated trial branch or throwaway project, retain a baseline trace/build, and merge only the smallest successful integration. With limited RAM and disk, run one Unreal editor/build at a time and avoid downloading every sample.

### A. Ground character and possession — 8 hours initial cap

**Trigger:** the stock ground controller is the largest visible weakness in a playable street mission. Trial ALS at the recorded SHA with its stock assets first. Build a short route containing stairs, slopes, a doorway, a low obstruction and the ship boarding point. Use the existing project's walking-to-flight input and custody boundary rather than inventing a parallel possession system.

**Pass:** editor and game targets compile on the engine pin; a cooked Windows test launches; keyboard/controller walking works; twenty board/unboard cycles produce no duplicate input, stale camera, stuck movement or unexpected actor collision. Save/load returns to a documented safe grounded state. Compare animation/game-thread cost against the same route with the baseline character, with the same resolution and NPC count. Record the difference rather than claiming a universal performance number.

**Fail or stop:** four hours disappear into unrelated engine/API porting, missing dependencies cannot be resolved within the existing budget, or stock assets work but the project character cannot pass a simple retarget demonstration. Preserve the findings and keep the baseline controller. Extension to sixteen hours requires a concrete remaining issue, not open-ended polishing.

### B. One consequential conversation — 10 hours initial cap

**Trigger:** the first mission needs branching dialogue that exceeds a small data-driven conversation. Choose DlgSystem for an Unreal graph workflow or Inkpot for a text workflow. Do not implement a full comparison unless the creator's authoring preference is still unclear; if needed, split twelve hours between equivalent six-hour prototypes.

Author one conversation that reads clinic status, offers two exclusive outcomes and requests a Sunseed action. The campaign, not the dialogue package, decides whether the action succeeds. Include unavailable-choice text and interruption. With Inkpot, prove source import before investing in the writing format; with either, record the exact asset/compiler/version inputs.

**Pass:** the same choice submitted twice changes state once; loading before and after the choice preserves the correct consequence; interrupt/restart follows the documented policy; leaving the sector during a response causes no late reward or stale actor access. Change a line of prose without breaking a saved campaign fixture. Package and launch the conversation with editor-only tooling absent. The creator must be able to add another branch without changing C++.

**Fail or stop:** dialogue must become a second campaign authority, an importer dependency remains unexplained, saving requires live actor pointers, or a clean package cannot run. Retain a small owned conversation data format until those specific problems are resolved.

### C. One mission graph — 12 hours, after the conversation loop works

**Trigger:** two completed missions repeat enough orchestration that handwritten sequencing is slower or harder to inspect. Trial FlowGraph **v2.3-5.8** at the recorded release SHA. Do not use default 5.x HEAD. SimpleQuest is an alternative later experiment if progression data/text import, rather than sequence visibility, is the actual bottleneck; it is not added alongside Flow for this test.

Implement six operations: query facts, start dialogue, submit action, await accepted result, wait for destination readiness, present completion. Each custom node delegates to an existing project service. Store graph checkpoint data inside the project's versioned save container. Test load into an already running world as well as startup loading.

**Pass:** identical replayed requests never duplicate consequences; cancellation during streaming leaves no graph-owned actor references; graph state and campaign revision restore together; one content rename has an explicit migration or a safe checkpoint fallback; cooked execution matches PIE. Measure the time to author a second small variation. Adoption requires a clear authoring improvement and zero new owners of campaign truth.

**Fail or stop:** graph integration demands replacing working travel/save services, upgrading UE, or maintaining two quest fact stores. Retain the existing sequencer and document the rejected boundary. A compelling editor screenshot is not the success criterion.

## 28 What stays out of the first installation list

Do not combine Lyra, ALS and a second full movement framework; do not combine Epic GAS and SimpleGAS as competing ability authorities; do not install DlgSystem, Inkpot, Flow and SimpleQuest all at once. These tools can coexist in larger projects, but the integration case must be explicit.

UnrealSharp waits for a demonstrated language-workflow benefit. Mass waits for a recorded crowd bottleneck at the intended scene density. Multiplayer infrastructure waits for a multiplayer requirement. Large sample downloads wait for a specific extraction question and sufficient disk headroom. None of those decisions prevents shipping the first Borrowed Sun mission; each protects time for playable movement, clear choices, persistence and a reliable packaged build.


## 29 Repository sprint: which tools improve this Unreal workflow?

Keep the working PowerShell build and test entrypoints. The best additions found in this sprint are a small, optional log-analysis experiment with OpenUnrealAutomationTools, selected naming guidance, and deliberate verification of Git LFS and ignore rules. None of the inspected projects justifies replacing the already-tested UE 5.8.2 foundation simply to acquire a larger framework. Container builders are credible future infrastructure, but the current laptop, under-€500 initial budget and solo production schedule make their setup and maintenance a poor immediate exchange.

This assessment covers eleven canonical repositories and one supporting documentation hub. Default-branch HEAD revisions and UTC commit dates came from GitHub's public commits API on 12 October 2026; files were inspected at those exact revisions. Allar's separate `v2` branch was also checked because its default branch explicitly redirects readers there. Hazelight's engine fork is the exception: its unauthenticated API returned 404, while its official documentation explains the source-access requirement. Its SHA, date and pinned license file remain unverified. No repository installer, plugin, container build, package install or downloaded script was executed. No paid service or new remote was created.

`Docs/Registers/repository-candidates.json` is the machine-readable evidence register. It records complete hashes, commit links, exact pinned license paths, compatibility evidence, limitations and bounded investigation plans. A recent commit is evidence of activity, not support quality or a successful project build. A minimum engine-version statement is not a test of 5.8.2. A GitHub repository's overall update date does not establish when an individual Unreal template changed. Archive flags were not independently queried; the maintenance judgments below use inspected activity and explicit project statements rather than inventing an archive status.

All effort numbers are planning allowances for the creator, not vendor estimates, promised savings or work already done. An adoption allowance is conditional on a successful experiment. A null estimate in the register means a migration is too uncertain to responsibly price from a README. The recommended immediate research-to-action work is approximately **5–8 focused hours total**, spread across existing tool-maintenance time: 3–4 hours for optional log analysis, 1–2 for LFS qualification, and 1–2 combined for naming/ignore review. These are alternative priorities inside the production schedule, not extra hours on top of its twenty focused hours each week.

## 30 The comparison rule: retain the guarantees we have already earned

The current workspace's scripts compile editor and game separately, require the reviewed engine version and changelist, retain native failures, bound timeout cleanup, require a valid cooked default map, and parse a fresh report containing all five expected successful smoke tests. Their tooling regression suite passed 27 checks under Windows PowerShell 5.1 and PowerShell 7.6.5. Separate installed-engine build and smoke-test evidence was produced during workspace validation. This is a modest but concrete baseline; it does not prove the future game or replace a real packaged playtest.

Any proposed replacement must preserve those properties and demonstrate a measurable gain. A useful extension can consume existing logs or reports without acquiring authority to select an engine, modify source assets, install dependencies, publish builds or decide a release passed. Keep one authoritative build configuration. If an external library saves twenty lines of invocation code but adds a parallel environment file, stale report behavior and another dependency chain, the project is worse off.

For AI-assisted work, prefer deterministic commands with explicit outputs over allowing the model to assemble a new shell pipeline for every task. The agent should call the project's reviewed entrypoints, inspect their evidence, and report exactly which checks ran. A model may propose a workflow improvement; it should not silently swap test filters, disable rendering, patch asset version headers, relax the engine lock or turn a timeout into a warning. Repository instructions and generated scaffolding remain reviewable source, not a new authority above the project contract.

## 31 DEV01 — OpenUnrealAutomationTools: strongest complementary candidate

Canonical repository: [JonasReich/OpenUnrealAutomationTools](https://github.com/JonasReich/OpenUnrealAutomationTools). Inspected HEAD **254b040fffc06e239ea73807c5a1e1e564975419**, committed **2026-10-07**. Its [pinned license](https://github.com/JonasReich/OpenUnrealAutomationTools/blob/254b040fffc06e239ea73807c5a1e1e564975419/LICENSE.md) is MIT.

The project provides Python automation utilities, including build/test handling and log parsing. Its README names several dependencies and notes that earlier PowerShell utilities were removed in January 2026. It describes use alongside proprietary studio tooling, supports Git and Perforce to varying degrees, and says contributions are not currently open. That is useful context, not evidence of a supported drop-in product for this project. The inspected README contains no exact 5.8.2 qualification. [Pinned README](https://github.com/JonasReich/OpenUnrealAutomationTools/blob/254b040fffc06e239ea73807c5a1e1e564975419/README.md)

Source inspection explains why the narrow adoption boundary matters. Its automation helper can reuse a previous report under an optional setting, adds NullRHI, constructs its own test command, and returns the engine exit code. It also contains optional report-viewer dependency installation. Those behaviors are not inherently wrong for its original environment, but they differ from our fresh-report, expected-name and explicit-rendering contract. The current HEAD commit also adds an asset engine-version-header patching script. Changing a package header cannot establish that its contents are compatible with an older or different engine. [Pinned automation helper](https://github.com/JonasReich/OpenUnrealAutomationTools/blob/254b040fffc06e239ea73807c5a1e1e564975419/python/openunrealautomation/automationtest.py) [Inspected commit](https://github.com/JonasReich/OpenUnrealAutomationTools/commit/254b040fffc06e239ea73807c5a1e1e564975419)

**Decision: try log analysis only.** Its potential benefit is turning a long failed cook log into a short list of actionable asset or compiler problems. In a 3–4 hour experiment, feed a pinned, isolated copy three sanitized logs: a successful build, a deliberately failed Blueprint compile and a missing cook-package case. Require each known failure to be found, ordinary warnings to remain distinguishable from fatal errors, and each summary to point back to its original line. Verify malformed input fails visibly. Do not run the asset patcher, hosted-service integrations or its test launcher.

Adopt an optional report generator only if it reduces diagnosis time without changing acceptance decisions. Allow another 4–8 hours for dependency pinning, notices, fixtures and documentation if the experiment wins. Keep the original logs; a parser's interpretation is supplementary evidence. If the logs remain easy to inspect with a few searches, stop after the experiment and retain no dependency.

## 32 DEV02 — ue4cli: useful portability, little immediate Windows benefit

Canonical repository: [adamrehn/ue4cli](https://github.com/adamrehn/ue4cli). Inspected HEAD **fed71c1af4cffe3fed9358b08430068ad9454b77**, committed **2024-03-13**. [Pinned license: MIT](https://github.com/adamrehn/ue4cli/blob/fed71c1af4cffe3fed9358b08430068ad9454b77/LICENSE).

The package supplies a `ue4` command that abstracts engine batch/shell entrypoints, generates project files and helps query compiler flags for external libraries. Its README specifies Unreal 4.19 or newer and Python 3.5 or newer. That minimum is broad historical documentation, not a verified promise for our exact engine, installed compiler and plugins. The inspected default branch has no newer commit than March 2024. [Pinned README](https://github.com/adamrehn/ue4cli/blob/fed71c1af4cffe3fed9358b08430068ad9454b77/README.md)

**Decision: defer until a second host or platform creates a real need.** The local Windows scripts already know the engine and project paths, invoke the required targets, and create our evidence records. Replacing them now mostly changes the command vocabulary. A CLI abstraction becomes more valuable if Linux development, native third-party libraries or several engine installations become recurring work.

The future 1–2 hour experiment should compare the wrapper's selected engine, target and arguments against the existing process on a harmless project. Do not run a migration simply because discovery is convenient. Adoption requires preserving exact engine locking, map prerequisites, timeouts and strict automation results, with an estimated 4–8 additional hours only if those needs exist. Keep it behind the project's entrypoint rather than making every task depend directly on a second interface.

## 33 DEV03 — ue4-docker: maintained, but premature infrastructure

Canonical repository: [adamrehn/ue4-docker](https://github.com/adamrehn/ue4-docker). Inspected HEAD **7f1eccc538b39097f1f196609642cf640d87226e**, committed **2026-08-17**, matching release **0.0.124**. [Pinned license: MIT](https://github.com/adamrehn/ue4-docker/blob/7f1eccc538b39097f1f196609642cf640d87226e/LICENSE).

This is genuine engine-container build infrastructure, with Windows/Linux paths and build, package and automation use cases. It is not obsolete merely because its name says UE4. The current release explicitly addresses a Linux UBA file-mapping problem introduced in UE 5.8.1 that could produce extremely large decompressed layers. That is concrete contemporary engine work; it is still not a successful 5.8.2 Borrowed Sun build. [Pinned README](https://github.com/adamrehn/ue4-docker/blob/7f1eccc538b39097f1f196609642cf640d87226e/README.md) [Release 0.0.124](https://github.com/adamrehn/ue4-docker/releases/tag/v0.0.124)

**Decision: defer to a funded dedicated builder.** The economic question is whether rebuilding and distributing controlled environments saves more time than maintaining engine images, caches, access credentials and storage. On a solo Windows laptop with constrained RAM and free disk, an already-working Launcher installation is the cheaper current environment. Containerization does not remove the engine's compiler, disk or asset costs; it adds image construction and retention to them.

Before an actual build, spend at most 4–6 hours on a design-only qualification: select host/target, exact engine source, required plugins, image rights, cache location, credential mechanism, storage budget and deletion policy. An actual future pilot is tentatively 16–40 focused hours once appropriate hardware exists; this allowance excludes engine download/compile waiting time and any plugin port. Pass only after a clean build, tests, package launch, repeat build and review of where credentials and licensed files remain.

The [Unreal Containers CI guide](https://unrealcontainers.com/docs/use-cases/continuous-integration) is a supporting reference, not a second runnable framework to install. It correctly emphasizes learning host command-line packaging before containers. Some examples retain UE4-era commands and its text explicitly qualifies its Gauntlet experience. Use it to understand boundaries, then verify against current engine behavior. Its site-content license is CC BY 4.0; do not confuse that documentation license with the licenses of engine images or included plugins.

## 34 DEV04 — Allar's style guide: adopt a few conventions, not an implied certification

Canonical repository: [Allar/ue5-style-guide](https://github.com/Allar/ue5-style-guide). Default HEAD **198a8550947de91f19362dabf57fbd1f1cdf0904**, committed **2022-03-07**. The inspected `v2` HEAD is **2127e9ab3ef740d5df3e2546205f2fcce7bc430e**, also **2022-03-07**. Both pinned license files are MIT. [Main license](https://github.com/Allar/ue5-style-guide/blob/198a8550947de91f19362dabf57fbd1f1cdf0904/LICENSE) [v2 license](https://github.com/Allar/ue5-style-guide/blob/2127e9ab3ef740d5df3e2546205f2fcce7bc430e/LICENSE)

The repository name is easy to overread. The default README says the document is currently for UE4 and points to `v2`; the inspected `v2` document calls itself a draft and retains UE4 wording. It is a community convention reference, not an executable scaffold, current Epic standard or UE 5.8.2 compatibility matrix. [Default README](https://github.com/Allar/ue5-style-guide/blob/198a8550947de91f19362dabf57fbd1f1cdf0904/README.md) [v2 draft](https://github.com/Allar/ue5-style-guide/blob/2127e9ab3ef740d5df3e2546205f2fcce7bc430e/README.md)

**Decision: adopt only rules that reduce ambiguity in this project.** Spend 30–60 minutes comparing folder boundaries, asset prefixes and Blueprint readability rules against the conventions already in AGENTS and the architecture documentation. Produce a one-page decision, not an 80,000-character prompt pasted into every AI task. New generated classes and assets should follow the chosen naming rules. Existing working assets should not be bulk-renamed for cosmetic agreement.

Allow 1–2 hours to integrate the selected policy and examples. A separate linter plugin is a separate dependency and must be evaluated separately; the guide itself does not enforce correctness. The practical acceptance test is that two independently written task briefs produce the same intended asset path and class naming pattern, while exceptions such as purchased content are explicit.

## 35 DEV05 — UnrealEnginePython: a legacy plugin, not the current editor Python API

Canonical repository: [20tab/UnrealEnginePython](https://github.com/20tab/UnrealEnginePython). Inspected HEAD **4b5da5bf4ca598e8d0f67eb938749bc381a67d6c**, committed **2020-04-17**. [Pinned license: MIT](https://github.com/20tab/UnrealEnginePython/blob/4b5da5bf4ca598e8d0f67eb938749bc381a67d6c/LICENSE).

This project embeds Python into editor and runtime workflows. Its README identifies 4.23 as its latest supported version, discusses 4.24 forks and 4.25 reflection changes, and seeks maintainers. It explicitly distinguishes itself from Epic's Python plugin. None of that establishes a current 5.8 port. [Pinned README](https://github.com/20tab/UnrealEnginePython/blob/4b5da5bf4ca598e8d0f67eb938749bc381a67d6c/README.md)

**Decision: reject for this baseline.** Do not use it to promise that AI can implement the game in Python without C++ or Blueprints. Its runtime VM, reflection integration and packaging responsibilities would become project maintenance work. MIT availability does not make the port small or its language/runtime dependencies disappear.

No implementation spike is justified. Historical examples may help formulate an editor automation task, but implement that task through the current approved engine facilities and validate it on the pinned engine. Keep gameplay in the chosen architecture. If the project later genuinely needs a runtime scripting language, evaluate that as an architecture decision with save compatibility, debugging, packaging and support requirements, not as a convenience installation.

## 36 DEV06 — Git LFS: retain the official infrastructure, verify the actual setup

Canonical repository: [git-lfs/git-lfs](https://github.com/git-lfs/git-lfs). Inspected HEAD **0043a645047926f4bd7f7091299095528253d575**, committed **2026-09-22**. The [pinned LICENSE.md](https://github.com/git-lfs/git-lfs/blob/0043a645047926f4bd7f7091299095528253d575/LICENSE.md) contains MIT terms plus notices for Go-derived BSD-licensed portions and references other dependency licenses. Calling every bundled component simply MIT would lose that detail.

Git LFS is engine-neutral source-control infrastructure. Its upstream development HEAD is useful evidence, not an instruction to build or install an unreleased version. Use a supported official release already available in the development environment and record its actual version. LFS payloads, locks and server quotas still require operational verification; a successful source-code commit does not prove that a map can be materialized from an independent checkout. [Pinned README](https://github.com/git-lfs/git-lfs/blob/0043a645047926f4bd7f7091299095528253d575/README.md)

**Decision: keep and qualify.** Spend 1–2 hours on a harmless binary asset: confirm the intended attributes, inspect the pointer stored by Git, verify the working bytes and confirm the backup contains usable content. Once a private remote has been explicitly configured, test actual lock support and conflict behavior with a controlled fixture before a collaborator depends on it. A no-remote solo project has no remote lock service.

These checks complement the existing scripts instead of replacing them. Do not introduce a new hosting subscription or migrate historical assets merely to perform the research. LFS storage and download charges belong to the chosen host, separate from the open-source client license. If the tests uncover a gap, correct the smallest configuration issue and preserve a restore test as evidence.

## 37 DEV07 — GitHub's gitignore templates: compare carefully

Canonical repository: [github/gitignore](https://github.com/github/gitignore). Inspected HEAD **0e5d690153ca3da8a4a1aef2d053406f408f531c**, committed **2026-10-02**. [Pinned license: CC0 1.0 Universal](https://github.com/github/gitignore/blob/0e5d690153ca3da8a4a1aef2d053406f408f531c/LICENSE).

The pinned Unreal template contains broad exclusions for binaries, Build content and built map data. It also excludes certain SourceArt images. Those rules are context-dependent: platform resources, a binary-only plugin and deliberately retained baked content may require different treatment. The template is a reference for generated files, not a source-of-truth definition of what this game's source is. Its repository HEAD date does not tell us the template was updated that day. [Pinned UnrealEngine.gitignore](https://github.com/github/gitignore/blob/0e5d690153ca3da8a4a1aef2d053406f408f531c/UnrealEngine.gitignore)

**Decision: retain our scoped ignore file and audit against this reference.** Spend 30–60 minutes checking representative authored configuration, a saved map, external actor assets, a platform icon/resource, generated compiler output, an ignored local environment file and a binary-only plugin fixture with `git check-ignore`. Make expectations explicit before changing a rule. The project's nested `Game/` layout must be accounted for.

The success criterion is that necessary authored or licensed dependencies cannot silently vanish from the backup/repository selection while regenerated output stays out. There is no need to replace a working ignore file wholesale or add another runtime component. Revisit exclusions when adopting a dependency that is only distributed as binaries.

## 38 DEV08 — Game-CI unity-builder: an engine mismatch

Canonical repository: [game-ci/unity-builder](https://github.com/game-ci/unity-builder). Inspected HEAD **ae0171202cd8b5ea3fc3f87b03bb6e676e4ea17e**, committed **2026-09-16**. [Pinned license: MIT](https://github.com/game-ci/unity-builder/blob/ae0171202cd8b5ea3fc3f87b03bb6e676e4ea17e/LICENSE).

The project is a Unity build action. Its description and `unityVersion` input make that scope explicit. Recent maintenance and broad recognition under a game-development brand do not establish Unreal support. [Pinned README](https://github.com/game-ci/unity-builder/blob/ae0171202cd8b5ea3fc3f87b03bb6e676e4ea17e/README.md) [Pinned action definition](https://github.com/game-ci/unity-builder/blob/ae0171202cd8b5ea3fc3f87b03bb6e676e4ea17e/action.yml)

**Decision: do not integrate.** It is included because generic searches frequently surface it as a game CI solution. A useful comparison stops once the engine contract fails; no installation experiment is warranted. General ideas such as separating build outputs from source, bounding jobs and exposing results remain transferable, but this action does not replace UBT/UAT or validate our game. Keep the existing disabled, owner-reviewed Windows workflow template.

## 39 DEV09 — Daedalic test automation: design reference, uncertain port

Canonical repository: [DaedalicEntertainment/ue4-test-automation](https://github.com/DaedalicEntertainment/ue4-test-automation). Inspected HEAD **d8bfd91fdd448860b596753339c7ce9f1f503438**, committed **2022-05-15**. [Pinned license: MIT](https://github.com/DaedalicEntertainment/ue4-test-automation/blob/d8bfd91fdd448860b596753339c7ce9f1f503438/LICENSE).

The project supplies Blueprint integration-test patterns and Gauntlet extensions. Its README lists engine support from 4.23 through 4.27; the inspected plugin descriptor declares EngineVersion 4.23.0. Its documentation also contains a timeout behavior that can proceed to assertions with warnings. These are specific integration choices, not a drop-in match for strict current tests. [Pinned README](https://github.com/DaedalicEntertainment/ue4-test-automation/blob/d8bfd91fdd448860b596753339c7ce9f1f503438/README.md) [Pinned descriptor](https://github.com/DaedalicEntertainment/ue4-test-automation/blob/d8bfd91fdd448860b596753339c7ce9f1f503438/DaedalicTestAutomationPlugin/DaedalicTestAutomationPlugin.uplugin)

**Decision: read patterns; implement the first functional test with current built-in facilities.** A 1–2 hour study can produce a test design for boarding, travel and return: arrange the world, perform the input, wait for a bounded condition and assert the preserved identity/state. A timeout must fail. The test should emit an exact name and reproducible report. This is useful when a playable map exists; adding the old plugin before that solves no current problem.

Do not estimate a 5.8 migration from its installation instructions alone. First establish a concrete missing capability that built-in automation and Functional Testing cannot reasonably satisfy. The old README's source-engine requirement belongs to that integration and time period; it must not be generalized into a universal claim about today's Gauntlet.

## 40 DEV10 — Hazelight AngelScript: an engine-strategy change

Canonical repository: [Hazelight/UnrealEngine-Angelscript](https://github.com/Hazelight/UnrealEngine-Angelscript). **HEAD, commit date and pinned LICENSE.md were inaccessible in this unauthenticated research.** Do not substitute an unrelated public fork or invent a SHA to fill the register.

The official installation guide requires changes to Unreal engine code and says additional plugins must be built from source; prebuilt binary plugins are not compatible with the fork. Its licensing page distinguishes proprietary Unreal engine code, the zlib-licensed AngelScript library and MIT-licensed integration plugin code. Its development-status page describes shipped studio use but recommends engine-programmer availability and makes no general support guarantee. These are meaningful primary statements, yet none certifies a particular 5.8.2 branch for this workspace. [Installation](https://angelscript.hazelight.se/getting-started/installation/) [Licensing](https://angelscript.hazelight.se/project/license/) [Development status](https://angelscript.hazelight.se/project/development-status/)

**Decision: defer outside the present architecture.** Faster gameplay iteration could be valuable, but the cost is a source-engine strategy, plugin rebuild requirements and another runtime language. It conflicts with the current Launcher-engine/Cargo qualification path. This is not equivalent to adding a text editor extension or code-generation template.

No current migration spike is proposed. A future staffed review must first obtain authorized source access, inspect the actual branch/license files, inventory plugin compatibility, measure engine build/upgrade costs and define how scripts interact with saves and packaged builds. Only then can an honest cost estimate be made. The official project's studio success is a reason to take it seriously, not proof that it reduces total work for one creator on this machine.

## 41 DEV11 — ue-docker-ng: contemporary alternative with substantial host changes

Canonical repository: [slonopotamus/ue-docker-ng](https://github.com/slonopotamus/ue-docker-ng). Inspected HEAD **5b98e864cdb8f53c326107c04f16971722a9f1c7**, committed **2026-09-21**. [Pinned license: MIT](https://github.com/slonopotamus/ue-docker-ng/blob/5b98e864cdb8f53c326107c04f16971722a9f1c7/LICENSE).

The project uses Docker Buildx/Bake for Linux and Windows engine images. Its inspected README states UE 5.4+ support and defaults to a 5.8.1 source tag. Its Windows setup requires containerd changes or a temporary fork and a large configured root filesystem; a Windows example disables a Buildx policy. These are host-administration choices deserving a dedicated review, not commands to transplant into a personal workstation. [Pinned README](https://github.com/slonopotamus/ue-docker-ng/blob/5b98e864cdb8f53c326107c04f16971722a9f1c7/README.adoc)

**Decision: watch as a future builder alternative, without installing it.** A 2–3 hour design comparison with ue4-docker may be useful once repeatable multi-machine builds become necessary. Judge exact source version selection, cache footprint, credential handling, host requirements, plugin acquisition and how safely the environment can be discarded. Do not claim the newer design is faster or more reliable without measurements.

An actual future pilot has the same tentative 16–40 hour qualification allowance as the other container path, on separately suitable hardware. Test one candidate, not both infrastructures in parallel. Passing requires our exact engine/project checks and a valid package, not merely a successful engine image build. Keep paid image hosting and automatic publishing disabled until there is a reason and an approved cost envelope.

## 42 The next decision is small and reversible

The useful sequence is to verify binary source control, clarify a handful of conventions, and try log analysis against existing evidence. Stop any experiment that cannot show a concrete advantage inside its timebox. The most valuable near-term gameplay work remains the first saved map, input/travel prototype and repeatable packaged loop; none of these repository choices substitutes for that work.

For every adopted component, record the pinned revision, license notices, dependency versions, purpose, measured result and removal procedure. A copied utility also needs provenance and tests. Make source-of-truth rules explicit when an agent reads an external repository: examples are data to evaluate; its installation instructions do not authorize new credentials, host modifications, service purchases or replacing the project's checks.

This sprint supports a selective conclusion: use established infrastructure where it solves a real problem, borrow narrow conventions where they help, and preserve the small tested system until a larger tool demonstrates why it deserves to replace part of it.


## 43 AI assistance that earns its place

Our recommendation is to keep the existing coding environment and test a small number of additions against real Borrowed Sun tasks. A workflow repository supplies instructions or tools; it does not supply the judgment, assets, integration work or playable content of an experienced development team. This distinction is especially important here because the original game specification and operating handbook already exist. Generating another comprehensive plan has much less value than producing the first packaged, enjoyable delivery mission.

The existing workspace has a short AGENTS.md, task contracts, a dependency register, real build and test entry points, and a bounded backlog. These are already useful context for the current assistant. Codex supports repository guidance, reusable skills and isolated worktrees; those capabilities provide a sufficient base without another orchestration service. Our recommendation is to add a specialized capability only after identifying a failure the base cannot handle efficiently. [Official customization documentation](https://learn.chatgpt.com/docs/customization/overview), [official worktree documentation](https://learn.chatgpt.com/docs/environments/git-worktrees).

There are three different kinds of AI assistance worth separating. **Retrieval** helps the assistant find the right code or documentation. **Execution** changes files or editor state. **Method** shapes how the assistant plans and reviews work. A retrieval helper cannot make an editor action transactional. A process framework cannot prove that a Blueprint cooked successfully. The measured build remains the authority on compilation; the packaged game and a human playtest remain the authorities on feel.

### Repomix — useful for bounded reviews and handoffs

**Observed:** 2026-10-11 commit [`55c6de6aebde`](https://github.com/yamadashy/repomix/commit/55c6de6aebde4c38877fbcaf005c4cc396bf6e2d). **License:** [MIT](https://github.com/yamadashy/repomix/blob/55c6de6aebde4c38877fbcaf005c4cc396bf6e2d/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Repomix assembles selected repository content into a single model-readable document. It supports include/exclude rules and token counting, and the current package declares Node 22 or newer. Its credential checks can flag recognized secrets, while remote repository configuration is not trusted by default. The MIT license covers the tool; it does not grant rights to share everything the tool can collect. [Pinned README](https://github.com/yamadashy/repomix/blob/55c6de6aebde4c38877fbcaf005c4cc396bf6e2d/README.md), [package metadata](https://github.com/yamadashy/repomix/blob/55c6de6aebde4c38877fbcaf005c4cc396bf6e2d/package.json).

**Decision: optional trial, 1–2 focused hours.** It is attractive for an architectural review by an assistant that cannot inspect the local repository, or for handing a deliberately small code sample to another service. It is unnecessary for this assistant's ordinary local edits, where direct searches and targeted reads avoid extra copies. Export only the source files, project-owned schemas and task acceptance criteria needed for the question. Exclude vendor assets, engine source, binary assets, private receipts, generated directories, logs with account data, and local configuration. Review the actual output before sharing it. A secret scanner cannot determine whether an asset license permits AI input.

The trial question is specific: can a reviewer correctly trace stable identity and sector-motion behavior from a packet smaller than 15,000 tokens? Record time to prepare the packet, missing facts the reviewer requests and errors in its conclusions. Keep it if preparation becomes a repeatable five-minute operation and the review is as accurate as local browsing. Do not export the entire Unreal engine or all research papers into every task.

### Serena — promising semantic navigation, with a real setup cost

**Observed:** 2026-10-12 commit [`0d4258ea7ad0`](https://github.com/oraios/serena/commit/0d4258ea7ad0000fd37882e3be5dc1ab2965700c). **License:** [GPL-3.0-or-later application; MIT SolidLSP](https://github.com/oraios/serena/blob/0d4258ea7ad0000fd37882e3be5dc1ab2965700c/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Serena adds symbol-oriented retrieval and editing using language servers. Its current Unreal setup guide describes a built project, generated headers and a usable compile_commands.json as prerequisites. The current application is GPL-3.0-or-later; SolidLSP is separately MIT. Its license file identifies v1.7.0 as the last MIT application release. The inspected pyproject labels this HEAD 2.0.0.dev0, so recent activity must not be confused with a stable release. [Pinned README](https://github.com/oraios/serena/blob/0d4258ea7ad0000fd37882e3be5dc1ab2965700c/README.md), [package metadata](https://github.com/oraios/serena/blob/0d4258ea7ad0000fd37882e3be5dc1ab2965700c/pyproject.toml). [Pinned Unreal setup guide](https://github.com/oraios/serena/blob/0d4258ea7ad0000fd37882e3be5dc1ab2965700c/docs/03-special-guides/unreal_engine_setup_guide_for_serena.md).

**Decision: defer until the native codebase is harder to navigate, then a 3–6-hour isolated trial.** Our current two-module foundation is too small to justify another indexing process. On the existing 16 GB machine, measure memory and indexing time before assuming a speed improvement. Use the language-server path for the experiment; the current README explicitly excludes Rider and CLion from its separate JetBrains plugin support. Do not purchase that plugin on the assumption that it accelerates Unreal work in Rider.

The trial should build first, generate a valid compilation database for this engine/toolchain, and ask for the definition and all project references of three known symbols. Include a reflected property, an interface call and a C++ method used by tests. Check results against direct searches and the editor. A C++ reference search does not establish the absence of Blueprint asset references. Renaming a UPROPERTY also needs Unreal-specific compatibility and asset checks, regardless of whether a language server reports success. Test an ordinary internal C++ symbol before any reflected API. Keep this as an external development tool; review the exact distribution and dependency terms before redistributing it or incorporating its code into another product.

### Context7 — a documentation experiment, not a compatibility authority

**Observed:** 2026-10-09 commit [`522c4db4fa2e`](https://github.com/upstash/context7/commit/522c4db4fa2e1e31f1b320f09fa8c8fab376987a). **License:** [MIT](https://github.com/upstash/context7/blob/522c4db4fa2e1e31f1b320f09fa8c8fab376987a/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Context7 offers library identification and documentation queries through CLI and MCP paths. Its repository publishes the MCP component under MIT, while its README says the supporting backend, parser and crawler are private. The current documentation explains authenticated setup and recommends version-specific questions. Those statements do not establish complete, authoritative Unreal 5.8 coverage. [Pinned README](https://github.com/upstash/context7/blob/522c4db4fa2e1e31f1b320f09fa8c8fab376987a/README.md), [package metadata](https://github.com/upstash/context7/blob/522c4db4fa2e1e31f1b320f09fa8c8fab376987a/package.json).

**Decision: optional 1–2-hour documentation benchmark, after a concrete retrieval need arises.** We already have official Epic documentation, local headers and web research. Add this only if it produces correct, version-matched results faster. Do not make it a mandatory dependency of every task, and do not assume its open client license means unlimited hosted access or a self-hostable equivalent of the whole service.

Benchmark ten actual questions: Enhanced Input initialization, subsystem lifetime, a save-version migration, a soft-reference load, a World Partition constraint, automation report behavior, PCG availability, asset rename semantics, Build.cs module visibility, and a Windows packaging flag. For each answer require the underlying primary URL and check against UE 5.8 documentation or the installed source. At least nine should be correct, with zero invented APIs in the critical save/build questions. Save query text and evidence, not a stream of private source code. If engine-version resolution is ambiguous, keep direct Epic/local lookup as the default. Pricing and account entitlements are unqualified here; this sprint creates no account or subscription.

### GitHub Spec Kit — strong structure, selective value after our current planning work

**Observed:** 2026-10-10 commit [`0443760542e7`](https://github.com/github/spec-kit/commit/0443760542e7e5269724744a2ac55c4bd386d095). **License:** [MIT](https://github.com/github/spec-kit/blob/0443760542e7e5269724744a2ac55c4bd386d095/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Spec Kit supplies a specification-led workflow and tooling for supported coding-agent integrations. Its current README requires Python 3.11+, uv and a supported agent, with Windows included. The code is MIT licensed. It is a development process toolkit, not an Unreal game framework. [Pinned README](https://github.com/github/spec-kit/blob/0443760542e7e5269724744a2ac55c4bd386d095/README.md), [package metadata](https://github.com/github/spec-kit/blob/0443760542e7e5269724744a2ac55c4bd386d095/pyproject.toml).

**Decision: reference now; a 2–3-hour comparison later only if tasks repeatedly arrive underspecified.** Our task template already records a player outcome, acceptance criteria, ownership, dependencies and evidence. Do not create parallel specifications in a new directory while continuing to update the existing backlog independently. If the toolkit wins a trial, choose one authoritative task record and explicitly map its artifacts to that record.

Use one difficult but bounded change for comparison, such as transactional cargo transfer between the courier and Kestrel. Time the planning phase and count unresolved questions discovered during implementation. Keep the richer process if it finds a consequential failure mode that our current template misses and does not materially delay ordinary work. Do not use it to create a forty-step planning sequence for changing one camera setting. Before running an initializer in a populated repository, inspect its generated-file plan in a throwaway directory; initialization is a repository mutation, not research.

### Superpowers — useful engineering practices, avoid competing instruction layers

**Observed:** 2026-10-10 commit [`bb92a7774141`](https://github.com/obra/superpowers/commit/bb92a77741419a4ab5f06e711a283343f1ada0c3). **License:** [MIT](https://github.com/obra/superpowers/blob/bb92a77741419a4ab5f06e711a283343f1ada0c3/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Superpowers packages a software-development method into automatically selected skills. Its README describes brainstorming, planning, testing, review and worktree workflows. The repository is MIT licensed, and the inspected HEAD is its v7.0.0 release commit. Its prescribed workflow is broader than an isolated tool and can change how routine tasks are handled. [Pinned README](https://github.com/obra/superpowers/blob/bb92a77741419a4ab5f06e711a283343f1ada0c3/README.md), [package metadata](https://github.com/obra/superpowers/blob/bb92a77741419a4ab5f06e711a283343f1ada0c3/package.json).

**Decision: reference selected practices; no wholesale activation in this workspace.** Systematic debugging, checking an actual failing reproduction, and reviewing consequential changes all fit Borrowed Sun. Mandatory ceremony for every edit does not. The user has explicitly asked for speed and dislikes unnecessary confirmation; the working contract should preserve that. A generic always-test-first rule also needs judgment for lighting, composition, animation feel and other work where the useful check is a playtest or screenshot rather than a unit test.

A 1–2-hour trial could compare its debugging method with our current approach on a seeded, reversible defect in the test harness. Measure time to identify the root cause and whether the resulting check fails on the original defect. Do not stack its planning and completion hooks with Spec Kit and BMAD simultaneously. Installed workflow instructions must be audited as operational dependencies: who triggers them, which files they write, which commands they execute and how they are removed. Their popularity is not evidence that their defaults fit this project.

### BMAD Method — mature breadth, a changing current branch

**Observed:** 2026-10-11 commit [`9f1987169a2e`](https://github.com/bmad-code-org/BMAD-METHOD/commit/9f1987169a2e5626e5ccb31bb9cee051fdb3df20). **License:** [MIT](https://github.com/bmad-code-org/BMAD-METHOD/blob/9f1987169a2e5626e5ccb31bb9cee051fdb3df20/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

The inspected BMAD README distinguishes stable v6.12 from a v7 preview-alpha branch and warns that the preview changes frequently. It describes modules for delivery, creative work, testing and games. The repository is MIT licensed; it is a collection of development workflows rather than a playable Unreal project. [Pinned README](https://github.com/bmad-code-org/BMAD-METHOD/blob/9f1987169a2e5626e5ccb31bb9cee051fdb3df20/README.md), [package metadata](https://github.com/bmad-code-org/BMAD-METHOD/blob/9f1987169a2e5626e5ccb31bb9cee051fdb3df20/pyproject.toml).

**Decision: defer full installation; reference the method when a specific workflow gap appears.** A default-branch SHA records what was inspected, not what should be installed. If adopted, select and pin a stable release plus compatible module versions. Never write a production lockfile that silently tracks the changing main branch. Also avoid unattended implementation loops while the project lacks a packaged map and gameplay acceptance tests: they have too little reliable feedback to decide what good looks like.

BMAD's strongest opportunity here is a narrowly requested design critique or retrospective. Its weakest use is repeatedly re-planning the entire ambitious game instead of testing the first delivery loop. The baseline is already sufficient for execution. An added planning layer should retire an existing source of confusion rather than create another backlog, architecture document and status vocabulary.

### BMAD Game Dev Studio — the most directly relevant process candidate

**Observed:** 2026-09-25 commit [`ff8870955203`](https://github.com/bmad-code-org/bmad-module-game-dev-studio/commit/ff8870955203e1665f4bf75a64049078e6d4080c). **License:** [MIT](https://github.com/bmad-code-org/bmad-module-game-dev-studio/blob/ff8870955203e1665f4bf75a64049078e6d4080c/LICENSE). **Local status:** not installed or executed. These are snapshot facts, not an endorsement of HEAD as a release.

Game Dev Studio explicitly includes Unreal Blueprint/C++ guidance and game-design, narrative, UX, architecture and production workflows. Its own README states that it does not create art, animation, music or a full game implementation from scratch. The MIT-licensed inspected package is version 0.7.2; its README says the game module can operate without the main BMM module. [Pinned README](https://github.com/bmad-code-org/bmad-module-game-dev-studio/blob/ff8870955203e1665f4bf75a64049078e6d4080c/README.md), [package metadata](https://github.com/bmad-code-org/bmad-module-game-dev-studio/blob/ff8870955203e1665f4bf75a64049078e6d4080c/package.json).

**Decision: first process trial if we need one; budget 2–3 hours, independently of the runtime trials.** Give it the existing original concept and a one-page account of the first mission. Ask for contradictions, missing player decisions and a practical playtest protocol. Keep new insights in our existing specification/backlog rather than accepting an automatic replacement GDD. Test whether it can preserve both ground and flight play while reducing the first playable mission's cost.

Success means identifying at least three concrete, non-duplicate improvements that can be tested in the next build, while maintaining the established scope, ownership and budget. Reject generic advice such as adding more factions, multiplayer or infinite worlds without evidence. A useful critique might notice that stealing the sunseed has no consequence during the return leg, or that the player cannot read a docking state. Its instructions and installer still need version review before use; this research did not load any of these third-party skills into the active assistant.

## 44 A small, measurable AI operating model

Keep four responsibilities explicit. The creator owns taste, feel and final product choices. The implementation assistant owns one bounded change and its evidence. A reviewer checks the resulting diff and known failure modes. Research assistance answers a specific uncertainty. Two of these can run concurrently when their files and decisions are independent. Adding five agents to a single map does not create five times the throughput.

The default task context should be small: the task record, relevant API contracts, engine pin, owned files and one short evidence record. A task involving a license also loads that dependency's license record. A task involving saves loads the save schema and migration tests. Neither task needs the whole 42-page handbook in its prompt. Preserve the handbook as a reference, with stable entry points; do not turn it into mandatory preamble.

Use this proposed acceptance contract for generated work:

```text
Outcome: the player can transfer one sunseed to Kestrel.
Invariant: the item exists in exactly one inventory after success or failure.
Ownership: Cargo subsystem and its tests; no map edits.
Evidence: failure-before/fix-after test plus one editor interaction check.
Limits: no inventory framework, database, or unrelated UI rewrite.
```

For editor automation, replace vague instructions such as “build the city” with a concrete manifest: exact assets already entitled and imported, allowed destination folder, placement transforms, maximum object count, overwrite behavior and a validation query. The assistant should show the planned asset changes, invoke a bounded operation, then inspect the resulting object names and references. The first retry should be safe: it either updates the same authored objects or reports their existence. Duplicate buildings on every retry are a pipeline defect.

Separate an action's return code from the evidence of its effect. A successful editor RPC can still leave an unsaved asset, a broken soft reference or a Blueprint compile error. An action is finished when its postcondition is checked. For a material change that may be an inspected parameter value and screenshot; for an imported ship it includes dimensions, collision, material assignment and a packaged view. Avoid requiring the entire release checklist after a tiny reversible edit, but do not call a transport-level success a gameplay success.

Keep a weekly five-minute measurement record: focused development hours, number of playable improvements, time from starting a task to seeing it in a build, build failures attributable to the pipeline, time spent repairing AI changes, and time spent maintaining tools. Compare two similar tasks before claiming a tool is faster. A useful calculation is:

```text
Net hours saved over four weeks = repeated work avoided
  - integration time - maintenance time - recovery from tool failures.
```

For illustration, a helper costing four hours to qualify and one hour to maintain must save more than five hours across the next month. Saving ten minutes on each of six tasks is a loss in that window. This is a planning model, not a measured result or a promise about any repository. Long-term foundational choices can still be justified, but they need a stated reason beyond excitement.

Limit tooling experiments to one at a time and no more than four hours of a twenty-hour development week unless the tool removes the current critical blocker. That is a proposed capacity rule. The repository candidates in this report are a menu for future problems, not an installation checklist. The immediate next milestones remain independent recovery, a real startup map, a packaged executable and a small mission that feels good to play.


## 45 The experiment queue

Do not spend a full week installing everything in this report. Finish BS-002 (independent recovery) and BS-003 (a project-owned startup map and real package) from the workspace backlog first. That gives every later experiment a recovery point and a minimal executable against which to compare. The following queue is ordered by likely relevance, but only the current production bottleneck activates a trial.

### X-001 — Native editor automation

**Trigger:** a real map exists and repeated editor operations are consuming useful development time. **Timebox:** two to four focused hours. **Inputs:** a clean starting commit, project-owned cubes/materials, one throwaway map, the engine's exact tool schema and a known current editor process. **Output:** a written action manifest plus a small verified scene change.

Start with read-only discovery of the current map and selected objects. Then place ten named proxy actors within an explicit test folder, assign one project-owned material, save the map and query the resulting transforms. Repeat the request and verify that it does not create twenty actors. Undo or explicitly remove exactly the trial-owned objects and confirm the starting scene is recoverable. Restart the editor and check that the saved state matches the intended outcome. Keep calls serial within that editor process; independent researchers can still work in parallel.

**Pass:** the client connects reliably, the correct project is targeted, object counts and transforms match, a duplicate request is harmless, logs identify each mutation, and the project still packages with the intended distribution configuration. **Fail:** silent wrong-project changes, uncontrolled duplicate creation, missing save state, or dependence on unsupported runtime automation in the shipped game. The first version may use a bounded project-owned helper to enforce the manifest; this report does not claim that every native tool already provides idempotence or transactions.

Do not turn auto-start on as a substitute for knowing which project and tools are active. Keep the experiment local to the developer machine, inspect the actual bind settings and stop the server when the test is over. This is a concrete consequence of the editor tool's access model, not a request to build a new security platform. Retain the manual editor path when automation is unavailable.

### X-002 — One repeatable art import

**Trigger:** the creator is making or adjusting models in Blender. **Timebox:** three to five focused hours for one candidate exporter; compare the other only if the first fails a relevant need. **Inputs:** a project-owned model with a known one-meter dimension, deliberate pivot, collision proxy, material slots, a socket or attachment marker and two simple LODs where supported.

Import it, place it beside a known-size Unreal cube, inspect collision, rotate it, change one mesh detail in Blender and reimport. Verify scale, orientation, names, materials, collision and placement all survive. Check that the process does not write outside its declared output folders or silently duplicate the asset. Package the test map and view it in the executable. Record the Blender version, exporter revision, options and destination package paths.

**Pass:** three edits can be reimported without hand-repairing transforms, collision or materials, and the result works in the package. **Fail:** the exporter saves a few clicks while requiring manual repair after each iteration. Exporting can remain a manual action during early graybox work; this dependency is unnecessary until original or modified models become a repeated task.

### X-003 — Deterministic district dressing

**Trigger:** a small hand-authored alley establishes an art vocabulary and repeated dressing is now the bottleneck. **Timebox:** four to six focused hours across two sessions. **Baseline:** build the smallest useful graph with native PCG first. Add PCG Extended Toolkit only for a specific missing graph operation whose implementation cost exceeds the trial cost.

Use a 20-by-40-meter proxy alley with 50–100 placements, a walkable route, a docking clearance and a mission sightline. These are experiment dimensions, not shipping budgets. Supply a fixed seed and an exclusion-volume manifest. Generate twice, compare transforms and assigned asset identifiers, then alter one exclusion zone and regenerate. Test collision and traversal on foot; check the docking clearance from the flight route. Capture construction time, editor memory and package behavior.

**Pass:** repeated generation is stable, authored exclusions remain respected, the route works, and changing the layout is faster than editing the equivalent placements by hand. **Fail:** randomness prevents meaningful diffs, repeated operations scatter duplicate objects, or the generator obscures the visual composition. Bake results when runtime generation is unnecessary, and test any claim of independence with the generating plugin disabled. Keep the source graph and seed so the scene remains reproducible.

### X-004 — A mission graph that can survive interruption

**Trigger:** two small missions work, but repeated sequencing has become slow to author. **Timebox:** eight to twelve focused hours, with a stop/review checkpoint after four. **Candidate:** the upstream UE 5.8 release of FlowGraph identified in this research; local qualification remains pending. **Inputs:** a project-owned mission-state contract and stable event IDs. A simpler quest implementation remains the comparison path.

Represent seven meaningful states: accept delivery, acquire sunseed, board Kestrel, reach the gate, redirect the cargo, return and resolve the local consequence. Add cancellation and one failure/recovery route. The graph calls game-owned actions; it does not become the only copy of cargo ownership or mission truth. Save at three boundaries, close the game, load again and confirm exactly one valid continuation. Trigger an interaction twice and verify that it does not grant cargo or rewards twice.

**Pass:** the creator can change the mission sequence faster, the packaged game works, saves preserve progress, missing actors after travel do not break the graph, and the logic can be inspected without opening numerous unrelated assets. **Fail:** state is split across graph nodes and saves with no defined authority, or plugin adoption requires rewriting the whole player/game-instance architecture. Record any custom nodes as the explicit integration surface.

### X-005 — One dialogue authoring choice

**Trigger:** dialogue branching or localization editing becomes a repeated burden. **Timebox:** six to twelve focused hours with exactly one candidate first, with an initial checkpoint after four. **Candidates:** DlgSystem for an Unreal-oriented graph workflow, or Inkpot when text-first ink authoring is a better fit. Do not decide based only on the prettiest editor screenshot.

Build a twenty-line exchange with three choices, one previously known fact, an interrupt, and a later callback to the sunseed decision. Test keyboard/controller selection, skip/reveal behavior, leaving the interaction area, saving after a choice, and loading into the correct continuation. Store display strings through the chosen localization path. Voice audio is deliberately outside this experiment.

**Pass:** the author can revise the conversation without breaking the mission, facts remain stable across reloads, text can be extracted and identified for localization, and runtime/importer requirements are clearly separated. **Fail:** the plugin quietly becomes a competing quest or save authority, or text changes demand fragile manual identifier repairs. Dialogue quality itself still needs a human read and playtest.

### X-006 — One AI workflow improvement

**Trigger:** an observed retrieval or planning failure has recurred. **Timebox:** two hours initially, with a separate extended Serena trial only if native-code navigation justifies it. **Candidate:** choose one of Repomix, Context7, BMAD Game Dev Studio or a selected debugging method; do not install a suite of overlapping frameworks.

Use a real completed task with known facts as a controlled benchmark. Count setup time, total tool calls, information the assistant missed, incorrect statements and manual repair. Compare against the existing file-search, source-reading and task-template workflow. A response sounding more confident is not a pass condition. Retain the tool only if it materially improves a task we perform repeatedly and preserves the current project rules.

## 46 The first two weeks after research

The following is a proposed order for roughly twenty focused hours per week, not a completion promise. **Week one:** prove recovery; author and package a startup map; establish ground input/camera from an appropriate donor; place the ship proxy; verify one board/unboard transition. Spend up to four hours on X-001 only if editor repetition has become a blocker. The output should be a small executable the creator can walk around and enter.

**Week two:** make a short flight route, landing/docking state and a visible delivery interaction; persist the one important cargo fact; record a short playtest. Use asset import or PCG trials only where they unblock that route. A well-lit proxy mission with readable controls is more informative than a large imported city with no loop. Keep the city's eventual richness and the city-to-orbit ambition in the design, while gathering evidence from the smallest route that includes both.

At the end of each week, decide with evidence: what became playable, which bottleneck dominated, which tool earned its upkeep, and what the next player-visible improvement should be. A rejected plugin is a useful research result when it prevents a week of integration work. Stop planning new automation layers while the existing layer already supports the next game task.

## 47 Delivery boundaries and next decision

The development handbook and initial workspace were finished before this additional sprint. This report adds pinned repository research, a machine-readable candidate register and a proposed experiment queue. It does not alter the engine pin, add runtime plugins, buy assets, change global Git settings, start an Unreal MCP server or install external workflow skills. The actual compiled/tested foundation remains separately documented in the workspace validation record.

The strongest next decision is to begin the first packaged ground-and-flight mission using the delivered baseline. Enable a trial when its named bottleneck appears. If the mission proves compelling, the toolkit can grow around known needs; if the mission is not yet compelling, more infrastructure will not answer the design question.


## 48 Evidence register and limits

All repository observations were collected on 12 October 2026. Links beside each card lead to primary sources, often at an exact commit. The accompanying repository-candidates.json preserves the full upstream metadata and per-candidate assessment. The report distinguishes installed-engine observations, upstream declarations, and proposed experiments. No new third-party runtime or editor plugin was built or packaged during this repository sprint.

A newer HEAD is not automatically a recommended release. Missing SHA or license verification is recorded explicitly in the relevant assessment.
