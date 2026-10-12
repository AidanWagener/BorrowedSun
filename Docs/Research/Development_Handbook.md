# Borrowed Sun development operating system

Version 1.0 | 12 October 2026 | Solo creator, Unreal Engine, AI assistance, initial spending below EUR 500

## 1 The operating decision

Build one local-first production system around a pinned Unreal project, Git with LFS, a small set of repeatable PowerShell commands, a short task board and a license-aware asset intake process. The objective is the shortest dependable path from an idea to a playable packaged build. Every tool must reduce a measured delay or protect work that would be expensive to recreate.

The creative destination remains Borrowed Sun: intimate city life connected to personal ship flight and orbit. The first production milestone is a ten-minute greybox expedition. Architecture, source control and automation are built to support that expedition. A huge framework integration, a beautiful unrelated demo or a functioning CI dashboard is not the milestone.

This handbook contains selected decisions, alternatives, rejection criteria, operational procedures and research. The accompanying workspace contains source files and tools. Candidate assets have not been purchased or silently accepted into the game. Vendor statements, proposed performance targets, locally observed facts and completed verification are distinct categories. The delivery validation record is the authority for what actually ran.

### What to optimize

Measure time to a correct playable change, time lost to rebuilding, rework from misunderstood requirements, and recovery time after failure. Count shipped behavior and resolved uncertainty. Lines of generated code, number of agents, purchased packs and board tickets are not throughput measures.

The proposed routine is one active feature, one concurrent research or review task, and one integration owner. The creator owns binary assets and final play feel. AI can research, draft code, implement bounded components, produce test cases, investigate logs and review diffs. The creator still plays the build, accepts visual quality and makes product decisions.

The minimum stack is Unreal Launcher, its supported Microsoft C++ toolchain, Git plus LFS, PowerShell, one existing editor, Blender when needed, the existing AI subscription, and local Markdown/JSON records. Native Unreal audio, UI and profiling serve the first prototype. Hosted CI, commercial middleware, multiple issue trackers and remote build farms are later options with adoption gates.

## 2 The machine changes the plan

Read-only local inspection on 12 October 2026 found Unreal 5.8.2, changelist 56702186, installed through the Launcher path; Visual Studio Build Tools 2022 17.14.16; MSVC folder 14.44.35207; Windows SDK folder 10.0.26100.0; Git 2.55.0.windows.1 and Git LFS 3.7.1. Installed folders alone do not prove that a project compiles. The validation record reports execution separately.

The development machine reports an AMD Ryzen 7 7435HS, eight cores and sixteen logical processors, an NVIDIA GeForce RTX 4070 Laptop GPU, approximately 15.7 GiB usable RAM and about 70 GiB free on its approximately 952 GiB C: volume. GPU memory was not independently measured. This is a development profile, not a promised customer minimum specification.

Epic's current hardware guide recommends 32 GB system memory and at least 8 GB graphics memory for development. That makes memory capacity and storage headroom reasonable priorities before buying visual content. Hardware purchases require checking this laptop's exact model, supported memory configuration, warranty and current pricing; no particular RAM module or SSD is endorsed without that information. [O01](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine)

### Working profile for this machine

- Begin with two compiler actions and one Unreal editor process. Increase only after measuring memory pressure and iteration time. Close heavy DCC applications during full builds; do not launch several shader-heavy sample projects simultaneously.
- Keep a warm local DDC while space allows. Do not routinely clear caches as a repair ritual: measure the cause first. Avoid downloading a second complete engine or rebuilding the engine from source for the initial prototype.
- Use a small representative block, one character and one ship. Import selected assets, not every model and texture from multiple kits. Download sizes, cooked sizes and resident memory are different quantities; record all three when relevant.
- Treat less than 30 GiB free as an investigation trigger and less than 15 GiB as a proposed stop for large imports/builds. These are local policy values, not Epic limits. Estimate peak temporary space before a package; a larger project needs larger reserves.
- Keep an independent recoverable backup. A second folder on C: protects against some mistakes but not disk failure. Do not count an untested sync service as a complete backup strategy.

The delivery directory is convenient for review. For sustained production, put a fresh clone in a short non-synchronized path such as C:\Dev\BorrowedSun on an appropriate drive. This is a recommendation, not a folder silently created elsewhere. Some Windows tools still encounter path limits even when long-path support is enabled, because application support matters as well as operating-system policy. [O02](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)

## 3 The spending envelope

The initial ceiling is below EUR 500. Use EUR 480 as a planning envelope to preserve a margin. The table contains allowances, not vendor quotations or purchase instructions. Existing AI access is assumed available; a new subscription would have to fit inside the envelope. Check VAT, currency conversion, recurring renewal dates and commercial-use tiers at checkout.

| Allowance | Ceiling | Release condition |
|---|---|---|
| Memory, storage or independent backup | EUR 240 | Confirm exact hardware compatibility and the actual limiting resource; spend first on recoverability if no independent backup exists |
| A specific asset or production aid | EUR 120 | Free prototype succeeds, the missing item has measurable integration value and checkout total fits |
| Initial operations or testing expense | EUR 40 | A concrete service/test need cannot be met with current tools |
| Uncommitted buffer | EUR 80 | Hold for tax/FX variance, replacement or an evidenced bottleneck |

These allocations can move; the total cannot. Do not purchase a USD 245 city kit and a set of subscriptions before assessing the machine and first block. A smaller prop/interior purchase may remove more work than another skyline. The asset research later in this handbook supplies a zero-cost candidate basket and conditional paid choices.

Use the purchase record to calculate total cost: gross checkout price plus required dependencies plus integration hours plus upkeep. For example, a EUR 50 item requiring three days of repair can cost more production time than a free simple alternative. An illustrative decision model is saved hours minus integration, learning, maintenance and replacement hours; do not assign a fictitious hourly wage just to make a purchase appear justified.

## 4 Workspace ownership and information architecture

The workspace is intentionally comprehensible at the root. Game holds the Unreal project. Tools owns reproducible operational commands. Config records the toolchain/engine agreement. Docs holds decisions, intake records, small task plans and evidence. .github contains an inactive hosted workflow example and contribution templates where provided. Generated Artifacts are ignored. Private Vault and machine-local settings are ignored. An ignored folder is not encrypted or backed up automatically.

### Authoritative locations

| Information | Owner and location | Rule |
|---|---|---|
| Engine and project behavior | Game/Config and version pin under Config | Track project defaults; keep personal paths in Tools/Environment.local.json |
| Gameplay source and tests | Game/Source | Runtime and editor dependencies stay separate |
| Authored content | Game/Content | Binary assets use LFS; one writer per asset; preserve external actor/object packages |
| Native art/audio source | SourceArt when introduced | Track selected originals with LFS or an explicit recoverable external source vault |
| Accepted technical decisions | Docs/Decisions | Record why, costs, revisit trigger and rollback |
| Current work | Docs/Registers/backlog.json | Every active item has an owner, acceptance test and dependency |
| Provenance and rights | Docs/Registers plus private Vault | Public metadata references restricted evidence without exposing payment/account information |
| Builds, tests and profiling | Artifacts | Store commit, engine, configuration and results; retain only useful runs locally |
| Documentation research | Docs/Research | Reference material, not an instruction to install everything mentioned |

Do not create one plugin per anticipated future game system. Two source modules establish the runtime/editor boundary. Feature folders can organize interaction, flight, missions, persistence and world travel within the runtime module until an actual dependency or reuse requirement justifies extraction. Each module has a compile and maintenance cost.

Vendor imports keep a stable original root if relocation would break hard references. Project wrappers, material instances, collision overrides and gameplay adapters live under the project's own root. If a vendor insists on a particular Content mount, document the exception rather than repeatedly renaming it. Migrate through Unreal, audit dependencies and package after import. Cosmetic folder purity is not worth broken references.

### Naming and persistent identity

Use readable Unreal prefixes for asset class, a clear feature/location name and a short variant suffix: SM_CollectorPanel_A, MI_MarketAwning_Red, BP_DockLift, DA_Job_BorrowedMorning, WBP_InteractionPrompt. Paths describe ownership and feature; IDs describe identity. Save IDs must survive display-name changes, map edits and reimport.

Reserve authored stable IDs for meaningful objects, stateful services and mission definitions. Generated decorative objects may use reproducible recipe identifiers only when needed for regeneration. Do not make save state depend on actor labels or array position. The engineering section specifies the boundary between authored identity and transient presentation.

## 5 The daily production loop

Begin a session by opening the last known build, reading the current task's acceptance example and choosing one outcome. Reproduce a bug before fixing it. For a feature, make the smallest playable version before adding polish. Keep the editor and source checkout aligned; close the editor before switching branches that modify reflected C++ or content.

The usual cycle is: define the outcome, implement, run the narrow verification, play the affected route, inspect the diff, commit a coherent state, and record the next observation. Broad clean rebuilds and full regression runs belong at integration points, not after every text edit. However, a change to save schemas, travel or packaging demands the relevant end-to-end checks even if the diff is short.

Suggested twenty-hour week: twelve hours of implementation/content, three of integration and regression, two of direct playtesting, two of research or learning driven by the current bottleneck, and one of planning/backups/cleanup. This is a planning allocation, not an empirical optimum. Review where actual hours went every two weeks and change the allocation.

At the end of each week, archive one identifiable playable build with a short note: what the player can now do, what broke, what was measured, and the next biggest uncertainty. A video is useful evidence but cannot replace the executable. If nothing playable improved for two weeks, reduce active scope or repair the build pipeline before opening another feature stream.

### Board states and limits

Use proposed, ready, active, review, blocked and done. Allow one active gameplay task and one independent research/review task. A blocked item records the precise missing condition and the next check; it does not trigger a new speculative subsystem. Keep the next two weeks detailed and later milestones coarse. Decompose any ticket whose outcome cannot be tested within roughly one or two focused sessions, unless it is an explicitly bounded experiment.

The initial backlog includes a sequence from a verified foundation to an empty packaged map, movement, interaction, persistent cargo, flight, travel, return consequences and the integrated expedition. Estimates are planning ranges with confidence. A checkbox is complete only with the specified evidence; generated code does not automatically complete a gameplay ticket.

## 6 AI development with explicit integration boundaries

