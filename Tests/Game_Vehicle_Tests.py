from game_script_test import run
out=run('''Run Libraries/Game/Runtime
Run Libraries/Game/Vehicle
Call Game_Start
Set_Var Vehicle_Throttle 100
Set_Var Vehicle_Forward_X 1
Set_Var Vehicle_Forward_Z 0
Set_Var Vehicle_Delta_Seconds 0.1
Set_Var Frame 0
Loop While Var Frame < 20
{
    Call Vehicle_Update
    Set_Var Frame (Add Var Frame 1)
}
Print Var Vehicle_Speed
Set_Var Vehicle_Throttle 0
Set_Var Vehicle_Brake 100
Set_Var Frame 0
Loop While Var Frame < 10
{
    Call Vehicle_Update
    Set_Var Frame (Add Var Frame 1)
}
Print Var Vehicle_Speed
Set_Var Vehicle_Throttle -100
Set_Var Vehicle_Brake 0
Set_Var Frame 0
Loop While Var Frame < 10
{
    Call Vehicle_Update
    Set_Var Frame (Add Var Frame 1)
}
Print Var Vehicle_Speed
Call Game_Pause
Call Vehicle_Update
Print Var Vehicle_Velocity_X
Print Var Vehicle_Speed
''')
assert list(map(float,out))==[300,0,-120,0,-120],out
out=run('''Add_Dynamic_Module Physics Appeal_Box2D_Physics
Run Libraries/Game/Runtime
Run Libraries/Game/Vehicle
Physics Set_Gravity 0 0 0
Physics Use Car Default
Physics Set_Box Car 2 1
Set_Var Vehicle_Body Car
Set_Var Vehicle_Forward_X 1
Set_Var Vehicle_Forward_Z 0
Set_Var Vehicle_Speed 10
Set_Var Vehicle_Throttle 0
Set_Var Vehicle_Drag 0
Call Game_Start
Call Vehicle_Update
Call Vehicle_Apply
Physics Step 0.1
Print (Physics Get Car Position_X)
Physics Release Car
''')
assert abs(float(out[0])-1)<.01,out
print('GAME_VEHICLE_TESTS_PASSED')
