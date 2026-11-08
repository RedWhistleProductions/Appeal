# Static 3D presentation — Step 17 partial

`Libraries/Game/View_3D` supplies static model transforms, perspective camera
configuration, offset following and Physics-to-model position copying. It does
not complete Step 17's externally animated character pipeline.

View3D_Apply sets the named window's camera location/orientation/FOV/near/far.
Defaults match the existing renderer's camera at (0,0,-6), FOV 45, near .1, far
1000. View3D_Follow adds Target_X/Y/Z and Offset_X/Y/Z and applies the camera.
This is positional following, not look-at, obstruction avoidance or orbit controls.
Use valid positive near/far/FOV values supported by the selected backend.

Load a mesh/asset with existing Graphics calls, set Model3D_Asset and its X/Y/Z,
Yaw/Pitch/Roll and Scale_X/Y/Z, then Model3D_Apply. To copy an attached physics
body, set Model3D_Body/Physics_Units and call Model3D_From_Body after stepping.
The scale converts physics units per visual unit; rotations/feet offsets remain
explicit. No automatic resource registry/World getter is required.

Real tests demonstrate OBJ geometry with the modern OpenGL renderer; Box2D/Jolt
integration is covered separately by Bodies. Source inspection found imported
meshes are pretransformed and the script animation loader exposes 2D sheets,
not external skeletal clips. Static imported geometry must not be presented as
proof of complete animated GLB characters. Native skin/clip/marker work is deferred
in CPP_Recommendations.md, not implemented as a placeholder script feature.

Run `python Tests/Game_View3D_Tests.py` for the real lit model/camera/transform test.
Other backends, full 3D scene save/rehydration and a finished 3D game still require
their own end-to-end validation. The remaining roadmap can proceed on 2D features.
