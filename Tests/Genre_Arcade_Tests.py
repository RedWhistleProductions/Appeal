"""Real Appeal playthroughs: every playable room, gaps, stale input and profile persistence."""
from game_script_test import run,ROOT
from pathlib import Path
import tempfile
SETUP='''Add_Dynamic_Module Storage Appeal_SQLite_Storage
Storage Open_Memory Arcade_Test
Run Games/Genre_Arcade/Core
Set_Var A_Profile Arcade_Test
Call Arcade_Init
'''
def act(*commands):
    return ''.join(f'Set_Var A_Command {c}\nCall Arcade_Command\n' for c in commands)
def wait(ms):
    return f'Set_Var Game_Max_Delta 10000\nSet_Var Game_Elapsed {ms}\nCall Game_Advance\nCall Arcade_Tick\n'
def ticks(n,ms=800):return wait(ms)*n
def start(i):return f'Set_Var A_Index {i}\nCall Arcade_Start\n'
CASES=[]
def win(i,body):
    CASES.append((i,body))
    run(SETUP+start(i)+body+'Print Var A_Result\nPrint Var A_Page\nSet_Var Save_Key Var A_Index\nPrint (Call Save_Setting_Get)\n',['Win','Result','Medal'])
    print('ROOM',i,'PASSED')
win(1,act(1,2,3))
win(2,act(1,2,3))
win(3,act(2,2,2))
win(4,act(2,3))
win(5,act(1,2,1))
win(6,act(3,1,2,3))
win(7,act(2)+ticks(6)+act(1,1)+ticks(3)+(act(2,2)+ticks(6)+act(1,1)+ticks(3))*3)
win(8,act(1,3,3))
# Lane sequence needs wrap from five to one with two left moves.
body=act(1,3)+wait(300)+act(2,3)+wait(300)+act(2,3)+wait(300)+act(1,1,3)+wait(300)+act(2,3)+wait(300)+act(2,3)
win(9,body)
win(10,act(1,1,1,3,3,3,1,4))
win(11,act(1,1,3,3,3))
win(12,act(1,1,1,2)+ticks(2,900)+act(2)+ticks(3,900)+act(2)+ticks(3,900))
win(13,act(3,3,3)+ticks(3,700)+act(1)+ticks(4,700))
win(14,act(1,2,3,2,3))
body=''
for _ in range(3):body+=act(1,1,1,3)+wait(300)+act(3)+wait(300)
win(15,body)
win(16,act(2,2,1,3,1))
win(17,act(1,2)+wait(500)+act(2)+wait(500)+act(2)+wait(500)+act(2))
win(18,act(1,1,1,1,4,4))
win(19,act(1,2)+ticks(3,1000)+act(1)+ticks(3,1000)+act(1)+ticks(3,1000))
win(20,act(1,2)+act(3)*6)
win(22,act(1,1,1,3,3,3))
win(27,act(1,1,1,2)+ticks(8,1800))
win(28,act(1,1,1)+ticks(2,900)+act(3)+ticks(1,900)+act(3)+ticks(1,900)+act(3)+ticks(3,900))
win(29,act(3,1,3,1,3,4))
# One continuous session catches shared state leaking between room types.
run(SETUP+''.join(start(i)+body+'Print Var A_Result\n' for i,body in CASES)+
    'Print Var A_Medals\n'+start(29)+act(3,1,3,1,3,4)+'Print Var A_Medals\n',
    ['Win']*24+['24','24'])
# Bad decisions and unattended live games must actually fail.
for i,body in [(1,act(4)),(3,act(1,1,1)),(4,act(4)*6),(5,act(3,3,3)),(8,act(1,1)),(7,ticks(36)),(9,ticks(12)),(11,act(4)*2+act(1,1)+act(1)*8),(14,act(4)*6),(16,act(4)*12),(17,ticks(4,1100)),(19,ticks(12,1000)),(20,act(2,2)+act(3)*4),(22,act(3)*5),(27,ticks(5,1800)),(28,act(1,1,1)+ticks(10,900))]:
    run(SETUP+start(i)+body+'Print Var A_Result\n',['Lose'])
# Pause freezes live timers and rejects commands. Replay restores health/state.
run(SETUP+start(17)+'''Call Arcade_Pause
Set_Var A_Command 1
Call Arcade_Command
'''+ticks(8,1100)+'''Print Var A_HP
Print Var A_Flag
Print Var Game_Time
Call Arcade_Pause
'''+ticks(4,1100)+'''Print Var A_Result
Call Arcade_Start
Print Var A_HP
Print Var A_Result
''',['10','0','0','Lose','10',''])
# Old queued UI input from the last challenge must not affect the new one.
run(SETUP+start(1)+'''Set_Var A_Old Var A_Epoch
Call Arcade_Hub
'''+start(2)+'''Events Emit Arcade_Action1 Var A_Old
Call Game_Dispatch_Events
Print Var A_Flag
Events Emit Arcade_Action1 Var A_Epoch
Events Emit Arcade_Action2 Var A_Epoch
Call Game_Dispatch_Events
Print Var A_Flag
Print Var A_Flag2
''',['0','1','1'])
for i in [21,23,24,25,26,30,31]:
    run(SETUP+start(i)+'Print Var A_Page\nSet_Var A_Command 1\nCall Arcade_Command\nPrint Var A_Page\n',['Gap','Gap'])
# Disk-backed medal and sound preference survive a fresh interpreter process.
with tempfile.TemporaryDirectory(prefix='Arcade_Profile_',dir=ROOT/'Tests') as tmp:
    prefix='Add_Dynamic_Module Storage Appeal_SQLite_Storage\n'+f'Storage Set_Paths "{Path(tmp).as_posix()}"\n'+'''Run Games/Genre_Arcade/Core
Call Arcade_Init
'''
    run(prefix+start(1)+act(1,2,3)+'Call Arcade_Sound\nPrint Var A_Result\n',['Win'])
    run(prefix+start(1)+'Print Var A_Medal\nPrint Var A_Sound\n',['Medal','0'])
print('GENRE_ARCADE_TESTS_PASSED')
