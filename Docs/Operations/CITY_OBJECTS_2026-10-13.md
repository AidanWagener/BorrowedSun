# Ordinary props in the main Orison city

This pass completes the already-started Development integration of the Second Dawn hand props into the main expedition. The build, 20 smoke tests and 57 rendered main-city checks passed. The user's exclusive development priority is now the [living planet bible](../World/README.md) and worldbuilding, before further gameplay systems. The earlier Shipping packages remain unchanged.

## Behavior and boundaries

Ten original props share the same definitions and meshes as the isolated object court. Their city instance IDs belong to `second_dawn`; the court retains `object_lab`. Neither array order nor loading regenerates identity. The city's object ledger is `orison:lower_quay_props_v1`, saved under `OrisonProps_v1` in ordinary play. Explicit automated runs require their own save namespace and player-data root.

The bench's enclosing collision hull is disabled in this placement. Separate boxes follow its tabletop, drawer carcasses, legs and vise so small items can rest on the surface rather than the upper face of an enclosing convex volume. Ten imported bodies retain their authored simple collision and unit scale. Other decorative scene objects remain static.

F / gamepad D-pad up grabs or drops. Left mouse / D-pad right throws. Hold right mouse / right-stick click while looking to rotate. Z / D-pad left transfers an ordinary object into the persistent ten-place collection stub. The HUD identifies a reachable or held prop and displays its controls. Collection currently has no inventory browser or retrieval interface; the message states this limitation.

Menu opening, input flush, possession loss, recovery and checkpoint restoration release physical grips. Saving also releases the grip: moving objects retain their last safe resting pose until they settle. F9 reloads the mission and ordinary object ledger, while their writes remain separate transactions. A partial save/reload is reported as such. Mission sunseed custody remains in its original subsystem and cannot become an ordinary collectible.

A new expedition resets the mission only. It preserves ordinary-object positions and collection records; its confirmation says so. This avoids turning a mission restart into duplication or a silent collection reset. Cross-session object persistence is independent of the current mission stage.

## Acceptance and evidence

The observable integration outcome is ten physical workshop props in the main city, safe grip release during state transitions, persistent collection without reconstruction of collected instances, and both original mission routes still completing.

The imported-model regression exercises gravity, settlement, carry eligibility, actual ray acquisition, physical grip, rotation, menu release and bounded throw velocity for all ten meshes. The rendered main-city harness approaches the workshop through character movement, grips and rotates the cup, commits its collection, reloads the ledger, checks the live actor count and then runs both lease routes.

- Final Development Editor and Game compilation passed: `Artifacts/build/20261013T083215Z-477f87e3`.
- All 20 expected smoke tests passed: `Artifacts/tests/20261013T082212Z-3b7246b4`. Subsequent runtime changes were the workshop book placement and rendered diagnostic checks; the physical-object implementation remained the tested version apart from a comment correction.
- Both mission routes and the new workshop checks passed at 1920 × 1080 with a real RHI offscreen viewport: `Artifacts/demo/20261013T083349Z-b6f07a3b`, 57 checks. QA used a fresh namespace and isolated player data.
- Thirteen runtime screenshots were produced. The held-cup HUD and bench-after-collection views were visually reviewed and retained in [the review directory](../Art/Reviews/city-objects-2026-10-13/). They show readable controls, the small prop palette and the cup's absence after collection. They do not demonstrate AAA visual quality or physical controller feel.

The tests found persistent rocking in the cup/flask after a drop. Increased local contact iterations, angular damping and an explicit physical material allowed all ten to settle. The material uses friction 0.7, restitution 0.05, a 1 cm/s linear sleep threshold and a 0.2 rad/s angular threshold sustained for 30 physics steps. The test measures actual transformed source vertices against the floor rather than using a conservative rotated bounding box. Throw impulses are measured after a physics step.

The city test separately found that the spanner settled against the adjacent book, causing the collision-checked pose save to refuse that arrangement. The book was moved to a clear part of the bench while preserving its instance ID. The final rendered check confirms clear poses for all ten city placements before collection.

![Main-city held-object controls](../Art/Reviews/city-objects-2026-10-13/held-cup.png)

![Bench after persistent collection](../Art/Reviews/city-objects-2026-10-13/bench-after-collection.png)

These checks do not establish a complete inventory, universal interaction with all decoration, a human evaluation of keyboard/controller feel, or AAA art quality. A Development build is not a new Shipping delivery. The earlier player packages and ordinary packaged launcher remain independently versioned.