Keep repository instructions short and task-specific. Official OpenAI guidance describes how AGENTS.md is discovered and layered, and recent guidance recommends avoiding bloated always-loaded instructions. The supplied file routes developers to relevant material instead of forcing the entire handbook into every request. [O03](https://learn.chatgpt.com/docs/agent-configuration/agents-md) [O04](https://developers.openai.com/blog/rethinking-skills-and-prompts-for-gpt-6-astra)

Use three roles as needed, not three permanently running agents: implementer for a bounded change, reviewer for independent failure analysis, and researcher for an unresolved API/dependency question. The creator integrates and plays. Parallel work helps when files and decisions are independent. Two agents redesigning the same interface or modifying the same Blueprint produce coordination cost, not speed.

Git worktrees can isolate concurrent text/code tasks. Each checkout still consumes working files and can generate its own Unreal intermediates; on this machine, avoid multiple heavyweight editor worktrees. Codex supports worktree-based independent chats, while Git defines their shared repository metadata and branch constraints. This project uses that capability only when isolation is worth its disk and integration cost. [O05](https://learn.chatgpt.com/docs/environments/git-worktrees) [O06](https://git-scm.com/docs/git-worktree.html)

### Task contract

Give an agent the player/developer problem, current reproduction or state, acceptance example, owned paths, constraints, relevant version, and required evidence. Ask it to inspect the actual code and pinned APIs before implementing. It should return changed behavior, test commands and results, remaining uncertainty and a small diff. For a local reversible task, authorization to implement and run its narrow tests is sufficient; do not create needless approval loops.

Useful implementation request: 'Implement BS-008's cargo custody invariant in the runtime module. Preserve one custodian across interrupted transfer and reload. Own only the persistence and cargo files listed in the task. Add failure-path tests and run the available suite. Report any unrun editor integration check.' A vague request to 'build the entire mission system perfectly' has no reviewable finish condition.

Review prompts should ask for concrete defects with a triggering scenario, file location and impact. The reviewer first checks ownership, lifecycle, save compatibility and error propagation. Cosmetic comments do not compensate for missing destructive-failure tests. Require changes only where evidence supports them; excessive generic review rules slow work and obscure serious faults.

### High-return uses

- Generate small test fixtures, log parsers, schema validators and migration dry runs; verify them against known failures.
- Trace API usage in the pinned engine source headers, explain build errors, and propose a minimal reproduction before broad rewrites.
- Draft editor utilities for bulk validation, naming or import reports; start in an isolated intake project and make mutations reversible.
- Draft dialogue alternatives, encounter variations and prop lists against a fixed creative brief, then curate for voice, cost and consistency.
- Summarize a performance capture only from supplied measurements; do not accept invented frame times or hardware claims.

Keep purchased source assets outside AI processing until the applicable license has been checked. Source availability and an AI subscription do not establish asset-upload rights. Do not put runtime generative dialogue, paid API calls or online agent dependencies into the first game build. The production AI workflow can be offline from the shipped executable.

### Rework control

If an agent's first patch fails, give the exact failure, a narrower boundary and the relevant evidence. After two unsuccessful repair cycles, stop expanding the patch and build a minimal reproduction or choose the simpler fallback. Preserve working checkpoints. Do not ask another agent to rewrite the entire subsystem without diagnosing the failure; that loses both evidence and the known-good path.

## 7 Content as a repeatable production process

The fast unit of environment production is an approved block recipe with predictable navigation, materials, collision, lighting and performance. It is not a large imported kit. Build one representative street segment containing a doorway, stairs, a lift, a roof, a ship approach and a distant landmark. Validate movement and sightlines before dressing it.

Create an intake project separate from the production game's authoritative Content. Admit one candidate item at a time. Check dimensions, pivots, orientation, naming, collisions, material slots, texture dimensions, references, skeletons and license evidence. Record a packaged measurement. Promote only selected assets. Keep sample demo maps, showcase lighting and vendor test characters out of the release cook unless actually required.

For an approved prop, the finished record should answer: where did it come from, what version, what may we do with it, what was changed, which recipe uses it, how expensive is it, and what replaces it if it breaks? This makes reimport and eventual outsourcing tractable.

Standardize a limited palette of architectural surfaces, trims, decals and light-temperature families. Convert variation into material instances and parameter data where appropriate. Distinguish a hero asset that carries the game's identity from repeatable supporting architecture. The ship silhouette and sunseed deserve bespoke decisions; the back of an alley utility cabinet usually does not.

### Automation ladder

First make an operation work manually. Second document its accepted output. Third time repeated instances. Fourth automate the stable part. A tool must have a preview/report mode, deterministic selection and a clear change summary before bulk edits. Batch tools that silently rewrite hundreds of packages are expensive to trust and difficult to review.

Good early candidates are naming checks, missing collision reports, texture dimension inventories, dependency reports, ID uniqueness checks and import manifests. Defer a universal city generator until manually approved blocks prove that its grammar creates navigable missions. Procedural generation is useful for repeatable dressing and assembly; authored mission routes remain owned.

## 8 Feedback, observability and the definition of done

Every build needs a visible or retrievable build identifier: commit, local dirty status, engine pin and build configuration. Player reports must identify the map/mission, reproduction and input device. Keep personal data out of issue records; local testing can use anonymous participant IDs and explicit recording consent.

A feature is done when its acceptance example works, its failure path is recoverable, it meets the current performance budget, it has no unapproved dependency/license obligation, and the relevant packaged route still completes. Not every feature needs a new automated test, but persistence, ownership and build-tool error handling deserve tests because silent regressions are costly.

Keep four evidence classes separate: static repository checks; compiled unit/automation tests; engine integration/cook/package; human play and visual checks. A headless NullRHI run cannot establish GPU performance or visual quality. A successful editor compile cannot establish Shipping packaging. A screenshot cannot establish save recovery. The supplied validation record uses these distinctions.

### Performance notebook

For each representative scenario record build ID, machine profile, resolution, scalability, frame generation/upscaling settings, warm/cold cache state and capture duration. Track frame-time distribution and worst hitches, game/render/GPU timing, RAM/VRAM pressure, streaming, draw/material costs and package size. Compare like with like. Editor viewport FPS is a diagnostic clue rather than the release benchmark.

Use one fixed market route, one fastest allowed city-flight route and one ascent/return route. Add stress cases only when their behavior is relevant. Fix the dominant measured cost first. Do not adopt a rendering feature because its demo looks better on different hardware; prove its cost and visual benefit in these scenes.

### Recovery notebook

Write a short runbook for corrupted saves, failed imports, bad engine upgrades, disk exhaustion and a lost machine. A good backup is defined by a successful restore into a separate directory, including LFS payloads and native source assets, followed by the relevant build. CISA's guidance supports offline protected backups and tested restoration; this project's practical adaptation is a small regular restore drill rather than an enterprise security program. [O07](https://www.cisa.gov/stopransomware/ransomware-guide)

## 9 The first ten working sessions

These are dependency-ordered sessions, not a promise of completion in ten calendar days. The foundation and engine qualification may consume several sessions. Stop a session with a clear observation, not an unreviewable collection of partial systems.

| Session | Concrete output | Evidence |
|---|---|---|
| 1 | Local workspace understood; engine/toolchain qualification recorded | Doctor, preflight and actual build outcome; no missing prerequisite hidden |
| 2 | Empty owned test map, correct default map and first package | Win64 executable starts from its staged folder |
| 3 | Ground character, camera, input and one doorway/stair/ledge route | Controller and keyboard complete route; recover after obstruction |
| 4 | One interaction contract and an accessible prompt | Cancel, leave range and unload do not consume the action |
| 5 | Persistent sunseed custody with save/reload | No duplicate or missing cargo after interruption and restart |
| 6 | One ship handling map and boarding transition | Repeat boarding, braking, landing and return to character |
| 7 | City/orbit handoff proof | State preserved; destination collision ready; interrupted travel recovers |
| 8 | One block recipe with selected free art | Packaged movement and flight still work; baseline performance captured |
| 9 | One service choice and visible return consequence | Revisit after reload shows correct service state |
| 10 | Ten-minute connected expedition | Unfamiliar player completes ground, flight, orbit and return; top failures recorded |

The longer twelve-week plan in the game specification remains the planning horizon for this engineering prototype. These sessions define the earliest sequence, not a replacement schedule. Content and UX polish enter only after the core integration proves viable.

## 10 Decisions deliberately postponed

Do not set up Kubernetes, distributed builds, a live backend, multiplayer replication, a custom planetary engine, runtime LLM services, a production analytics pipeline or multiple commercial frameworks now. Each remains available if a measured requirement justifies it. A postponed decision has a trigger: long local build time, confirmed co-op funding, proven need for radial gravity, or a release support requirement.

The decisive comparison for any framework is its delivered narrow capability minus learning, integration, upgrade, test and replacement work. A large framework can be valuable when it already owns the game's central problem and its architecture is accepted. It can also force an unrelated architecture onto the game. The separate repository research sprint will apply this test to specific open-source candidates before adoption.

The best workspace is allowed to change. Review its friction every two weeks: the longest repeatable delay, the most frequent failure, the least-used tool and the next simplification. Delete a redundant procedure only after identifying what protection or information it supplied. Keep the production system small enough that the creator can explain it and recover it.



Research checked 12 October 2026. Engine facts below cite primary Epic or vendor sources as [E##]; those references resolve in `engineering_sources.json`. Project policies, budgets, architecture and acceptance thresholds are recommendations. They are not features already implemented in the starter. Read `Docs/Operations/engine-validation.json` for the exact build/test evidence rather than inferring readiness from source files. The pinned engine successfully compiled both editor and game targets, and all five foundation tests passed; no map, cook, package or gameplay qualification is claimed.

## 11 The concrete recommendation

Use the installed **Epic Games Launcher Unreal Engine 5.8.2**, Windows only, with one runtime C++ module, one editor C++ module, Enhanced Input, Blueprint presentation and a small authored district. Establish a repeatable editor compile and five real foundation tests before buying assets or building a procedural city. Build one walking route, one interaction, one stationary docking exchange and one short ship route before adding the full expedition. Preserve the earlier specification's authored-sector street-to-orbit design.

Epic released UE 5.8 in June 2026; its official hotfix announcement confirms 5.8.3 on 22 September 2026. The newest verified released patch is therefore 5.8.3, while the installed and initially pinned build is 5.8.2. Those are deliberately separate facts. A new project without this existing installation could reasonably qualify 5.8.3; this workstation should first prove its current tools, then evaluate the hotfix against a small reproducible project. [E01](https://www.unrealengine.com/news/unreal-engine-5-8-is-now-available) [E02](https://forums.unrealengine.com/t/5-8-3-hotfix-released/2833315)

The under-€500 initial budget changes the production order. The observed workstation has approximately 16 GB system RAM, a Ryzen 7 7435HS, an RTX 4070 Laptop GPU and about 70 GB free on the system drive at inspection. That is enough evidence to try a small C++ foundation, not evidence that a dense imported city will fit. Current Cargo guidance specifies 32 GB RAM, 8 GB VRAM and at least 15 GB free per kit. This machine's system RAM is below that vendor guidance. Keep the first import tiny; prioritize confirmed compatible RAM/storage capacity over a second architecture kit. No hardware purchase or installation was performed. [E33](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines)

Do not begin with a source engine, City Sample as the production base, a full planet, a runtime city generator, moving walkable ship interiors, replicated multiplayer, a collection of marketplace gameplay frameworks, or a remote build farm. Each adds ownership and integration work before the singleplayer loop is proven. The initial technical product is a packaged ten-minute route with retained state and understandable recovery, not an engine-technology inventory.

## 12 Engine, compiler and plugin pinning

`Game/BorrowedSun.uproject` uses `EngineAssociation: 5.8`, which locates a minor engine family. It cannot express an exact hotfix. `Config/engine-lock.json` records 5.8.2, changelist 56702186, compatible changelist 55116800 and the Launcher distribution. Every build entry point should read the selected engine's `Engine/Build/Build.version`, compare that tuple and fail on mismatch. An explicit reviewed lock change is required to accept another build; merely discovering a newer installation is not acceptance.

Epic's current 5.8 guidance supports Visual Studio 2022 17.14+ and Visual Studio 2026 18.0+, recommending 2026 for general development. Its compiler/SDK table gives MSVC 14.38 minimum / 14.50 recommended and Windows SDK 10.0.22621 minimum / 10.0.26100 or newer recommended. Installed compatibility and the recommended fresh installation are different questions. The local UBT run selected VS 2022's 14.44.35217 toolchain from the 14.44.35207 directory and SDK 10.0.26100.0, using bundled .NET 10; it did not require installing another IDE. [E03](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine)

Record compiler version, SDK version, engine changelist and build settings in the first accepted build report. Keep generated solution files disposable. UBT uses module Build.cs and target Target.cs files, rather than treating the IDE solution as the authoritative build definition. The starter explicitly uses `BuildSettingsVersion.V7` and `EngineIncludeOrderVersion.Unreal5_8`; the first real build rejected older default settings shared with the installed editor, providing direct local evidence for that adjustment. [E04](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules)

Prefer Launcher because it already exists, avoids the maintenance burden of an engine fork, and matches Cargo's documented support. Cargo currently supports official Launcher 5.2–5.8 and explicitly excludes source-built engines. If an actual blocking defect later requires a fork, make a minimal reproduction, document the exact upstream commit/patch, prove same-version content migration in an isolated copy, and expect to retain a supported Launcher intake project. Do not assume the connector will work against that fork. [E33](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines)

The plugin lock initially names only Enhanced Input as an explicitly enabled project dependency. AndroidFileServer is explicitly disabled for this Windows-only scaffold: a local editor execution had written an unused Android server section with a generated token into tracked defaults, so that section was removed and the plugin disabled before delivery. Inspect configuration diffs after editor runs. Other engine defaults remain tied to the engine lock; this is not a claim that only one built-in plugin loads. Add PCG when the first repeatable block recipe exists, StateTree when the guard behavior requires it, and Python/editor scripting when an approved intake task can be automated. Cargo belongs to the intake workflow. The vendor documents excluding its editor connector from packaged builds; dependency inspection and a cook must verify that the actual imported content does not retain a required runtime dependency. [E34](https://help.kitbash3d.com/en/articles/12689467-exclude-kitbash3d-cargo-plugin-from-unreal-engine-builds)

Every added plugin requires a record with source, exact version or revision, engine compatibility, license evidence, enabled modules, editor/runtime classification, migration instructions and the smallest passing cook. Reject an asset purchase that requires an unexplained stack of abandoned plugins. A cheap plugin that takes a week to integrate is not cheap for this production.

The upgrade gate is: clone/copy the known good workspace; record new lock; build and run tests; open/save a copy of the representative map; regenerate derived content; cook; run the fixed walking and flight routes; compare save fixtures and performance; then accept or discard the change. Never resave the sole copy of binary assets under a candidate engine. No date-based auto-upgrade is proposed.

## 13 Runtime ownership and durable identity

The starter has two compiled modules: `BorrowedSun` (runtime) and `BorrowedSunEditor` (editor). Gameplay folders become ownership boundaries as behavior arrives. Public headers expose small data/contracts; private implementation owns engine interactions. Editor dependencies must not enter the runtime module. This follows Unreal's supported module/dependency mechanism; the particular boundaries are a project design. [E04](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules)

| Runtime area | Owns | Does not own |
|---|---|---|
| Core | Stable IDs, revisions, definition lookup, typed results | Actor spawning or mission decisions |
| WorldState | Campaign facts, custody, charges, rewards, checkpoint snapshots | Visual attachment or animation |
| Interaction | Option queries and command submission | A second inventory database |
| Travel | Readiness, transfer state, fixed anchor mapping, recovery | City-wide navigation or planetary physics |
| Population | Named NPC records and active representations | Global mission truth inside NPC Actors |
| Presentation | Animation, camera, sound, UI and effects | Granting rewards or changing custody |
| Editor module | Validation, intake reports, recipe generation | Packaged gameplay |

Use a GameInstance-scoped campaign service for durable in-memory state and world-scoped coordinators for streamed representations. The campaign service must not hold strong references to ordinary district Actors. Actor load/unload registers or removes a representation for an existing ID; it never invents or deletes the underlying named person. Player possession selects an existing campaign identity.

The supplied `FBSStableId` stores `Namespace` and `Key`, with a canonical lower-case ASCII grammar: letters, digits, dot, underscore and hyphen, at most 128 characters per part. Its canonical serialization is `namespace:key`; delimiters are forbidden inside parts. For example, `orison.market:stall-001` persists through Actor renaming, array reordering and mesh replacement. Allocate keys once in a reviewed manifest. The helper validates identity; it is not an automatic allocator or a content hash.

Separate three things: persistent entity ID (`orison.market:stall-001`), definition ID (`building:market_stall_a`) and mutable Unreal asset path (`/Game/BorrowedSun/Art/Adapted/...`). The Asset Manager supports primary assets and bundles; use it to resolve definitions and asynchronously prepare optional representations. A renamed path needs a manifest/redirect review, not a new entity ID. Do not serialize raw UObject pointers as persistent identity. [E19](https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine)

Project assets belong under `/Game/BorrowedSun`, with Core, Characters, Ships, Interaction, Missions, UI, Worlds, Tests and Art/Adapted. Preserve vendor source folders where their references expect them; a cosmetic desire for one folder does not justify bulk filesystem moves. External Actors and External Objects are authored project content, despite their encoded names, and must travel with maps in version control and backups.

Implement the transaction service as a separate small increment with tests before tying it to pickups:

```text
Command = { CampaignId, Sequence, ActorId, TargetId, ExpectedRevision, Verb, Payload }
Result = { Accepted, Reason, Revision, Events }

Apply(command):
  if command is in persisted RecentOutcomes: return its previous result
  if command.Sequence <= DecidedHighWater: reject ExpiredWithoutReplay
  require command came from the single ordered dispatcher
  validate campaign, current revision, ownership and preconditions
  compute result and check global invariants
  commit state, decided outcome and high-water mark together
  publish presentation events after commit
```

One dispatcher assigns monotonic sequence values. Persist at most 1,024 recent outcomes and a high-water mark; pruning a detailed outcome must not permit replay. Keep permanent unique reward/consumption keys separately so submitting an equivalent reward under a new request ID cannot duplicate it. Decide and journal rejected requests consistently. The sunseed has exactly one custodian; charge and mission changes join the custody transaction. Before commit cancellation preserves the source. After commit failure recreates the destination representation. The scaffold does not yet implement this journal.

## 14 Input, movement, abilities and NPCs

Enhanced Input provides action assets, mapping contexts, modifiers, triggers and context priorities, including adding/removing contexts through the local-player subsystem. The starter enables and configures the plugin; it does not pretend binary action assets already exist. Create Common, Walking, Ship and Menu contexts during the first playable increment. [E05](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)

The proposed controller owns context transitions. Common contains pause and basic UI; exactly one locomotion context is active; Menu consumes conflicting controls. A possession transition removes the old locomotion context before adding the new one, clears held interaction state, and restores mappings after travel/reload. Bind one-shot interactions to an intentional started/completed trigger, not accidentally to every triggered frame. Test controller disconnect, focus loss, remapping, pausing while holding an interaction and rapidly entering/exiting the dock.

Use Character Movement for the first grounded humanoid. Use a separate arcade ship movement component with one swept collision root; create readable pitch/yaw/roll, acceleration and braking first. Keep art mesh and camera offset below the root, and make flight tuning data rather than scattered Blueprint constants. Do not combine a kinematic movement owner with an independently integrating Chaos rigid body for the same craft. Moving ship interiors remain excluded; walking interiors are stationary docked spaces.

GAS supplies attributes, abilities, effects and asynchronous ability tasks. That is valuable for many interacting statuses, stacked modifiers, cooldowns and complex action cancellation. It is not required to implement a small offline game with health, a few attacks and three sunseed charges. Start with explicit components and the authoritative command service. Reconsider GAS when a written combat matrix demonstrates interactions that would otherwise reproduce its machinery; if chosen, designate GAS as the owner of combat attributes rather than maintaining duplicate values. [E06](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)

For the first guard, choose one decision framework. StateTree is a reasonable default for Patrol, Suspicious, Investigating, Engaged and Returning. Its hierarchical states can run tasks concurrently and a task's completion can trigger transition, so a perpetual look-at task and a finite movement task need deliberate completion rules. Unreal Behavior Trees remain a sound alternative, with event-driven evaluation and Blackboard state. Neither should become a second mission-state database. [E07](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine) [E08](https://dev.epicgames.com/documentation/en-us/unreal-engine/behavior-tree-in-unreal-engine---overview)

Use ordinary Actors for the first small population. Reduce offscreen update frequency and suspend insignificant behavior before adopting an entity framework. MassEntity is a data-oriented fragments/processors system; switching to it is an architectural change, not a density checkbox. Introduce it only after a representative crowd test shows the Actor approach fails a measured CPU target and the desired representation tiers are clear. Named crew remain logically persistent even when their visual representation changes. [E35](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine)

Ground NPCs use bounded walking nav and explicit links for stairs/doors. Flight uses a authored route graph plus local avoidance and exclusion volumes. World Partition Navmesh still carries an Experimental label in current documentation: prove repeated packaged unload/reload pathfinding before relying on it. If that fails, keep a small resident nav set, or separate walking maps behind an authored dock/door transition. Do not let experimental streamed nav become an invisible dependency of every mission. [E14](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partitioned-navigation-mesh)

## 15 World construction, streaming and street-to-orbit

Create a conventional small `L_Test_Startup` map under `/Game/BorrowedSun/Maps/Tests` first, then qualify a compact World Partition pilot before committing the production city. World Partition streams cells around sources and can bundle Actors with hard references; Level Blueprint references can also undermine intended streaming. Keep mission references as IDs/soft definitions and audit reference chains. Only the player/ship, environment coordinator and lightweight campaign/travel services should be intentionally persistent. [E09](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)

OFPA reduces map-edit overlap by storing Actor instances externally in the editor; cooked output embeds Actors in their levels. It does not make arbitrary concurrent edits safe. Keep one writer per district/recipe generation operation and review all changed external packages with the map. Epic's specialized changelist display is Perforce-specific; a Git workflow must provide its own complete change review rather than assuming that integration exists. [E10](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine)

Use Data Layers for authored organization and controlled variants: base architecture, mission state, signage/activity variants and orbit staging. Loaded and Activated have distinct runtime meanings; the documentation's example makes Loaded invisible and Activated visible. For this project, merely knowing a layer is Loaded is not a sufficient collision/nav readiness test. Separate readiness assertions from visual activation. [E11](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)

HLOD represents unloaded architecture with proxies; Nanite handles geometry detail inside a representation. Plan HLOD layers by distance and material behavior, not a single merge-everything setting. Exclude interactive doors, dynamic custody objects and gameplay triggers from static architectural proxies. Rebuild derived proxies after material/collision-relevant changes and inspect approach/return transitions in a packaged run. [E12](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine)

PCG supports partitioned, hierarchical and runtime modes. The project's initial use is editor-baked block composition and decoration. Collision-bearing routes and landing clearances are authored inputs, not stochastic outcomes. Runtime generation, if introduced, is limited to disposable decoration that cannot block doors, cover objectives or change saved identity. Do not choose runtime PCG to avoid defining an offline production process. [E13](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-pcg-generation-modes-in-unreal-engine)

The placement manifest is the boundary between generation and saving:

```text
Recipe { DistrictId, RecipeVersion, Seed, AssetSetVersion }
Placement { StablePlacementKey, DefinitionId, Transform, Overrides }
EntityId = DistrictId + ':' + retained StablePlacementKey
RegenerationReport { AddedKeys, RemovedKeys, ChangedKeys, MigrationRequired }
```

Retain placement keys across regeneration; PCG output index is never an ID. Compare canonicalized manifests, excluding transient timestamps, to check repeatability. A seed change is a reviewed content revision. Preserve handmade overrides separately and list orphaned overrides as errors. Validate doorway/camera/collision clearance, duplicate IDs, roof exclusion volumes and expected navigation paths before saving the generated district.

The normal flight path uses one persistent world with authored city, ascent and orbital sectors, plus a distant moon/city proxy. The first version has no continuous spherical terrain, arbitrary planet landing or geographically correct orbital simulation. A cloud mask conceals a controlled rigid-anchor relocation. LWC introduces double-precision support and conversion considerations, but does not supply this gameplay or erase float precision risk in all subsystems. [E32](https://dev.epicgames.com/documentation/en-us/unreal-engine/large-world-coordinates-in-unreal-engine-5)

The travel service owns `Idle → Preparing → Ready → Committing → Reconciling → Complete`. Preparing adds a destination streaming source, prepares registered collision/navigation, and suppresses destination rendering, shadows, local audio and NPC behavior. A persistent environment controller prevents a second global sun/sky from changing the current sector during preload. Ready requires actual sweeps, arrival clearance, needed nav paths, docking anchors and asset/PSO readiness; a streaming completion flag alone is insufficient.

At Committing, finish preceding interaction/damage commands, revalidate the current craft and destination, and pause integration at a movement/physics boundary. Capture current state and relocate craft, attachments and camera once without sweeping across the gap. Fixed anchors have no scale or motion. With `R = destination rotation × inverse(source rotation)`, map position by `destination origin + R × (position − source origin)`, and rotate orientation, linear velocity and angular velocity by R. Reset movement/interpolation/camera previous-transform caches before resuming on the next update.

The supplied helper implements only this mathematical mapping and rejects scaled/invalid anchors. Its tests are not proof of collision synchronization or seamless travel. Reconciling reveals the destination, restores control context, projects pursuit into the supported destination form, verifies identity/custody and finally releases source streaming. Before commit, cancel back to source. After commit, recover at a valid destination pad using captured state. On process restart use the newest intact compatible **saved** revision; unflushed memory is not durable progress.

Begin preload at measured worst-case speed multiplied by measured load time, plus braking distance and a margin. Test reverse crossings, maximum speed, contact, pause and same-frame damage. If readiness cannot be met on the laptop, reduce corridor speed or use explicit transit/loading. That fallback is preferable to shipping entry into absent collision.

## 16 Rendering and animation choices

Start with DX12 and a measured scalable rendering profile. Nanite is useful for suitable architecture, but instance count, materials and resolution still need measurement. Current5.8 documentation includes skeletal mesh support while explicitly stating that Morph Target deformation is unsupported with Nanite. Do not convert a facial rig that depends on morph targets merely because a mesh checkbox exists. Keep transparent pieces and incompatible deformation on a qualified conventional path. [E15](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine)

Begin with Lumen software tracing as a candidate profile and compare hardware tracing on the actual content/GPU before deciding. Hardware capability is not proof that hardware tracing is faster in an overlapping city. Epic's guide describes High/Epic console lighting goals of 60 / 30 fps and approximate 4 / 8 ms budgets at 1080p internal resolution; those are not whole-game or laptop promises. Current5.8 Medium behavior differs from older tutorials, so validate actual settings rather than copying an old scalability recipe. [E16](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-performance-guide-for-unreal-engine)

Lumen updates and representation have limits during fast movement. Thin walls, combined complex interior meshes and overlapping geometry can produce costly or incorrect results. Test one interior, one emissive night market, one glass-heavy view and one maximum-speed ascent before multiplying the art set. Inspect Lumen scene/surface-cache views to distinguish missing representation from an exposure or material problem. [E17](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine)

Use one compatible humanoid rig family for the first player, crew member and guard. IK Retargeting supplies chain/goal-based transfer tools; it does not guarantee convincing foot/hand contacts. Test start/stop, turns, stairs, carrying, aiming, seated posture and interaction reach against the actual collision capsule and sockets. Begin with a small locomotion state machine and authored montages; add Motion Matching only if the animation library and transition-quality problem justify it. [E18](https://dev.epicgames.com/documentation/en-us/unreal-engine/ik-rig-animation-retargeting-in-unreal-engine)

Keep hero identity in silhouette, materials, hair/head choices, clothing and props before creating many skeleton variants. Animation update/representation tiers should preserve named identity. A distant pedestrian need not run the same rig, tick rate and behavior as a speaking crewmate. Budget glass, emissive signs, dynamic lights, hair and shadows together; none is free because the building mesh is Nanite.

The proposed laptop qualification route starts at 1080p output with recorded internal resolution and scalable quality. Seek steady-state frame p95≤16.67ms, GPU p95≤13.3ms, game-thread p95≤10ms and p99 < 25 ms; investigate any unexplained100 ms stall. These are design targets, not measurements. If 60 fps fails, reduce visual scope and density before formalizing a 30 fps profile. Log power mode and thermals: laptop sustained behavior is more useful than a short editor screenshot.

## 17 Fast iteration without infrastructure overhead

Use a local SSD-backed DDC and leave it out of source control and authoritative backup. UE 5.4+ uses local Zen by default; derived data can be regenerated from source assets. Zen's unauthenticated service belongs on localhost or a trusted network, not an exposed internet endpoint. Record cache paths and size so the 70 GB free disk constraint does not disappear into unexplained storage growth. [E22](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-derived-data-cache-in-unreal-engine)

The observed build used UBA's local executor. UBA/Horde also supports distributing compiler work to other agents, but that requires server/agent/network operations. A solo laptop with no spare build machine gains little from establishing that infrastructure immediately. Keep the local default while it works; if an executor-specific hang is reproduced, record the log and compare a documented local executor fallback on the same source. Do not immediately fork the engine or buy build services. [E23](https://dev.epicgames.com/documentation/en-us/unreal-engine/horde-unreal-build-accelerator-and-remote-compilation-tutorial-for-unreal-engine)

Limit initial compile parallelism to 2 actions on 16 GB RAM and close unnecessary heavy editors during a clean build. The command-line override is recognized by the actual UBT log; UBT documents parallel-action controls. Increase it only after measuring peak committed memory and elapsed time. More logical cores do not guarantee a faster build once paging dominates. Reuse intermediates for ordinary iteration, but periodically prove a clean rebuild. [E24](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-configuration-for-unreal-engine)

Use Live Coding only for suitably small implementation iteration after the baseline works. Treat reflected header/layout changes, target/module dependencies and plugin changes as requiring a deliberate editor restart and full build. Never judge a release candidate by what happened to work in a long-lived editor process with stale state.

Use Unreal Python for bounded editor tasks such as intake metadata, approved asset edits and validation reports; it is not packaged game logic. Enable the editor plugin only when its first script is qualified, keep scripts versioned, and perform asset moves through Unreal APIs. A successful Python process without a report of changed assets is not acceptance. [E20](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python)

World Partition builders provide batch HLOD, nav and PCG operations. The PCG builder normally loads the whole world, with iterative cell loading available to constrain memory. On this machine, run one bounded district and use iterative generation when justified. Do not invoke all-world generation every time a gameplay line changes. Save builder logs, changed package lists and recipe hashes; stop on generation errors rather than submitting partial output. [E21](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-builder-commandlet-reference)

The developer command surface should stay small: Doctor, BuildEditor, TestFoundation, ValidateContent, BuildWorld, Package, RunRoute and BackupVerify. All accept the same EngineRoot/ProjectPath configuration and explicit overrides. They return a process status plus a machine-readable report. No wrapper may silently pick another engine, treat skipped tests as passed, clean outside a resolved workspace path, or upload/publish anything.

## 18 Build, automation, packaging and profiling gates

Unreal's automation framework supports C++ unit/feature tests and content/visual tests. Smoke tests should be short; five supplied tests verify stable identity, invalid IDs, rigid motion conservation, round-trip mapping and rejection without output mutation. They run in the editor test context under the `BorrowedSun.Smoke` names. They do not assert that the game is playable. [E26](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine)

Run the commandlet with the explicit project, unattended/no-source-control options, the BorrowedSun filter, an export directory and an exit condition. Epic documents command-line test filters and JSON report export. The runner must inspect the report: expected test names present, all completed successfully, no failures and no not-run items. Zero discovered tests is failure even when the process exits 0. NullRHI is suitable for the mathematical foundation tests, not rendering or gameplay performance. [E27](https://dev.epicgames.com/documentation/en-us/unreal-engine/run-automation-tests-in-unreal-engine)

Proposed gate sequence:

| Gate | Trigger | Evidence required |
|---|---|---|
| Source | Every bounded code change | Correct engine lock; compile; relevant fast tests |
| Content | Asset/recipe change | Manifest, ID and reference validation; actual Blueprint compilation |
| World | Structural district change | Generated collision/nav/HLOD outputs plus changed-package review |
| Package | First playable increment, then daily candidate | Win64 Development cook/stage/package; explicit real maps |
| Route | Candidate package | Launch, input, interaction, dock, flight, save/reload, clean exit |
| Recovery | Persistence/travel change | Corrupt-save fallback, interrupted transfer, duplicate commands |
| Performance | Content expansion or system change | Comparable recorded route and Insights trace |
| Release | Milestone candidate | Shipping package, fresh location launch, settings/save compatibility |

Packaging is a distinct build/cook/stage/package process, commonly driven by UAT BuildCookRun. Compiling an editor module does not qualify cooking, and a packaged executable without a real default map is not a playable deliverable. The starter intentionally contains no fake `.umap`. Author `/Game/BorrowedSun/Maps/Tests/L_Test_Startup`, set GameDefaultMap and the explicit cook list, then run the package gate. Include a visible Quit action before Shipping, where development console behavior is not a user interface. [E29](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project)

Gauntlet becomes useful once there is a packaged route worth repeatedly launching and observing. It orchestrates engine sessions; the project must still supply the gameplay route, completion signals and assertions. Start with a deterministic route harness and a small controller before adding a farm or dashboard. A process staying alive for sixty seconds is not a mission test. [E28](https://dev.epicgames.com/documentation/en-us/unreal-engine/gauntlet-automation-framework-in-unreal-engine)

Insights provides timing, memory and asset-loading analysis. Record engine/build identity, hardware/driver/power profile, map/content revision, resolution/scalability, route seed and trace channels for every comparison. Use a warm repeatable route for regression comparisons and a separate first-launch scenario for shader/PSO and cache behavior. Reserve full memory traces for targeted investigations when their overhead matters. [E25](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-insights-in-unreal-engine)

PSO precaching automatically gathers/compiles supported pipeline states asynchronously, with validation controls. It reduces a class of first-use work but is not proof that a route is hitch-free. Test initial combat, a new material, a new sky/sector and menu overlays in packaged first-use scenarios. Do not fix a hitch report solely by replaying until the driver's cache hides it. [E30](https://dev.epicgames.com/documentation/en-us/unreal-engine/pso-precaching-for-unreal-engine)

Suggested first route: thirty seconds walking through the block; one interaction with duplicate input injection; enter ship; short launch and return; dock; save; terminate normally; reload; verify custody, ship position and mission fact. Extend with pre/postcommit travel interruption only after that route works. Capture expected events as machine-readable milestones and bound every wait with a meaningful timeout/recovery reason.

## 19 Save compatibility and technical recovery

Unreal provides USaveGame objects and synchronous/asynchronous save functions. It does not choose the application's schema, transaction semantics or migration policy. Save a plain campaign snapshot with a header containing schema version, build ID, content manifest version and checksum; include world revision, identities, custody/charges, missions, ship state, district deltas, recipe versions, travel record, recent outcomes, high-water mark and permanent award keys. [E31](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine)

Serialize one immutable snapshot per write; do not mix values sampled on different frames. Serialize write requests so an older completion cannot overwrite a newer checkpoint. Keep two validated generations, each with revision/checksum metadata. On load, select the newest intact compatible saved generation, migrate in memory, validate invariants, then instantiate representations. An unsupported future schema produces a clear error instead of silently starting a new campaign.

Maintain fixtures for every shipped save version and explicit migrations for renamed definition IDs or retired placements. A missing optional decoration can be omitted; missing sunseed custody or a critical destination needs a defined recovery mapping. Transient command queues are discarded on restart; persisted outcomes/high-water state prevent old requests being replayed. Current input state and active animation are rebuilt from committed gameplay state.

Recovery tests include an interrupted write, one corrupt generation, both generations invalid, a rejected future schema, journal pruning followed by replay, reward under a new request ID, missing destination content, and a transfer interrupted on both sides of commit. A rollback procedure includes the matching source/config/content revision and save schema, not just yesterday's executable.

## 20 What the starter establishes and what remains

The supplied project establishes module boundaries, an explicit engine/plugin decision, canonical identity validation, fixed rigid-motion math, five registered automation tests and transparent content requirements. It contains no marketplace content, no purchased dependency, no binary map and no implementation of inventory, saves, ship control or streaming. That small scope makes real compiler/test feedback possible without claiming the design document has already become a game.

Next implement the foundation map and walking/input loop, then the journal plus one sunseed interaction, then stationary dock/ship control, then a packaged checkpoint route. After those gates pass, conduct the small Neo City intake and one block recipe. Each increment should leave a runnable package or a narrowly verified tool improvement. Only after the same block works on the actual machine should more art, crowd complexity and sector travel be added.



Research checked 12 October 2026. Scope: one solo creator, an initial cash ceiling below €500, Windows Unreal development, and the original Borrowed Sun design. This is a procurement and integration plan. No assets, installers, subscriptions or plug-ins were bought or downloaded during this research; no local runtime compatibility has been demonstrated. Vendor claims, observed prices and proposed acceptance tests are distinguished below.

## 21 The decision: prove the entire journey with a €0 asset basket

Continue from the delivered, owned C++ scaffold with separate runtime and editor modules. It is not a Third Person template project, and no Third Person character, input setup, animation or map is claimed to exist in it. For the next movement step, evaluate Unreal's Third Person template in a separate intake project and migrate only the chosen mannequin, animation and movement content into that scaffold. Cargo's free access path, Neo City, Mission to Minerva and Blender are later zero-cost candidates for architectural dressing, industrial and orbital shapes, a temporary ship and original sunseed/Kestrel blockouts. The project retains ownership of the sunseed custody contract, interaction, mission state, reliable saves, sector handoff, assisted flight and a meaningful return to Orison.

Add one free interior bundle only when the docked crew space becomes playable. Evaluate Arghanion's free flight sample separately before buying a flight plug-in. Use the Game Animation Sample as an isolated animation experiment after basic movement, rather than making a large sample project the foundation by default. These are recommendations for this project, not judgments that the larger samples are poor products.

The first paid purchase should solve a demonstrated bottleneck within the qualified-asset allowance of €120. A small prop purchase or REAPER may qualify once its tax-inclusive checkout total and value are established; neither is an automatic recommendation to spend. Larger city kits such as CyberPunk or Cyber District are deferred. They require an explicit budget reallocation after machine capacity and independent recovery needs have been funded or resolved. None is necessary to find out whether carrying the sunseed, boarding the ship, reaching orbit and returning to changed streets is compelling.

The approach protects what makes the game distinctive. Generic structure can be purchased. The sunseed's recognizable shape, Kestrel's asymmetry, the mirror silhouette, faction marks, foreground signs, crew identity and musical motif should be original. Spend creator time there after the complete loop works.

## 22 A budget that cannot quietly turn into a subscription stack

The authoritative budget is **Docs/Registers/budget.json: €480 total planning envelope**, with €0 spent during setup. Its allowances are €240 for memory, storage or independent backup; €120 for a qualified asset or production aid; €40 for operations or testing; and €80 uncommitted buffer. These are shared allowances, not separate budgets for this chapter. Reconcile any hosting, backup, AI, distribution or hardware spending against that register. The total stays below the user's €500 limit.

| Allowance | Ceiling | Release condition |
|---|---|---|
| Memory, storage or independent backup | €240 | Confirm compatibility and the limiting resource; prioritize recoverability |
| Qualified asset or production aid | €120 | Free prototype works; measured gap; inclusive checkout fits |
| Operations or testing | €40 | A concrete need cannot be met with existing tools |
| Uncommitted buffer | €80 | Retain for tax/FX variance, replacement or an evidenced bottleneck |

The initial asset basket still costs €0. Allowance changes must be recorded in the authoritative budget while preserving the €480 total. A larger city kit is eligible for reconsideration only after machine and recovery requirements are confirmed and funded or resolved, a reallocation is recorded, and the actual checkout fits. The asset register does not preallocate the buffer or hardware allowance to visual content.

Displayed US dollar prices below are observations, not euro quotes. The total that matters is the final local checkout amount, including applicable tax, payment-provider conversion charges and the entire committed subscription period. Do not calculate affordability by adding dollar storefront headlines and treating the answer as euros. The budget cannot promise hardware upgrades or months of paid services unless those are explicitly included.

For example, the direct store displayed CyberPunk at $245 and Cyber Streets at $95. That is $340 before the final checkout treatment and is outside the initial qualified-asset plan; it is not a proposed shopping basket. The alternative Fab channel displayed different “from” prices. Preserve the seller, channel and selected license tier alongside the quote; do not combine the cheapest headline from one channel with benefits assumed from another. [A09](https://kitbash3d.com/products/cyberpunk) [A11](https://kitbash3d.com/products/cyber-streets) [A53](https://www.fab.com/sellers/KitBash3D?q=cyber)

The initial recurring recommendation is **€0**. Purchased KitBash kits can be accessed through Cargo without paying for a Cargo subscription. A subscription is a separate procurement decision with different continuing rights; it is unnecessary for the proposed free and individually purchased path. [A04](https://help.kitbash3d.com/en/articles/7833678-how-to-access-purchased-kits)

## 23 Foundation and importing

### C01 — Unreal Third Person template: donor for the next movement step

Epic's template includes a controllable character, basic movement and jumping, mannequins and elementary geometry. Current documentation is for UE 5.8. Its price here is no separate template charge; the project's Unreal license remains applicable. [A01](https://dev.epicgames.com/documentation/unreal-engine/third-person-template-in-unreal-engine?lang=en-US)

Use a separate template intake project, then migrate only selected content into the existing two-module C++ scaffold. Do not replace that project's architecture or imply the template has already been imported. The first donor gate is a packaged doorway/stair/ledge route completed with keyboard and controller. Interaction, payload custody, possession, loading and audio contracts follow as separate owned steps. Keep animation behind the character interface so improving it does not rewrite missions. The replacement is a small owned Character/Pawn implementation.

### C02 — Cargo: choose the free import path, qualify the connector

Cargo currently lists official Launcher UE 5.2–5.8 and Blender.org builds 4.2–5.2; source-built Unreal is unsupported. Its September 2026 product update lists version 3.2.1 and an option to import standard Blueprints instead of Packed Level Actors. Record the actual installed connector revision rather than assuming a generic “Cargo 3” label is enough. [A02](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines) [A48](https://kitbash3d.com/pages/product-updates)

Import one asset into a disposable intake project, identify every generated dependency, then package a small destination project. Inspect collision, material instances, pivot, scale and assembly structure. Do not put mission logic inside the imported vendor assembly. Prefer a project-owned wrapper containing interaction and state. If the connector fails, use an approved vendor-native package/manual workflow while retaining the same audit. Do not make undocumented registry edits the normal onboarding process.

### A compatible version is a tested combination

Choose the engine patch, compiler, Cargo connector and asset revisions together. Vendor support means the combination is a reasonable candidate; it does not prove this game's final package works. For each promoted asset, record the import settings, engine version, package hash, selected resolution and accepted license. A future upgrade gets a new intake branch and the same smoke test. Never convert the only copy of the working project in place.

The register marks all candidates as researched, not acquired or installed. Most store pages did not expose an exact current asset revision or every supported engine version. Those fields deliberately remain unresolved. An attractive screenshot, “Unreal” file format or high review score is not evidence of compatibility with the selected patch.

## 24 City, industrial spaces, orbit and the crew home

### C03 — Neo City: first architectural trial

The official page displays $0, nine models and sixty PBR materials. It is a compact selection of urban forms, not a complete playable city. [A07](https://kitbash3d.com/products/mini-kit-neo-city)

Use a handful of pieces around an authored intersection, with one roof route, doorway, service socket and landing sightline. Adjust materials toward chalk, copper and indigo; make the clinic legible through original signage and human activity. Test the large forms in daylight, twilight and an unpowered state. If they resist the intended scale, keep them in the skyline and construct simple playable frontage in Blender. The free label does not replace retaining the actual free-offering Usage Rights.

### C04 — Mission to Minerva: first industrial and orbital donor

KitBash's direct page displays $0 and a Gameplay Ready designation. Its space settlement collection includes habitats, walkways, landing structures, machinery and a cargo ship. [A08](https://kitbash3d.com/products/mission-to-minerva)

This is the highest-value companion to Neo City for Borrowed Sun. Select a landing pad, gantry, habitat wall, crane and temporary ship. Place them around the mirror approach and dock rather than importing an entire colony. The advertised collection is large, so source-library size should not become runtime residency. Gate the selected subset at flight speed and at pedestrian distance. Its cargo ship is a visual placeholder; no ship-handling implementation is implied. Replace the hero silhouette with Kestrel once route dimensions stabilize.

### C05 — CyberPunk: deferred architecture after budget reallocation

The official direct listing displays $245, 198 models and a Gameplay Ready badge. It advertises Nanite, collision and runtime assembly preparation. These are vendor claims, not a local performance result. [A09](https://kitbash3d.com/products/cyberpunk)

This kit is not an initial shopping recommendation within the €120 asset allowance. Reconsider it only after machine capacity and recovery requirements are confirmed and funded or resolved, the budget records a reallocation, and the inclusive checkout fits. The representative block must also prove a need for specific shapes: clinic frontage, lift wall, rooftop exit, launch corridor and vista. A purchase earns its place if those shapes remove substantial authoring work without forcing oversized streets or inaccessible interiors. Test facade, roof and collision cases after acquisition. Replacement: Neo City massing and a smaller original facade grammar.

### C06 — Cyber District: choose instead of CyberPunk if the route needs it

The direct page displays $245, 241 models and Gameplay Ready preparation. [A10](https://kitbash3d.com/products/cyber-district)

Its street-oriented science fiction vocabulary may suit a market-heavy slice. It has the same deferral and budget-reallocation gate as CyberPunk. Compare them through the actual block recipe, not model counts; select at most one after the larger purchase becomes eligible. Duplicate coverage would consume most of the initial budget while adding little playable capability. Replacement: the other kit or the free blockout. Gate: written piece-to-route mapping, resolved machine/recovery funding, recorded reallocation and an inclusive euro quote before purchase.

### C07 — Cyber Streets: conditional small purchase after a gap list

The direct page displays $95 and a Gameplay Ready badge for its street-level prop collection. [A11](https://kitbash3d.com/products/cyber-streets)

Require an inclusive checkout within the remaining €120 asset allowance and a list of ten missing objects with a gameplay or storytelling purpose: a service counter, useful cover, a readable queue boundary, repair work or an inhabited balcony. Reject additions that only make every view noisier. Retain one visual language for signs, containers and repair marks across all vendors. Prototype substitutes include Neo City pieces, original awnings and simple crates. Gate: a before/after route test showing improved recognition without new collision snags or excessive material variation.

### C08 — Modular Scifi Season 2 Starter Bundle: first interior trial

Jonathon Frederick's Fab listing currently says Free. It includes the Engineer Hallways, Props and Interiors collections and examples. The description originates in the UE4 period; current compatibility is not established by the listing text. [A13](https://www.fab.com/listings/cb3c4494-4060-4a80-b079-e46936cb8dd0)

Choose this one free interior trial for the docked Kestrel. Build a single walkable room with three crew stations and a cargo cradle. Recolor and simplify it so the home feels repaired and personal rather than like an anonymous laboratory. Test doorway height, camera clearance, lighting, collisions and a packaged reload. Preserve the precise acquisition license: a legacy marketplace asset should not automatically be described as Fab Standard. Replacement: an original box room with a small trim sheet.

### C09 — Modular SciFi Season 1 Starter Bundle: free fallback

The other Jonathon Frederick bundle is also currently Free on Fab and includes modular hallways, interiors, props and command-center content. [A12](https://www.fab.com/listings/86913335-3c75-42bf-8404-54fe9d9d7396)

Evaluate it only if the Season 2 trial fails the chosen interior look or geometry. There is no production benefit in loading two broad legacy libraries before one room works. If it succeeds, migrate only the selected room pieces and their dependencies. Do not buy the individual packs merely because their separate listings show a price; first check whether the required content already exists in the free bundle. Version and license acceptance follow the same intake gate as C08.

### C10 — Spaceship Interior Environment Set: a researched reserve

Denys Rutkovskyi's Fab listing describes modular room, corridor and cargo-area pieces, props and a demonstration scene. It still references a UE 4.18 update. The current price and full modern compatibility range were not exposed in the accessible listing; neither is guessed here. [A14](https://www.fab.com/listings/e55bb035-8720-487f-8a36-4ce50d16f344?lang=en)

This is a fallback for a specific visual gap, not an initial buy. Resolve the exact price, rights and engine version before considering it. Its test is the same third-person doorway and crew-room test, which makes alternatives comparable. No interior pack removes the need for original crew possessions, conversation staging or the ship's recurring sense of home.

### C11 — Spaceships: Armada individual vehicles: defer

KitBash's vehicle catalog displays $75 for individual Armada vehicles, including Fighter, Scout and larger ships. That is an individual vehicle observation, not the cost of an entire collection. [A15](https://kitbash3d.com/collections/vehicles)

One enemy silhouette may eventually save useful time. The initial slice needs only a proxy pursuer whose behavior can be read. Buying several ships before interception is enjoyable would multiply material, collision and effect work. Inspect engine locations, weapon mounts, scale, collision and memory before adopting one. An exterior model does not prove a navigable interior, articulated landing gear, damage states or flight controls. The hero ship should remain recognizably original.

### C12 and C13 — Poly Haven first; ambientCG only for a missing surface

Poly Haven supplies downloadable HDRIs, textures and models under CC0; its website content and access terms are separate. ambientCG also publishes assets under CC0. Both are free alternatives to immediately subscribing to a material library. [A16](https://polyhaven.com/license) [A17](https://docs.ambientcg.com/license/)

Choose Poly Haven as the first search location and acquire only a few named surfaces. Start with plaster, worn metal, concrete, fabric and one utility surface. Use ambientCG when a required surface is missing, rather than duplicating both catalogs. Test map channel packing, normal orientation, roughness and texture resolution under the game's lighting. Flat colors and a handmade trim sheet remain valid replacements. These libraries supply inputs; a coherent palette and consistent texel density remain original production decisions.

## 25 Animation, characters and flight

### C14 — Game Animation Sample: high-value donor, substantial integration choice

Epic's 12 August 2026 update explicitly targets UE 5.8 and adds several advanced animation examples; its Look-At solver is described as Experimental. Fab currently lists the sample as Free. [A18](https://www.unrealengine.com/tech-blog/download-the-latest-game-animation-sample-project-now-updated-for-ue-5-8) [A49](https://www.fab.com/category/tutorials-examples?lang=en)

Run a bounded comparison: existing movement versus one useful imported stop, turn or mantle solution. Evaluate responsiveness, interruption, foot contact, required assets and CPU cost. Keep only a demonstrated improvement. Do not adopt ragdolls, takedowns, multiplayer interactions and new movement architecture solely because they arrive in the same download. Borrowed Sun's core must survive animation replacement. Replacement: the template state machine plus a small retargeted or authored set.

### C15 — Lyra: reference for a specific problem

Lyra is Epic's evolving modular sample with substantial gameplay architecture; its current Fab catalog price is Free. [A19](https://dev.epicgames.com/documentation/unreal-engine/lyra-sample-game-in-unreal-engine?lang=en-US) [A49](https://www.fab.com/category/tutorials-examples?lang=en)

Use it to study one implementation when the need exists, such as a UI or input pattern. Adopting its entire architecture would be a consequential decision for an offline, tightly scoped adventure. A solo creator should be able to explain the imported dependency graph and package a small extracted example before accepting it. The replacement is the owned project foundation. Familiarity can change this choice: a creator already shipping Lyra projects may have lower integration cost than a beginner.

### C16 — City Sample: learn from it without inheriting its city

Epic documents a combination of World Partition, lighting, traffic, crowds and other systems. Its recommended machine includes 64GB RAM and a powerful GPU; it is explicitly a demanding sample. It is currently Free in the Fab catalog. [A20](https://dev.epicgames.com/documentation/unreal-engine/city-sample-project-unreal-engine-demonstration?lang=en-US) [A49](https://www.fab.com/category/tutorials-examples?lang=en)

Read its documentation first. Downloading and understanding a huge urban example is not the fastest way to author one consequential market route. Use a separate project to inspect an individual technique only when a measured problem calls for it. Do not import its whole crowd or vehicle simulation to make Orison feel alive. A few authored workers, readable routines and sound layers can meet the slice's emotional requirement with much less integration.

### C17 — Content Examples: the default reference library

Epic's Content Examples provides focused demonstrations of engine techniques; its Fab listing explicitly limits use to Unreal Engine-based products. The sample is Free. [A21](https://dev.epicgames.com/documentation/en-us/unreal-engine/content-examples-sample-project-for-unreal-engine) [A51](https://www.fab.com/listings/4d251261-d98c-48e2-baee-8f4e47c67091)

Use it like a technical reference. Find the one behavior needed, reproduce it in a small map, explain it, then adapt it to the game's contracts. A teaching example is not automatically optimized, accessible or robust under save/load. Keep the source sample separate from the game. This resource should usually precede a new paid effects or interaction pack. Replacement: a similarly small first-party experiment built from documentation.

### C18 — MetaHuman: defer until one hero needs a face

Current MetaHuman licensing is included in the standard Unreal license, with the stated revenue threshold and relevant AI terms. This differs from older blanket assumptions about how MetaHuman could be used. [A22](https://www.metahuman.com/license?lang=en-US)

Prototype with mannequins. Later, qualify exactly one hero under the actual dialogue camera and street lighting. Measure skin, hair, shadow and animation costs; evaluate distant LODs and whether the result matches the intended stylized realism. The two adapted lineages should share a verified rig wherever feasible. Do not create a crowd of expensive faces before establishing a character budget. Replacement: a deliberately simpler original character sharing the approved skeleton.

### C19 — Mixamo: fill an animation gap, not an entire personality

Adobe's FAQ says Mixamo is free with an Adobe ID and permits royalty-free characters and animations in games; the auto-rigging scope is bipedal humanoids. Additional and general Adobe terms govern the service. [A23](https://helpx.adobe.com/creative-cloud/faq/mixamo-faq.html) [A24](https://wwwimages2.adobe.com/content/dam/cc/en/legal/servicetou/Mixamo-Addl-Terms-en_US-20210623.pdf)

Use one clip to solve one need: an idle, short gesture or temporary civilian action. Verify root motion, skeleton retargeting, hand contact and foot sliding before acquiring a batch. Store the original export and retarget settings. Do not distribute raw animation libraries with the public workspace. The crew's important gestures should eventually be adapted to biography and scene context. Replacement: GASP donor motion or a small authored Control Rig animation.

### C20 — Arghanion's Space Ship Flight System: the first external flight trial

This Fab sample is advertised as Free, built for UE 5.8 and entirely Blueprint. It includes physics-based flight, boarding, landing and cargo examples. The included modified Corinth Cargo Transporter is credited under CC BY 4.0; the listing also marks AI usage “No.” These are source facts, not validation of this game's controller or save behavior. [A50](https://www.fab.com/listings/62a69df5-b447-415d-8d91-82f9f1a46894)

Timebox a separate-project evaluation. Try keyboard handling first, then controller, hover near a balcony, an authored landing pad and several frame rates. Disable unsupported midflight exit. Replace loose demo cargo behavior with the game's one-custodian sunseed contract. Test possession, save/load and sector travel before adopting any module. Resolve actual NoAI terms before giving downloaded code or assets to an AI tool. Replacement: a small owned arcade-flight Pawn. A successful trial earns selective reuse, not automatic ownership of the whole game architecture.

### C21 — SixDOF Movement: paid fallback only

mookie's Fab listing describes a movement component that can transform an actor or apply physics forces. The seller catalog displayed a starting price of $19.99; the exact tier and current engine compatibility were not exposed. [A25](https://www.fab.com/listings/01288642-e34b-408e-816d-f6a5a31944bc) [A26](https://www.fab.com/sellers/mookie?lang=fr)

Consider it only if the free experiment or owned movement component cannot meet a defined requirement economically. Confirm supported engine binaries/source, maintenance and packaging before buying. The game needs hover, assisted turning, readable braking and repeatable docking; a six-axis component does not by itself implement those experiences. Keep mission state and ship presentation outside the movement plug-in so replacing it is possible. Avoid buying aerodynamic simulation for a ship whose fiction and controls deliberately allow hover and space maneuvering.

## 26 Authoring tools

### C22 — Blender 4.5 LTS: the one essential external DCC

Blender's official LTS page lists 4.5.14 from 15 September 2026 and support through July 2027. It is free software; the GPL for the application does not force the creator's artwork under GPL. Cargo lists the relevant Blender.org version range as supported. [A27](https://www.blender.org/releases/4-5/) [A28](https://www.blender.org/about/license/) [A02](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines)

Pin a tested official build. Use it for the sunseed, Kestrel proxy, collision corrections, UVs and a few facade modules. First round trip: a meter-scaled object with known pivot, a collision mesh and a simple material, compared in Unreal. Preserve editable originals outside runtime Content. Avoid accumulating paid modeling add-ons before the basic export loop is dependable. Replacement for a temporary shape: Unreal's own modeling tools.

### C23 — Krita: choose for signs and small texture work

Krita is GPL software that permits commercial artwork; its application license does not claim the artist's output. The official free distribution is sufficient. [A29](https://krita.org/en/about/license/)

Use it for original signs, faction marks, painted masks and decal sheets. Make a single sign-language guide and a small atlas instead of dozens of unrelated textures. Test legibility after mipmapping and under both power states. Keep editable files and flattened exports together by stable asset ID. This is a task-triggered addition, not an instruction to install a second painting package if the creator already has a suitable licensed tool. Replacement: simple UMG text or material masks during prototyping.

### C24 — Substance 3D Painter: optional after a measured texture bottleneck

Adobe's current plans page displayed the US individual Collection at $59.99 per month. A 2025 announcement described a cheaper Texturing plan, but that historical statement is not a verified current local checkout offer. No cheaper current subscription or perpetual price is assumed here. [A30](https://www.adobe.com/products/substance3d/plans.html) [A54](https://blog.adobe.com/en/publish/2025/02/20/substance-3d-innovations-pricing-updates)

Use Blender, Krita and Unreal material instances first. Reconsider Painter when repeated hero-asset texture work has a measurable cost and the creator can immediately complete several approved assets within the purchased period. Check full contractual commitment, cancellation and asset-library terms. Do not use a noncommercial educational entitlement for the commercial project. Bake portable outputs and keep source maps so the runtime does not require a subscription.

### C25 — Houdini Indie: powerful, outside the initial stack

SideFX currently displays $299 for one year or $449 for two years. Indie has revenue, funding, entity and file-format restrictions; the vendor page specifies these conditions. [A31](https://www.sidefx.com/products/houdini-indie/)

The relevant decision is whether repeated authoring work justifies another procedural environment. One approved city block does not yet justify a city-generation pipeline. Start with hand-authored modules, Blueprint construction and later Unreal PCG. Reconsider Houdini when the creator can demonstrate the same bounded operation being repeated many times and can maintain the graph. Qualification requires deterministic baked outputs and a packaged Unreal build without an accidental live authoring dependency. Do not substitute Apprentice's learning entitlement for production rights.

### C38 — Unreal PCG: automate the proven recipe later

Epic describes PCG as an in-engine graph framework for procedural tools and content, requiring the relevant plug-in. [A45](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-overview)

Adopt it after one district assembly has passed gameplay review. Good first tasks are roof utilities, cable dressing or distant facade variation. Keep mission entrances, cover, landing pads and traversal paths authored. Use fixed seeds, exclusion zones and a way to inspect generated instances. Gate generation against a known route and test cooking and reload. A small manual assembly remains the replacement. The objective is fewer repeated edits, not a procedural promise that outgrows the game's authored mission design.

## 27 Audio and music

### C26 — Audacity: enough for initial editing

Audacity is a free open-source audio editor available for commercial work. Rights to imported recordings remain separate from the software license. [A32](https://manual.audacityteam.org/man/faq_about_audacity.html)

Use it to trim, clean and loop selected effects, normalize exports deliberately and record scratch dialogue. Keep original recordings untouched. Establish one consistent WAV export convention and a naming scheme connected to gameplay events. Gate every loop in the actual game, including transitions and distance attenuation, because a clean file can still produce an unpleasant mix. Avoid installing optional cloud or generative services simply because the desktop editor is useful.

### C27 — REAPER: the most defensible small paid tool, if needed

Cockos displays $60 for a qualifying discounted license or $225 commercial, excluding taxes. The discounted commercial route includes a yearly gross revenue condition of no more than US$20,000; the full-function evaluation is sixty days. [A33](https://www.cockos.com/reaper/purchase.php?l=1)

Choose it when multitrack engine layers, musical stems or dialogue editing exceed the basic editor workflow. Evaluate using one complete sound-design task, not by collecting plug-ins. Keep projects portable, freeze or render any third-party effects, and export clean stems. An evaluation is not a permanent free license. Replacement: Audacity and simpler engine-side mixing until the work warrants the cost.

### C28 — Sonniss GDC 2026 bundle: first effects source

The official bundle page offers free commercially usable effects without required attribution. Its separate bundle license prohibits AI training. [A34](https://gdc.sonniss.com/) [A35](https://sonniss.com/gdc-bundle-license/)

Choose a small set: mechanical loop, impact, servo, metal footstep, cloth movement, warning, socket and atmosphere layers. Do not download or index hundreds of gigabytes as a substitute for designing the scene. Track original publisher and filename even where public credit is optional. Keep raw recordings outside public source packages. Test peaks, loops, repetition, distance and simultaneous playback. Replace missing signature sounds with original recordings and synthesis; the sunseed should not simply sound like a recognizable stock notification.

### C29 — Freesound: targeted fallback with per-file decisions

Freesound hosts recordings under different licenses, including CC0, CC BY and CC BY-NC, and acknowledges uploader-provenance limitations. The entire site is not one commercially cleared library. [A36](https://freesound.org/help/faq/)

Initially accept only clearly sourced CC0 or CC BY files with complete attribution records; exclude noncommercial, legacy or unclear entries. Record author, title, source URL, license version and modifications. Reject recognizable film dialogue, songs, or recordings whose surrounding rights are unresolved. Search for a named missing sound, then stop. Do not build a production dependency on automated catalog harvesting. Replacement: the vetted Sonniss subset or an original recording.

### C30 — Incompetech: a temporary music option with visible credits

Kevin MacLeod's FAQ describes the attribution route, with guidance for games and other media. Select the actual track and its terms before using it. [A37](https://incompetech.com/music/royalty-free/faq.html)

This can provide a lawful temp cue while pacing is tested. It does not supply an exclusive Borrowed Sun identity or guarantee separated stems. Preserve the required credit in both the game and any applicable video description, including modification notes where required. Avoid composing the whole sequence around a temp track that cannot support the final interactive transitions. The best final replacement is a commissioned original motif with explicit game, trailer, soundtrack and streaming rights, after the slice earns that investment.

### C31 — Unreal audio and MetaSounds: choose the native runtime

Epic's MetaSounds documentation covers procedural sound authoring, templates and music-related workflows. It is part of the Unreal toolset; every imported recording still needs its own rights. [A38](https://dev.epicgames.com/documentation/unreal-engine/metasounds-in-unreal-engine)

Begin with ordinary sound playback where adequate. Add MetaSounds for useful continuous behavior such as hover-to-cruise engine layers or a power circuit responding to load. Set concurrency, ducking, pause and restart policy before filling the city with emitters. Gate a flight sequence followed by landing and reload: loops must stop, dialogue must remain intelligible and the same engine must not stack twice. A simple sound component is the fallback, not an automatic second middleware system.

### C32 — FMOD: credible future alternative, unnecessary initially

FMOD's limited-commercial terms include a project budget below US$600,000, developer gross revenue/funding below US$200,000 per year, project registration and attribution, alongside other conditions. Free eligibility must be checked against the actual entity. [A39](https://fmod.com/legal)

Defer it while native audio meets the design. Consider it when a sound collaborator already works in FMOD or a demonstrated interactive-music requirement makes the extra build pipeline worthwhile. Qualification includes matching integration, bank and runtime versions, a clean packaged build and a missing-bank fallback. Do not use both audio runtimes broadly merely to keep options open. Choose one authoritative event system for the shipped game.

## 28 UI, fonts and narrative

### C33 — Kenney Input Prompts: choose a small set

Kenney's Input Prompts is offered as free CC0 artwork. [A40](https://kenney.nl/assets/input-prompts)

Import only the symbols used by supported keyboard, mouse and controller layouts. Connect them to actual remapped actions, not hardcoded tutorial text. Test a device switch while a prompt is visible, enlarged UI and small-screen readability. Preserve a plain-text fallback for unrecognized devices. Replace decorative platform assumptions with the correct prompt family during future platform work. The asset solves drawing time; the interaction binding remains project-owned.

### C34 — Lucide: optional general-purpose symbols

Lucide is licensed under ISC, with additional inherited notices included in its license file. Retain the applicable full notices when redistributing selected assets. [A41](https://github.com/lucide-icons/lucide/blob/main/LICENSE)

Use a few clear symbols for settings, volume, journal and navigation. Export a fixed subset with a consistent stroke weight; do not add a web icon runtime to Unreal. Critical game concepts such as the routing lease, capacitor charge and cargo custodian need labels and, eventually, original symbols. Test eight necessary icons at their real HUD size against bright and dark scenes. Replacement: text labels, which are often clearer in the prototype.

### C35 — Inter: one readable interface font

Inter is a free font under the SIL Open Font License 1.1. [A42](https://rsms.me/inter/)

Choose a small number of static weights and bundle the font locally so the offline game has no web-font dependency. Preserve its license. Test subtitle expansion, speaker names, digits, punctuation and the longest settings labels. Inter is not a guarantee of coverage for every future script; add an appropriately licensed fallback after localization languages are selected. Avoid designing a multilingual city through random glyph substitution. Replacement: the engine's basic font until the text layout works.

### C36 — ink and Inky: useful for the story rehearsal

inkle offers ink and Inky under MIT; Inky can export compiled Ink JSON and a playable web version. The linked Unreal integration is a separate project requiring its own qualification. [A43](https://www.inklestudios.com/ink/)

Use a text rehearsal to test the clinic-versus-lift choice, crew disagreement and the return beat. Keep effects explicit: grant lease, mark promise, update contact. Compiled Ink JSON is the input of an Ink runtime, not a generic dialogue table Unreal understands automatically. For the bounded slice, manually author or convert approved content into the project's validated dialogue schema. Adopt a runtime plug-in only after a separate test. Replacement: plain versioned dialogue data and a minimal previewer.

### C37 — articy:draft X FREE: an alternative visual authoring route

The current free product allows commercial use and exports, with a limit of 700 objects per project. Exceeding the cap can lock the project until content is reduced or the license changes. [A44](https://www.articy.com/en/articydraft/free/)

Choose it instead of Inky if visual flow editing materially improves the creator's writing process. Count objects during the opening chapter; do not discover the limit halfway through a campaign. Test one exported conversation and reconstruct its conditions without relying on an unqualified importer. Keep narrative IDs stable across revisions. The slice should have one narrative source of truth. Maintaining the same branching content simultaneously in articy, ink, documents and hand-edited Unreal graphs would erase the intended time savings.

## 29 The integration workflow that makes purchased assets efficient

### Accept a capability, then accept an asset

Every intake ticket begins with a missing capability: a readable clinic entrance, a ship doorway, a stop animation or a boost sound. Attach the candidate, the replacement, the proposed timebox and an acceptance scene. “It looks impressive” is not an acceptance criterion. An asset that saves mesh creation but adds days of repair may still be the wrong purchase.

For the representative block, define the walking capsule, cover height, mantle height, camera space, ship approach width and sightlines before dressing. These are project dimensions to establish through play, not vendor dimensions to adopt blindly. Layout and silhouette must survive replacing all materials with flat colors. Add hand-scale detail only after the route remains understandable.

### Quarantine, promote and preserve

Keep vendor source archives, license records and untouched source projects outside the runtime game. Maintain a separate intake project for each engine-version family. Promote only approved content and its required dependencies. Project wrappers own gameplay; vendor assemblies own presentation. Keep stable IDs so missions reference a clinic interaction point rather than a particular vendor mesh name.

The asset register records a candidate, not entitlement. After an authorized acquisition, add the invoice or free receipt, license text, entity/tier, exact revision, file hash, attribution, source location and destination path. Update local test status only after the test is run. An explicit failure is useful: it prevents repeatedly reconsidering a rejected pack.

Do not edit vendor masters casually. Create derived material instances or project variants with documented changes. Maintain a replacement path so the game can still package with a proxy. Keep source-level access private where the license requires it. A public repository can contain original systems and instructions without containing commercial models, textures, motion files or audio libraries.

### Use a fixed qualification scene

The environment test contains one daylight/twilight block, a narrow doorway, a roof, a service socket, a landing approach and a distant view. Walk and fly through it in a packaged build. Capture frame timing, memory residency, collision trouble and visual mismatch. Test the representative instance count that the actual slice needs. Inspect shadows and material complexity; a low polygon count alone does not decide runtime cost.

The animation test uses the same body and terrain for every candidate. Check start/stop, turn, jump, mantle, interruption, aim and a short conversation. The audio test includes silence, overlapping speech, engine layers, pause and reload. UI tests include remapping, long labels, large subtitles and device changes. Save the evidence beside the acceptance ticket.

These tests are proposed gates, not completed benchmarks. Their purpose is to uncover integration cost before the project depends on the asset. Once a candidate passes, stop repeatedly testing unrelated cases unless new usage creates a new risk.

### Make the first two weeks deliberately small

The first playable room can be boxes. The first flight can use a wedge. Day-one success is a repeatable packaged build and a core interaction, not a photorealistic city. Next, create one complete street-to-orbit-to-street loop with the same mission state. Only then replace a few proxy objects with the free library subset and measure the difference.

Use the first interior trial for a single scene with the crew. Use the first external flight sample only to answer whether it reduces the cost of the required handling. Keep a daily note of time spent creating, integrating, debugging and searching. If acquisition research begins taking more time than implementing the next playable step, close the storefronts and work from the current basket.

## 30 Rights and AI boundaries in this stack

There is no blanket statement that a purchased asset may be uploaded to an AI service, used as generative input or included in training. These are distinct uses. KitBash's current EULA contains restrictions on AI inputs/training in section 3.2.7 and specific free-offering conditions in section 3.2.22. Review the precise rights retained at acquisition. [A47](https://kitbash3d.com/pages/license-text)

KitBash's licensing overview distinguishes perpetual and subscription rights and ties individual eligibility to a genuinely single-person entity; bringing contractors into the business can change the applicable tier. Subscription cancellation does not create an unrestricted reusable raw-asset library. [A05](https://kitbash3d.com/pages/licenses) [A52](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs)

Fab's Standard summary permits commercial incorporated use and restricts standalone redistribution, but a product may instead use another license or carry separate components. Preserve the actual acquisition record rather than treating a marketplace badge as the whole contract. A NoAI field is a reason to resolve the applicable conditions, not permission to infer that every non-training upload is approved. [A46](https://www.fab.com/eula)

AI assistance can still work on original specifications, generic implementation, validation scripts, naming, documentation and project-owned code. For asset-specific automation, prefer local deterministic scripts operating within the permitted workflow. Ask for bounded transformations with a reversible output and compare the result. Do not expose licensed raw files merely to let an assistant inspect their appearance or infer a shader.

## 31 What not to buy yet

Do not buy multiple primary city kits, a full ship fleet, a crowd framework, a dialogue megasystem, a planet generator or an all-in-one multiplayer space project for this slice. Do not subscribe to Cargo, Substance and Houdini together under the initial cap. Do not purchase cinematic quality that is invisible at the actual gameplay camera. Do not buy individually what the selected free bundle already contains.

Do not add an asset because its demo suggests a feature outside the approved design. A walkable moving ship, dynamic boarding, full planets, universal crowds and procedural conversations each introduce new engineering contracts. They belong behind separate expansion gates. The asset strategy succeeds when each accepted dependency reduces the time required to ship the specific playable journey.

The machine-readable register contains thirty-eight researched candidates with observed-price status, license notes, version uncertainty, integration risk, replacement and a local test gate. Its first basket remains zero-cost and subordinate to the €480 authoritative budget. External candidate acquisition and qualification remain pending. The delivered C++ scaffold and any executed engine checks are described separately in the delivery validation record; this chapter does not claim that sample content is already integrated.




## 32 Development operations: protect the project without building a studio-sized platform

The initial infrastructure decision is local Git plus Git LFS, a reviewed engine lock, a few PowerShell entrypoints, and verified backups on storage separate from the working copy. Keep the task board, design decisions, asset register and test evidence in files that remain useful without a subscription. A private GitHub repository is an optional second location after checking asset rights, repository size and spending controls. A cloud build service, Perforce server, always-on AI agent, remote Derived Data Cache, Kubernetes cluster and elaborate release platform are not prerequisites for the first playable experiment.

This is a budget decision as much as an engineering decision. The user's initial budget is **under €500**, not a commitment to spend €500 and not a production budget for the finished game. The delivered scripts require no purchased infrastructure. Preserve that cash until a measured obstacle appears: reliable backup capacity, a narrowly selected asset that saves real production time, or one specific review the creator cannot perform. Existing hardware, internet, electricity and already-owned software are assumed available; they are not claimed to be free. No subscription, paid account, remote repository, service connection or tool installation has been created by this work.

The operations system has one purpose: make a small game change repeatable, inspectable and recoverable. Its success measures are time from task to tested change, number of avoidable integration failures, time to restore a known build, and time lost tending the tools. If upkeep consumes more than one of the twenty focused weekly production hours for two successive weeks, remove optional machinery or correct the recurring cause. Do not optimize for a large company's dashboard count.

## 33 Repository contract and the first trustworthy baseline

`Game/BorrowedSun.uproject` is the project entrypoint. `Game/Source`, authored `Game/Config`, intended content and required platform resources are source. `Tools` holds operations scripts; `Config` holds engine and plugin decisions. Documentation and task records belong beside the source so the commit can explain why the behavior exists. Local machine paths live in ignored `Tools/Environment.local.json`; the tracked example contains defaults and the minimum smoke-test contract. No credentials belong in either file.

Generated outputs are different. Editor autosaves, intermediate compiler output, local binaries, caches, IDE state and packaged builds should not expand the ordinary project history. Epic's directory documentation distinguishes authored configuration and source from generated directories, while its DDC documentation explains that derived data is regenerated and cached rather than checked into source control. The exception is a dependency delivered only as binary files: its required files need an explicit reviewed exception and acquisition record rather than silently disappearing behind a broad ignore rule. [D19](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-directory-structure) [D15](https://dev.epicgames.com/documentation/unreal-engine/using-derived-data-cache-in-unreal-engine?lang=en-US)

Establish the first baseline in this order: inspect the delivered files; configure the engine locally; compile editor and game; run the five expected smoke tests; inspect the ignored-file rules; confirm `.uasset` and `.umap` are assigned to LFS; make the reviewed initial commit; produce an external verified backup. A generated source archive is a starting artifact, not proof that the project can be rebuilt or restored. Until that sequence is complete, label the workspace a foundation under validation.

The branch policy is deliberately small. Keep one main branch that can compile and pass its defined checks. Make a short branch for each consequential feature, engine upgrade or asset import. Prefer one coherent change that can be completed in one or two work sessions. Large uncommitted art experiments belong in a sandbox with their own backup, followed by a selected import. A branch is not a license to leave broken integration work hanging for weeks.

Before accepting a change, read its file list and the text diff, inspect changed binary assets in the editor, run the appropriate checks, and commit source and dependent assets together. A local review note is sufficient for a solo creator; a hosted pull request is optional. Record the task ID in the commit message and the commit in the task's evidence field. Avoid requiring a human to maintain the same status in three tools.

## 34 Git LFS now; Perforce when an observed problem justifies it

Git gives the solo creator a compact local history, familiar code review and no mandatory always-on server. Git LFS places large payloads outside ordinary Git objects, with pointer records in the repository. That split means a successful Git clone or source ZIP does not by itself prove that the corresponding art files are available. GitHub currently lists a 2 GB per-file LFS limit for Free and Pro, 4 GB for Team and 5 GB for Enterprise Cloud. Ordinary GitHub repositories warn above 50 MiB per file and block files above 100 MiB; those are not recommendations for how large individual Unreal assets should become. [D07](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-git-large-file-storage) [D08](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github)

The supplied attributes assign Unreal binary assets and relevant authoring formats to LFS before the first import. Establish this before committing a purchased kit: retroactively moving a large existing history requires migration and coordination. Confirm the rules on one harmless test asset, make a small commit, and inspect `git lfs ls-files` before admitting gigabytes of content. Keep original vendor archives outside the game repository. Import the selected project-ready subset, with provenance pointing back to the controlled original. Do not create three complete copies of every unused kit within active source.

Treat binary editing as ownership. When another person is involved, claim the task and acquire the applicable remote LFS lock before changing an asset; save and inspect it; submit its dependencies; then release the lock. LFS locking requires an appropriate server. Its documented `locksverify` setting matters: explicitly enabled verification fails a push on lock conflicts or server verification trouble, while the unset default can warn and continue on errors. A `lockable` attribute alone is not an unconditional server-enforced collaboration guarantee. Test the actual host and client together before trusting this workflow. With no remote, there is no remote lock service; the initial solo workspace uses task ownership and one writer. [D05](https://github.com/git-lfs/git-lfs/blob/main/docs/man/git-lfs-config.adoc) [D06](https://github.com/git-lfs/git-lfs/blob/main/docs/man/git-lfs-lock.adoc)

World Partition and One File Per Actor change what constitutes a complete edit. External actor packages are still source assets. Epic documents that OFPA reduces overlapping edits by saving actor instances separately, but incomplete submissions can leave dangling references. Its integrated changelist inspection also has provider-specific limits. For Git, inspect the entire changed asset set and reopen the level from a clean working copy at a milestone; do not assume a one-file map commit captures a level edit. [D16](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine)

Perforce remains a credible later option. Its official pricing currently offers a self-hosted free tier for up to five users and twenty workspaces; the infrastructure and administration still belong to the operator. Its managed cloud product has a separate advertised price of $39 per user per month and included storage. Those facts do not make either product inherently faster for this particular project. A server migration only earns its cost when it resolves a demonstrated need. [D04](https://www.perforce.com/resources/vcs/helix-core-pricing)

Reconsider Perforce after two or more active art collaborators produce recurring binary coordination incidents, or when measured LFS history growth, checkout delays and bandwidth exceed the chosen spending/storage envelope. Write a one-page migration decision using four weeks of observed data: active users, working asset size, history growth, restore time, conflict count and administration hours. Test one representative branch, lock conflict, submit, backup and restore on a candidate setup before transferring the main project. A hybrid Git-plus-Perforce arrangement adds two authorities and is inappropriate as the default solo foundation.

## 35 Storage economics and a budget that cannot silently grow

As checked on 12 October 2026, GitHub Free includes 10 GiB LFS storage and 10 GiB monthly download bandwidth. Changed versions consume their full file size. The current billing documentation says a zero LFS budget blocks further usage after the quota is exhausted, while deleting the budget removes that spending cap. Configure and verify the actual account's controls before the first large upload; do not assume an old prepaid LFS data-pack tutorial reflects current billing. [D01](https://docs.github.com/en/billing/concepts/product-billing/git-lfs)

For planning only, suppose the admitted game assets total 3 GiB and 1 GiB of unique full-file revisions is retained each week. That is approximately 7 GiB after four weeks, before other retained history. Three fresh downloads of a 3 GiB working set consume roughly 9 GiB before further assets or historical fetches. Compression, deduplication, which objects are already local and provider measurement affect the actual number; this arithmetic illustrates why frequent large reimports and disposable complete clones deserve attention. It is not a bill estimate.

Track current source size, local LFS storage, accumulated artifact size and free disk space once a week. Set project intervention points relative to the selected allocation: investigate at 70 percent, stop nonessential imports at 85 percent, and preserve headroom for the next cook and backup. These are workflow thresholds, not vendor limits. Estimate each import's working copy, LFS payload, possible intermediate data, cooked output and backup copies before admitting a city kit. A download that fits on disk can still leave too little room to use it.

The observed laptop has limited system memory and only a modest amount of free storage relative to a large Unreal content workflow. Accordingly, the configuration starts with two parallel compilation actions, one engine operation at a time, and no background farm. This is an initial setting to measure, not a universal CPU rule. Close the editor for a normal compile; avoid simultaneous imports, shader compilation, packaging and backup. If two actions trigger memory pressure, lower the value. If measurements show adequate headroom, adjust it in local configuration without changing the project logic.

No paid cloud storage is budgeted by default. If existing separate backup storage is unavailable, obtaining a reliable backup destination competes ahead of optional convenience plugins within the under-€500 cap. Record an actual quote and its recurring cost before committing. Cloud estimates should state currency, tax treatment, included usage, storage growth, monthly egress and cancellation conditions. Prices quoted in US dollars are not euro totals. The provider's current account calculator and purchase terms govern, not this document's date-stamped research. [D18](https://github.com/pricing/calculator)

## 36 Executable operations and what each result means

The delivered tools use Windows PowerShell 5.1-compatible syntax and also run under PowerShell 7. Configuration resolution is explicit: tracked defaults, optional local configuration, then a one-run `-EngineRoot`/`-UE_ROOT` argument; the `UE_ROOT` environment variable can supply an otherwise absent engine path. A configuration key typo fails instead of being silently ignored. Project and artifact paths must stay under the workspace. Backup destinations must be outside it. Reparse points and traversal outside expected roots are rejected by these tools.

`Doctor.ps1` reports whether PowerShell, Git, Git LFS, the project and the configured engine are available. It does not install or log in to anything. `-RequireEngine` additionally requires a valid engine and its reviewed version lock. Availability is weaker than readiness: a present executable can still lack its SDK, a plugin or a compatible compiler. The next compile is the evidence that resolves those uncertainties.

`Preflight.ps1` validates configuration and project JSON, parses top-level operations scripts, examines local Unreal asset headers for unresolved LFS pointers, and can require real maps or a clean committed repository. When an engine is configured, its `Build.version` must match the reviewed version and changelist. Preflight does not claim to certify gameplay, licenses, shader compilation or package correctness. Its job is to fail cheaply on known prerequisites before an expensive engine process starts.

`Build.ps1` defaults to a dry run. `-Execute` compiles both the editor target and the game target using the installed engine's batch entrypoint. Editor compilation remains Development when the game configuration is Shipping. The reviewed engine lock must match; silently selecting another patch is not allowed. Every executed run gets a unique artifact directory, separate logs for its stages, and a small JSON result recording commit identity, dirty state, configuration, engine version, selected maps and outcome. A native failure causes a PowerShell failure rather than a reassuring final success message.

`Build.ps1 -Mode Package` additionally requires saved `.umap` files named in `MapsToCook`, an explicit project `GameDefaultMap` in the correct configuration section and inclusion of that default in the cooked map set. It invokes BuildCookRun with build, cook, stage, package and archive stages, using a new archive directory, and requires the expected project executable afterwards. Epic describes these as separate operations and recommends starting from the command produced by a Project Launcher profile when adapting complex packaging needs. The initial command is deliberately Win64-focused. A new platform, distribution setting or nonstandard plugin needs its own reviewed profile and validation; simply adding an unverified flag is not platform support. [D10](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine)

`Test.ps1` also defaults to a dry run. An executed run launches the editor command executable with the configured automation filter and a fresh report directory, then waits for the automation queue to finish. It does not reuse yesterday's report. Unreal's documented command interface supports selecting automation tests and exporting report output. This implementation supplements the engine process status with a strict parser: no report, no tests, too few tests, a missing expected name, a duplicate expected name or any nonsuccess state fails validation. An engine return code of zero is therefore insufficient on its own. [D11](https://dev.epicgames.com/documentation/en-us/unreal-engine/run-automation-tests-in-unreal-engine)

The initial expected suite contains five exact test names covering stable identity and rigid travel-transform behavior. Those tests establish a small code foundation; they do not establish playable movement, seamless streaming, saved-state migration, combat balance or the game's creative quality. Add a named test when a consequential invariant becomes executable. Do not lower the expected count to make an absent test disappear. An engine upgrade that changes the JSON schema is a review event; the parser should fail rather than reinterpret unknown output as success.

`-NullRHI` is available only for tests known not to need rendering. A graphics-free logic pass cannot prove lighting, shader behavior, streaming hitches, controller feel, UI focus, accessibility or frame time. Package launch and human play remain separate checks. The first package gate includes reaching the intended map from a fresh installation directory, using expected input devices, completing the tiny experiment, quitting, and checking its log. Preserve a short checklist and measured results, not a fictional universal “all tests passed” label.

The process runner treats command arguments as data, uses conventional native quoting and rejects shell metacharacters for Windows batch calls. It sets a bounded timeout and terminates only the child process tree it launched if the limit is exceeded. It never kills every process with an Unreal-like name. Builds can still write the normal engine/project caches and intermediates; a dry run is the way to inspect a plan without those changes. No operation performs recursive filesystem deletion or automatic retention pruning.

## 37 Backup, recovery and the difference between a copy and a restoration

Use three distinct concepts: version history, a working snapshot and an off-device backup. Git history is excellent for explaining source changes but omits ignored files and cannot by itself protect against losing the same physical drive. LFS payloads require separate attention. A backup on the same disk can protect against some mistakes while failing completely against disk loss. For valuable work, keep another independently recoverable copy off the workstation, with access and encryption appropriate to the licensed assets.

`Backup.ps1` requires an existing destination outside the workspace and at least one reviewed Git commit. Its default is a plan. On explicit execution it creates a new timestamped snapshot, copies tracked and nonignored untracked working files, creates and verifies a full Git bundle, and copies locally present LFS objects. It hashes sources and copied bytes, records file sizes, and writes a completed manifest only when the operation succeeds. Local environment files, ignored secrets, caches and normal build artifacts are omitted by the Git selection. Purchased original downloads and receipts need their own protected archive policy.

Git's documented bundle format can preserve refs and reachable Git history for transfer and cloning, with verification of its prerequisites. It does not automatically include external LFS payloads or uncommitted working files; those are why this workspace adds separate snapshot components. The supplied backup records **local LFS coverage only**. It does not fetch missing historical objects, contact a server or promise every historical commit can materialize offline. A deliberate history-complete archive requires auditing retained pointer objects against stored LFS data and obtaining missing data within the approved bandwidth and storage budget. [D09](https://git-scm.com/docs/git-bundle)

Pause the editor, importers and other writers for a snapshot. The script checks copied bytes and detects changes to the repository's commit or selected file list, but it is not a transactional volume snapshot of a live changing game project. If an operation fails, preserve its incomplete directory for inspection; do not label it a verified backup. The script does not delete failed attempts or older snapshots. Review retention manually after confirming a newer independent backup and a successful restore drill.

`VerifyBackup.ps1` validates the completed manifest, all recorded sizes and SHA-256 values. It rejects duplicate or escaping paths and reparse points. A hash failure is an incident to investigate, not an invitation to update the manifest to match corrupted data. The manifest is not cryptographically signed; someone able to replace both data and manifest could change them consistently. That is why access controls and an independent copy matter alongside checksums.

Perform a monthly restore drill, and another before an engine upgrade or significant history migration. Use a new empty directory, clone the local Git bundle with LFS smudging temporarily disabled, restore the stored LFS payloads, and restore the authoritative current working snapshot. Be careful with uncommitted deletions: blindly overlaying a snapshot on a clone can resurrect files that existed in the commit but were intentionally removed later. Compare the manifest and individually review extras. Recreate machine configuration manually. With network disabled, run preflight, compile, automation and the smallest saved-map package. Record the snapshot ID, time required and every missing dependency.

Initial recovery objectives are management targets: lose no more than one working day of committed/locally snapshotted progress, and restore a usable current project within one workday once compatible hardware and the approved engine are available. A slow first restore does not mean the project is safe; it supplies the actual baseline for improving those targets. Keep the last two milestone packages separately with their manifests and necessary notices, so a corrupt development state does not erase the latest playable evidence.

## 38 Local continuous validation before hosted automation

For the first prototype, continuous validation is a repeatable local habit. Run cheap preflight checks before starting a consequential change; compile and execute relevant automation before accepting it; make a playable package at the weekly integration boundary once a real map exists. These commands are the future CI interface. There is no benefit in paying to reproduce an unreliable local process on someone else's computer.

GitHub's current Free private-repository allowance lists 2,000 standard runner minutes per month, 500 MB artifact storage and 10 GB cache storage per repository. Shared artifact storage is not a suitable default warehouse for repeated large Unreal packages. Current documentation says self-hosted runner usage is free; an earlier announced self-hosted charge was postponed. Recheck the active account immediately before enabling services because runner rates and quota policies can change. None of these allowances should be interpreted as a free hosted Unreal build farm. [D02](https://docs.github.com/en/billing/concepts/product-billing/github-actions) [D14](https://github.com/resources/insights/2026-pricing-changes-for-github-actions)

The supplied `.github/manual-windows.yml.example` is disabled by its filename and location. It is a reviewable future template, not an active workflow. It has a manual trigger only, requires a private repository and a deliberate enable variable, targets a named trusted Windows runner, validates the requested full commit SHA against a preprepared local checkout, requires a clean tree, and runs the same scripts. It does not checkout arbitrary branch input, accept pull-request events, publish artifacts or expose repository secrets to the script environment.

If that template is eventually enabled, use a dedicated restricted account or isolated machine, only trusted repository access, explicit runner labels, narrow permissions, and no overlapping jobs. Keep unrelated personal credentials away from the runner account. An owner clicking a manual button must first review the code that will execute, including build rules, scripts and dependency updates. Disabling pull-request triggers reduces one path of attack; it does not make arbitrary repository code safe.

GitHub warns that self-hosted machines can remain compromised after running untrusted workflows and that even private-repository pull requests can expose a runner. For third-party actions introduced later, pin reviewed full commit SHAs rather than mutable tags, and use the minimum token permissions. The initial template avoids third-party actions altogether. Never add `pull_request_target`, comment-triggered execution or an unattended agent that runs instructions from unknown issue text on the creator's workstation. [D03](https://docs.github.com/en/actions/reference/security/secure-use)

A useful promotion trigger is measurable: only add unattended CI after four consecutive weeks of reliable local validation, a real recurring need to run it when the owner is absent, and an acceptable maintenance and security arrangement. A second developer may justify earlier shared automation. A solo laptop that must remain awake and available for cooking is not automatically a better production system because it has a remote Start button.

## 39 Artifacts, release evidence and dependency provenance

Treat artifacts as evidence with an expiry decision. For each accepted milestone retain a source commit, engine lock, plugin lock, selected cook maps, build/test summaries, package hash, essential notices, known issues and the playtest checklist. Retain ordinary local logs long enough to investigate recent failures; keep milestone summaries longer. Do not upload the entire `Saved` directory, derived caches, raw vendor assets or signing credentials merely because a pipeline supports attachments. Log files can contain machine paths and third-party diagnostic data even when the compact JSON result does not.

A package manifest should identify the source commit and whether the tree was dirty, the exact engine build, target configuration and platform, accepted tests, intended entry map, package SHA-256, build date, version and incompatible save changes. A dirty experimental package may be useful for a local test if labelled honestly. It should not become a supposedly reproducible public release. At release time, promote an already tested package; rebuilding after review introduces a new artifact that needs its own verification.

GitHub releases associate release information and binary attachments with tags. Its release documentation currently allows up to 1,000 assets per release, each under 2 GiB, with no stated aggregate release size or bandwidth cap. That is a distribution feature, not permission to redistribute purchased source assets or an assurance it is the right customer-facing channel. Verify current limits and access controls when publishing; choose distribution according to the audience and license obligations. This workspace publishes nothing. [D12](https://docs.github.com/en/repositories/releasing-projects-on-github/about-releases)

Every dependency admitted to the project needs an origin, exact version or commit, acquisition date, license record, responsible owner, reason it is needed, compatibility evidence and an exit plan. For a downloaded plugin or asset archive, record its checksum and keep the original receipt/license evidence in a protected place. The repository should point to the controlled record without exposing order numbers, account details or private download links. “Free on GitHub” is not a license. A plausible AI-generated license explanation is not a substitute for reading the actual license and distribution terms.

A plugin is executable code with a maintenance tail. Test a new one in an isolated copy or intake project, inspect its declared modules and dependencies, compile editor and game, cook the representative map, and confirm the packaged behavior. Record what breaks when it is disabled and whether authored data becomes inaccessible. A paid binary-only plugin must be reacquirable in its approved version or archived where its terms allow. Do not let an indispensable dependency exist only in one user's cache.

AI-assisted code follows the same contract. Give the agent a bounded task, allowed files, expected behavior, test criteria and a ban on unapproved external actions. Require a concise change explanation and evidence from the actual commands. Inspect new network access, shell commands, package installation, filesystem mutations and dependency additions before execution. Do not copy private vendor source, account data or complete licensed asset libraries into arbitrary external services. Generated work enters the product only after the creator reviews technical correctness and the applicable rights record.

Keep secrets out of source, task records, prompts, command-line arguments and ordinary logs. Use an appropriate credential store only when a real integration needs authentication. Source inventories and summary reports should expose relative asset names, versions, hashes and review states; they should not print environment variables, credential-bearing remotes, private receipts or authenticated URLs. If a credential is exposed, revoke or rotate it first. GitHub's guidance cautions that removing sensitive history has coordination and recontamination consequences; merely deleting the visible file in a later commit does not revoke a secret. [D13](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/removing-sensitive-data-from-a-repository)

Windows execution policy is an additional guard against accidental script execution, not a complete security boundary. Review downloaded scripts and follow the machine's legitimate policy; do not globally lower execution policy or permanently disable protections to make setup convenient. The tooling does not change execution policy, install certificates or request administrator rights. [D17](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_execution_policies)

## 40 Failure handling, tested boundaries and the operating cadence

When a build fails, preserve its unique run directory and classify the cause before changing code. An engine-lock mismatch requires a reviewed engine decision. A missing compiler or SDK requires the official installation path and a small verification build. An unresolved LFS pointer requires deliberate asset retrieval, not editing the pointer as though it were a map. An automation report with no tests requires test discovery/module investigation. A map cook error requires inspecting its dependencies and the packaging log. A backup hash failure requires protecting the originals and repeating from a quiet source. Never “fix” these by weakening all gates at once.

Separate warnings from failed guarantees. The current doctor can report an absent optional tool while the source remains inspectable. A package without a saved intended map cannot meet its purpose and must stop. A build may succeed while its game has no meaningful gameplay yet. A five-test pass can coexist with an untested renderer. This vocabulary keeps status useful: planned, dry-run, compiled, automated-tests-passed, packaged, launched, playtested and restored are distinct evidence states.

The delivered tooling regression suite runs without Unreal, external packages, Pester or network access. It has been executed successfully under Windows PowerShell 5.1 and PowerShell 7.6.5 with 27 checks. It exercises native batch argument handling with spaces, shell-metacharacter rejection, nonzero native exits, timeout handling and retained diagnostics, successful and unsuccessful report parsing, missing or insufficient tests, required cook/default maps, a full mock editor/game/UAT packaging route, UAT failure propagation, backup byte corruption and traversal rejection. It also creates a local fixture Git repository, produces a complete bundle-and-working-snapshot backup, checks an untracked working file is preserved, and verifies an ignored local secret is excluded.

Those are operations tests with a fake engine and fake map bytes. They prove the scripts' selected control paths, not Unreal correctness. The actual installed-engine compilation and five-test smoke evidence belong to the engineering validation record and should be reported separately. A real city kit import, playable map, successful cook, graphics/performance test, external-drive recovery drill, hosted LFS lock test and remote CI run remain distinct work. The disabled workflow has not been activated or run against GitHub.

Use a ten-minute end-of-session routine: save authored work; inspect the changed-file list; run the checks appropriate to the change; commit coherent source; update the task with evidence and the next concrete action. At the weekly integration boundary, run the whole approved smoke suite, test the saved-map package when available, inspect storage growth, create a verified independent snapshot, and record one largest source of wasted time. At the monthly review, restore a snapshot, check dependency and service changes, inspect recurring spending, and retire any process that costs more than it protects.

The first practical objective is not “fully automated development.” It is that the creator can make one change, know why it is acceptable, recover it after a mistake, and hand the same evidence to a future collaborator. Keep the tooling proportional to that objective while Borrowed Sun earns the right to become larger.


## 41 Sources and evidence

Research checked on 12 October 2026. Linked vendor documentation establishes claims at the time of research, not measured project compatibility. Prices are observed offers or explicitly marked allowances; licenses and checkout terms must be checked again when acquiring an item. Local observations and project proposals are labeled in the body.

### Operating model

- **O01** [Epic hardware and software specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine). Current development hardware recommendations; not a benchmark for this project.
- **O02** [Microsoft maximum path length limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation). Long path behavior also depends on application opt-in.
- **O03** [OpenAI AGENTS.md guidance](https://learn.chatgpt.com/docs/agent-configuration/agents-md). Repository instruction discovery and layering.
- **O04** [OpenAI rethinking skills and prompts](https://developers.openai.com/blog/rethinking-skills-and-prompts-for-gpt-6-astra). Current guidance on concise task-scoped instructions, not a benchmark claim.
- **O05** [OpenAI worktrees documentation](https://learn.chatgpt.com/docs/environments/git-worktrees). Worktree workflows and isolation in the app.
- **O06** [Git worktree reference](https://git-scm.com/docs/git-worktree.html). Shared repository metadata and worktree administration.
- **O07** [CISA StopRansomware Guide](https://www.cisa.gov/stopransomware/ransomware-guide). Offline protected backups and restore testing.

### Unreal engineering

- **E01** [Unreal Engine 5.8 is now available](https://www.unrealengine.com/news/unreal-engine-5-8-is-now-available). Official June 2026 major-release announcement; roadmap statements are intentions, not a guarantee.
- **E02** [5.8.3 Hotfix Released](https://forums.unrealengine.com/t/5-8-3-hotfix-released/2833315). Epic staff announcement dated 2026-09-22 verifies released 5.8.3; does not qualify the project or prove every plugin compatible.
- **E03** [Setting Up Visual Studio](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine). 5.8 compatibility table: VS2022 17.14+ or VS2026 18.0+; MSVC14.38 minimum/14.50 recommended; SDK22621 minimum/26100 recommended. Current docs may change.
- **E04** [Unreal Engine Modules](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules). Build.cs and Target.cs govern UBT; runtime/editor module types and public/private dependencies. Project architecture is our proposal.
- **E05** [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine). Input actions, mapping contexts, priorities and local-player subsystem. Does not implement the game's possession or UI policy.
- **E06** [Gameplay Ability System](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine). Abilities, attributes, effects and tasks. Deferring GAS is a scope recommendation, not an Epic requirement.
- **E07** [StateTree Overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-state-tree-in-unreal-engine). Hierarchical states and concurrent tasks; first task completion may trigger transition. No claim that it automatically supplies NPC behavior.
- **E08** [Behavior Tree Overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/behavior-tree-in-unreal-engine---overview). Event-driven Unreal Behavior Trees and Blackboard-driven decisions; selection between frameworks is project-specific.
- **E09** [World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine). Streaming sources, actor references and cell streaming. Readiness for the project's collision/gameplay still needs testing.
- **E10** [One File Per Actor](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine). External actor files reduce edit overlap; editor-only storage, actors embedded at cook; special changelist support is Perforce-specific.
- **E11** [World Partition Data Layers](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine). Editor/runtime layer organization and state management; cannot infer collision readiness solely from Loaded state.
- **E12** [World Partition HLOD](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine). Instancing/merged/simplified representation for unloaded cells; must qualify actual materials and build memory.
- **E13** [PCG Generation Modes](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-pcg-generation-modes-in-unreal-engine). Partitioned, hierarchical and runtime generation; persistent placement identity is project responsibility.
- **E14** [World Partitioned Navigation Mesh](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partitioned-navigation-mesh). Current page labels feature Experimental. Fallback and packaged pathfinding gates are proposed project requirements.
- **E15** [Nanite Virtualized Geometry](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine). Current5.8 docs include skeletal support but explicitly exclude Morph Target deformation; material support and scene limits still apply.
- **E16** [Lumen Performance Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-performance-guide-for-unreal-engine). High/Epic60/30 console goals and approximate4/8ms lighting budgets at1080p internal; not laptop or whole-frame guarantees. Medium behavior has changed in5.8.
- **E17** [Lumen Technical Details](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine). Scene representation, rapid-movement update limits, material/geometry constraints and hardware RT overlap costs.
- **E18** [IK Rig Animation Retargeting](https://dev.epicgames.com/documentation/en-us/unreal-engine/ik-rig-animation-retargeting-in-unreal-engine). Chain mapping, goals and retarget setup; contact quality must be checked in project animations.
- **E19** [Asset Management](https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine). Primary assets, IDs, bundles and loading. Stable logical IDs must be separated from mutable asset paths.
- **E20** [Scripting Unreal Editor Using Python](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python). Python editor automation; not packaged gameplay. Plugin/API availability must be checked on pinned version.
- **E21** [World Partition Builder Commandlet Reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-builder-commandlet-reference). HLOD/nav/PCG builders; PCG normally loads complete world, iterative cell option bounds scope. Verify command names and output on actual content.
- **E22** [Derived Data Cache](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-derived-data-cache-in-unreal-engine). Local Zen default since5.4; cache regenerable; unauthenticated Zen requires trusted network boundaries.
- **E23** [Horde UBA and Remote Compilation](https://dev.epicgames.com/documentation/en-us/unreal-engine/horde-unreal-build-accelerator-and-remote-compilation-tutorial-for-unreal-engine). Explains distributed compiler execution and prerequisites. Page retains5.5 platform wording; not used to claim current5.8 platform coverage.
- **E24** [Build Configuration](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-configuration-for-unreal-engine). UBT executor and parallelism controls. Actual local build confirms -MaxParallelActions=2 recognized.
- **E25** [Unreal Insights](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-insights-in-unreal-engine). Timing, memory and asset-loading analysis; measurement depends on trace channels and build configuration.
- **E26** [Automation Test Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine). C++ unit/feature/content/visual tests; smoke intended under1second. Test names alone are not proof of execution.
- **E27** [Run Automation Tests](https://dev.epicgames.com/documentation/en-us/unreal-engine/run-automation-tests-in-unreal-engine). Command-line run filters and ReportExportPath JSON; project runner must detect missing/zero-test reports.
- **E28** [Gauntlet Automation Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/gauntlet-automation-framework-in-unreal-engine). Session-level orchestration; project gameplay tests/controllers are still required.
- **E29** [Packaging Unreal Projects](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project). Build/cook/stage/package distinction, GameDefaultMap and UAT BuildCookRun. Generic prerequisite wording does not replace C++ toolchain requirements.
- **E30** [PSO Precaching](https://dev.epicgames.com/documentation/en-us/unreal-engine/pso-precaching-for-unreal-engine). Automatic asynchronous precaching and validation. Coverage is not a promise that every first-use stall disappears.
- **E31** [Saving and Loading Your Game](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine). USaveGame and async saves. Atomic custody, revisions, journaling and migrations are proposed application contracts.
- **E32** [Large World Coordinates](https://dev.epicgames.com/documentation/en-us/unreal-engine/large-world-coordinates-in-unreal-engine-5). Double-precision changes and float conversion risks; page retains Beta label. Does not provide planetary gameplay.
- **E33** [Cargo Supported Software and Engines](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines). Launcher5.2–5.8 only; source builds unsupported. Current vendor minimum32GBRAM,8GBVRAM,15GBfree perkit.
- **E34** [Exclude Cargo from Unreal Builds](https://help.kitbash3d.com/en/articles/12689467-exclude-kitbash3d-cargo-plugin-from-unreal-engine-builds). Vendor guidance for editor connector exclusion; actual imported runtime dependencies must be checked.
- **E35** [MassEntity Overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine). Data-oriented fragments/processors; no evidence that adopting it is necessary for the first small district.

### Assets and production tools

- **A01** [Epic Games: Third Person template](https://dev.epicgames.com/documentation/unreal-engine/third-person-template-in-unreal-engine?lang=en-US). UE 5.8 documentation; template includes controllable mannequin and elementary geometry.
- **A02** [KitBash3D: Cargo software compatibility](https://help.kitbash3d.com/en/articles/7833635-cargo-supported-3d-software-and-render-engines). Launcher UE 5.2–5.8; Blender.org 4.2–5.2; source-built Unreal unsupported.
- **A04** [KitBash3D: Access purchased kits](https://help.kitbash3d.com/en/articles/7833678-how-to-access-purchased-kits). Purchased-kit individual imports do not require a paid Cargo subscription.
- **A05** [KitBash3D: Licensing overview](https://kitbash3d.com/pages/licenses). Commercial larger works, perpetual versus subscription, organization tier.
- **A07** [KitBash3D: Neo City](https://kitbash3d.com/products/mini-kit-neo-city). Direct page $0; 9 models; 60 materials.
- **A08** [KitBash3D: Mission to Minerva](https://kitbash3d.com/products/mission-to-minerva). Direct page $0; industrial/space settlement content and cargo ship.
- **A09** [KitBash3D: CyberPunk](https://kitbash3d.com/products/cyberpunk). Direct page $245; Gameplay Ready badge; 198 models.
- **A10** [KitBash3D: Cyber District](https://kitbash3d.com/products/cyber-district). Direct page $245; Gameplay Ready badge; 241 models.
- **A11** [KitBash3D: Cyber Streets](https://kitbash3d.com/products/cyber-streets). Direct page $95; Gameplay Ready badge; street props.
- **A12** [Jonathon Frederick: Modular SciFi Season 1 Starter Bundle](https://www.fab.com/listings/86913335-3c75-42bf-8404-54fe9d9d7396). Fab Free; legacy modular hallways/interiors/props/command center.
- **A13** [Jonathon Frederick: Modular Scifi Season 2 Starter Bundle](https://www.fab.com/listings/cb3c4494-4060-4a80-b079-e46936cb8dd0). Fab Free; Engineer Hallways, Props, Interiors; current UE revision not exposed.
- **A14** [Denys Rutkovskyi: Spaceship Interior Environment Set](https://www.fab.com/listings/e55bb035-8720-487f-8a36-4ce50d16f344?lang=en). Modular ship interior; listing references UE 4.18 update; current price not exposed.
- **A15** [KitBash3D: Vehicles / Spaceships Armada](https://kitbash3d.com/collections/vehicles). Catalog displays $75 each for individual Armada vehicles; not a whole-pack price.
- **A16** [Poly Haven: Asset license](https://polyhaven.com/license). CC0 downloadable assets; website content and access terms separate.
- **A17** [ambientCG: License](https://docs.ambientcg.com/license/). CC0 assets, commercial use, no mandatory credit.
- **A18** [Epic Games: GASP UE 5.8 update](https://www.unrealengine.com/tech-blog/download-the-latest-game-animation-sample-project-now-updated-for-ue-5-8). 2026-08-12 update; UE 5.8 compatibility and experimental Look-At example.
- **A19** [Epic Games: Lyra documentation](https://dev.epicgames.com/documentation/unreal-engine/lyra-sample-game-in-unreal-engine?lang=en-US). Modular learning sample with multiple gameplay systems.
- **A20** [Epic Games: City Sample documentation](https://dev.epicgames.com/documentation/unreal-engine/city-sample-project-unreal-engine-demonstration?lang=en-US). Systems reference; vendor recommendation includes 64GB RAM and powerful GPU.
- **A21** [Epic Games: Content Examples](https://dev.epicgames.com/documentation/en-us/unreal-engine/content-examples-sample-project-for-unreal-engine). Focused engine examples; compatible engine acquisition required.
- **A22** [Epic Games: MetaHuman license](https://www.metahuman.com/license?lang=en-US). Current MetaHuman included under standard Unreal license; threshold and AI terms must be reviewed.
- **A23** [Adobe: Mixamo FAQ](https://helpx.adobe.com/creative-cloud/faq/mixamo-faq.html). Free with Adobe ID; royalty-free characters/animation for games; bipedal scope.
- **A24** [Adobe: Mixamo Additional Terms](https://wwwimages2.adobe.com/content/dam/cc/en/legal/servicetou/Mixamo-Addl-Terms-en_US-20210623.pdf). Binding additional terms dated 2021-06-23, subject to general terms.
- **A25** [mookie: SixDOF Movement](https://www.fab.com/listings/01288642-e34b-408e-816d-f6a5a31944bc). Movement component; transform or physics force modes; current exact engine compatibility not exposed.
- **A26** [mookie: Fab seller catalog](https://www.fab.com/sellers/mookie?lang=fr). Catalog search result shows SixDOF Movement from $19.99, not a checked-out tier price.
- **A27** [Blender Foundation: Blender 4.5 LTS](https://www.blender.org/releases/4-5/). 4.5.14 listed 2026-09-15; support until July 2027.
- **A28** [Blender Foundation: License](https://www.blender.org/about/license/). GPL software; creator artwork and exported data are not forced under GPL.
- **A29** [Krita Foundation: License](https://krita.org/en/about/license/). GPLv3 application; commercial artwork allowed; official free distribution.
- **A30** [Adobe: Substance 3D plans](https://www.adobe.com/products/substance3d/plans.html). Observed US Collection individual $59.99/month; local/contract pricing must be checked.
- **A31** [SideFX: Houdini Indie](https://www.sidefx.com/products/houdini-indie/). $299 one year / $449 two years displayed; revenue/funding/entity constraints.
- **A32** [Audacity: Application FAQ](https://manual.audacityteam.org/man/faq_about_audacity.html). Free open-source audio editor available for commercial work.
- **A33** [Cockos: REAPER purchase](https://www.cockos.com/reaper/purchase.php?l=1). $60 qualifying discounted / $225 commercial; 60-day evaluation; tax extra.
- **A34** [Sonniss: GDC 2026 Game Audio Bundle](https://gdc.sonniss.com/). Free commercial-media audio collection with no attribution requirement.
- **A35** [Sonniss: GDC bundle license](https://sonniss.com/gdc-bundle-license/). Separate bundle license; AI training prohibition; embedded-media use.
- **A36** [Freesound: FAQ and licenses](https://freesound.org/help/faq/). Per-file CC0, BY, BY-NC and legacy licenses; provenance varies.
- **A37** [Kevin MacLeod: Incompetech music FAQ](https://incompetech.com/music/royalty-free/faq.html). Attribution music pathway and credit guidance including games.
- **A38** [Epic Games: MetaSounds](https://dev.epicgames.com/documentation/unreal-engine/metasounds-in-unreal-engine). Built-in procedural sound authoring examples and reference.
- **A39** [Firelight Technologies: FMOD legal](https://fmod.com/legal). Limited-commercial free license criteria, registration and attribution.
- **A40** [Kenney: Input Prompts](https://kenney.nl/assets/input-prompts). Free CC0 input symbols; asset license must accompany local acquisition.
- **A41** [Lucide contributors: License](https://github.com/lucide-icons/lucide/blob/main/LICENSE). ISC and notices for Feather-derived icons; retain bundled notices.
- **A42** [Rasmus Andersson: Inter](https://rsms.me/inter/). Free font under SIL Open Font License 1.1.
- **A43** [inkle: ink and Inky](https://www.inklestudios.com/ink/). MIT narrative tools; JSON/web export; Unreal integration link is separate project.
- **A44** [Articy: articy:draft X FREE](https://www.articy.com/en/articydraft/free/). Free commercial use with 700 objects/project; exports available.
- **A45** [Epic Games: PCG overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-overview). In-engine procedural graphs; enabled plugin; points/instances and authoring.
- **A46** [Epic Games: Fab license overview](https://www.fab.com/eula). Standard-license summary, source/reference distinction; actual acquired license controls.
- **A47** [KitBash3D: License agreement](https://kitbash3d.com/pages/license-text). Current 18-page EULA; 3.2.7 AI-input wording; free offerings 3.2.22; USD/tax sections.
- **A48** [KitBash3D: Product updates](https://kitbash3d.com/pages/product-updates). 2026-09-11 Cargo 3.2.1; standard Blueprint versus Packed Level Actor option.
- **A49** [Epic Games: Fab examples catalog](https://www.fab.com/category/tutorials-examples?lang=en). GASP, City Sample, Lyra, Content Examples displayed Free.
- **A50** [Arghanion's Puzzlebox: Space Ship Flight System](https://www.fab.com/listings/62a69df5-b447-415d-8d91-82f9f1a46894). Free UE5.8 Blueprint flight tutorial; ship CC BY4.0; listing NoAI tag; local qualification pending.
- **A51** [Epic Games: Content Examples Fab license notice](https://www.fab.com/listings/4d251261-d98c-48e2-baee-8f4e47c67091). Explicit Unreal Engine-based products restriction on listing.
- **A52** [KitBash3D: Subscription FAQ](https://help.kitbash3d.com/en/articles/16635224-subscription-faqs). 2026-08-25 FAQ on entity scope and post-subscription project rights.
- **A53** [KitBash3D: Fab seller Cyber results](https://www.fab.com/sellers/KitBash3D?q=cyber). Observed channel differences: CyberPunk from $249.99, CyberStreets from $94.99.
- **A54** [Adobe: Substance pricing announcement](https://blog.adobe.com/en/publish/2025/02/20/substance-3d-innovations-pricing-updates). Historical US Texturing $24.99/month announcement; not a current local checkout quote.

### Source control and DevOps

- **D01** [GitHub: Git Large File Storage billing](https://docs.github.com/en/billing/concepts/product-billing/git-lfs). Free quotas; full changed-file versions count; zero-budget stop behavior. Checked 2026-10-12; terms can change.
- **D02** [GitHub: GitHub Actions billing](https://docs.github.com/en/billing/concepts/product-billing/github-actions). Private Free account allowances and current self-hosted billing; verify before enabling infrastructure.
- **D03** [GitHub: Secure use reference](https://docs.github.com/en/actions/reference/security/secure-use). Self-hosted runner compromise, untrusted code and action SHA pinning.
- **D04** [Perforce: P4 pricing and plans](https://www.perforce.com/resources/vcs/helix-core-pricing). Free self-hosted five users/twenty workspaces; hosted price differs. Vendor primary source, not independent performance evidence.
- **D05** [Git LFS: locking configuration](https://github.com/git-lfs/git-lfs/blob/main/docs/man/git-lfs-config.adoc). locksverify true/false/default semantics and custom LFS storage.
- **D06** [Git LFS: git-lfs-lock manual](https://github.com/git-lfs/git-lfs/blob/main/docs/man/git-lfs-lock.adoc). Lock behavior and prerequisites; server capability must be checked.
- **D07** [GitHub: About Git Large File Storage](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-git-large-file-storage). Pointer model, per-file limits and source archive settings.
- **D08** [GitHub: About large files](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github). 50 MiB warning, 100 MiB ordinary Git block, repository size guidance.
- **D09** [Git: git-bundle documentation](https://git-scm.com/docs/git-bundle). Full Git bundles, verification, clone/restore semantics; does not include external LFS payloads.
- **D10** [Epic: Build operations](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine). BuildCookRun stages and Project Launcher generated command approach.
- **D11** [Epic: Run automation tests](https://dev.epicgames.com/documentation/en-us/unreal-engine/run-automation-tests-in-unreal-engine). Command execution and ReportExportPath output. Script report schema is additionally checked against installed engine.
- **D12** [GitHub: About releases](https://docs.github.com/en/repositories/releasing-projects-on-github/about-releases). Release/tag and binary asset restrictions; verify immediately before publication.
- **D13** [GitHub: Removing sensitive data from a repository](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/removing-sensitive-data-from-a-repository). Rotate/revoke exposed credentials first; history rewriting has consequences.
- **D14** [GitHub: 2026 Actions pricing changes](https://github.com/resources/insights/2026-pricing-changes-for-github-actions). Previously announced self-hosted billing postponed; do not repeat superseded announcement as current price.
- **D15** [Epic: Derived Data Cache](https://dev.epicgames.com/documentation/unreal-engine/using-derived-data-cache-in-unreal-engine?lang=en-US). Regenerable derived data and cache hierarchy; exclude DDC from authored source history.
- **D16** [Epic: One File Per Actor](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine). External actor files and submission completeness; editor changelist support caveats.
- **D17** [Microsoft: PowerShell execution policies](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_execution_policies). Execution policy is not a security boundary; do not globally bypass policies.
- **D18** [GitHub: Pricing calculator](https://github.com/pricing/calculator). Prices depend on plan and purchase conditions; use current account checkout for estimates, not old LFS packs.
- **D19** [Epic: Unreal Engine directory structure](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-directory-structure). Distinguishes authored configuration/source from generated caches and outputs.
