from pathlib import Path
import os,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
def run(body,expected):
    with tempfile.NamedTemporaryFile(mode='w',suffix='.Appeal',dir=root/'Tests',delete=False) as f:
        path=Path(f.name);f.write('Run Libraries/Game/Runtime\nRun Libraries/Game/Rounds\n'+body+'\nDone\n')
    try:
        p=subprocess.run([str(root/'Appeal.exe'),str(path)],cwd=root,
            env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources')),capture_output=True,text=True,timeout=15)
        assert not p.stderr and p.returncode==0,(p.stdout,p.stderr)
        assert p.stdout.splitlines()==expected,(p.stdout,expected)
    finally:path.unlink()
run('''
Call Game_Start
Set_Var Round_Total 3
Set_Var Round_Pass_Score 2
Call Round_Start
Set_Var Round_Correct 1
Call Round_Submit
Call Round_Submit
Print Var Round_Score
Call Game_Pause
Call Round_Next
Print Var Round_Item
Call Game_Resume
Call Round_Next
Set_Var Round_Correct 0
Call Round_Submit
Call Round_Next
Set_Var Round_Correct 1
Call Round_Submit
Call Round_Next
Call Round_Next
Print Var Round_State
Print Var Round_Outcome
Print Var Round_Answered
Call Round_Start
Print Var Round_Score
Print Var Round_Item
''',['1','1','Finished','Win','3','0','1'])
run('''
Call Game_Start
Call Round_Start
Set_Var Round_Correct 0
Call Game_Pause
Call Round_Submit
Print Var Round_Accepted
Call Game_Resume
Call Round_Submit
Call Round_Next
Print Var Round_Outcome
Set_Var Round_Total 0
Call Round_Start
Print Var Round_State
''',['0','Lose','Invalid'])
print('GAME_ROUNDS_TESTS_PASSED')
