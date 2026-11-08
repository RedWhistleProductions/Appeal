# Controller control sets

The GUI edits named rows in `User_Setings.appealdb`'s `Control_Sets` table. Each
Nintendo Switch Pro button has a TEXT column for its press function and an optional
`<Button>_Up` column for its release function. **Capture is excluded.** Touchpad
and four paddle pairs are optional extras for other controllers.

In Sprite Garden, press F1 and scroll to the controller settings. Enter exact
Appeal function names and click **Save and Apply**. Blank fields unbind a button.
Changing the control-set name before saving creates another row. **Load and Apply**
selects an existing row. A missing name leaves the active bindings intact; empty
names cannot be saved. The active set loads again when the GUI starts.

The seeded `Test_Game` set maps Nintendo B (south) to Adam_Jump; A (east) to
Adam_Pad_Duck_Down/Adam_Pad_Duck_Up; Y (west) to Adam_Pad_Run_Down/Up; D-pad left/right
to Adam_Pad_Left_Down/Up and Adam_Pad_Right_Down/Up. Minus defaults to
`Input.Toggle_Focus`. F1 remains available even if Minus is reassigned. Both press
and release use no-argument functions defined by the game before loading the GUI.

SDL uses physical positions: Nintendo A/B/X/Y correspond to Xbox B/A/Y/X. L/R
and ZL/ZR correspond to LB/RB and LT/RT. Triggers generate digital edges after
Input's existing dead-zone processing. Home/Guide depends on the OS/driver
allowing SDL to receive the button. Bindings apply globally to connected
controllers; stick axes and keyboard bindings remain unchanged.

After loading Graphics, Input, Storage, a graphics window and the game's callbacks:

```text
Run GUIs/CONFIG_Controller
```

The script performs database setup once and loads/applies the active set. Call
`CONFIG_Controller_GUI` each frame for a panel, or embed `CONFIG_Controller_Controls`
in your existing GUI window. Enable GUI visibility; Input focus gates rendering.
The script owns no loop. `Controller_Control_Sets.Appeal` holds persistence helpers.

Storage provides `Get_Field Table Key_Column Key Field` (returns text, or empty for
a missing row/NULL) and `Set_Field Table Key_Column Key Field Value` (upserts one
field). Identifiers are validated and row keys/values are bound parameters:

```text
Storage Use User_Setings
Print (Storage Get_Field Control_Sets Name Test_Game B)
```

Existing Set/Get and Insert/Find continue to use the original records table.
Tests: `python Tests/Controller_Database_Tests.py` verifies persistence across
sessions, names with apostrophes, missing sets and unbinding. The native tests in
`Tests/SDL2_Input_Events.ps1` cover button/trigger press and release callbacks.
