# Appeal Libraries

Reusable libraries written in Appeal live here, organized into subfolders by
purpose. Unlike standalone utilities, libraries supply functions for a host
script to use and normally do not own its window or application loop.

Load a library using its resource-relative name:

```text
Run Libraries/Game/Runtime
Call Game_Start
```

`Libraries/...` resolves beneath the configured resource folder, independently
of the working directory. The `.Appeal` suffix is optional. Use subfolders such
as `Game`, and document each library's inputs, outputs, dependencies, and naming
prefix to avoid collisions in Appeal's shared function/variable namespace.

Available libraries:

- `Game/Runtime`: game state, timing, events, and timers.
- `Game/Scenes`: scene transitions, reload/cleanup, and update/draw hooks.
- `Game/Menus`: renderer-independent menu selection and action events.
- `Game/Interface`: optional Graphics GUI adapter for menu entries.
- `Game/Saves`: named save slots, custom progress values, and settings.
- `Game/Rules`: reusable All/Any condition groups and failure feedback.
- `Game/Interactions`: guarded actions and result notifications.
- `Game/Controls`: game/UI input gating and queued commands.
- `Game/Sound`: event-driven sound cues and explicit music selection.
- `Game/Rounds`: sequential round scoring, feedback and win/lose outcomes.
- `Game/Camera_2D`: follow/bounds, coordinate conversion and zoomed 2D drawing.
- `Game/Bodies`: fixed physics stepping, World/Mechanics sync and contact callbacks.
- `Game/Poses`: animation selection from gameplay state without per-frame resets.
- `Game/FSM`: simple NPC state/enter/exit/update lifecycle and gameplay-time age.
- `Game/Inventory`: item stacks, per-owner equipment and saveable snapshots.
- `Game/Combat`: health/armor, protection windows, healing and resource/cooldown gates.
- `Game/Dialogue`: guarded branching conversations with stale-choice rejection.
- `Game/Turns` / `Game/Grid`: action budgets, turn order and logical grid moves.
- `Game/View_3D`: static model/camera/physics-position helpers; animated 3D remains partial.
- `Game/Crafting`: atomic multi-ingredient resource costs and item production.
- `Game/Generation`: deterministic connected tile layouts and World map construction.
- `Game/Agents`: bounded round-robin decision scheduling; full crowd simulation remains open.
- `Game/Vehicle`: arcade forward/reverse acceleration and braking; full vehicle simulation remains partial.
- `Game/Action_Window`: gameplay-timed windup/active/recovery; precision animation/sports remain partial.
