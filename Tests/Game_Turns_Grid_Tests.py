from game_script_test import run
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Turns
Call Game_Start
Set_Var Turn_Count 2
Set_Var Turn_Budget 2
Call Turn_Start
Set_Var Turn_Actor 0
Set_Var Turn_Expected_Revision Var Turn_Revision
Call Turn_Spend
Print Var Turn_AP
Call Turn_Spend
Print Var Turn_Accepted
Set_Var Turn_Expected_Revision Var Turn_Revision
Call Turn_Spend
Print Var Turn_AP
Set_Var Turn_Expected_Revision Var Turn_Revision
Call Turn_Spend
Print Var Turn_Accepted
Call Turn_End
Print Var Turn_Owner
Set_Var Turn_Expected_Revision Var Turn_Revision
Call Turn_End
Print Var Turn_Accepted
Set_Var Turn_Actor 1
Call Turn_End
Print Var Turn_Owner
Print Var Turn_Number
Call Game_Pause
Set_Var Turn_Actor 0
Set_Var Turn_Expected_Revision Var Turn_Revision
Call Turn_Spend
Print Var Turn_Accepted
''',['1','0','0','0','1','0','0','2','0'])
run('''Run Libraries/Game/Grid
Set_Var Grid_To_X 1
Call Grid_Check
Print Var Grid_Allowed
Set_Var Grid_To_Y 1
Call Grid_Check
Print Var Grid_Allowed
Set_Var Grid_Diagonal 1
Call Grid_Check
Print Var Grid_Allowed
Set_Var Grid_Corner_Clear 0
Call Grid_Check
Print Var Grid_Allowed
Set_Var Grid_To_X 8
Call Grid_Check
Print Var Grid_Allowed
Set_Var Grid_To_X 2
Set_Var Grid_To_Y 3
Set_Var Grid_Origin_X 10
Set_Var Grid_Origin_Y 20
Call Grid_To_World
Print Var Grid_World_X
Print Var Grid_World_Y
''',['1','0','1','0','0','138','212'])
print('GAME_TURNS_GRID_TESTS_PASSED')
