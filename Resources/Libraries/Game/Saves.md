# Save/load — Step 4 MVP

Reusable named save slots using the existing SQLite Storage and World plugins.
This library adds no production C++. It saves one active scene's World snapshot,
selected runtime state, and game-defined string values. Settings are stored
separately and are not reverted when an older slot is loaded.

## Setup and normal use

```text
Add_Dynamic_Module World Appeal_Standard_World
Add_Dynamic_Module Storage Appeal_SQLite_Storage
Run Libraries/Game/Runtime
Run Libraries/Game/Scenes
Run Libraries/Game/Saves
Set_Var Save_Database My_Game_Profile
Call Save_Init

// After opening a scene through Game/Scenes:
Set_Var Save_Slot Quick_Save
Call Save_To_Slot
Print Var Save_Status

// At a safe host frame boundary:
Call Save_Load_Slot
Print Var Save_Status
```

Choose a database name/path unique to the game and player profile. Names resolve
under Storage's configured path. The host can set Storage paths before Save_Init
or provide its own writable absolute database path. User installation/save-root
policy remains separate from this library. Save_Init opens/selects the database
and creates its tables; it does not close other databases or change Storage paths.
Operations select Save_Database and leave it selected, so a host using another
database must explicitly select that database afterward.

## Game-specific values

The library cannot automatically enumerate Appeal variables. Supply short
collection/restore hooks for score, inventory, quest flags, checkpoint data, etc.

```text
Define_Function Collect_Progress
    Set_Var Save_Key Score
    Set_Var Save_Value Var Score
    Call Save_Put
End_Function
Define_Function Restore_Progress
    Set_Var Save_Key Score
    Set_Var Score (Call Save_Get)
End_Function
Set_Var Save_On_Save Collect_Progress
Set_Var Save_On_Load Restore_Progress
```

Save_Put updates the draft by key. Save_Get returns one string; a missing key or
an explicitly empty value returns an empty string. Games needing defaults should
handle that result. Values are bound parameters, not interpolated SQL: quotes,
spaces, and parentheses are data. Use text values, not embedded-NUL/binary blobs.

Collection starts with an empty draft for every save, so a slot overwrite removes
keys no longer supplied by the game. The load hook sees only the selected slot's
draft. Save_Put is intended for the save hook; Save_Get for the load hook.

## What is restored

- World entities, typed IDs, properties, transforms, sections, tile references,
  and other state represented by World Export_Level/Import_Level.
- Scene_Name, Game_State, Game_Running, Game_Paused, Game_Time and Game_Time_Scale.
- Custom values applied explicitly by Save_On_Load.

On load, the outgoing Scene_On_Exit runs while its World objects are available.
The saved snapshot is imported, then the old differently named World scene is
closed. Scene_On_Enter is **not** called: that would recreate objects already in
the snapshot. Save_On_Load restores script variables and rebuilds external
Graphics/Physics/Mechanics/AI/Audio resources as appropriate for the game.

The game clock is rebased. Real elapsed time, deltas, fixed-step backlog, pending
events and scheduled timers reset. Event bindings survive; reschedule needed
timers in Save_On_Load. Initial loading does not emit Game_Started. Successful
operations queue Game_Saved or Game_Loaded with the slot name; loading also
queues Scene_Changed with the restored scene name.

Do not call save/load from a scene enter/exit/update/draw hook. Use an event
handler or a host frame boundary outside scene execution. Scene_Busy and
Save_Busy reject overlapping operations. Loading during event dispatch clears
the remainder of the stale event batch through the existing runtime reset.

## Settings

```text
Set_Var Save_Key Master_Volume
Set_Var Save_Value 75
Call Save_Setting_Put
Set_Var Master_Volume (Call Save_Setting_Get)
```

The game applies and validates settings values itself; this does not call Audio,
Input or Graphics. Settings persist independently of slots and slot deletion.

## Functions and variables

| Function | Behavior |
|---|---|
| Save_Init | Initialize the selected per-game/profile database; require nonempty Save_Database. |
| Save_To_Slot | Capture current scene/runtime and custom values into Save_Slot. Reject empty slots, no active scene, uninitialized/busy runtime. |
| Save_Load_Slot | Load a supported existing slot and restore through hooks. Missing/version-invalid slots do not change the live scene. |
| Save_Delete_Slot | Atomically delete a slot and its custom values. Missing slots are harmless. |
| Save_Put / Save_Get | Write/read a custom draft string using Save_Key/Save_Value; Get is an expression. |
| Save_Setting_Put / Save_Setting_Get | Write/read a persistent setting string; Get is an expression. |

| Variable | Role/default |
|---|---|
| Save_Database | Required host configuration, initially empty. Keep fixed after initialization; use a separate interpreter/session for another profile. |
| Save_Slot | Input slot name, initially empty. |
| Save_Key / Save_Value | String inputs to custom-value/settings helpers. |
| Save_On_Save / Save_On_Load | Optional existing function names, initially empty. Hooks must not change library-owned state/database/slot or call another save/load. |
| Save_Ready / Save_Busy | Initialized/busy flags, initially 0. |
| Save_OK / Save_Status | Last top-level operation result: 1 on success, otherwise 0 and a message. Helpers do not replace the top-level result. |
| Other Save_ variables | Internal staging/validation state; do not edit. |

The library owns Game_Save_Slots, Game_Save_Values, Game_Save_Settings, the
Game_Save_Catalog view, and temporary Game_Save_Draft/Context/Meta tables. Do not
alter these in game hooks. Save format version is 1; future migrations are not
implemented. The catalog exposes Slot/Version/Scene/Present for a future slot UI;
script-friendly arbitrary row enumeration is not added here.

## Atomicity and limits

Snapshot, metadata, and custom slot values commit together using a savepoint.
If a write fails, the previous slot remains and Save_OK is 0. Do not wrap these
calls in your own uncommitted transaction; the host could otherwise undo or delay
durability. Settings writes are individual atomic upserts.

Missing/version-invalid slots and invalid basic metadata return a status before
touching the scene. Malformed numeric data, corrupted snapshots, database failures
or script-hook errors can still stop the interpreter. The library has no native
exception recovery/rollback of external resources. World import validates before
replacing the World scene, but the exit hook may already have released resources.
Saved metadata and embedded World scene names must agree; arbitrary hand-edited
or hostile databases are not supported. This is not a full corruption recovery,
authentication, encryption, migration, or backup system.

Only one scene is captured, not all retained World scenes, arbitrary module
state, input queues, active audio playback, native pointers, or executable
functions. These boundaries and future serialization improvements are recorded
in [CPP_Recommendations.md](../../../Docs/CPP_Recommendations.md).

Saves are synchronous and stage a full snapshot in memory. Keep them at deliberate
save/checkpoint boundaries and profile real content before adding asynchronous or
incremental native work. No large-world performance claim is made.

## Validation

`python Tests/Game_Saves_Tests.py` uses real World/SQLite DLLs and separate
processes. It checks snapshot/section references, custom text, runtime state,
settings, multiple slots/databases, stale-work reset, missing/unsupported/invalid
metadata, busy guards, overwrite/delete behavior, and rollback after an injected
write failure.

`Appeal.exe Examples/Game/Save_Load` is an in-memory runnable example and does
not create disk saves. Normal named database use, as above, persists on disk.
