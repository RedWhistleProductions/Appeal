"""Real World DLL integration for reusable scene lifecycle scripts."""
from pathlib import Path
import os
import subprocess
import tempfile
import time

root = Path(__file__).resolve().parents[1]
env = dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root/'Resources'))

def run(body, expected):
    with tempfile.NamedTemporaryFile(mode='w', suffix='.Appeal', dir=root/'Tests',
                                     delete=False, encoding='utf-8') as f:
        path = Path(f.name)
        f.write('Add_Dynamic_Module World Appeal_Standard_World\n'
                'Run Libraries/Game/Runtime\nRun Libraries/Game/Scenes\n' + body + '\nDone\n')
    try:
        p = subprocess.run([str(root/'Appeal.exe'), str(path)], cwd=root, env=env,
                           capture_output=True, text=True, timeout=20)
        assert p.returncode == 0 and not p.stderr, (p.stdout, p.stderr)
        assert p.stdout.splitlines() == expected, (p.stdout, expected)
    finally:
        path.unlink()

hooks = '''
Set_Var Enters 0
Set_Var Exits 0
Set_Var Updates 0
Set_Var Draws 0
Define_Function Enter
    Set_Var Enters (Add Var Enters 1)
    World Create_Typed_Entity Hero Character 1 0
    World Set Hero Health 100
    World Set_Position Hero 0 0 0
End_Function
Define_Function Exit
    Set_Var Exits (Add Var Exits 1)
End_Function
Define_Function Update
    Set_Var Updates (Add Var Updates 1)
    World Move Hero 5 0 0
End_Function
Define_Function Draw
    Set_Var Draws (Add Var Draws 1)
End_Function
Set_Var Scene_On_Enter Enter
Set_Var Scene_On_Exit Exit
Set_Var Scene_On_Update Update
Set_Var Scene_On_Draw Draw
Call Game_Start
'''

# Creation, identification, properties, updates, clones, activation, destruction.
run(hooks + '''
Set_Var Scene_Next A
Call Scene_Request
Print Var Scene_Name
Call Scene_Apply
Print (World Entity_Type Hero)
Print (World Entity_ID Hero)
World Clone_Entity Hero Copy
World Entity_Set_Active Copy 0
World List_Entities
World Entity_Destroy Copy
World Entity_Destroy Copy
Call Scene_Update
World Get Hero Transform.X
Call Game_Pause
Call Scene_Update
Call Scene_Draw
World Get Hero Transform.X
Call Game_Resume
Call Scene_Update
World Get Hero Transform.X
Print Var Updates
Print Var Draws
World List_Entities
''', ['', 'Character', '1', 'World entities in A:',
      '  Copy tag= active=0', '  Hero tag= active=1', '5.000000',
      '5.000000', '10.000000', '2', '1', 'World entities in A:', '  Hero tag= active=1'])

# Same-name selection preserves the scene; explicit reload destroys/rebuilds it.
run(hooks + '''
Set_Var Scene_Next A
Call Scene_Request
Call Scene_Apply
Call Scene_Update
Call Scene_Request
Call Scene_Apply
World Get Hero Transform.X
Call Scene_Request_Reload
Call Scene_Apply
World Get Hero Transform.X
Print Var Enters
Print Var Exits
Set_Var Scene_Next ""
Call Scene_Request
Call Scene_Apply
Print Var Scene_Name
Print Var Scene_Previous
Call Scene_Update
Call Scene_Draw
Print Var Updates
Print Var Draws
Print Var Exits
''', ['5.000000', '0.000000', '2', '1', '', 'A', '1', '0', '2'])

# An update hook can request transition but cannot close objects while executing.
run(hooks + '''
Define_Function Move_To_B
    Set_Var Scene_Next B
    Call Scene_Request
    Call Scene_Apply
    Print Var Scene_Name
    World Get Hero Health
End_Function
Set_Var Scene_On_Update Move_To_B
Set_Var Scene_Next A
Call Scene_Request
Call Scene_Apply
Call Scene_Update
Print Var Scene_Name
Call Scene_Apply
Print Var Scene_Name
Print Var Scene_Previous
Print Var Enters
Print Var Exits
World Get Hero Transform.X
''', ['A', '100', 'A', 'B', 'A', '2', '1', '0.000000'])

# Requests made by an enter hook survive until the following frame boundary.
run(hooks + '''
Define_Function Redirect
    If Var Scene_Name == A
    {
        Set_Var Scene_Next B
        Call Scene_Request
    }
End_Function
Set_Var Scene_On_Enter Redirect
Set_Var Scene_Next A
Call Scene_Request
Call Scene_Apply
Print Var Scene_Name
Print Var Scene_Pending
Call Scene_Apply
Print Var Scene_Name
Print Var Scene_Pending
''', ['A', '1', 'B', '0'])

# World cleanup runs before closure; unrelated runtime timers/events survive.
run(hooks + '''
Define_Function Cleanup
    World Get Hero Health
    Events Cancel Owned_Timer
End_Function
Define_Function Global
    Print global
End_Function
Set_Var Scene_On_Exit Cleanup
Events Bind Global Global
Set_Var Scene_Next A
Call Scene_Request
Call Scene_Apply
Events After Owned_Timer 100 Owned ""
Events After_Real Global_Timer 100 Global ""
Set_Var Scene_Next B
Call Scene_Request
Call Scene_Apply
Print (Events Timer_Count)
Set_Var Game_Elapsed 100
Call Game_Advance
Call Game_Dispatch_Events
''', ['100', '1', 'global'])

# Step 1 restart can deliberately request a scene reload through a bound handler.
run(hooks + '''
Define_Function Restart_Scene
    Call Scene_Request_Reload
End_Function
Events Bind Game_Restarted Restart_Scene
Set_Var Scene_Next A
Call Scene_Request
Call Scene_Apply
Call Scene_Update
Call Game_Restart
Call Game_Dispatch_Events
Call Scene_Apply
World Get Hero Transform.X
Print Var Enters
Print Var Exits
''', ['0.000000', '2', '1'])

# Repeated transitions reuse the typed ID without registry collisions/stale objects.
start = time.perf_counter()
run(hooks + '''
Set_Var Cycle 0
Loop While Var Cycle < 100
{
    Set_Var Scene_Next A
    Call Scene_Request
    Call Scene_Apply
    World Clone_Entity Hero Temporary
    Set_Var Scene_Next B
    Call Scene_Request
    Call Scene_Apply
    Set_Var Cycle (Add Var Cycle 1)
}
Print Var Enters
Print Var Exits
World List_Entities
''', ['200', '199', 'World entities in B:', '  Hero tag= active=1'])
elapsed = time.perf_counter() - start
print(f'GAME_SCENES_TESTS_PASSED (200 tiny-scene entries: {elapsed:.3f}s, includes process startup)')
