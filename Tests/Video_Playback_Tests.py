"""Native video -> Graphics texture and Audio mixer integration."""
from pathlib import Path
import os,subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
script=root/'Resources/Appeal/Test'/('Video_Render_'+uuid.uuid4().hex+'.Appeal')
try:
 with tempfile.TemporaryDirectory(prefix='Video_Render_',dir=root/'Tests') as temp:
    folder=Path(temp); meshes=folder/'Graphics/Meshes';meshes.mkdir(parents=True)
    (meshes/'Quad.obj').write_text('v -1 -1 0\nv 1 -1 0\nv 1 1 0\nv -1 1 0\nvt 0 1\nvt 1 1\nvt 1 0\nvt 0 0\nf 1/1 2/2 3/3\nf 1/1 3/3 4/4\n')
    exe=folder/'Check.exe';deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'include/SDL2'),'Tests/Video_Playback_Tests.cpp',*objects,'-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=root,check=True)
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Audio Appeal_SDL2_Audio\nAdd_Dynamic_Module Video Appeal_FFmpeg_Video\n'+
        f'Graphics Set_Paths "{folder.as_posix()}"\n'+
        'Graphics Add_Window Main "Video integration check" 640 480\nGraphics Set_GUI_Visible False\nGraphics Set_FPS 120\n'+
        'Video Load Intro Sample.mp4\nVideo Bind_Texture Intro Live_Video\nVideo Update\nVideo Draw Intro 20 20 192 128\n'+
        'Graphics Load_Mesh Quad Quad.obj\nGraphics Load_Asset Panel Quad\nGraphics Set_Texture Panel Live_Video\nGraphics Paste Panel 0 0 0\nVideo Play\nReturn\n')
    p=subprocess.run([str(exe),'Test/'+script.stem],cwd=root,env=dict(os.environ,SDL_AUDIODRIVER='dummy',PATH=str(root)+os.pathsep+os.environ.get('PATH','')),capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    assert p.stdout.strip()=='VIDEO_PLAYBACK_PASSED',p.stdout
    print(p.stdout.strip())
except subprocess.TimeoutExpired as e:
 print(e.stdout,e.stderr)
 raise
finally:script.unlink(missing_ok=True)
