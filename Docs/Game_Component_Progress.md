# Game component progress

## Game integration checkpoint — 2026-11-09

[Bananas Genre Arcade](../Resources/Appeal/Games/Genre_Arcade/README.md) combines
existing runtime, profile/settings, combat, FSM, inventory/crafting and generation
libraries with current art/audio in a hub of 24 short challenges. Every challenge
has a tested completion and replay path, and medals persist across processes.
The native UI suite renders all 31 catalog entries and exercises keyboard/mouse
launch, story completion, lane movement/fire, pause, replay and hub navigation.
Its [coverage table](../Resources/Appeal/Games/Genre_Arcade/Coverage.md) distinguishes
23 basic ruleset challenges, a bounded sandbox prototype and seven unimplemented
genre demos. Simplified challenge rules do not close the open native/game-system
requirements below. No production C++ changed and no readiness score was raised.

Updated 2026-11-08. This is the living, breadth-first runtime roadmap. Utilities
and packaging/selling remain separate. **MVP** means a tested reusable foundation
with the linked scope, not every feature in a broad category or every genre.
**Partial** highlights a narrower result or a native prerequisite still open.
All production work in this continuation used Appeal scripts/existing APIs;
C++ files added under Tests are test harnesses only.

| # | Component | Status / evidence | Remaining practical boundary |
|---:|---|---|---|
| 1 | State, timing, events | MVP — a921805 | Module clocks still require explicit integration. |
| 2 | Objects/scenes | MVP — 8dabbc9 | Generic entity queries and cross-module ownership remain explicit. |
| 3 | Player interface | MVP — 6c86e37; actual mouse/keyboard tests | Full game-specific screens/accessibility/device coverage vary. |
| 4 | Save/load | MVP — 75d50e8; process restart and rollback tested | Migration/corruption recovery and external state hooks remain open. |
| 5 | Interactions/rules | MVP — 3566ee6 | Games own targets, rule state and effects. |
| 6 | Input integration | MVP — a767132; virtual controller and GUI routing tested | Retained movement reset/rebinding and physical devices need game tests. |
| 7 | Audio integration | MVP — d82a8a5; real mixer with dummy output | Buses/fades/per-instance lifetime remain native follow-ups. |
| 8 | Progression/outcomes | MVP — aa84d14; playable quiz a4b8570 | Complex quests/unlocks are additional game policies. |
| 9 | 2D camera/presentation | MVP — 3400e9c; transform/bounds + real pixel test | Partial clipping/layering and advanced camera behavior remain open. |
| 10 | Collision/physics integration | MVP — f7e6451; real Box2D/Jolt floor/sensor/sync tests | Scene resource ownership and game controllers remain explicit. |
| 11 | Animation integration | Partial — 5fd78ba; clip selection/isolation/no-reset test | Simulation-clock pause, markers and completion need native work. |
| 12 | NPC AI | FSM MVP — 862fee5; patrol/chase/flee example | Navigation/perception and actor record ergonomics remain open; GA/NN later. |
| 13 | Inventory/equipment | MVP — d87edb0; stacks/equipment/snapshot/save tests | Rich item rules, UI, transfer atomicity and native record simplification remain open. |
| 14 | Combat/abilities | MVP — 60730d8; damage, death, grace and cost/cooldown tests | Projectiles/hitboxes, attack deduplication and advanced status rules remain game work. |
| 15 | Dialogue/narrative | MVP — 454be89; conditional/stale choice and restore tests | Content, localization, voice and richer quest systems remain open. |
| 16 | Turns/grids | MVP — c74137d; budget/revision/endpoint checks | Pathfinding, occupancy and genre-specific rules remain explicit. |
| 17 | Complete 3D integration | Partial — b4e7916; real static lit model/camera transform test | External skeletal character pipeline and complete 3D game proof still needed. |
| 18 | Economy/construction/crafting | Craft/resource MVP — 83a3502; multi-input atomicity/rollback | Full building/economic simulation remains open. |
| 19 | Procedural generation | Bounded 2D MVP — 48de64c; connectivity, seeds and World construction tested | Broader generators/spawn balancing and runtime-scale performance remain open. |
| 20 | Many-agent simulation | Partial — 7151e83; fair budgeted decision scheduler | Real crowds, orders, navigation and representative load profiles remain open. |
| 21 | Vehicles/specialized motion | Partial — 1d93cd2; arcade acceleration/brakes + real body test | Steering, suspension, vehicle-specific physics and flight remain open. |
| 22 | Precision combat/sports | Partial — be357de; timed action phases and missed-window detection | Animation/physics precision, combos, ball/team rules remain open. |
| 23 | Audio-clock timing | Not implemented; prerequisite recorded | Exposed playback clock, input timestamps and calibration needed. |
| 24 | Large-world management | Not implemented; ownership/loading requirements recorded | Streaming/cancellation, resource budgets and repeated-travel proof needed. |
| 25 | Multiplayer sync | Not implemented; transport API assessment recorded | Script message delivery, authority, sessions, replication and reconnect tests needed. |
| 26 | Persistent online worlds | Not implemented | Server architecture, persistence, concurrent-client tests and scalability needed. |

## Where to use the new scripts

The [Libraries catalog](../Resources/Libraries/README.md) lists each component and
its adjacent usage guide. [C++ recommendations](CPP_Recommendations.md) contains
the exact native gaps, script workarounds and acceptance checks. No placeholder
implementation is counted as a completed advanced feature.

The complete [Five Questions game](../Resources/Appeal/Games/Five_Questions/README.md)
still proves basic single-player trivia. The new components broaden foundations
for other genres, but their [readiness estimates](Game_Genre_Readiness.md) have not
been automatically increased without further representative-game integration.

## Measured boundaries

- The continuation's final regression run passed 24 selected component/game
  suites, including camera rendering, static 3D rendering, physics, pose, FSM,
  inventory, combat, dialogue, turns/grid, crafting, generation, scheduling,
  vehicle/action windows, earlier runtime/save/input/audio and quiz logic tests.
  This is not a claim that every test in the repository was rerun.
- Camera, static 3D and pose tests use real Graphics plugins; collision tests use
  both real Box2D and Jolt. Other tests use the actual Appeal interpreter and
  relevant Storage/World APIs, not mocks of the script implementation.
- A seed/max-size generation batch took about one second; separate 32x24
  generation plus World construction took about 0.13 seconds, including process
  startup. Generation belongs at load boundaries. Profile connectivity queries
  and per-cell World calls before choosing a native optimization.
- A scheduler sample selected 3,200 slots in about 0.27 seconds including startup,
  without NPC behavior/pathfinding. Decision budgeting doesn't prove large crowds.
- Inventory/crafting hide substantial SQL record/snapshot plumbing. Game-facing
  calls stay short; a future record/map or high-level Storage API is recommended.

Next work should close a selected genre's demonstrated gaps or tackle Step 23's
native prerequisites explicitly. Advanced Steps 11/17/20–22 remain partial, so
the list must not be read as every component through 22 being finished.
