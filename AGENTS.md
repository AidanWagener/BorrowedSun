# Borrowed Sun working contract

This is a solo-developed Unreal city-to-orbit adventure. Preserve both ground and flight play. README.md is the entry point. Read only the relevant design, architecture or operations document for the current task; the full research handbook is not mandatory context for a small edit.

- Work on one reviewable player outcome or infrastructure defect at a time. State its observable acceptance condition before a substantial implementation.
- Use the engine/toolchain pin in Config and the local Tools environment file. Do not upgrade the engine or add a framework merely to fix an unrelated issue.
- Runtime code must not depend on editor-only modules. Prefer small C++ contracts with Blueprint-authored presentation and content.
- Never manufacture .uasset or .umap bytes. Use Unreal's editor APIs, commandlets or editor. Treat binary assets as exclusive-write files; one editor owns a given map/asset at a time.
- Authored IDs persist across save/load and placement edits. No random ID regeneration on load/construction. Sector transfer must preserve state and recover safely; a successful math helper is not evidence that streaming works.
- Paid/vendor assets and private license records are not AI input unless their permissions have been checked. Do not transmit raw asset files, keys, account tokens or private player data.
- Generated caches, build output and secrets stay outside version control. Preserve unrelated user changes. Keep vendor originals traceable, and never move a package by filesystem rename while Unreal references it.
- Independent code-only tasks may use separate branches/worktrees. Do not run two Unreal builds against the same checkout or edit the same binary assets in parallel.
- Use Tools/Preflight.ps1 for repository checks. Use Tools/Build.ps1 and Tools/Test.ps1 for relevant executable verification; their dry runs are not builds or tests. Check Tools/README.md for exact switches and prerequisites.
- Add meaningful regression tests for persistence, transforms, pipeline failures and gameplay invariants. Do not generate tests that merely echo the implementation. Record unrun editor, rendering and play tests honestly.
- Keep source URLs, license decisions and local test evidence beside dependency/asset records. A marketplace claim is a candidate until packaged on the pinned engine.
- Before completing work, review the diff, summarize changed behavior and actual validation, and leave an explicit next action if external accounts, purchases or missing prerequisites remain.

## Code review rules

Flag lost/duplicated cargo, unsafe save migration, stale UObject references across travel, editor modules in Shipping, unbounded Tick work, missed native command exit codes, zero-test passes, and deletion/copy paths that escape their declared root. Do not block a minor reversible edit with unrelated release procedures.
