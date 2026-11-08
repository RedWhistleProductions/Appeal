"""Verify code-pane font size/color and reset pixels in both OpenGL plugins."""
from pathlib import Path
import subprocess,tempfile,os
root=Path(__file__).resolve().parent.parent
base=root/'Resources/Modules/Appeal_Module_Graphics'
deps=base/'Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
with tempfile.TemporaryDirectory(prefix='IDE_Font_',dir=root/'Tests') as temp:
    folder=Path(temp);exe=folder/'Check.exe'
    subprocess.run(['g++','-std=c++17','-I'+str(deps/'include/SDL2'),'Tests/IDE_Font_Render_Tests.cpp','-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],cwd=root,check=True)
    for plugin in ['Appeal_SDL2_Legacy_OpenGL_Graphics','Appeal_SDL2_Modern_OpenGL_Graphics']:
        env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH',''))
        result=subprocess.run([str(exe),str(folder),str(base/plugin/(plugin+'.dll')),'C:/Windows/Fonts/consola.ttf'],cwd=root,env=env,capture_output=True,text=True,timeout=30)
        assert result.returncode==0 and not result.stderr,(plugin,result.stdout,result.stderr)
        print(plugin+': '+result.stdout.strip())
