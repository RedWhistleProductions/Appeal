"""Save slots through real Appeal + SQLite + World, including process restarts."""
from pathlib import Path
from contextlib import closing
import os, sqlite3, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
env=dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root/'Resources'))

with tempfile.TemporaryDirectory(prefix='Game_Saves_',dir=root/'Tests') as temp:
    folder=Path(temp)
    def run(body, expected, game='Game_One'):
        script=folder/'Driver.Appeal'
        script.write_text('Add_Dynamic_Module World Appeal_Standard_World\n'
            'Add_Dynamic_Module Storage Appeal_SQLite_Storage\n'
            'Run Libraries/Game/Runtime\nRun Libraries/Game/Scenes\nRun Libraries/Game/Saves\n'
            f'Storage Set_Paths "{folder.as_posix()}"\nSet_Var Save_Database {game}\nCall Save_Init\n'+body+'\nDone\n',encoding='utf-8')
        p=subprocess.run([str(root/'Appeal.exe'),str(script)],cwd=root,env=env,capture_output=True,text=True,timeout=20)
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        assert p.stdout.splitlines()==expected,(p.stdout,expected)
    setup='''
Define_Function Build
    World Create_Typed_Entity Hero Character 1 0
    World Set_Position Hero 25 50 0
    World Set Hero Health 70
    World Create_Section Map 2 2
    World Set_Tile Map 1 0 Hero
End_Function
Set_Var Scene_On_Enter Build
Call Game_Start
Set_Var Scene_Next Garden
Call Scene_Request
Call Scene_Apply
'''
    collect='''
Define_Function Collect
    Set_Var Save_Key Score
    Set_Var Save_Value Var Score
    Call Save_Put
    Set_Var Save_Key Text
    Set_Var Save_Value "A quoted ' value; (literal)"
    Call Save_Put
    Set_Var Save_Key Empty
    Set_Var Save_Value ""
    Call Save_Put
End_Function
Set_Var Save_On_Save Collect
'''
    restore='''
Define_Function Restore
    Set_Var Save_Key Score
    Set_Var Score (Call Save_Get)
    Set_Var Save_Key Text
    Print (Call Save_Get)
End_Function
Set_Var Save_On_Load Restore
'''
    # Persist world, runtime, custom values, and independent settings.
    run(setup+collect+'''
Set_Var Score 42
Set_Var Game_Elapsed 100
Call Game_Advance
Call Game_Pause
Set_Var Save_Slot "Slot ' One"
Call Save_To_Slot
Print Var Save_OK
Print Var Save_Status
Set_Var Save_Key Volume
Set_Var Save_Value 75
Call Save_Setting_Put
''',['1','Saved'])
    # A second process restores without invoking the scene builder.
    run(restore+'''
Set_Var Save_Slot "Slot ' One"
Call Save_Load_Slot
Print Var Save_OK
Print Var Save_Status
Print Var Scene_Name
Print Var Game_State
Print Var Game_Running
Print Var Game_Paused
Print Var Game_Time
Print Var Score
World Get Hero Health
World Get Hero Transform.X
Print (World Get_Tile Map 1 0 Name)
Set_Var Save_Key Volume
Print (Call Save_Setting_Get)
Print (Events Timer_Count)
''',["A quoted ' value; (literal)",'1','Loaded','Garden','Playing','1','1','100','42','70','25.000000','Hero','75','0'])
    # Load from another live scene: exit before importing, no duplicate enter.
    run(setup+restore+'''
Define_Function Exit
    Print Var Scene_Name
    World Get Hero Health
End_Function
Set_Var Scene_On_Exit Exit
Set_Var Scene_Next Other
Call Scene_Request
Call Scene_Apply
World Set Hero Health 1
Events After Stale 100 Wrong ""
Events Emit Wrong ""
Set_Var Save_Slot "Slot ' One"
Call Save_Load_Slot
Print Var Scene_Previous
Print (Events Timer_Count)
Print (Events Pending)
World Get Hero Health
''',['Garden','70','Other','1',"A quoted ' value; (literal)",'Other','0','2','70'])
    # Missing/version-invalid slots must not touch the active scene or events.
    run(setup+'''
Set_Var Save_Slot Missing
Events After Keep 100 E ""
Call Save_Load_Slot
Print Var Save_OK
Print Var Save_Status
Print Var Scene_Name
Print (Events Timer_Count)
Storage Set_Field Game_Save_Slots Slot Future Version 99
Set_Var Save_Slot Future
Call Save_Load_Slot
Print Var Save_Status
Print Var Scene_Name
''',['0','Save slot not found.','Garden','1','Unsupported save version.','Garden'])
    # Invalid basic metadata leaves the live scene intact.
    run(setup+'''
Storage Set_Field Game_Save_Slots Slot Broken Version 1
Storage Set_Field Game_Save_Slots Slot Broken Game_Time 0
Storage Set_Field Game_Save_Slots Slot Broken Time_Scale 1
Set_Var Save_Slot Broken
Call Save_Load_Slot
Print Var Save_OK
Print Var Save_Status
Print Var Scene_Name
World Get Hero Health
''',['0','Invalid save metadata.','Garden','70'])
    # Independent slots and whole-slot replacement remove obsolete custom keys.
    run(setup+collect+'''
Set_Var Score 9
Set_Var Save_Slot Second
Call Save_To_Slot
Set_Var Save_On_Save ""
Set_Var Save_Slot "Slot ' One"
Call Save_To_Slot
Set_Var Save_Slot Second
Call Save_Load_Slot
Set_Var Save_Key Score
Print (Call Save_Get)
Set_Var Save_Slot "Slot ' One"
Call Save_Load_Slot
Print (Call Save_Get)
''',['9',''])
    # A failed replacement rolls back both world metadata and custom values.
    run(setup+collect+'''
Storage Execute_SQL "CREATE TRIGGER Fail_Save BEFORE INSERT ON Game_Save_Values BEGIN SELECT RAISE(ABORT,'save test failure'); END;"
Set_Var Score 1000
Set_Var Save_Slot Second
World Set Hero Health 5
Call Save_To_Slot
Print Var Save_OK
Print Var Save_Status
Storage Execute_SQL "DROP TRIGGER Fail_Save;"
Call Save_Load_Slot
Set_Var Save_Key Score
Print (Call Save_Get)
World Get Hero Health
''',['0','save test failure','9','70'])
    # Reentrant/busy and missing-scene guards.
    run('''
Set_Var Save_Slot No_Scene
Call Save_To_Slot
Print Var Save_Status
Set_Var Scene_Busy 1
Call Save_Load_Slot
Print Var Save_Status
''',['No scene is open.','A save or scene operation is busy.'])
    # Settings and slots are isolated by game/profile database.
    run('''
Set_Var Save_Slot Second
Call Save_Load_Slot
Print Var Save_OK
Set_Var Save_Key Volume
Print (Call Save_Setting_Get)
''',['0',''],game='Game_Two')
    # Deleting a slot removes values but leaves settings and other slots alone.
    run('''
Set_Var Save_Slot Second
Call Save_Delete_Slot
Print Var Save_OK
Call Save_Load_Slot
Print Var Save_Status
Set_Var Save_Key Volume
Print (Call Save_Setting_Get)
Set_Var Save_Slot "Slot ' One"
Call Save_Load_Slot
Print Var Save_OK
''',['1','Save slot not found.','75','1'])
    with closing(sqlite3.connect(folder/'Storage/Game_One.appealdb')) as db:
        assert db.execute("SELECT count(*) FROM Game_Save_Values WHERE Slot='Second'").fetchone()[0]==0
        assert db.execute("SELECT Value FROM Game_Save_Settings WHERE Key='Volume'").fetchone()[0]=='75'
print('GAME_SAVES_TESTS_PASSED')
