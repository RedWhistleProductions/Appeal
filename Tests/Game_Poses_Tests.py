from game_script_test import run
out=run('''Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics
Run Libraries/Game/Poses
Graphics Add_Window Main "Pose test" 400 300
Graphics Load_Animation Actor Idle Examples/Characters/Adam/Adam_Movement.png 64 64 0 8
Graphics Load_Animation Actor Walk Examples/Characters/Adam/Adam_Movement.png 64 64 8 8
Graphics Load_Animation Actor Hurt Examples/Characters/Adam/Adam_Movement.png 64 64 0 8
Graphics Load_Animation Other Idle Examples/Characters/Eve/Eve_Movement.png 64 64 0 8
Graphics Set_Animation_FPS Actor Idle 1
Set_Var Pose_Asset Actor
Call Pose_Apply
Graphics Paste Actor 0 0 0
Graphics Render
Delay 1
Call Pose_Apply
Graphics Render
Print (Graphics Animation_Frame Actor)
Set_Var Pose_Moving 1
Call Pose_Apply
Print (Graphics Current_Animation Actor)
Set_Var Pose_Hurting 1
Call Pose_Apply
Print (Graphics Current_Animation Actor)
Set_Var Pose_Dying 1
Set_Var Pose_Dead Missing
Call Pose_Apply
Print Var Pose_OK
Print (Graphics Current_Animation Actor)
Set_Var Pose_Asset Other
Set_Var Pose_Dying 0
Set_Var Pose_Hurting 0
Set_Var Pose_Moving 0
Call Pose_Apply
Print (Graphics Current_Animation Other)
Print (Graphics Current_Animation Actor)
Graphics Close_All
''')
assert int(out[0])>=1,out
assert out[1:]==['Walk','Hurt','0','Hurt','Idle','Hurt'],out
print('GAME_POSES_TESTS_PASSED')
