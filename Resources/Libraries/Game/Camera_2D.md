# 2D camera and presentation — Step 9 MVP

Load `Libraries/Game/Camera_2D`. It supplies one reusable camera context and no
module dependency until drawing. Camera_X/Y are the world coordinate of the
viewport's top-left; +Y points down. Zoom defaults to 1. Set viewport Width/Height
and Viewport_X/Y when the host's layout changes. Read the actual window dimensions
from Graphics when resizing; the library does not poll them or own a render loop.

- Set Target_X/Y and call Camera_Follow for an immediate centered follow.
- Set Bounded=1 and Left/Top/Right/Bottom for world-edge clamping. A world smaller
  than the viewport aligns to its minimum edge. Call Camera_Clamp after manual moves.
- Set World_X/Y, call Camera_To_Screen, read Screen_X/Y. Reverse with Camera_To_World
  after setting Screen_X/Y (for mouse picking). These names all use the Camera_ prefix.
- Set Object_Width/Height and world position, call Camera_Object_Rect for zoomed
  drawing dimensions/right/bottom and a conservative Visible flag.
- Camera_Draw_Rect uses Camera_R/G/B/A (default white) through Graphics Draw_Rect.
  Camera_Draw_Sprite uses Camera_Asset through floating-point Graphics Paste_Rect.

The host draws background/world/HUD deliberately in its own order. HUD coordinates
normally bypass the camera. Paste_Rect avoids integer-Paste truncation warnings.
The MVP culls fully off-screen objects but does not clip partial objects, provide
general cross-type layering, smooth follow, shake, rotation, split-screen rendering
or automatic Y-up physics conversion. Use full-window viewports unless the host
supplies native clipping. All Camera_ variables are shared single-camera state.

Run `python Tests/Game_Camera_Tests.py` for follow/bounds/round-trip/culling tests
and `python Tests/Game_Camera_Render_Tests.py` for a real rendered rectangle check.
