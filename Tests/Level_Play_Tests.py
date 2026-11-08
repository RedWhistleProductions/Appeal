"""Level play tests use live layouts, saved maps, and restore unsaved drafts."""
from pathlib import Path
import subprocess,tempfile,uuid,shutil
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Level_Editor'
script=root/'Resources/Appeal'/base/('Play_Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Input Appeal_SDL2_Input\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'+
        f'Run {base}/Tile_Editor/Catalog\nRun {base}/Tile_Editor/Editor\nRun {base}/Editor\n'+body+'\nCall LT_Stop\nStorage Close Tile_Editor\nGraphics Close_All\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    return result.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Level_Play_',dir=root/'Tests') as temp:
    folder=Path(temp)
    images=folder/'Graphics/Sprites/Examples';images.mkdir(parents=True)
    shutil.copyfile(root/'Resources/Graphics/Sprites/Examples/Place_Holder_Tiles.png',images/'Place_Holder_Tiles.png')
    assert run(folder,"""
Call TE_New
Call LE_New
Call LT_Start
Print Var LT_Playing
Print Var LE_Status
""")==['0','Save a tile map and assign it to the level before testing.']
    setup="""
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
Call LE_New
Set_Var LE_Name Unsaved_Layout
Set_Var LE_Selected 0
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Selected 1
Set_Var LE_Assign_Name Blue
Call LE_Assign
Set_Var LE_Selected 2
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite green_100
Call TE_Paint
Set_Var Draft (World Export_Level)
"""
    out=run(folder,setup+"""
Call LT_Start
Print Var GE_Mode
Print Var Runtime_Cell
Print Var Runtime_Map_Name
Print (World Get_Tile Map 0 0 Name)
Set_Var LT_Move_X 1
Set_Var LT_Move_Y 0
Set_Var LT_Distance 100
Call LT_Step
Print Var Runtime_Cell
Print Var Runtime_Map_Name
Set_Var LT_Move_X -1
Call LT_Step
Print Var Runtime_Cell
Set_Var LT_Move_X 0
Set_Var LT_Move_Y -1
Call LT_Step
Print Var Runtime_Player_Y
Call LT_Stop
Print Var GE_Mode
Print Var LE_Dirty
Print Var TE_Dirty
Print (World Get_Tile Map 0 0 Name)
If Var Draft == (World Export_Level) { Print DRAFT_PRESERVED }
Print (Storage Get_Field Editor_Game_Levels Name Unsaved_Layout Name)
Storage Execute_SQL "CREATE TEMP VIEW LT_Link_Count AS SELECT 'All' AS Name,count(*) AS Count FROM Editor_Game_Level_Links;"
Print (Storage Get_Field LT_Link_Count Name All Count)
Storage Set_Field LE_Cells Cell 1 Map_Name Missing
Call LT_Start
Print Var LT_Playing
Print Var LE_Status
""")
    assert out==['Play','0','Red','red_100','1','Blue','0','0','Level','1','1','green_100','DRAFT_PRESERVED','','0','0','A referenced tile map is missing. Reassign it before testing.'],out
    out=run(folder,setup+"""
Graphics Add_Window Main "Level play integration" 1100 800
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Docking True
Graphics Set_GUI_Visible True
Input Set_Focus GUI
Set_Var LE_Render 1
Call LE_Build_Thumbnails
Set_Var Cycle 0
Loop While Var Cycle < 3
{
    Call LT_Start
    Call LE_Workspace
    Graphics Render
    Graphics Update
    Print Var LT_Playing
    Print Var GE_Mode
    Set_Var LT_Move_X 1
    Set_Var LT_Move_Y 0
    Set_Var LT_Distance 100
    Call LT_Step
    Call LE_Workspace
    Graphics Render
    Graphics Update
    Print Var Runtime_Map_Name
    Call LE_Show_Level
    Call LE_Workspace
    Graphics Render
    Graphics Update
    Print Var LT_Playing
    Print (Input Focus)
    If Var Draft == (World Export_Level) { Print DRAFT_PRESERVED }
    Set_Var Cycle (Add Var Cycle 1)
}
""")
    assert out==['1','Play','Blue','0','GUI','DRAFT_PRESERVED']*3,out
    print('LEVEL_PLAY_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
