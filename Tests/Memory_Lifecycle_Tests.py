"""Bounded startup/reload/render/shutdown regressions across installed real plugins."""
from pathlib import Path
import os
import socket
import time
import subprocess
import tempfile
import uuid
root=Path(__file__).resolve().parent.parent
script=root/'Resources/Appeal/Test'/('Memory_Lifecycle_'+uuid.uuid4().hex+'.Appeal')
def run(body, expected):
    script.write_text(body)
    result=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,
        env=dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root/'Resources')),
        capture_output=True,text=True,timeout=20)
    assert result.returncode==0 and not result.stderr, f'exit={result.returncode}\n{result.stdout}{result.stderr}'
    assert result.stdout.strip().splitlines()==expected,result.stdout
try:
    with tempfile.TemporaryDirectory(prefix='Memory_Lifecycle_',dir=root/'Tests') as temp:
        prefix=f'Storage Set_Paths "{Path(temp).as_posix()}"\n'
        # A second DLL load must replace its dictionary pointer and close old handles.
        commands=[]
        for i in range(50):
            commands+=['Add_Dynamic_Module Storage Appeal_SQLite_Storage',prefix,
                       'Storage Use Lifecycle',f'Storage Set Value {i}','Storage Get Value']
        run('\n'.join(commands)+'\nDone\n',[str(i) for i in range(50)])
        # Leave a real server worker active when the module shuts down.
        for i in range(3):
            with socket.socket() as sock:
                sock.bind(('127.0.0.1',0)); port=sock.getsockname()[1]
            script.write_text(f'Add_Dynamic_Module Network Appeal_SDL2_Network\nNetwork Configure Echo 127.0.0.1 {port}\nNetwork Server\nDelay 1\nPrint NETWORK_LIFECYCLE_PASSED\nDone\n')
            proc=subprocess.Popen([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,
                env=dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root/'Resources')),
                stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
            try:
                for attempt in range(50):
                    try:
                        connection=socket.create_connection(('127.0.0.1',port),timeout=.2)
                        break
                    except OSError: time.sleep(.01)
                else: raise AssertionError('Network lifecycle server did not start')
                with connection:
                    connection.settimeout(2); connection.sendall(b'memory-check\n')
                    assert b'memory-check' in connection.recv(100)
                out,err=proc.communicate(timeout=5)
                assert proc.returncode==0 and not err and 'NETWORK_LIFECYCLE_PASSED' in out,(out,err,proc.returncode)
            finally:
                if proc.poll() is None: proc.kill(); proc.wait()
        scene=(root/'Resources/Appeal/Examples/World/Sprite_Garden.Appeal').read_text()
        scene=scene.replace('Run Setup/Config','Run Setup/Config\n'+prefix)
        scene=scene.replace('Loop While Var Scene_Open == 1','Set_Var Audit_Frames 0\nLoop While Var Audit_Frames < 8')
        scene=scene.replace('    Input Update Default','''    Set_Var Audit_Frames (Add Var Audit_Frames 1)
    Input Update Default
    If Var Audit_Frames == 2
    {
        Input Set_Focus GUI
    }
    If Var Audit_Frames == 4
    {
        Input Set_Focus Game
    }
    If Var Audit_Frames == 6
    {
        Mechanics Set Adam Position_X 801
    }''')
        scene=scene.replace('Done','Print MEMORY_LIFECYCLE_PASSED\nDone')
        for i in range(10):
            body=scene
            if i % 2:
                # Exercise fallback destruction with live windows, controllers and world state.
                for command in ('Input Close_Controller -1', 'Graphics Close_All', 'World Close_Scene Sprite_Garden',
                                'Mechanics Release Adam', 'Mechanics Release Eve'):
                    body=body.replace(command, '')
            run(body,['MEMORY_LIFECYCLE_PASSED'])
    print('MEMORY_LIFECYCLE_PASSED (50 SQLite reloads; 3 live server shutdowns; 10 full-module scene sessions)')
finally: script.unlink(missing_ok=True)
