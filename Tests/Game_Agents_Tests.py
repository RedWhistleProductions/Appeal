from collections import Counter
import time
from game_script_test import run
out=run('''Run Libraries/Game/Runtime
Run Libraries/Game/Agents
Call Game_Start
Set_Var Agents_Count 100
Set_Var Agents_Budget 3
Set_Var Frame 0
Loop While Var Frame < 37
{
    Call Agents_Begin
    Loop While (Call Agents_Next) == 1 { Print Var Agents_Index }
    Set_Var Frame (Add Var Frame 1)
}
''')
counts=Counter(map(int,out));assert len(out)==111 and set(counts)==set(range(100)),counts
assert min(counts.values())==1 and max(counts.values())==2
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Agents
Call Game_Start
Set_Var Agents_Count 5
Set_Var Agents_Budget 10
Call Agents_Begin
Loop While (Call Agents_Next) == 1 { Print Var Agents_Index }
Set_Var Agents_Count 2
Call Agents_Begin
Loop While (Call Agents_Next) == 1 { Print Var Agents_Index }
Call Game_Pause
Call Agents_Begin
Print (Call Agents_Next)
''',['0','1','2','3','4','0','1','0'])
start=time.perf_counter()
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Agents
Call Game_Start
Set_Var Agents_Count 1000
Set_Var Agents_Budget 32
Set_Var Frame 0
Set_Var Count 0
Loop While Var Frame < 100
{
    Call Agents_Begin
    Loop While (Call Agents_Next) == 1 { Set_Var Count (Add Var Count 1) }
    Set_Var Frame (Add Var Frame 1)
}
Print Var Count
''',['3200'])
print(f'GAME_AGENTS_TESTS_PASSED (3200 scheduled slots: {time.perf_counter()-start:.3f}s including startup; no NPC workload)')
