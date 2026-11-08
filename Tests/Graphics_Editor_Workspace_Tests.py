"""Check fullscreen, docking, viewport rendering/input, and layout persistence."""
from pathlib import Path
import os
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
deps = root / 'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
with tempfile.TemporaryDirectory(prefix='Editor_Workspace_', dir=root / 'Tests') as temp:
    folder = Path(temp)
    executable = folder / 'Check.exe'
    subprocess.run(['g++', '-std=c++17', '-I' + str(deps / 'include/SDL2'),
                    str(root / 'Tests/Graphics_Editor_Workspace_Tests.cpp'),
                    '-L' + str(deps / 'lib'), '-lSDL2', '-lopengl32', '-o', str(executable)],
                   cwd=root, check=True)
    env = dict(os.environ, PATH=str(root) + os.pathsep + os.environ.get('PATH', ''))
    # Repeat with the saved ini to check that a restored dock layout works too.
    for _ in range(2):
        subprocess.run([str(executable), str(folder)], cwd=root, env=env, check=True, timeout=30)

# Verify the module bridge and expression return through the real interpreter.
script = root / 'Resources/Appeal/Test' / ('Fullscreen_' + uuid.uuid4().hex + '.Appeal')
try:
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nGraphics Add_Window Main "Fullscreen API" 800 600\nPrint (Graphics Toggle_Fullscreen Main)\nPrint (Graphics Toggle_Fullscreen Main)\nPrint (Graphics Toggle_Fullscreen Missing)\nGraphics Close_All\nDone\n')
    result = subprocess.run([str(root/'Appeal.exe'), 'Test/'+script.stem], cwd=root, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0 and not result.stderr, (result.stdout,result.stderr)
    assert result.stdout.strip().splitlines() == ['1','1','0'], result.stdout
    script.write_text('Add_Dynamic_Module Graphics Test\nPrint (Graphics Toggle_Fullscreen Main)\nDone\n')
    result = subprocess.run([str(root/'Appeal.exe'), 'Test/'+script.stem], cwd=root, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0 and not result.stderr, (result.stdout,result.stderr)
    assert 'Toggle_Fullscreen Main' in result.stdout and '1' in result.stdout.splitlines(), result.stdout
    print('GRAPHICS_FULLSCREEN_API_PASSED')
finally:
    script.unlink(missing_ok=True)
