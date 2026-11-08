from game_script_test import run
run('''Run Libraries/Game/Camera_2D
Set_Var Camera_Zoom 2
Set_Var Camera_Target_X 500
Set_Var Camera_Target_Y 400
Call Camera_Follow
Print Var Camera_X
Print Var Camera_Y
Set_Var Camera_World_X 500
Set_Var Camera_World_Y 400
Call Camera_To_Screen
Print Var Camera_Screen_X
Print Var Camera_Screen_Y
Call Camera_To_World
Print Var Camera_World_X
Print Var Camera_World_Y
Set_Var Camera_Bounded 1
Set_Var Camera_Right 200
Set_Var Camera_Bottom 100
Call Camera_Follow
Print Var Camera_X
Print Var Camera_Y
Set_Var Camera_World_X -20
Set_Var Camera_World_Y 0
Set_Var Camera_Object_Width 10
Call Camera_Object_Rect
Print Var Camera_Visible
Set_Var Camera_Zoom 0
Call Camera_Clamp
Print Var Camera_Zoom
''',['300','250','400','300','500','400','0','0','0','1'])
print('GAME_CAMERA_TESTS_PASSED')
