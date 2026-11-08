from game_script_test import run
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Dialogue
Set_Var Quest 0
Define_Function Content
    Set_Var Dialogue_Speaker Guard
    If Var Dialogue_Node == Greeting { Set_Var Dialogue_Text "Will you help?" }
    If Var Dialogue_Node == Quest { Set_Var Dialogue_Text "Find the key." }
End_Function
Define_Function Effect
    Set_Var Quest (Add Var Quest 1)
    Call Dialogue_Select
End_Function
Set_Var Dialogue_On_Node Content
Set_Var Dialogue_On_Choice Effect
Set_Var Dialogue_Next_Node Greeting
Call Dialogue_Start
Print Var Dialogue_Text
Set_Var Dialogue_Expected_Revision Var Dialogue_Revision
Set_Var Dialogue_Choice_ID Accept
Set_Var Dialogue_Choice_Next Quest
Set_Var Dialogue_Choice_Available 0
Call Dialogue_Select
Print Var Dialogue_Accepted
Set_Var Dialogue_Choice_Available 1
Call Dialogue_Select
Print Var Dialogue_Text
Print Var Quest
Call Dialogue_Select
Print Var Dialogue_Accepted
Print Var Quest
Set_Var Dialogue_Expected_Revision Var Dialogue_Revision
Set_Var Dialogue_Choice_ID Goodbye
Set_Var Dialogue_Choice_Next ""
Call Dialogue_Select
Print Var Dialogue_Open
Print Var Quest
Set_Var Dialogue_Node Greeting
Call Dialogue_Refresh
Print Var Dialogue_Text
Print Var Quest
''',['Will you help?','0','Find the key.','1','0','1','0','2','Will you help?','2'])
print('GAME_DIALOGUE_TESTS_PASSED')
