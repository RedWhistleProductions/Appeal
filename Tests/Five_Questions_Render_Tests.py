"""Play the actual quiz with native keyboard/mouse; verify saved results."""
from pathlib import Path
from contextlib import closing
import os,sqlite3,subprocess,tempfile,struct,zlib
root=Path(__file__).resolve().parents[1]
deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps'
with tempfile.TemporaryDirectory(prefix='Quiz_Render_',dir=root/'Tests') as temp:
    folder=Path(temp);setup=folder/'Setup.Appeal';frame=folder/'Frame.Appeal';exe=folder/'Check.exe';shot=folder/'Result.ppm'
    setup.write_text('''
Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics
Add_Dynamic_Module Input Appeal_SDL2_Input
Add_Dynamic_Module World Appeal_Standard_World
Add_Dynamic_Module Storage Appeal_SQLite_Storage
Add_Dynamic_Module Audio Appeal_SDL2_Audio
Run Games/Five_Questions/Core
Run Games/Five_Questions/Interface
'''+f'Storage Set_Paths "{folder.as_posix()}"\n'+'''
Set_Var Save_Database Quiz_Render
Call Quiz_Init
Set_Var Quiz_Duration 10000000
Audio Load_SFX Quiz_Correct "Quiz/Correct.wav"
Audio Load_SFX Quiz_Wrong "Quiz/Wrong.wav"
Set_Var Quiz_Audio_Ready 1
Graphics Add_Window Main "Five Questions test" 960 640
Graphics Set_FPS 10000
Graphics Set_GUI_Docking False
Graphics Set_GUI_Visible True
Graphics GUI_Set_Window_Functions Five_Questions Quiz_Click_Quit ""
Input Set_Focus GUI
Return
''',encoding='utf-8')
    frame.write_text('Input Update Default\nCall Game_Update\nCall Game_Dispatch_Events\nCall Quiz_Draw\nGraphics Render\nGraphics Update\nReturn\n',encoding='utf-8')
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'windows-x64/include/SDL2'),
        '-I'+str(deps/'imgui'),'Tests/Five_Questions_Render_Tests.cpp',*objects,
        '-L'+str(deps/'windows-x64/lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=root,check=True)
    p=subprocess.run([str(exe),str(setup),str(frame),str(shot)],cwd=root,
        env=dict(os.environ,SDL_AUDIODRIVER='dummy',PATH=str(root)+os.pathsep+os.environ.get('PATH','')),
        capture_output=True,text=True,timeout=60)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    with closing(sqlite3.connect(folder/'Storage/Quiz_Render.appealdb')) as db:
        assert db.execute("SELECT Value FROM Game_Save_Settings WHERE Key='Best'").fetchone()[0]=='5'
        assert db.execute("SELECT Value FROM Game_Save_Settings WHERE Key='Sound'").fetchone()[0]=='0'
        assert db.execute("SELECT Value FROM Game_Save_Values WHERE Key='Score'").fetchone()[0]=='0'
    artifact=root/'Tests/Artifacts/Five_Questions_Results.png'
    artifact.parent.mkdir(parents=True,exist_ok=True)
    # Encode the captured RGB screenshot without an optional imaging dependency.
    _,dimensions,_,rgb=shot.read_bytes().split(b'\n',3)
    width,height=map(int,dimensions.split())
    def chunk(kind,data):
        return struct.pack('>I',len(data))+kind+data+struct.pack('>I',zlib.crc32(kind+data)&0xffffffff)
    pixels=b''.join(b'\0'+rgb[y*width*3:(y+1)*width*3] for y in range(height))
    artifact.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',width,height,8,2,0,0,0))+
                         chunk(b'IDAT',zlib.compress(pixels))+chunk(b'IEND',b''))
    print(p.stdout.strip())
