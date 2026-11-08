# Seeded procedural layouts — Step 19 MVP

Load Storage and `Libraries/Game/Generation`, set Generation_Database to a
host-owned connection and call Generation_Init. Set Width/Height (integer 3..64),
Seed (integer 0..2147483646) and Density (integer wall percentage 0..90), all with
the Generation_ prefix, then Generation_Create. Inputs use decimal digits only.
Generation_OK/Status report success. Invalid input or failed SQL preserves the
previous generated cells/metadata.

The deterministic algorithm builds solid borders, an entrance at (1,1), an exit
at (width-2,height-2), an explicit L-shaped connecting route and seed-dependent
interior walls. A flood fill removes disconnected floor pockets. Every retained
floor is reachable from the entrance; it is not a maze-quality/randomness promise.
The integer hash/version and seed define repeatability within this algorithm.
Density is a source-wall probability, not a guaranteed final wall percentage
because the route/connectivity pass changes it. It is not a cryptographic RNG.

The Appeal script uses existing SQLite recursive queries/integers for generation,
avoiding a slow emulated modulo/random loop. This is native Storage work driven
by reusable script, not a new C++ API. Generate at level/checkpoint boundaries.

Generation_Read uses Cell_Index (row-major) and returns X/Y/Walkable; missing
indices return empty strings. `(Call Generation_Export)` returns versioned JSON
including metadata and cell flags. Persist the seed/version or export through
custom save hooks. There is no JSON import API here; rebuild from a supported
seed/version, or save the built World snapshot with Game/Saves.

For World integration, create an active scene and existing shared Floor/Wall
prototype entities, set Generation_Section/Floor/Wall, and call
Generation_Build_World **after successful generation**. The section name must be
unused. It creates a tile section through existing World calls, not collision
bodies or renderer bindings. Building uses stored successful dimensions even
if later requested parameters were invalid. Scene cleanup remains World-owned.

This deliberately bounded 2D layout generator does not supply terrain noise,
room graphs, spawn balancing, loot tables, 3D worlds or runtime streaming.
Gen_ tables/views and all intermediate outputs are library-owned. Calls select
Generation_Database and leave it selected. JSON/recursive-query support comes
from the tested SQLite backend; no new plugin is needed.

`python Tests/Game_Generation_Tests.py` checks deterministic/different seeds,
connectivity/borders, maximum dimensions, invalid-input preservation and an actual
World tile-map build. It reports a small generation/build cost sample so future
bulk APIs can be discussed using evidence rather than speculation.
