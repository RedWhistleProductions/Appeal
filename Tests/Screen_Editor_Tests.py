"""Screen persistence, real-time phase logic, real audio, and standalone/host GUI."""
from pathlib import Path
import subprocess,tempfile,uuid,os,struct,wave
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Screen_Editor'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body,host=False):
    setup='Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module Files Appeal_Standard_Files\n'
    if host:setup+='Add_Dynamic_Module World Appeal_Standard_World\n'
    setup+=f'Storage Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'
    if host:setup+='Run Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog\nRun Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor\nRun Utilities/Game_Editor/Level_Editor/Editor\nRun Utilities/Game_Editor/Editor\n'
    else:setup+='Storage Open Tile_Editor\n'
    script.write_text(setup+f'Run {base}/Editor\n'+body+'\nCall SS_Stop\nStorage Close Tile_Editor\nGraphics Close_All\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,env=dict(os.environ,SDL_AUDIODRIVER='dummy'),capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    return result.stdout.strip().splitlines()
def bmp(path,rgb):
    data=bytes(reversed(rgb))*64*48
    path.write_bytes(b'BM'+struct.pack('<IHHI',54+len(data),0,0,54)+struct.pack('<IiiHHIIiiII',40,64,48,1,24,0,len(data),0,0,0,0)+data)
try:
 with tempfile.TemporaryDirectory(prefix='Screen_Editor_',dir=root/'Tests') as temp:
    folder=Path(temp)
    splash=folder/'Splash.bmp';title=folder/'Title.bmp';sfx=folder/'Splash.wav';theme=folder/'Theme.wav'
    bmp(splash,(255,0,0));bmp(title,(0,0,255))
    for path in (sfx,theme):
        with wave.open(str(path),'wb') as out:
            out.setparams((1,2,22050,0,'NONE','not compressed'));out.writeframes(b'\0\0'*220500)
    settings=f'Set_Var SS_Splash "{splash.as_posix()}"\nSet_Var SS_Title "{title.as_posix()}"\nSet_Var SS_SFX "{sfx.as_posix()}"\nSet_Var SS_Theme "{theme.as_posix()}"\n'
    out=run(folder,'Set_Var SS_New_Game First\nCall SS_Create_Game\n'+settings+"""
Set_Var SS_Seconds 2.5
Call SS_Save
Print Var SS_Dirty
Set_Var SS_New_Game Second
Call SS_Create_Game
Print Var SS_Seconds
Print Var SS_Splash
Set_Var SS_Next_Game First
Call SS_Use_Game
Print Var SS_Seconds
Print Var SS_Splash
Set_Var SS_Seconds -1
Call SS_Save
Print Var SS_Status
Set_Var SS_Dirty 1
Set_Var SS_Next_Game Second
Call SS_Use_Game
Print Var SS_Game
Set_Var SS_Discard 1
Call SS_Use_Game
Print Var SS_Game
""")
    assert out==['0','3','','2.5',splash.as_posix(),'Splash duration must be a number from 0 to 86400 seconds.','First','Second'],out
    out=run(folder,"""
Set_Var SS_Next_Game First
Call SS_Use_Game
Graphics Add_Window Main "Screen audio test" 800 600
Call SS_Test
Print Var SS_Phase
Print (Audio Is_Playing)
Set_Var SS_Now (Add Var SS_Start 1000)
Call SS_Step
Print Var SS_Phase
Set_Var SS_Now (Add Var SS_Start 3000)
Call SS_Step
Print Var SS_Phase
Print Var SS_Blend
Print (Audio Is_Playing)
Set_Var SS_Now (Add Var SS_Start 3500)
Call SS_Step
Print Var SS_Phase
Print Var SS_Blend
Call SS_Stop
Print (Audio Is_Playing)
Call SS_Test
Print Var SS_Phase
Print Var SS_Playing
""")
    assert out==['Splash','0','Splash','Fade','0.5','1','Title','1','0','Splash','1'],out
    out=run(folder,f"""
Call TE_New
Set_Var TE_Dirty 0
Call LE_New
Set_Var LE_Dirty 0
Set_Var GE_Selected First
Call GE_Open
Graphics Add_Window Main "Screen GUI test" 1100 800
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Docking True
Graphics Set_GUI_Visible True
Call GE_Show_Screens
Call LE_Workspace
Graphics Render
Graphics Update
Set_Var SS_Asset_Root "{folder.as_posix()}"
Call SS_Browse_Splash
Call LE_Workspace
Graphics Render
Graphics Update
Graphics GUI_File_Picker_Select SS_File_Picker "{splash.as_posix()}"
Call SS_Picked
Print Var SS_Splash
Graphics GUI_Set_Text_Value SS_Seconds_Control 0
Call SS_Changed
Call SS_Test
Print Var SS_Playing
Set_Var SS_Now (Add Var SS_Start 500)
Call SS_Step
Print Var SS_Phase
Call GE_Show_Level
Call LE_Workspace
Graphics Render
Graphics Update
Print Var SS_Playing
Call GE_Show_Screens
Graphics GUI_Set_Text_Value SS_Seconds_Control 4
Call SS_Changed
Call SS_Save
Print Var SS_Seconds
Set_Var SS_Dirty 1
Set_Var GE_Selected Second
Call GE_Open
Print Var GE_Game
Set_Var SS_Discard 1
Call GE_Open
Print Var SS_Game
""",True)
    assert out==[splash.as_posix(),'1','Fade','0','4','First','Second'],out
    for entry,counter in [('Screen_Editor','SS'),('', 'TE')]:
        source=root/'Resources/Appeal/Utilities/Game_Editor'/entry/'Main.Appeal'
        bounded=source.read_text()
        needle='Storage Open Tile_Editor' if entry else 'Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"'
        bounded=bounded.replace(needle,f'Storage Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'+needle)
        loop=f'Loop While Var {counter}_Running == 1' if entry else 'Loop While Var TE_Open == 1'
        bounded=bounded.replace(loop,'Set_Var SS_Tick 0\nLoop While Var SS_Tick < 2').replace('    Graphics Update','    Graphics Update\n    Set_Var SS_Tick (Add Var SS_Tick 1)')
        script.write_text(bounded)
        result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
        assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
    exe=folder/'Render.exe'
    subprocess.run(['g++','-std=c++17','-I'+str(deps/'include/SDL2'),'Tests/Screen_Editor_Render_Tests.cpp','-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=root,check=True)
    env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH',''),SDL_AUDIODRIVER='dummy')
    result=subprocess.run([str(exe),str(folder),str(splash),str(title)],cwd=root,env=env,capture_output=True,text=True,timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    assert 'SCREEN_EDITOR_RENDER_PASSED' in result.stdout,result.stdout
    print('SCREEN_EDITOR_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
