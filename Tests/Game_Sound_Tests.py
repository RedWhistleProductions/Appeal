from pathlib import Path
import os,subprocess,tempfile,wave
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='Sound_',dir=root/'Tests') as temp:
    folder=Path(temp);tone=folder/'Cue.wav';script=folder/'Test.Appeal';exe=folder/'Check.exe'
    with wave.open(str(tone),'wb') as f:
        f.setparams((1,2,22050,0,'NONE','not compressed'));f.writeframes(b'\0\0'*22050)
    script.write_text('Add_Dynamic_Module Audio Appeal_SDL2_Audio\nRun Libraries/Game/Sound\n'
        f'Audio Load_SFX Cue "{tone.as_posix()}"\nSet_Var Sound_Cue Cue\nCall Sound_Play_Cue\nReturn\n')
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','Tests/Game_Sound_Tests.cpp',*objects,'-o',str(exe)],cwd=root,check=True)
    p=subprocess.run([str(exe),str(script)],cwd=root,env=dict(os.environ,SDL_AUDIODRIVER='dummy',
        PATH=str(root)+os.pathsep+os.environ.get('PATH','')),capture_output=True,text=True,timeout=20)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    print(p.stdout.strip())
