"""Native mouse/keyboard play and real asset rendering for the genre collection."""
from pathlib import Path
import os,subprocess,tempfile,struct,zlib
ROOT=Path(__file__).resolve().parents[1]
deps=ROOT/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps'
with tempfile.TemporaryDirectory(prefix='Arcade_Render_',dir=ROOT/'Tests') as tmp:
    folder=Path(tmp);setup=folder/'Setup.Appeal';frame=folder/'Frame.Appeal';command=folder/'Command.Appeal';exe=folder/'Check.exe'
    setup.write_text('''Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics
Add_Dynamic_Module Input Appeal_SDL2_Input
Add_Dynamic_Module Storage Appeal_SQLite_Storage
Add_Dynamic_Module Audio Appeal_SDL2_Audio
Storage Open_Memory Arcade_Render
Run Games/Genre_Arcade/Core
Run Games/Genre_Arcade/Interface
Set_Var A_Profile Arcade_Render
Call Arcade_Init
Audio Load_SFX Arcade_Win "Quiz/Correct.wav"
Audio Load_SFX Arcade_Lose "Quiz/Wrong.wav"
Set_Var A_Audio 1
Graphics Add_Window Main "Bananas Genre Arcade - test" 1100 800
Graphics Set_FPS 10000
Graphics Set_GUI_Docking False
Graphics Set_GUI_Visible True
Graphics GUI_Set_Window_Functions Genre_Arcade Arcade_Quit ""
Input Set_Focus GUI
Return
''')
    frame.write_text('Input Update Default\nCall Game_Dispatch_Events\nCall Arcade_Draw\nGraphics Render\nGraphics Update\nReturn\n')
    objects=[str(p) for p in (ROOT/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'windows-x64/include/SDL2'),'-I'+str(deps/'imgui'),
        'Tests/Genre_Arcade_Render_Tests.cpp',*objects,'-L'+str(deps/'windows-x64/lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=ROOT,check=True)
    p=subprocess.run([str(exe),str(setup),str(frame),str(command),str(folder/'Arcade_')],cwd=ROOT,
        env=dict(os.environ,SDL_AUDIODRIVER='dummy',PATH=str(ROOT)+os.pathsep+os.environ.get('PATH','')),capture_output=True,text=True,timeout=60)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    for shot in folder.glob('Arcade_*.ppm'):
        _,size,_,rgb=shot.read_bytes().split(b'\n',3);w,h=map(int,size.split())
        def chunk(kind,data):return struct.pack('>I',len(data))+kind+data+struct.pack('>I',zlib.crc32(kind+data)&0xffffffff)
        pixels=b''.join(b'\0'+rgb[y*w*3:(y+1)*w*3] for y in range(h))
        out=ROOT/'Tests/Artifacts'/('Genre_'+shot.stem+'.png');out.parent.mkdir(exist_ok=True)
        out.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(pixels))+chunk(b'IEND',b''))
    print(p.stdout.strip())
