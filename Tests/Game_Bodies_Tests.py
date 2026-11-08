from game_script_test import run
for plugin,mechanics,shape in [
    ('Appeal_Box2D_Physics','Appeal_Default_2D_Side_Scrolling_Mechanics','Physics Set_Box Floor 2000 64'),
    ('Appeal_Jolt_Physics','Appeal_Default_3D_Mechanics','Physics Set_Box_3D Floor 2000 64 2000')]:
    door='Physics Set_Box Door 400 400' if 'Box2D' in plugin else 'Physics Set_Box_3D Door 400 400 400'
    out=run(f'''Add_Dynamic_Module Physics {plugin}
Add_Dynamic_Module Mechanics {mechanics}
Add_Dynamic_Module World Appeal_Standard_World
Run Libraries/Game/Runtime
Run Libraries/Game/Bodies
World Create Test
World Create_Typed_Entity Actor Character 1 0
Physics Set_Units_Per_Meter 64
Physics Set_Gravity 0 -1800 0
Physics Use Floor Default
Physics Set_Body_Type Floor Static
{shape}
Physics Set_Position Floor 0 -32 0
Physics Use Actor Default
Physics Set_Capsule Actor 20 100
Physics Set_Position Actor 0 300 0
Mechanics Use Actor Default
Mechanics Set Actor External_Physics 1
Set_Var Body_Entity Actor
Set_Var Body_With_Mechanics 1
Set_Var Body_Foot_Offset 50
Call Game_Start
Set_Var Frame 0
Loop While Var Frame < 120
{{
    Call Bodies_Step
    Set_Var Frame (Add Var Frame 1)
}}
Call Body_Sync
Print Var Body_Grounded
Print Var Body_Y
Print (Mechanics Get Actor Position_Y)
World Get Actor Transform.Y
Call Game_Pause
Call Bodies_Step
Print Var Body_Stepped
Call Game_Resume
Set_Var Body_Delta_Seconds 0.2
Call Bodies_Step
Print Var Body_Stepped
Set_Var Body_Delta_Seconds 0.016666667
Physics Use Door Default
Physics Set_Body_Type Door Static
Physics Set Door Sensor 1
Physics Set Door Tag Door
{door}
Physics Set_Position Door 0 100 0
Set_Var Begins 0
Set_Var Ends 0
Define_Function Seen
    If Var Body_Event_Other == Door
    {{
        If Var Body_Event_Type == Begin {{ Set_Var Begins (Add Var Begins 1) }}
        If Var Body_Event_Type == End {{ Set_Var Ends (Add Var Ends 1) }}
    }}
End_Function
Set_Var Body_On_Event Seen
Call Bodies_Step
Call Body_Read_Events
Physics Set_Position Actor 1000 1000 0
Call Bodies_Step
Call Body_Read_Events
Print Var Begins
Print Var Ends
Physics Release Actor
Physics Release Door
Physics Release Floor
Mechanics Release Actor
''')
    assert out[0]=='1' and abs(float(out[1])-50)<2,out
    assert abs(float(out[2]))<2 and abs(float(out[3])-float(out[1]))<0.01,out
    assert out[4:]==['0','0','1','1'],out
print('GAME_BODIES_TESTS_PASSED')
