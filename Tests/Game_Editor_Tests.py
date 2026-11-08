"""Game catalog persistence, membership, component reuse, and unsaved guards."""
from pathlib import Path
import subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'+
        f'Run {base}/Level_Editor/Tile_Editor/Catalog\nRun {base}/Level_Editor/Tile_Editor/Editor\nRun {base}/Level_Editor/Editor\nRun {base}/Editor\n'+body+'\nDone\n')
    p=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    return p.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Game_Editor_',dir=root/'Tests') as temp:
    folder=Path(temp)
    assert run(folder,"""
Call TE_New
Set_Var TE_Dirty 0
Set_Var GE_New_Name First
Call GE_Create
Print Var GE_Game
Print Var GE_Mode
Print Var LE_Name
Call LE_Save_Level
Print Var LE_Level_Count
Set_Var GE_New_Name Second
Call GE_Create
Print Var GE_Game
Print Var LE_Level_Count
Set_Var LE_Name Second_Level
Call LE_Save_Level
Set_Var GE_Selected First
Call GE_Open
Print Var GE_Game
Print Var LE_Name
Print Var LE_Level_Count
Set_Var LE_Dirty 1
Set_Var GE_Selected Second
Call GE_Open
Print Var GE_Game
Set_Var LE_Dirty 0
Set_Var TE_Dirty 1
Call GE_Open
Print Var GE_Game
Set_Var TE_Dirty 0
Call GE_Open
Print Var GE_Game
Print Var LE_Name
Set_Var GE_New_Name First
Call GE_Create
Print Var GE_Count
Print Var GE_Game
Call GE_Show
Print Var GE_Mode
Call GE_Show_Level
Print Var GE_Mode
""")==['First','Level','First_Level_1','1','Second','0','First','First_Level_1','1','First','First','Second','Second_Level','2','Second','Game','Level']
    assert run(folder,"""
Call GE_Refresh
Print Var GE_Count
Set_Var Callback_Value 0
Call GE_Changed
Call GE_Open
Print Var GE_Game
Print Var LE_Name
Print Var LE_Level_Count
Set_Var GE_New_Name ""
Call GE_Create
Print Var GE_Count
""")==['2','First','First_Level_1','1','2']
    assert run(folder,f"""
Graphics Set_Paths "{folder.as_posix()}"
Graphics Add_Window Main "Game Editor GUI test" 1100 800
Graphics Set_GUI_Docking True
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Visible True
Set_Var GE_Mode Game
Call LE_Workspace
Graphics Render
Graphics Update
Graphics GUI_Set_Text_Value GE_New_Name_Control GUI_Game
Call GE_Create
Print Var GE_Game
Print Var GE_Mode
Call LE_Workspace
Graphics Render
Graphics Update
Call GE_Show
Call LE_Workspace
Graphics Render
Graphics Update
Print Var GE_Mode
Graphics GUI_Text_Value GE_New_Name_Control
Set_Var_Global Check_Name Graphics.GUI.GE_New_Name_Control.Value
Print Var Check_Name
Graphics Close_All
""")==['GUI_Game','Level','Game','GUI_Game']
    print('GAME_EDITOR_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
