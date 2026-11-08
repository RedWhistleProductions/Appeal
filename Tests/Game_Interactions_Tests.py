"""Reusable rules and interactions through the real interpreter/World DLL."""
from pathlib import Path
import os, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources'))
def run(body,expected):
    with tempfile.NamedTemporaryFile(mode='w',suffix='.Appeal',dir=root/'Tests',delete=False) as f:
        path=Path(f.name)
        f.write('Run Libraries/Game/Runtime\nRun Libraries/Game/Rules\nRun Libraries/Game/Interactions\n'+body+'\nDone\n')
    try:
        p=subprocess.run([str(root/'Appeal.exe'),str(path)],cwd=root,env=env,capture_output=True,text=True,timeout=15)
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        assert p.stdout.splitlines()==expected,(p.stdout,expected)
    finally:path.unlink()

run('''
Call Rule_Begin_All
Set_Var Rule_Left 5
Set_Var Rule_Right 10
Set_Var Rule_Operator >=
Set_Var Rule_Message "Need ten coins."
Call Rule_Check
Set_Var Rule_Left Locked
Set_Var Rule_Right Open
Set_Var Rule_Operator ==
Set_Var Rule_Message "Door is locked."
Call Rule_Check
Call Rule_End
Print Var Rule_Passed
Print Var Rule_Reason
Call Rule_Begin_Any
Call Rule_Check
Set_Var Rule_Right Locked
Call Rule_Check
Call Rule_End
Print Var Rule_Passed
Print Var Rule_Reason
Call Rule_Begin_All
Call Rule_End
Print Var Rule_Passed
Call Rule_Begin_Any
Call Rule_End
Print Var Rule_Passed
Print Var Rule_Reason
''',['0','Need ten coins.','1','','1','0','No alternative matched.'])

for op,left,right,expected in [('==','same','same','1'),('!=','a','b','1'),
    ('<','-2','0','1'),('<=','2','2','1'),('>','5','2','1'),('>=','3','4','0')]:
    run(f'''Call Rule_Begin_All
Set_Var Rule_Left {left}
Set_Var Rule_Right {right}
Set_Var Rule_Operator "{op}"
Call Rule_Check
Call Rule_End
Print Var Rule_Passed''',[expected])
run('''
Call Rule_Begin_Any
Set_Var Rule_Left A
Set_Var Rule_Right A
Set_Var Rule_Operator ==
Call Rule_Check
Set_Var Rule_Operator invalid
Call Rule_Check
Set_Var Rule_Operator ==
Call Rule_Check
Call Rule_End
Print Var Rule_Passed
Print Var Rule_Reason
''',['0','Unsupported rule operator.'])

setup='''
Set_Var Uses 0
Set_Var Key_Count 0
Define_Function Check
    Set_Var Rule_Left Var Key_Count
    Set_Var Rule_Right 1
    Set_Var Rule_Operator >=
    Set_Var Rule_Message "A key is required."
    Call Rule_Check
End_Function
Define_Function Apply
    Set_Var Uses (Add Var Uses 1)
    Set_Var Key_Count (Subtract Var Key_Count 1)
End_Function
Set_Var Interact_Actor Player
Set_Var Interact_Target Door
Set_Var Interact_Action Unlock
Set_Var Interact_Check Check
Set_Var Interact_Apply Apply
Call Game_Start
'''
run(setup+'''
Call Interact_Try
Print Var Interact_OK
Print Var Interact_Reason
Set_Var Key_Count 1
Call Interact_Try
Print Var Interact_OK
Print Var Interact_Reason
Call Interact_Try
Print Var Interact_OK
Print Var Uses
Print Var Key_Count
''',['0','A key is required.','1','','0','1','0'])
run(setup+'''
Set_Var Key_Count 10
Call Game_Pause
Call Interact_Try
Call Game_Resume
Set_Var Interact_Enabled 0
Call Interact_Try
Set_Var Interact_Enabled 1
Set_Var Interact_Target ""
Call Interact_Try
Set_Var Interact_Target Door
Set_Var Interact_Action ""
Call Interact_Try
Set_Var Interact_Action Unlock
Set_Var Interact_Apply ""
Call Interact_Try
Call Game_Stop
Call Interact_Try
Print Var Uses
Print Var Key_Count
''',['0','10'])
run(setup+'''
Define_Function Nested
    Set_Var Interact_Target Other
    Call Interact_Try
    Set_Var Uses (Add Var Uses 1)
End_Function
Set_Var Interact_Apply Nested
Set_Var Key_Count 1
Call Interact_Try
Print Var Uses
Print Var Interact_OK
Print Var Interact_Reentered
Print Var Interact_Busy
Print Var Interact_Current_Target
''',['1','1','1','0','Door'])

# Result hooks see full synchronous context; queued events only carry the target.
run(setup+'''
Define_Function Feedback
    Print Var Interact_Actor
    Print Var Interact_Action
    Print Var Interact_Reason
End_Function
Define_Function Notice
    Print Var Event_Name
    Print Var Event_Value
End_Function
Set_Var Interact_On_Result Feedback
Events Bind Interaction_Denied Notice
Events Bind Interaction_Succeeded Notice
Call Interact_Try
Set_Var Key_Count 1
Call Interact_Try
Call Game_Dispatch_Events
''',['Player','Unlock','A key is required.','Player','Unlock','',
    'Interaction_Denied','Door','Interaction_Succeeded','Door'])

# Actual reusable pickup: destroy World object once, then deny repeated requests.
run('''
Add_Dynamic_Module World Appeal_Standard_World
World Create Garden
World Create_Typed_Entity Coin Item 1 0
Set_Var Available 1
Set_Var Coins 0
Define_Function Check_Pickup
    Set_Var Rule_Left Var Available
    Set_Var Rule_Right 1
    Set_Var Rule_Operator ==
    Set_Var Rule_Message "Already collected."
    Call Rule_Check
End_Function
Define_Function Pickup
    World Entity_Destroy Var Interact_Current_Target
    Set_Var Available 0
    Set_Var Coins (Add Var Coins 1)
End_Function
Set_Var Interact_Target Coin
Set_Var Interact_Action Collect
Set_Var Interact_Check Check_Pickup
Set_Var Interact_Apply Pickup
Call Game_Start
Call Interact_Try
Call Interact_Try
Print Var Coins
Print Var Interact_Reason
World List_Entities
''',['1','Already collected.','World entities in Garden:'])
print('GAME_INTERACTIONS_TESTS_PASSED')
