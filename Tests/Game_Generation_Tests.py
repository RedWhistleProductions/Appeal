import json,time
from collections import deque
from game_script_test import run
setup='''Add_Dynamic_Module Storage Appeal_SQLite_Storage
Storage Open_Memory Generator
Run Libraries/Game/Generation
Set_Var Generation_Database Generator
Call Generation_Init
Set_Var Generation_Width 32
Set_Var Generation_Height 24
Set_Var Generation_Seed 123
Call Generation_Create
'''
start=time.perf_counter()
out=run(setup+'''
Print (Call Generation_Export)
Call Generation_Create
Print (Call Generation_Export)
Set_Var Generation_Seed 124
Call Generation_Create
Print (Call Generation_Export)
Set_Var Generation_Width 0
Call Generation_Create
Print Var Generation_OK
Print (Call Generation_Export)
Set_Var Generation_Width 64
Set_Var Generation_Height 64
Call Generation_Create
Print Var Generation_OK
''')
a,b,c=json.loads(out[0]),json.loads(out[1]),json.loads(out[2])
assert a==b and a['cells']!=c['cells']
assert out[3]=='0' and json.loads(out[4])==c and out[5]=='1',out[3:]
w,h=a['width'],a['height'];cells=a['cells']
assert len(cells)==w*h and cells[w+1]==1 and cells[(h-2)*w+w-2]==1
assert all(cells[x]==cells[(h-1)*w+x]==0 for x in range(w))
assert all(cells[y*w]==cells[y*w+w-1]==0 for y in range(h))
seen={(1,1)};q=deque(seen)
while q:
    x,y=q.popleft()
    for nx,ny in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]:
        if 0<=nx<w and 0<=ny<h and cells[ny*w+nx] and (nx,ny) not in seen:
            seen.add((nx,ny));q.append((nx,ny))
assert len(seen)==sum(cells)
generation_seconds=time.perf_counter()-start
build_start=time.perf_counter()
run(setup+'''
Add_Dynamic_Module World Appeal_Standard_World
World Create Generated
World Create_Typed_Entity Floor Tile 1 1
World Create_Typed_Entity Wall Tile 2 1
Set_Var Generation_Floor Floor
Set_Var Generation_Wall Wall
Call Generation_Build_World
Print (World Get_Tile Generated 1 1 Name)
Print (World Get_Tile Generated 0 0 Name)
World Close_Scene Generated
''',['Floor','Wall'])
print(f'GAME_GENERATION_TESTS_PASSED (seed/max-size batch {generation_seconds:.3f}s; 32x24 generation+World build {time.perf_counter()-build_start:.3f}s; includes processes)')
