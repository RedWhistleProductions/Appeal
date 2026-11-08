"""Renderer-independent menu policy through the real interpreter."""
from pathlib import Path
import os, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources'))
def run(body, expected):
    with tempfile.NamedTemporaryFile(mode='w',suffix='.Appeal',dir=root/'Tests',delete=False) as f:
        path=Path(f.name)
        f.write('Run Libraries/Game/Runtime\nRun Libraries/Game/Menus\n'+body+'\nDone\n')
    try:
        p=subprocess.run([str(root/'Appeal.exe'),str(path)],cwd=root,env=env,capture_output=True,text=True,timeout=15)
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        assert p.stdout.splitlines()==expected,(p.stdout,expected)
    finally:path.unlink()
setup='''
Define_Function Selected
    Print Var Event_Value
End_Function
Events Bind Action Selected
Events Bind Menu_Back Selected
Define_Function Build
    Call Menu_Begin
    Set_Var Menu_Item_Event Action
    Set_Var Menu_Item_Value first
    Call Menu_Item
    Set_Var Menu_Item_Value second
    Call Menu_Item
    Set_Var Menu_Item_Value third
    Call Menu_Item
    Call Menu_End
End_Function
Set_Var Menu_Next Title
Call Menu_Show
Call Build
'''
run(setup+'''
Call Menu_Previous
Call Menu_Accept
Call Build
Call Game_Dispatch_Events
Print Var Menu_Selected
Call Menu_Next_Item
Call Menu_Accept
Call Build
Call Game_Dispatch_Events
Call Menu_Next_Item
Call Menu_Accept
Call Build
Call Game_Dispatch_Events
Call Build
Call Game_Dispatch_Events
Call Menu_Back
Call Game_Dispatch_Events
''',['third','3','first','second','Title'])
run(setup+'''
Call Menu_Hide
Call Menu_Accept
Call Menu_Back
Call Menu_Next_Item
Call Build
Print (Events Pending)
Set_Var Menu_Next Pause
Call Menu_Show
Print Var Menu_Selected
Print Var Menu_Count
Call Menu_Accept
Call Build
Call Game_Dispatch_Events
Call Menu_Begin
Call Menu_End
Call Menu_Accept
Call Menu_Begin
Call Menu_End
Print (Events Pending)
''',['0','1','0','first','0'])
run(setup+'''
Run Libraries/Game/Interface
Set_Var UI_Selected_Item 2
Call UI_Select_Item
Call Build
Call Game_Dispatch_Events
Call Build
Call Game_Dispatch_Events
''',['second'])
print('GAME_MENUS_TESTS_PASSED')
