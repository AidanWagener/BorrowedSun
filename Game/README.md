# Borrowed Sun engine foundation

This is a small C++ source foundation, not a playable game. The runtime module implements an authored stable-ID value type and a rigid sector-motion mapping helper. The editor module registers five automation tests under `BorrowedSun.Smoke`. It does not implement ship movement, a transaction journal, saving, streaming, or game content.

The project association is `5.8`; the exact approved Launcher build is pinned separately in `../Config/engine-lock.json` to **5.8.2 / CL 56702186**. Do not open and resave using an arbitrary newer engine. Use the workspace Tools scripts for consistent environment discovery, build and test reports.

Runtime source ownership starts in Core and Travel. Add WorldState, Interaction, Population and Presentation folders when their first behavior is required; do not create empty plugins for each responsibility. The editor target includes BorrowedSunEditor; the game target does not depend on editor modules.

Enhanced Input is explicitly enabled and configured. AndroidFileServer is explicitly disabled for this Windows-only project to prevent unused server settings from being generated into tracked configuration. Input Action and Mapping Context assets are intentionally not fabricated. The first playable increment must create walking/ship/menu contexts and switch them through the local-player input subsystem, including removing contexts on possession changes.

The stable-ID grammar prevents delimiter ambiguity. IDs are not hashes of content paths and the scaffold does not allocate persistent keys automatically. The sector helper rejects scaled anchors and preserves position/orientation/linear/angular velocity under a fixed rigid mapping. It does not teleport Actors or claim to synchronize the movement or physics system.

The five tests cover canonical identity, rejection of ambiguous identity, rigid-motion conservation, forward/return mapping and invalid-anchor failure without output mutation. A command-line process returning zero is not proof the tests ran: the automation report must show all five discovered tests and no failures.

No project-owned `.umap` exists yet. Follow Content/README.md to author the first map, then set GameDefaultMap and explicit cook maps. Do not treat a successful editor compile as successful packaging, rendering, input, collision, navigation or streaming validation.

Compiler and test execution status is recorded in `../Docs/Operations/engine-validation.json`. The editor target and standalone game target compiled successfully on the pinned engine, and all five foundation tests passed. No cook or package was performed. Read that report before describing the limits of what is built or tested.
