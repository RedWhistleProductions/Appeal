"""Render real saved map thumbnails and exercise the shared component host."""
from pathlib import Path
import os,shutil,subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Level_Editor'
script=root/'Resources/Appeal'/base/('Render_Test_'+uuid.uuid4().hex+'.Appeal')
frame_script=script.with_name('Frame_'+uuid.uuid4().hex+'.Appeal')
try:
 with tempfile.TemporaryDirectory(prefix='Level_Render_',dir=root/'Tests') as temp:
    folder=Path(temp);images=folder/'Graphics/Sprites/Examples';images.mkdir(parents=True)
    shutil.copyfile(root/'Resources/Graphics/Sprites/Examples/Place_Holder_Tiles.png',images/'Place_Holder_Tiles.png')
    exe=folder/'Check.exe'
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'include/SDL2'),'Tests/Level_Editor_Render_Tests.cpp',*objects,'-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=root,check=True)
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'+
        f'Run "{base}/Tile_Editor/Catalog"\nRun "{base}/Tile_Editor/Editor"\nRun "{base}/Editor"\n'+
        """
Graphics Add_Window Main "Level rendering check" 1536 864
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Docking True
Graphics Set_GUI_Visible True
Set_Var TE_Render 1
Set_Var TE_New_Columns 7
Set_Var TE_New_Rows 5
Set_Var TE_Default red_100
Call TE_New
Set_Var TE_Name Red
Call TE_Save_Map
Set_Var TE_Default blue_100
Set_Var TE_New_Columns 11
Set_Var TE_New_Rows 7
Call TE_New
Set_Var TE_Name Blue
Call TE_Save_Map
Set_Var TE_New_Columns 7
Set_Var TE_New_Rows 5
Set_Var TE_Default red_050
Call TE_New
Set_Var TE_Name Semi
Call TE_Save_Map
Call TE_Clear_Drawables
Set_Var LE_Render 1
Call LE_Refresh_Maps
Call LE_New
Set_Var LE_Assign_Name Red
Call LE_Assign
Set_Var LE_Selected 1
Set_Var LE_Assign_Name Blue
Call LE_Assign
Set_Var LE_Selected 2
Set_Var LE_Assign_Name Semi
Call LE_Assign
Set_Var LE_Selected 1
Call LE_Show_Tile
Set_Var TE_X 1
Set_Var TE_Y 1
Set_Var TE_Paint_Sprite gray_100
Call TE_Paint
Set_Var Draft (World Export_Level)
Call LE_Show_Level
If Var Draft != (World Export_Level)
{
    Print DRAFT_CHANGED
}
Set_Var LE_Selected 0
Call LE_Show_Tile
If Var GE_Mode != Level
{
    Print UNSAVED_GUARD_FAILED
}
Set_Var TE_Confirm 1
Call LE_Show_Tile
Call LE_Show_Level
Set_Var Check 0
Loop While Var Check < 4
{
    Call LE_Grid
    Call LE_Workspace
    Graphics Render
    Graphics Update
    Set_Var Check (Add Var Check 1)
}
Done
""")
    frame_script.write_text('If Var GE_Mode == Level { Call LE_Grid }\nIf Var GE_Mode == Tile { Call TE_Grid }\nCall LE_Workspace\nGraphics Render\nIf Var GE_Mode == Level { Call LE_Input }\nIf Var GE_Mode == Tile { Call TE_Poll_Folders Call TE_Input }\nGraphics Update\nReturn\n')
    env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH',''))
    p=subprocess.run([str(exe),base+'/'+script.stem,base+'/'+frame_script.stem],cwd=root,env=env,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    assert p.stdout.strip()=='LEVEL_EDITOR_RENDER_PASSED',p.stdout
    print(p.stdout.strip())
finally:
    script.unlink(missing_ok=True)
    frame_script.unlink(missing_ok=True)
