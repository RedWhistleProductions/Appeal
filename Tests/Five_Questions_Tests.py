"""Complete quiz logic/persistence flows through the actual game scripts."""
from pathlib import Path
from contextlib import closing
import os,sqlite3,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='Quiz_Game_',dir=root/'Tests') as temp:
    folder=Path(temp)
    def run(body,expected,db='Quiz'):
        path=folder/'Test.Appeal'
        path.write_text('Add_Dynamic_Module Input Appeal_SDL2_Input\n'
            'Add_Dynamic_Module World Appeal_Standard_World\n'
            'Add_Dynamic_Module Storage Appeal_SQLite_Storage\n'
            'Run Games/Five_Questions/Core\n'
            f'Storage Set_Paths "{folder.as_posix()}"\nSet_Var Save_Database {db}\nCall Quiz_Init\n'
            'Input Set_Focus GUI\n'+body+'\nDone\n',encoding='utf-8')
        p=subprocess.run([str(root/'Appeal.exe'),str(path)],cwd=root,
            env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources')),
            capture_output=True,text=True,timeout=20)
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        assert p.stdout.splitlines()==expected,(p.stdout,expected)
    def answer(event):
        return f'Events Emit {event} Var Round_Item\nCall Game_Dispatch_Events\n'
    nextq='Events Emit Quiz_Next Var Round_Item\nCall Game_Dispatch_Events\n'
    win='Call Quiz_New\n'
    for event in ['Quiz_B','Quiz_A','Quiz_B','Quiz_A','Quiz_C']:
        win+=answer(event)+nextq
    run(win+'''Print Var Round_Score
Print Var Quiz_Page
Print Var Round_Outcome
Print Var Quiz_Best
''',['5','Results','Win','5'])
    # A second process can continue the finished result; settings persist too.
    run('''Print Var Quiz_Has_Save
Call Quiz_Continue
Print Var Quiz_Page
Print Var Round_Outcome
Print Var Round_Score
Print Var Quiz_Best
''',['1','Results','Win','5','5'])
    lose='Call Quiz_New\n'
    for event in ['Quiz_A','Quiz_B','Quiz_C','Quiz_B','Quiz_A']:
        lose+=answer(event)+nextq
    run(lose+'''Print Var Round_Score
Print Var Round_Outcome
Print Var Quiz_Best
Call Quiz_New
Print Var Round_Score
Print Var Round_Item
''',['0','Lose','5','0','1'])
    # Duplicate answers and delayed commands cannot score a new question.
    run('Call Quiz_New\n'+'''
Events Emit Quiz_B 1
Events Emit Quiz_A 1
Call Game_Dispatch_Events
Print Var Round_Score
Events Emit Quiz_Next 1
Events Emit Quiz_Next 1
Events Emit Quiz_A 1
Call Game_Dispatch_Events
Print Var Round_Item
Print Var Round_State
Print Var Round_Score
''',['1','2','Question','1'])
    # Paused gameplay time does not advance; active timeout is accepted once.
    run('''Call Quiz_New
Call Quiz_Pause
Set_Var Game_Max_Delta 40000
Set_Var Game_Elapsed 31000
Call Game_Advance
Call Game_Dispatch_Events
Print Var Round_State
Print Var Game_Time
Call Quiz_Resume
Call Game_Advance
Call Game_Dispatch_Events
Print Var Round_State
Print Var Quiz_Feedback
Print Var Round_Score
''',['Question','0','Feedback','Time is up. The correct answer is:','0'])
    # Save a paused partial game and sound preference, including a timer deadline.
    run('Call Quiz_New\n'+answer('Quiz_B')+nextq+'''
Set_Var Game_Elapsed 500
Call Game_Advance
Call Quiz_Pause
Call Quiz_Title
Call Quiz_Options
Call Quiz_Sound_Toggle
Call Quiz_Title
Call Quiz_Quit
''',[])
    run('''Call Quiz_Continue
Print Var Quiz_Page
Print Var Round_Item
Print Var Round_Score
Print Var Game_Paused
Print Var Game_Running
Print Var Quiz_Sound
Print (Events Timer_Count)
Call Quiz_Resume
Print Var Quiz_Page
''',['Pause','2','1','1','1','0','1','Play'])
    # Missing continue is a harmless title-screen error.
    run('Call Quiz_Continue\nPrint Var Quiz_Page\nPrint Var Save_Status',
        ['Title','Save slot not found.'],db='Empty')
    with closing(sqlite3.connect(folder/'Storage/Quiz.appealdb')) as db:
        assert db.execute("SELECT Value FROM Game_Save_Settings WHERE Key='Best'").fetchone()[0]=='5'
print('FIVE_QUESTIONS_TESTS_PASSED')
