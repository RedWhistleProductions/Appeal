# Game genre readiness

Updated 2026-11-08 after the Step 1–8 MVPs and the Five Questions game. These are provisional planning
estimates of readiness to build a basic representative game using Appeal scripts,
existing modules/plugins, and external assets. They exclude utilities, packaging,
distribution, selling, and the effort of creating a particular game's content.

The percentages are judgment estimates rounded to five points, not measured
feature coverage, development effort completed, or a guarantee that no C++ gaps
will emerge. The original estimates were not calculated from a weighted checklist;
this revision preserves their scale for continuity. Use requirements and practical
game tests to make work decisions rather than small differences between scores.

## What changed

| Step | Implemented foundation | Commit | Important remaining boundary |
|---|---|---|---|
| 1 | Game states, monotonic timing, pause/scaling, fixed-step helper, events/timers | a921805 | Existing module clocks are not automatically synchronized. |
| 2 | Scene lifecycle hooks, safe-boundary transitions, reload and closure | 8dabbc9 | Generic entity iteration and automatic cross-module cleanup remain open. |
| 3 | Reusable menu navigation and optional Graphics GUI adapter | 6c86e37 | Full game interfaces and physical controller validation remain open. |
| 4 | Named save slots, World/runtime state, custom progress hooks, separate settings | 75d50e8 | External module resources/timers require explicit rebuild hooks; corruption recovery/migration remain open. |
| 5 | All/Any rules and guarded interactions | 3566ee6 | Game-specific state and effect hooks remain explicit. |
| 6 | Game/UI input gating, neutral reads and queued commands | a767132 | Physical device coverage and complete movement adapters remain open. |
| 7 | Sound cues and music policy | d82a8a5 | Full audio groups, fades and per-instance cleanup remain open. |
| 8 | Sequential progression, scoring and outcomes | aa84d14 | Complex quests/unlocks and other progression policies remain game-specific. |

Script/runtime tests, real World/SQLite integration, cross-process persistence,
save-write rollback, and native mouse/keyboard GUI activation passed during
implementation. Component tests alone do not establish complete games. The
additional Five Questions integration tests now demonstrate one complete basic quiz.

Other genre scores retain the prior Steps 1–4 estimates; they have not been
automatically increased by the new components. Quiz readiness moves from the last
90% estimate to 100% for the defined basic scope proven by Five Questions.
Online readiness is unchanged: replication and authoritative sessions remain open.

## Updated genre estimates

| Genre | Original estimate | Current | Main remaining needs or integration proof |
|---|---:|---:|---|
| Text adventure / interactive fiction | 90% | **95%** | Story/inventory/branching rules; complete playable story and save/load integration. |
| Visual novel | 85% | **90%** | Dialogue sequencing, choices, transitions and narrative progression. |
| Board / card / dice games | 85% | **90%** | Turn management, complete rules, selection/dragging and opponent behavior where needed. |
| 2D puzzle | 85% | **90%** | Puzzle rules, feedback, objectives and complete level progression. |
| Trivia / quiz | 85% | **100% — basic single-player scope** | Five Questions demonstrates timed questions, feedback, score, outcomes, pause, restart, settings and save/load. Advanced variants remain outside this scope. |
| Point-and-click adventure | 75% | **80%** | Hotspots, inventory interactions, dialogue and story conditions. |
| 2D arcade action | 75% | **80%** | Enemy spawning, collisions, scoring, difficulty and end-to-end game flow. |
| 2D platformer | 75% | **80%** | Movement/physics/animation integration, hazards, camera and transitions. |
| 2D shoot 'em up | 75% | **80%** | Projectiles, damage, enemy patterns, waves and effects. |
| 2D top-down action / adventure | 70% | **75%** | Combat, interactions, enemies, inventory and world progression. |
| Turn-based strategy / tactics | 65% | **70%** | Turns, grid navigation, action rules and opponent decisions. |
| Tower defense | 65% | **70%** | Path following, targeting, waves, placement and resource economy. |
| 2D racing / driving | 65% | **70%** | Vehicle handling, track contacts, opponents, laps and checkpoints. |
| 2D fighting | 60% | **65%** | Attack timing, hitboxes, combos, blocking and animation synchronization. |
| 2D beat 'em up | 60% | **65%** | Combat states, enemy coordination, hit reactions and encounters. |
| Turn-based RPG | 60% | **65%** | Battles, abilities, inventory/equipment, dialogue and quests. |
| Action RPG | 55% | **60%** | Real-time combat, equipment, enemy behavior and quests. |
| Roguelike / roguelite | 55% | **60%** | Procedural generation, encounters, items and run progression. |
| Real-time strategy | 50% | **55%** | Multi-unit navigation, selection/orders, economy and opponent AI. |
| Management / tycoon / city builder | 50% | **55%** | Simulation scheduling, economy, construction and agent behavior. |
| Rhythm / music game | 50% | **55%** | Audio-clock timing, input judgment and latency calibration; ordinary timers do not solve these. |
| Sports | 45% | **50%** | Specialized movement/ball physics, rules, animation and opponent/team AI. |
| 3D platformer / adventure | 45% | **50%** | Model animation, character controller, camera and scene integration. |
| First-person shooter | 40% | **45%** | First-person controller, aiming/hits, weapons, enemies and 3D integration. |
| Third-person action / shooter | 40% | **45%** | Character animation, aiming, camera obstruction, combat and 3D integration. |
| 3D racing | 40% | **45%** | Vehicle physics, track handling, chase camera and opponents. |
| Survival / crafting | 40% | **45%** | Inventory, crafting, gathering, building, persistent-world behavior and AI. |
| Flight / space simulation | 35% | **40%** | Specialized controls/physics, instruments, targeting and world scale. |
| Open-world sandbox | 30% | **35%** | Streaming, persistent entities, scalable simulation and asset management. |
| Online competitive / cooperative game | 30% | **30%** | Replication, authority, latency handling, sessions and reconnects. |
| MMORPG / persistent online world | 15% | **15%** | Authoritative servers, persistent shared state, replication and scalability. |

These categories overlap. Multiplayer is also a modifier of other genres, and
genres such as horror depend on their underlying adventure/action/survival design.
An advanced example of a high-scoring genre may require more work than a small
example of a lower-scoring one.

## Keeping the list useful

100% means the currently identified requirements for a defined basic game are
implemented and demonstrated together: beginning, playable loop, outcomes or
continuation, restart, and save/load when that game requires it. New requirements
can still emerge afterward. Trivia/quiz reaches this threshold for the basic
single-player game defined in [Five Questions](../Resources/Appeal/Games/Five_Questions/README.md).
This is a demonstrated capability, not a claim that every trivia feature exists.

The first playable genre goal is met. Subsequent work added tested foundations
through Step 22, with animation/3D/crowd/vehicle/sports gaps explicitly partial.
See [component progress](Game_Component_Progress.md) for commits, tests and scope;
other genre percentages have not been automatically raised by that work.
NPC AI should begin with
a simple finite state machine; genetic algorithms and neural networks are later
directions.

References: [runtime](Game_Runtime.html), [scenes](../Resources/Libraries/Game/Scenes.md),
[menus](../Resources/Libraries/Game/Menus.md), [saves](../Resources/Libraries/Game/Saves.md),
and [deferred C++ recommendations](CPP_Recommendations.md).
