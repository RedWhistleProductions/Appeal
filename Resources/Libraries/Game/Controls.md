# Input to gameplay — Step 6 MVP

Load `Libraries/Game/Controls` after Runtime and Input. The host owns bindings,
controller selection and `Input Update Default` once per frame. No binding or
focus changes happen implicitly. Games route outputs to their selected player
or interaction target; no particular actor, action name or movement is assumed.

- `Controls_Context` is `Game` (default) or `UI`. `Controls_Blocked` defaults to 0.
- `Controls_Check` sets Controls_Allowed. Game requires Game input focus, running,
  not paused and not blocked. UI requires GUI focus and not blocked, including
  title/pause/results screens when simulation is stopped or paused.
- Set `Controls_Action` to an existing Input binding, then call
  `Controls_Read_Button` for Controls_Down/Pressed/Released or `Controls_Read_Axis`
  for Controls_Axis. Disallowed input returns neutral zero values. UI context does
  not poll gameplay getters: Graphics handles native GUI navigation.
- Set `Controls_Event` and string `Controls_Value`, then call `Controls_Queue` to
  queue an allowed command. GUI callbacks can use this without executing gameplay
  during rendering. `Controls_Queue_Pressed` polls a game binding and queues only
  on its pressed edge. Poll each binding once per frame, not once per fixed step.

Handlers must revalidate their own current scene/target/state when dispatching.
Use an immutable question/turn/scene generation in the payload when stale commands
could affect new content. Queueing-time gates alone do not guarantee future validity.

For movement, consume neutral values every frame and stop/reset retained movement
on pause/focus/selected-player changes; this library does not track every entity's
held actions. Input owns controller dead zones and mapped analog values; do not
apply a second dead zone here. Mechanics-specific actions remain explicit.

The ordinary game-author path is bind input, read/queue an action, then handle
its event outside rendering. Complex rebinding screens and physical controller
coverage remain follow-up work. Native GUI mouse/keyboard activation is already
tested in the Interface suite.
