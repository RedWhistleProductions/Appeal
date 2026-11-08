# Player interfaces and navigation — Step 3 MVP

`Game/Menus` supplies one active menu's renderer-independent navigation.
`Game/Interface` optionally renders its entries using existing Graphics GUI
buttons. Games own their title/pause/options/results flow and HUD content.
Neither library opens a window, owns the loop, pauses the game, changes input
focus, or loads assets. Load each once after `Game/Runtime`.

## Reusable menu policy

```text
Run Libraries/Game/Runtime
Run Libraries/Game/Menus
Set_Var Menu_Next Title
Call Menu_Show

// Build the current menu each frame; its entries may vary between pages.
Call Menu_Begin
Set_Var Menu_Item_Event Start_Game
Set_Var Menu_Item_Value ""
Call Menu_Item
Set_Var Menu_Item_Event Show_Options
Call Menu_Item
Call Menu_End
```

Bind game handlers to these events using `Events Bind`, and call
`Game_Dispatch_Events` at your normal safe boundary. Menus emit events; handlers
can request scene changes, pause/resume, show another menu, or set game state.
No game action runs synchronously inside the GUI render callback.

| Function | Behavior |
|---|---|
| `Menu_Show` | Open/reset the menu named by Menu_Next. Selection becomes 1, known count becomes 0, pending input clears. |
| `Menu_Hide` | Hide it and discard pending movement/acceptance. |
| `Menu_Previous` / `Menu_Next_Item` | Request one selection movement for the next build. Last movement request per build wins. Wrap using the previous build's count. |
| `Menu_Accept` | Request activation of the selected row in the next build. Repeated requests before a build produce one activation. |
| `Menu_Back` | Queue a Menu_Back event carrying the current menu name. The game decides whether this means close, resume, or return to a parent. |
| `Menu_Begin` | Begin an ordered entry list and apply movement. Call once before the entries. |
| `Menu_Item` | Register the next 1-based entry. Set Menu_Item_Event and Menu_Item_Value first. If accepted and selected, queue that event/payload once per build. Empty event names do not activate. |
| `Menu_End` | Finalize count, normalize an out-of-range selection, and consume acceptance. |

Only visible/enabled entries should be registered; omit unavailable entries.
There is no fixed maximum number of entries or duplicate registry/database.
Keep ordering stable while a page is open: selection is an ordinal, not a stable
entry ID. If a page's contents change, call Menu_Show to reset selection before
accepting input. Empty menus emit nothing. Previous/next/back/accept are ignored
while hidden.

## Optional Graphics adapter

```text
Run Libraries/Game/Interface
Define_Function Click_Start
    Set_Var UI_Selected_Item 1
    Call UI_Select_Item
End_Function

// Between Menu_Begin and Menu_End, inside your own GUI window:
Set_Var UI_Item_ID Title_Start
Set_Var UI_Item_Label Start
Set_Var UI_Item_Callback Click_Start
Set_Var Menu_Item_Event Start_Game
Set_Var Menu_Item_Value ""
Call UI_Menu_Item
```

Each button callback supplies its row number and calls UI_Select_Item. Native
mouse/keyboard/controller activation then uses the same Menu_Item event path.
The next build consumes this selection; the next event dispatch delivers the
game action. Do not use this callback for immediate scene mutation.

Use unique control IDs and unique labels within a window. The current backend
uses label text as the ImGui button ID; append `##UniqueSuffix` to otherwise
duplicate labels if necessary. The suffix is hidden visually.

| Variable | Role/default |
|---|---|
| Menu_Next | Input name for Menu_Show. |
| Menu_Name / Menu_Open | Current menu name / visible flag; initially empty / 0. |
| Menu_Selected / Menu_Count | Selected 1-based row / count from the last build; initially 1 / 0. |
| Menu_Item_Event / Menu_Item_Value | Current entry's event name and string payload; initially empty. Set explicitly as needed for every row. |
| Menu_Index / Menu_Move / Menu_Confirm / Menu_Accepted | Internal frame/navigation state; use the functions above. |
| UI_Item_ID / UI_Item_Label / UI_Item_Callback | Current Graphics row's control ID, label and callback function name. |
| UI_Selected_Item | Input row number for UI_Select_Item; default 1. |
| UI_Show_Marker | Default 0. Optional `>` marker for custom script-driven selection. Native GUI focus uses the backend's own highlight instead. |
| UI_Marker | Internal row marker text. |

All Menu_/UI_ names belong to these libraries. Hook/callback names must already
be defined. UI_Menu_Item does not open/end a GUI window; the host does that.
Do not call Menu_Item again for a row already registered by UI_Menu_Item.

## Input focus and game integration

With `Input Set_Focus GUI`, use the Graphics backend's native mouse, Tab/arrows,
Enter and gamepad navigation. Input gameplay getters intentionally return no
input in this focus; do not poll Input Pressed for your menu there. Controller
behavior depends on the backend/device mapping and has not been physically
validated in this change.

With a custom-drawn interface and Game input focus, map your own input actions
to Menu_Previous, Menu_Next_Item, Menu_Accept and Menu_Back. Do not toggle focus
every frame to poll both systems; focus transitions intentionally clear state.

For a pause screen, the host calls Game_Pause and Menu_Show, routes focus to the
interface, and keeps UI rendering/event dispatch active. Resume handlers hide
the menu, restore the intended input focus, and call Game_Resume. Pause remains
separate from menu visibility. HUDs/prompts/dialogue can be drawn with existing
Graphics text/image/GUI commands; this library does not invent game stats.

## Validation and remaining work

- `python Tests/Game_Menus_Tests.py`: selection, wrapping, hidden/empty menus,
  one-shot activation, back events, and renderer-independent activation.
- `python Tests/Game_Interface_Render_Tests.py`: real SDL/ImGui mouse and
  keyboard activation, correct payload routing, and no repeated action on
  later frames. The C++ file is a test harness; production C++ is unchanged.
- Example: `Appeal.exe Examples/Game/Player_Interface`.

This is the reusable menu/navigation foundation, not a finished game's full UI.
Game-specific options, HUD layouts, styling and full-screen flows still need to
be composed and tested. Physical controller testing and accessible/custom
navigation refinements remain open. Native simplification recommendations are
recorded in [CPP_Recommendations.md](../../../Docs/CPP_Recommendations.md).
