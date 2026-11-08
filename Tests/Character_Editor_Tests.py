"""Appeal character persistence, game-defined stats, draft guards and shared GUI."""
from pathlib import Path
import subprocess,tempfile,uuid,sqlite3
from contextlib import closing
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Character_Editor'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body,host=False):
    setup='Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\n'
    if host: setup+='Add_Dynamic_Module World Appeal_Standard_World\n'
    setup+=f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'
    if host:
        setup+='Run Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog\nRun Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor\nRun Utilities/Game_Editor/Level_Editor/Editor\nRun Utilities/Game_Editor/Editor\n'
    else:setup+='Storage Open Tile_Editor\n'
    script.write_text(setup+f'Run {base}/Editor\n'+body+'\nStorage Close Tile_Editor\nGraphics Close_All\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    return result.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Character_Editor_',dir=root/'Tests') as temp:
    folder=Path(temp)
    out=run(folder,"""
Set_Var CE_New_Game First
Call CE_Create_Game
Set_Var CE_Name Alice
Call CE_Save
Set_Var First_ID Var CE_ID
Set_Var CE_New_Stat Health
Set_Var CE_New_Default 100
Call CE_Add_Stat
Print Var CE_Stat_Count
Set_Var CE_Value 125.5
Call CE_Value_Changed
Call CE_Save
Print Var CE_Status
Call CE_New
Set_Var CE_Name Bob
Call CE_Save
Print (Storage Get_Field CE_Draft Name Health Value)
Print Var CE_Count
Set_Var Callback_Value 0
Call CE_Character_Changed
Print Var CE_Name
Print Var CE_Value
Set_Var CE_Name Alice_Renamed
Call CE_Save
Print Var CE_Count
Print (Storage Get_Field Editor_Characters ID Var First_ID Default_Name)
Set_Var CE_Value invalid
Call CE_Value_Changed
Call CE_Save
Print Var CE_Status
Set_Var Callback_Value 1
Call CE_Character_Changed
Print Var CE_Name
Set_Var CE_Discard 1
Call CE_Character_Changed
Print Var CE_Name
Set_Var CE_New_Game Second
Call CE_Create_Game
Print Var CE_Game
Print Var CE_Stat_Count
Set_Var CE_Name Other
Set_Var CE_New_Stat Speed
Set_Var CE_New_Default 2.5
Call CE_Add_Stat
Call CE_Save
Print Var CE_Stat_Count
""")
    assert out==['1','Character saved.','100.0','2','Alice','125.5','2','Alice_Renamed','Enter a finite numeric stat value, for example 100, -5 or 2.5.','Alice_Renamed','Bob','Second','0','1'],out
    out=run(folder,"""
Set_Var CE_Next_Game First
Call CE_Use_Game
Print Var CE_Name
Print Var CE_Value
Set_Var CE_New_Stat Health
Set_Var CE_New_Default 10
Call CE_Add_Stat
Print Var CE_Status
Set_Var CE_New_Stat Broken
Set_Var CE_New_Default 1e999
Call CE_Add_Stat
Print Var CE_Status
Print Var CE_Stat_Count
Set_Var CE_Name ""
Call CE_Save
Print Var CE_Status
""")
    assert out==['Alice_Renamed','125.5','That stat already exists in this game.','Enter a finite numeric default value for the stat.','1','Enter a default name for the character.'],out
    out=run(folder,"""
Call TE_New
Set_Var TE_Dirty 0
Call LE_New
Set_Var LE_Dirty 0
Set_Var GE_Selected First
Call GE_Open
Call GE_Show_Character
Graphics Add_Window Main "Character Editor integration" 1100 800
Graphics Set_GUI_Docking True
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Visible True
Call LE_Workspace
Graphics Render
Graphics Update
Graphics GUI_Set_Text_Value CE_Name_Control GUI_Name
Call CE_Name_Changed
Graphics GUI_Set_Text_Value CE_Value_Control 250
Call CE_Value_Changed
Call CE_Save
Print Var CE_Status
Print Var GE_Mode
Call CE_Open_Add
Call LE_Workspace
Graphics Render
Graphics Update
Graphics GUI_Set_Text_Value CE_New_Stat_Control Mana
Graphics GUI_Set_Text_Value CE_New_Default_Control 50.5
Call CE_Add_Stat
Print Var CE_Stat_Count
Print Var CE_Value
Set_Var Callback_Value (Storage Get_Field CE_Stats Name Health Number)
Call CE_Stat_Changed
Call GE_Show_Level
Call LE_Workspace
Graphics Render
Graphics Update
Print Var GE_Mode
Call GE_Show_Character
Call LE_Workspace
Graphics Render
Graphics Update
Print Var CE_Name
Print Var CE_Value
Graphics GUI_Set_Text_Value CE_Value_Control 333
Call CE_Value_Changed
Set_Var GE_Selected Second
Call GE_Open
Print Var GE_Game
Set_Var CE_Discard 1
Call GE_Open
Print Var GE_Game
Print Var CE_Game
Print Var CE_Name
""",host=True)
    assert out==['Character saved.','Character','2','50.5','Level','GUI_Name','250.0','First','Second','Second','Other'],out
    with closing(sqlite3.connect(folder/'Storage/Tile_Editor.appealdb')) as db:
        assert db.execute("SELECT Default_Name FROM Editor_Characters WHERE Game_Name='First' ORDER BY Default_Name").fetchall()==[('Bob',),('GUI_Name',)]
    standalone=(root/'Resources/Appeal'/base/'Main.Appeal').read_text()
    standalone=standalone.replace('Storage Open Tile_Editor',f'Storage Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\nStorage Open Tile_Editor')
    standalone=standalone.replace('Loop While Var CE_Running == 1','Set_Var CE_Tick 0\nLoop While Var CE_Tick < 2').replace('    Graphics Update','    Graphics Update\n    Set_Var CE_Tick (Add Var CE_Tick 1)')
    script.write_text(standalone)
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    host_main=(root/'Resources/Appeal/Utilities/Game_Editor/Main.Appeal').read_text()
    host_main=host_main.replace('Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"',f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\nRun "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"')
    host_main=host_main.replace('Loop While Var TE_Open == 1','Set_Var CE_Tick 0\nLoop While Var CE_Tick < 2').replace('    Graphics Update','    Graphics Update\n    Set_Var CE_Tick (Add Var CE_Tick 1)')
    script.write_text(host_main)
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    print('CHARACTER_EDITOR_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
