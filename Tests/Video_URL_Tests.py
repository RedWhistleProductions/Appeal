"""Serve a generated file over loopback HTTP and load/seek its media URL."""
from pathlib import Path
from http.server import BaseHTTPRequestHandler,ThreadingHTTPServer
import subprocess,threading,uuid
root=Path(__file__).resolve().parent.parent
payload=(root/'Resources/Video/Movies/Sample.mp4').read_bytes()
class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        start=0;end=len(payload)-1;header=self.headers.get('Range')
        if header:
            bounds=header.removeprefix('bytes=').split('-',1)
            start=int(bounds[0] or 0);end=min(end,int(bounds[1])) if bounds[1] else end
        self.send_response(206 if header else 200)
        self.send_header('Content-Type','video/mp4');self.send_header('Accept-Ranges','bytes')
        self.send_header('Content-Length',str(end-start+1))
        if header:self.send_header('Content-Range',f'bytes {start}-{end}/{len(payload)}')
        self.end_headers()
        try:self.wfile.write(payload[start:end+1])
        except (BrokenPipeError,ConnectionResetError):pass
    def log_message(self,*args):pass
script=root/'Resources/Appeal/Test'/('Video_URL_'+uuid.uuid4().hex+'.Appeal')
server=ThreadingHTTPServer(('127.0.0.1',0),Handler)
thread=threading.Thread(target=server.serve_forever,daemon=True);thread.start()
try:
    url=f'http://127.0.0.1:{server.server_address[1]}/Sample.mp4'
    script.write_text('Add_Dynamic_Module Video Appeal_FFmpeg_Video\n'+
        f'Print (Video Load Web "{url}")\n'+
        'Video Audio_Output Web False\nPrint (Video Width)\nPrint (Video Seek 0.5)\nVideo Play\nPrint (Video Update)\nVideo Close_All\nDone\n')
    p=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,capture_output=True,text=True,timeout=20)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    assert p.stdout.strip().splitlines()==['1','96','1','1'],p.stdout
    print('VIDEO_HTTP_PASSED')
finally:
    script.unlink(missing_ok=True);server.shutdown();server.server_close();thread.join()
