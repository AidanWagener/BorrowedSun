# Daily operations

## Open

Check the current branch and working tree before opening Unreal. Read the current task's acceptance example. Run the last known executable when investigating a regression. Keep one editor for the authoritative content checkout.

## Implement

Build the smallest observable behavior. C++ owns lifecycle/invariants; Blueprint/data own safe tuning and presentation. Add source or assets only where their owner is clear. Do not move an Unreal package with Explorer. Do not let two agents edit the same Blueprint or map.

## Verify

Use the narrowest meaningful check first. Persistence, ID, transform and tooling changes need automated failure cases. Rendering/content changes need an editor and packaged visual check. A build-system change needs an actual native failure-propagation check. Routine documentation edits do not need a full cook.

Run preflight, inspect changes and commit the completed outcome. Never record a dry run as successful compilation. Never accept zero selected tests. Keep machine paths and private records out of commits.

## Weekly

Package one build, play market/flight/return routes, record the biggest unresolved issue, and measure a fixed performance route. Verify a recent backup. Review next week's two highest-value tasks and remove one unnecessary tool or process if it creates friction without protection.

## Monthly or at milestone

Restore a snapshot into a separate location, including LFS payloads and native source. Rebuild and run. Review dependency/license/version pins and recurring costs. Update only when a measured need outweighs upgrade and regression work.

## Incident triage

- Build breaks: reproduce on the same pin; read the earliest causal error; check toolchain and changed dependencies before deleting caches.
- Asset disappears: stop editing, inspect LFS materialization and references, recover the known package revision, validate in editor.
- Save breaks: preserve the failed file privately, reproduce against the schema, exercise last-known-good fallback; do not silently reset player progress.
- Disk fills: stop imports/cooks, inventory generated outputs and documented caches, retain useful evidence and backups; do not delete source or unpushed work.
- Upgrade breaks: return to the prior engine/plugin lock and known-good source/content commit. Do not downgrade modified binary assets in place.

Record build IDs and recovery results. A new process rule should address a repeated failure, not an imagined enterprise requirement.
