# Sprite Garden

Run `Appeal.exe Examples/World/Sprite_Garden` from the Appeal folder.

This script-only scene uses the existing Standard World, SDL2 Graphics/Input,
and default 2D side-scrolling Mechanics plugins. Adam uses A/D, left Shift to
run, left Ctrl to duck, and Space to jump. Eve uses arrows, right Shift/Ctrl,
and Enter. F1 changes input focus; Escape closes the window in Game focus.

`Setup_Sprite_Garden.Appeal` creates six adjacent ground sections and three scenery
sections, sharing immutable grass, soil, stone, trees, bushes, rocks, signs and
fences. The crate and both characters are mutable entities. Characters are freely
positioned outside the tile array; their Mechanics positions are copied into
World each frame, including the current camera offset. The existing flat-ground Mechanics behavior is used; props
are decorative and do not add collision, climbing or swimming.

`Bind_Sprite_Garden.Appeal` reads actual tile pointers through World queries and
creates separate Graphics placements for occupied cells. Graphics caches the
sprite sheets, while each placement keeps its own position. This is the script
bridge to Graphics; World Draw does not automatically render SDL sprites.
Re-run the binding script after editing tile placements to update this fixed
scene (removing tiles also requires removing their previous Graphics assets).
The atlases and their original license files remain in Graphics/Sprites/Examples.

The 1024px view allows character X positions from 0 to 832 (192px sprites).
Either character passing X=800 moves both players to the next screen, entering
at X=64 and X=256. Passing left of X=32 moves them to the previous screen,
entering at X=576 and X=768. Garden/Orchard, Trail/Ridge and Camp/Bridge loop continuously
in either direction. Jump heights are preserved and new spawn positions are
clear of the transition thresholds. If players cross opposite edges in the
same frame, the right transition wins. Each screen keeps its own World tile
positions; the script selects the screen and adjusts the camera offset.

The renderer definitions are loaded once from Define_Sprite_Garden_Renderer.Appeal.
Bind_Sprite_Garden.Appeal reuses the same drawable slots for the current sections
and subtracts Scene_Camera_X from tile coordinates. Sprite sheets stay cached.
Sprite_Garden_Transitions.Appeal owns the shared camera transition.

F1 (or controller Back) gives focus to a visible Sprite Garden Controls panel,
with Adam/Eve movement, speed, run/duck and controller dead-zone controls.
The panel is hidden while Game has focus. Sprite_Garden_GUI.Appeal builds
the panel inside the existing render loop.

Level designs can now be saved to Test_Game using Save_Sprite_Garden_Level.Appeal
and restored with Load_Sprite_Garden_Level.Appeal. See
[database level design documentation](../../Setup/Level_Designs.md).
