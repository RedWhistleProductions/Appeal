"""Placement-specific exits, runtime map crossing, and database level selection."""
from pathlib import Path
from contextlib import closing
import sqlite3,subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Level_Editor'
script=root/'Resources/Appeal'/base/('Links_'+uuid.uuid4().hex+'.Appeal')
try:
 with tempfile.TemporaryDirectory(prefix='Level_Links_',dir=root/'Tests') as temp:
    folder=Path(temp)
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'+f'Run "{base}/Tile_Editor/Catalog"\nRun "{base}/Tile_Editor/Editor"\nRun "{base}/Editor"\n'+"""
Set_Var TE_New_Columns 2
Set_Var TE_New_Rows 2
Set_Var TE_Default red_100
Call TE_New
Set_Var TE_Name Red
Call TE_Save_Map
Set_Var TE_Default blue_100
Call TE_New
Set_Var TE_Name Blue
Call TE_Save_Map
Call LE_Refresh_Maps
Set_Var LE_New_Columns 2
Set_Var LE_New_Rows 2
Call LE_New
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Selected 1
Set_Var LE_Assign_Name Blue
Call LE_Assign
Set_Var LE_Selected 2
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Name Traverse
Call LE_Save_Level
Call LE_Refresh_Levels
Set_Var Callback_Value (Storage Get_Field LE_Saved_Levels Name Traverse Number)
Set_Var LE_Name ""
Call LE_Level_Changed
Print Var LE_Name
"""+f'Run "{base}/Runtime"\n'+"""
Set_Var Runtime_Level_Name Traverse
Set_Var Runtime_Cell 0
Set_Var Runtime_Player_X 0
Set_Var Runtime_Player_Y 20
Set_Var Notify_Count 0
Define_Function Notify
    Set_Var Notify_Count (Add Var Notify_Count 1)
End_Function
Set_Var Runtime_On_Map_Loaded Notify
Call Runtime_Level_Load
Set_Var Runtime_Player_X 135
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Map_Name
Print Var Runtime_Player_X
Set_Var Runtime_Player_X -3
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Player_X
Set_Var Runtime_Player_Y 130
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Player_Y
Set_Var Runtime_Player_X 129
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Transitioned
Set_Var Runtime_Player_Y -4
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Player_Y
Print Var Notify_Count
Set_Var Runtime_Player_X -1
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Player_X
Set_Var LE_New_Columns 1
Set_Var LE_New_Rows 2
Call LE_New
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Selected 1
Set_Var LE_Assign_Name Blue
Call LE_Assign
Set_Var LE_Name One_Column
Call LE_Save_Level
Set_Var Runtime_Level_Name One_Column
Set_Var Runtime_Cell 0
Set_Var Runtime_Player_X 20
Set_Var Runtime_Player_Y 129
Call Runtime_Level_Load
Set_Var Runtime_Player_Y 129
Call Runtime_Level_Update
Print Var Runtime_Cell
Print Var Runtime_Direction
Set_Var LE_Name Typed_New
Call LE_Save_Level
Print (Storage Get_Field LE_Level_Catalog Name All Count)
Done
""")
    p=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    assert p.stdout.strip().splitlines()==['Traverse','1','Blue','7','0','125','2','2','2','0','0','124','5','0','0','1','Below','3'],p.stdout
    with closing(sqlite3.connect(folder/'Storage/Tile_Editor.appealdb')) as db:
        assert set(db.execute("SELECT Cell,Direction,Destination_Cell FROM Editor_Game_Level_Links WHERE Level_Name='Traverse'"))=={('0','Right','1'),('1','Left','0'),('0','Below','2'),('2','Above','0')}
    print('LEVEL_LINKS_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
