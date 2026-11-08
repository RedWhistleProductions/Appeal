"""Native SDL drag and drop through the real Playlist Editor widgets."""
from pathlib import Path
from contextlib import closing
import os, sqlite3, subprocess, tempfile, uuid, wave
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Playlist_Editor'
script=root/'Resources/Appeal'/base/('Render_'+uuid.uuid4().hex+'.Appeal')
frame=script.with_name('Frame_'+uuid.uuid4().hex+'.Appeal')
try:
 with tempfile.TemporaryDirectory(prefix='Playlist_Render_',dir=root/'Tests') as temp:
    folder=Path(temp); music=folder/'Audio/Music/Sub Folder';music.mkdir(parents=True)
    with wave.open(str(music/'Song One.wav'),'wb') as w:
        w.setparams((1,2,22050,0,'NONE','not compressed'));w.writeframes(b'\0\0'*2205)
    (music/'Ignore.txt').write_text('Not music')
    deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps'
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    exe=folder/'Check.exe'
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'windows-x64/include/SDL2'),'-I'+str(deps/'imgui'),
        'Tests/Playlist_Editor_Render_Tests.cpp',*objects,'-L'+str(deps/'windows-x64/lib'),'-lSDL2','-o',str(exe)],cwd=root,check=True)
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\n'+
        f'Graphics Set_Paths "{folder.as_posix()}"\nStorage Set_Paths "{folder.as_posix()}"\nStorage Use Tile_Editor\nRun {base}/Editor\n'+
        'Graphics Add_Window Main "Playlist test" 1280 800\nGraphics Set_GUI_Editor_Mode True\nGraphics Set_GUI_Docking False\nGraphics Set_GUI_Visible True\nSet_Var PE_Visible 1\nSet_Var PE_Next_Level Garden\n'+
        f'Set_Var Check_Resources "{folder.as_posix()}"\n' +
        'Graphics GUI_Begin_Frame\nCall PE_GUI\nGraphics Render\nGraphics Update\nReturn\n')
    frame.write_text('Graphics GUI_Begin_Frame\nIf Var PE_Visible == 1 { Call PE_GUI }\nGraphics Render\nReturn\n')
    p=subprocess.run([str(exe),base+'/'+script.stem,base+'/'+frame.stem],cwd=root,
        env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH','')),capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    with closing(sqlite3.connect(folder/'Storage/Tile_Editor.appealdb')) as db:
        filename=db.execute("SELECT File FROM Playlists WHERE Level_Name='Garden'").fetchone()[0]
    lines=(folder/'Audio/Playlist'/filename).read_text().splitlines()
    assert lines==[], lines
    print(p.stdout.strip())
finally:
    script.unlink(missing_ok=True);frame.unlink(missing_ok=True)
