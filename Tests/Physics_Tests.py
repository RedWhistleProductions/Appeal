"""Build real collision tests and exercise DLL APIs through Appeal scripts."""
from pathlib import Path
import os,platform,subprocess,tempfile,uuid
root=Path(__file__).resolve().parents[1]
physics=root/'Resources/Modules/Appeal_Module_Physics'
windows=os.name=='nt';system=platform.system().lower()
env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources'))
def run_script(body):
 path=root/'Resources/Appeal/Test'/('Collision_'+uuid.uuid4().hex+'.Appeal')
 try:
  path.write_text(body+'\nDone\n')
  result=subprocess.run([str(root/'Appeal.exe'),'Test/'+path.stem],cwd=root,env=env,capture_output=True,text=True,timeout=30)
  assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
  return result.stdout.strip().splitlines()
 finally:path.unlink(missing_ok=True)
with tempfile.TemporaryDirectory(prefix='Physics_') as temp:
 for dimension,plugin,test in [(2,'Appeal_Box2D_Physics','Physics_Collision_Tests.cpp'),(3,'Appeal_Jolt_Physics','Physics_3D_Collision_Tests.cpp')]:
  dependency=physics/plugin/'.deps'/('box2d' if dimension==2 else 'jolt')
  include=dependency/'source'/('include' if dimension==2 else '')
  library=dependency/('lib' if dimension==2 else 'build')/system/('libbox2d.a' if dimension==2 else 'libJolt.a')
  executable=Path(temp)/('test_'+str(dimension)+('.exe' if windows else ''))
  args=['g++','-std=c++17','-O2','-I'+str(include)]
  if dimension==3:args+=['-DJPH_OBJECT_STREAM','-DNDEBUG']
  subprocess.run(args+[str(root/'Tests'/test),str(library),'-lpthread','-o',str(executable)],check=True,cwd=root)
  subprocess.run([str(executable)],check=True,timeout=30)
  if not windows:continue
  box='Set_Box' if dimension==2 else 'Set_Box_3D'
  depth='' if dimension==2 else ' 200'
  mechanics='Appeal_Default_2D_Side_Scrolling_Mechanics' if dimension==2 else 'Appeal_Default_3D_Mechanics'
  body=f"""Add_Dynamic_Module Physics {plugin}
Add_Dynamic_Module Mechanics {mechanics}
Add_Dynamic_Module World Appeal_Standard_World
World Create Collision_Test
World Create_Typed_Entity Player Character 1 0
World Create_Typed_Entity NPC Character 2 0
Physics Set_Units_Per_Meter 64
Physics Set_Gravity 0 -1800 0
Physics Use Floor Default
Physics Set_Body_Type Floor Static
Physics {box} Floor 2000 64{depth}
Physics Set_Position Floor 0 -32 0
Run Examples/Physics/Update_Entity_Mode
"""
  for entity,x in [('Player',0),('NPC',120)]:
   body+=f"""Mechanics Use {entity} Default
Mechanics Set {entity} External_Physics 1
Physics Use {entity} Default
Physics Set_Capsule {entity} 20 100
Physics Set_Position {entity} {x} 300 0
"""
  body+="""Physics Use Clock Default
Physics Set Clock Gravity_Scale 0
Physics Set_Position Clock 0 2000 0
Physics Set_Velocity Clock 60 0 0
Physics Update All
Print (Physics Get Clock Position_X)
Set_Var Frame 0
Loop While Var Frame < 120
{
 Physics Update All
 Set_Var Frame (Add Var Frame 1)
}
Print (Physics Get Player Grounded)
Print (Physics Get NPC Grounded)
Print (Physics Get Player Position_Y)
"""
  for i,tag in enumerate(['Water','Ladder','Climbable','Vehicle']):
   body+=f"""Physics Use Volume_{i} Default
Physics Set_Body_Type Volume_{i} Static
Physics Set Volume_{i} Sensor 1
Physics Set Volume_{i} Tag {tag}
Physics {box} Volume_{i} 500 200{depth}
Physics Set_Position Volume_{i} 60 50 0
Physics Step 0.016666667
Set_Var Collision_Entity Player
Call Collision_Update_Mode
Set_Var Collision_Entity NPC
Call Collision_Update_Mode
Print (Mechanics Get Player Mode)
Print (Mechanics Get NPC Mode)
"""
  for i in range(4):body+=f'Physics Release Volume_{i}\n'
  body+="""Physics Step 0.016666667
Call Collision_Update_Mode
Print (Mechanics Get NPC Mode)
"""
  output=run_script(body)
  assert abs(float(output.pop(0))-1)<0.01,output
  assert output[:2]==['1','1'] and abs(float(output[2])-50)<2,output
  assert output[3:]==['Swimming','Swimming','Ladder','Ladder','Climbing','Climbing','Vehicle','Vehicle','Ground'],output
 if windows:
  output=run_script((root/'Resources/Appeal/Test/Physics_Integration.Appeal').read_text().removesuffix('Done\n'))
  assert float(output[0])>600 and output[1]=='1' and abs(float(output[2]))<2,output
print('Physics DLL, player/NPC mode, and Sprite Garden integration tests passed')
