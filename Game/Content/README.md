# Content ownership

This starter contains no generated binary assets or playable map. Create assets in Unreal; never fabricate .uasset/.umap placeholders or rename binary assets in the filesystem.

Project-owned assets belong beneath `/Game/BorrowedSun`: Core, Characters, Ships, Interaction, Missions, UI, Worlds/Orison, Worlds/Orbit, Tests, and Art/Adapted. Vendor content stays in its original vendor folders (for example `/Game/KitBash3D`) unless migration has been qualified. Do not bulk-move imports to satisfy a cosmetic folder convention.

Create the first project map at `/Game/BorrowedSun/Maps/Tests/L_Test_Startup`. Save the actual map, set it as GameDefaultMap, and register it as a cook target before the first package attempt. Then verify that the packaged executable launches the map. It should contain a walking floor, collision test wall, PlayerStart, dock marker and a small lighting rig. It is a qualification scene, not a city sample. Later feature-test maps also belong under `/Game/BorrowedSun/Maps/Tests`.

Authored IDs use lower-case ASCII namespace and key components containing only letters, digits, dots, underscores or hyphens. Example: `orison.market:stall-001`. Allocate keys once and retain them in the reviewed placement manifest; names, paths and array order may change without replacing identity.
