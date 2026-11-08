from game_script_test import run
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Action_Window
Call Game_Start
Set_Var Window_ID Slash
Set_Var Window_Windup_MS 100
Set_Var Window_Active_MS 50
Set_Var Window_Recovery_MS 100
Call Window_Start
Print Var Window_Phase
Call Window_Start
Print Var Window_Accepted
Set_Var Game_Elapsed 100
Call Game_Advance
Call Window_Tick
Print Var Window_Phase
Call Game_Pause
Call Game_Advance
Call Window_Tick
Print Var Window_Phase
Call Game_Resume
Set_Var Game_Elapsed 50
Call Game_Advance
Call Window_Tick
Print Var Window_Phase
Set_Var Game_Elapsed 100
Call Game_Advance
Call Window_Tick
Print Var Window_Phase
Print Var Window_Missed
Call Window_Start
Set_Var Game_Elapsed 250
Call Game_Advance
Call Window_Tick
Print Var Window_Phase
Print Var Window_Missed
Call Window_Start
Call Window_Cancel
Print Var Window_Phase
''',['Windup','0','Active','Active','Recovery','Finished','0','Finished','1','Cancelled'])
print('GAME_ACTION_WINDOW_TESTS_PASSED')
