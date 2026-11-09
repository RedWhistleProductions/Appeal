# Bananas Genre Arcade

Launch from the Appeal checkout:

```powershell
.\Appeal.exe Games/Genre_Arcade/Main
```

A single hub connects 24 short playable challenges using existing character,
slime, terrain and sound assets. Its catalog includes all 31 roadmap genres:
23 entries have a basic challenge, one has a bounded sandbox prototype, and
seven show their remaining integration/prerequisite work instead of a fake demo.
See [the full coverage table](Coverage.md).

Use the mouse, or native GUI Tab/arrows and Enter. Each room explains its four
actions and objective. Live rooms advance automatically; Pause / resume freezes
time and rejects actions. Win a room to earn a medal, replay it, or return to the
hub. Attempts begin fresh when you leave; earned medals and sound preference
persist in `Resources/Storage/Genre_Arcade.appealdb`. The medal total includes the
sandbox challenge, so collecting 24 medals does **not** mean all genres are ready.
The window close button exits normally.

## What these challenges demonstrate

- Story and novel choices, conditional item use, a solvable switch puzzle,
  take-away rules with a competitive opponent, and a three-question quiz.
- Timed lane avoidance, lane shooting, discrete jumps over pits, a guarded relic,
  a grid duel, tower waves, lane driving, telegraphed attacks and brawler waves.
- Mana/potion decisions, real-time equipment and cooldown decisions, a new seeded
  connected ruin each attempt, an aggregate worker/army economy and farm/home upkeep.
- Wind-adjusted golf scoring, inventory-backed shelter crafting and night survival,
  thrust/fuel/heat management, and distinct camp placement in a bounded sandbox.

These are small ruleset demonstrations. They are not full genre reference games:
the jump room is a discrete jump puzzle without gravity/Box2D, shooting uses instant
lane hits rather than projectile bodies, driving uses abstract speed and lane
contacts, fighters use a telegraphed turn exchange, RTS uses aggregate counters
rather than independent moving units, golf uses one-dimensional shot distance,
and the space room uses abstract travel rather than flight dynamics. The action
RPG room is an equipment/cooldown encounter, without a full quest or loot system.
The ruin has generated walls and hunger, without combat or an item-drop economy.
The bounded sandbox has neither streaming nor persistent placed objects.

Rhythm, four 3D categories and two online categories have no playable rooms yet.
The 3D categories require further controller/camera/game integration; they are not
all necessarily blocked on new native functions. Audio synchronization, streaming
and authoritative networking have the specific open contracts described in the
[native recommendations](../../../../Docs/CPP_Recommendations.md).
No genre readiness percentages are increased by these abbreviated challenges.

## Structure and reuse

`Core.Appeal` owns game lifecycle, profile medals, pause, input dispatch and combat
bridging. `Catalog.Appeal` contains stable IDs, room descriptions and entry buttons.
`Story.Appeal`, `Strategy.Appeal` and `Action.Appeal` hold game-specific rules;
`Board.Appeal` and `Interface.Appeal` render them. A room supplies Setup, Act and
(when live) Tick functions. Its common scratch fields are reset on every start.
An input event carries the attempt epoch, so a queued click from an old room is
rejected; distinct event names preserve each action within a dispatch batch.

Existing reusable libraries provide runtime clocks, settings storage, combat,
the rival's finite state machine, inventory, atomic crafting and connected-map
generation. Scenes/Saves are loaded together for the existing settings helpers;
this game does not claim World snapshot saving. There are no production C++ changes.
NPC behavior starts with a simple FSM; genetic algorithms and neural networks
remain later options.

The map view caches generated cells at room start. Normal rendering and live
updates do not query SQLite per tile/frame. Inventory/crafting queries run on
player actions, and profile writes run on victory/settings changes. No measured
gameplay bottleneck has been established. The fixed 35-cell view and explicit
If-branch snapshots are somewhat verbose; larger maps should use a proper native
record/tile rendering interface rather than expand this fixed board implementation.

## Assets and validation

The game references the current `Graphics/Sprites/Examples/Characters` villagers
and Slime, `Graphics/Sprites/Examples/Terrain/Terrain_Tiles_Top_Down.png`, and the
existing `Audio/SFX/Quiz` tones. It does not copy or replace them. Original license
files stay beside their assets; the referenced generic art is supplied as CC0.

```powershell
python Tests/Genre_Arcade_Tests.py
python Tests/Genre_Arcade_Render_Tests.py
```

The first suite plays all 24 rooms to victory through the real interpreter, checks
representative loss paths, pause/replay, stale/batched input, unavailable rooms,
and cross-process medal/settings persistence. The second compiles a test-only
C++ harness against existing interpreter objects, plays native keyboard/mouse
flows, renders every entry with actual assets and captures screenshots under
`Tests/Artifacts/Genre_Arcade_*.png`. Audio uses dummy output in that suite; physical
controllers and audible output are not certified. These tests establish the stated
challenge scope, not complete coverage of each broad genre.
