from game_script_test import run
run('''Run Libraries/Game/Runtime
Run Libraries/Game/FSM
Define_Function Enter
    Print Var FSM_State
    Call FSM_Tick
End_Function
Define_Function Exit
    Print Var FSM_State
End_Function
Define_Function Think
    If Var FSM_Age >= 100 { Set_Var FSM_Next_State Chase }
End_Function
Call Game_Start
Set_Var FSM_On_Enter Enter
Set_Var FSM_On_Exit Exit
Set_Var FSM_On_Update Think
Set_Var FSM_Next_State Patrol
Call FSM_Start
Set_Var Game_Elapsed 100
Call Game_Advance
Call FSM_Tick
Print Var FSM_Age
Call Game_Pause
Call Game_Advance
Call FSM_Tick
Print Var FSM_Age
Print Var FSM_State
Call Game_Resume
Set_Var NPC_State Var FSM_State
Set_Var NPC_Time Var FSM_Entered_At
Set_Var FSM_State Idle
Set_Var FSM_Entered_At 0
Call FSM_Tick
Print Var FSM_State
Set_Var FSM_State Var NPC_State
Set_Var FSM_Entered_At Var NPC_Time
Call FSM_Tick
Print Var FSM_State
Print Var FSM_Age
''',['Patrol','Patrol','Chase','0','0','Chase','Idle','Chase','Chase','Chase','0'])
print('GAME_FSM_TESTS_PASSED')
