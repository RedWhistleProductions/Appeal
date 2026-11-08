"""Echo-template coverage, multiple formats, failure preservation, and reloads."""
from pathlib import Path
import os,re,subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
video=root/'Resources/Modules/Appeal_Module_Video'
ffmpeg=video/'Appeal_FFmpeg_Video/.deps/ffmpeg/bin/ffmpeg.exe'
script=root/'Resources/Appeal/Test'/('Video_Check_'+uuid.uuid4().hex+'.Appeal')
def run(body):
    script.write_text(body+'\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,capture_output=True,text=True,timeout=15)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    return result.stdout.strip().splitlines()
try:
    result=subprocess.run([str(root/'Appeal.exe'),'Test/Video'],cwd=root,capture_output=True,text=True,timeout=15)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    exports=re.findall(r'extern "C" \S+ (\w+)\(', (video/'Test/src/main.cpp').read_text())
    called={line.split()[0] for line in result.stdout.splitlines() if line}
    assert set(exports)<=called,set(exports)-called
    with tempfile.TemporaryDirectory(prefix='Video_Formats_',dir=root/'Tests') as temp:
        folder=Path(temp);avi=folder/'Clip.avi';webm=folder/'Silent.webm'
        subprocess.run([str(ffmpeg),'-hide_banner','-loglevel','error','-y','-i',str(root/'Resources/Video/Movies/Sample.mp4'),'-c:v','mpeg4','-c:a','pcm_s16le',str(avi)],check=True)
        subprocess.run([str(ffmpeg),'-hide_banner','-loglevel','error','-y','-i',str(root/'Resources/Video/Movies/Sample.mp4'),'-an','-c:v','libvpx-vp9',str(webm)],check=True)
        output=run(f'''
Add_Dynamic_Module Video Appeal_FFmpeg_Video
Print (Video Load AVI "{avi.as_posix()}")
Video Audio_Output AVI False
Print (Video Has_Audio)
Print (Video Load WebM "{webm.as_posix()}")
Print (Video Has_Audio)
Video Play
Print (Video Update)
Print (Video Load Bad "{(folder/'Missing.mp4').as_posix()}")
Print (Video Current_Video)
Video Close_All
''')
        assert output==['1','1','1','0','1','0','WebM'],output
        assert run('''
Add_Dynamic_Module Video Appeal_FFmpeg_Video
Video Load First Sample.mp4
Video Audio_Output First False
Video Load Second Sample.mp4
Video Audio_Output Second False
Video Use First
Video Auto_Play_Next True
Video Play
Video Seek 2.99
Delay 1
Video Update
Print (Video Current_Video)
Print (Video Auto_Play_Next_Status)
Video Close_All
''') == ['Second','1']
        # Repeated load/replacement and module unload must release decoder state.
        body=''
        for i in range(10):
            body+='Add_Dynamic_Module Video Appeal_FFmpeg_Video\n'
            body+='Video Load Intro Sample.mp4\nVideo Audio_Output Intro False\nVideo Play\nVideo Update\nVideo Pause\nVideo Close_All\n'
        assert run(body)==[]
    print('VIDEO_MODULE_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
