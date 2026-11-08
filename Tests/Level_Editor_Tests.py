"""Saved map placement, component handoff, drafts, and reusable level layouts."""
from pathlib import Path
import subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Level_Editor'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'+
        f'Run "{base}/Tile_Editor/Catalog"\nRun "{base}/Tile_Editor/Editor"\nRun "{base}/Editor"\n'+body+'\nDone\n')
    p=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    return p.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Level_Editor_',dir=root/'Tests') as temp:
    folder=Path(temp)
    assert run(folder,"""
Call TE_New
Set_Var TE_Name Red
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite red_100
Call TE_Paint
Call TE_Save_Map
Set_Var TE_Name Blue
Set_Var TE_Paint_Sprite blue_100
Call TE_Paint
Call TE_Save_Map
Call LE_Refresh_Maps
Call LE_New
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Selected 1
Set_Var LE_Assign_Name Blue
Call LE_Assign
Set_Var LE_Name My_Level
Call LE_Save_Level
Set_Var LE_Selected 0
Call LE_Show_Tile
Print Var GE_Mode
Print Var TE_Name
Print (World Get_Tile Map 0 0 Name)
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite green_100
Call TE_Paint
Call LE_Show_Level
Print Var GE_Mode
Print (World Get_Tile Map 0 0 Name)
Set_Var LE_Selected 1
Call LE_Show_Tile
Print Var GE_Mode
Print Var TE_Name
Set_Var TE_Confirm 1
Call LE_Show_Tile
Print Var GE_Mode
Print Var TE_Name
Print (World Get_Tile Map 0 0 Name)
Set_Var TE_Name Renamed_Draft
Set_Var TE_Dirty 1
Call LE_Show_Level
Call LE_Show_Tile
Print Var GE_Mode
Print Var TE_Name
""")==['Tile','Red','red_100','Level','green_100','Level','Red','Tile','Blue','blue_100','Tile','Renamed_Draft']
    assert run(folder,"""
Call TE_New
Set_Var TE_Dirty 0
Set_Var LE_Name My_Level
Call LE_Load_Level
Print (Storage Get_Field LE_Cells Cell 0 Map_Name)
Print (Storage Get_Field LE_Cells Cell 1 Map_Name)
Set_Var LE_Selected 2
Call LE_Show_Tile
Print Var TE_Name
Set_Var TE_Name New_Map
Call TE_Save_Map
Call LE_Show_Level
Print (Storage Get_Field LE_Cells Cell 2 Map_Name)
Call LE_Save_Level
Call LE_Clear_Cell
Print (Storage Get_Field LE_Cells Cell 2 Map_Name)
Call LE_Load_Level
Print (Storage Get_Field LE_Cells Cell 2 Map_Name)
""")==['Red','Blue','My_Level_Map_2','New_Map','','New_Map']
    print('LEVEL_EDITOR_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
