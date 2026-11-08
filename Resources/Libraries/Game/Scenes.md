# Scene lifecycle (Step 2 MVP)

Own one transient World scene at a time, with reusable lifecycle functions and
game-defined hooks. This library does not own the window, rendering loop, assets,
or another module's resources. Load it once after the runtime and World plugin.

```text
Add_Dynamic_Module World Appeal_Standard_World
Run Libraries/Game/Runtime
Run Libraries/Game/Scenes

Define_Function Build_Level
    World Create_Typed_Entity Player Character 1 0
    World Set_Position Player 0 0 0
End_Function
Set_Var Scene_On_Enter Build_Level
Call Game_Start
Set_Var Scene_Next First_Level
Call Scene_Request
Call Scene_Apply
```

## Host loop

Call these functions inside your existing loop:

```text
Call Game_Update
// Update input here, even when paused.
Call Game_Dispatch_Events
Call Scene_Apply
Call Scene_Update
Call Scene_Draw
// Submit/render the frame here.
```

Transitions requested during update/draw take effect at the following
`Scene_Apply`. Calls to `Scene_Apply`, `Scene_Update`, or `Scene_Draw` from a
lifecycle hook do nothing while another hook is executing. A hook can still
request a transition for the next boundary. Do not bypass this guard by closing
scenes directly inside a hook.

For fixed-step simulation, call `Scene_Update` inside the `Game_Next_Step` loop
and use `Game_Step_Seconds` in the update hook. Draw once afterward. For
variable-step simulation, use `Game_Delta_Seconds` in the update hook.

## Functions

| Function | Behavior |
|---|---|
| `Scene_Request` | Copy `Scene_Next` into the pending request. Last request wins. An empty name requests closure. No World changes happen yet. |
| `Scene_Request_Reload` | Request destruction and reconstruction of the current scene, including when its name is unchanged. |
| `Scene_Apply` | At a safe boundary, exit/close the old scene, create/clear the requested scene, call its enter hook, then queue `Scene_Changed` with the new scene name (empty on closure). Same-name requests do nothing unless reload was requested. |
| `Scene_Update` | Select the owned World scene and call its update hook only while Game_Running=1 and Game_Paused=0. It does not enumerate entities automatically. |
| `Scene_Draw` | Select the owned World scene and call its draw hook, including while paused/stopped. It does not submit a Graphics frame or draw World automatically. |

`Scene_Request` and `Scene_Request_Reload` do not return a value. They do not
change `Game_State`, pause status, or the game clocks. `Game_Restart` does not
implicitly reload a scene; bind a handler to `Game_Restarted` that calls
`Scene_Request_Reload` when that is the game's desired behavior.

## Variables and hooks

Names starting `Scene_` are owned by this library. Only set the documented
input/configuration variables; read the status variables.

| Variable | Role |
|---|---|
| `Scene_Next` | Input name for Scene_Request. |
| `Scene_Name` | Current owned scene name, or empty when none is open. |
| `Scene_Previous` | Last scene replaced/closed; initially empty. |
| `Scene_On_Enter` | Optional function name; creates the current scene's objects and external resources. |
| `Scene_On_Exit` | Optional function name; releases external resources and cancels owned timers/bindings **before** World closes the scene. World objects are still available here. |
| `Scene_On_Update` | Optional function name; updates game objects using the selected delta. |
| `Scene_On_Draw` | Optional function name; draws game objects/backgrounds/UI. |
| `Scene_Pending` | Whether a request awaits application. |
| `Scene_Busy` | Reentrancy guard; normally 0 outside hooks. |
| `Scene_Requested`, `Scene_Reload`, `Scene_Changing`, `Scene_Do_Change` | Internal request/transition variables. Do not edit. |

Hook names default to empty, meaning no callback. Define a function before
assigning its name. These four hooks are shared across scenes; branch on
`Scene_Name` when scenes have different behavior. In the exit hook Scene_Name
is the outgoing scene; in the enter hook it is the incoming scene. Requests
made by hooks remain pending for the next application.

## Object operations: use the existing World commands

The library does not add wrappers around commands that are already simple:

```text
World Create_Typed_Entity Enemy Character 0 0
Set_Var Enemy_ID (World Entity_ID Enemy)
World Set Enemy Health 100
World Set_Position Enemy 10 20 0
World Move Enemy 5 0 0
World Clone_Entity Enemy Second_Enemy
World Entity_Set_Active Second_Enemy 0
World Entity_Set_Active Second_Enemy 1
World Entity_Destroy Second_Enemy
```

Use 0/1 for boolean arguments. ID 0 requests an assigned ID; IDs are scoped by
type and scene. Store stable IDs when appropriate, but do not retain native
references after scene closure. Names must be unique within the scene. Clone
requires an existing source and unused target. Destroying an already missing
object is harmless. Activation affects World visibility, not automatic AI,
physics, audio, or custom script updates: the game must gate those separately.

For the MVP, update known game objects explicitly in the scene hook. World Get,
Has_Component, and list/query commands currently print values rather than return
usable expression results. Do not try to use `(World Get Enemy Health)` in a
condition. Returned-value access and generic iteration are recorded in the
[C++ recommendations](../../../Docs/CPP_Recommendations.md).

## Ownership and limitations

- Scene names used here must be reserved for this library. Entering a scene
  clears its existing World contents; this is a transient scene policy, not a
  persistent multi-scene cache. Load a saved World/level inside the enter hook
  only if it retains the same scene name.
- World closure releases World-owned entities/sections. Game-specific exit hooks
  must separately release Physics, Mechanics, Graphics, Input bindings, and other
  resources they created. This library cannot infer those dependencies.
- Do not globally reset events on transitions: unrelated game/menu timers must
  survive. Cancel owned timers and unbind owned handlers in the exit hook. An
  expired timer may already have queued its event; its handler must check the
  relevant scene/generation before acting. Automatic owner scopes are deferred.
- Changes occur synchronously at Scene_Apply. There is no background loading,
  rollback, or script-level exception recovery. World errors stop execution;
  hook functions must exist because the interpreter can silently ignore an
  unknown function name when debugging is off.
- Reusable per-object automatic iteration and end-to-end cross-module cleanup
  remain follow-up work. No additional database registry or C++ changes are
  introduced by this MVP.

## Validation

Run `python Tests/Game_Scenes_Tests.py`. Tests load the real World plugin and
exercise object creation/clone/activation/destruction, property mutation, paused
updates, draw hooks, scene changes, reload/closure, reentrant requests, restart
integration, timer cleanup, and 200 repeated tiny-scene entries.

Runnable example: `Appeal.exe Examples/Game/Scene_Lifecycle`.
