"""Small real-interpreter harness shared by component integration tests."""
from pathlib import Path
import os,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
def run(body,expected=None):
    with tempfile.NamedTemporaryFile(mode='w',suffix='.Appeal',dir=ROOT/'Tests',delete=False,encoding='utf-8') as f:
        path=Path(f.name);f.write(body+'\nDone\n')
    try:
        p=subprocess.run([str(ROOT/'Appeal.exe'),str(path)],cwd=ROOT,
            env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(ROOT/'Resources')),
            capture_output=True,text=True,timeout=30)
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        lines=p.stdout.splitlines()
        if expected is not None:assert lines==expected,(lines,expected)
        return lines
    finally:path.unlink()
