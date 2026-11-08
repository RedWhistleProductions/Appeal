"""Core-profile pixels, GUI, shader errors, contexts, and host API checks."""
from pathlib import Path
import os,subprocess,tempfile,shutil,uuid
root=Path(__file__).resolve().parents[1]
plugin='Appeal_SDL2_Modern_OpenGL_Graphics'
legacy='Appeal_SDL2_Legacy_OpenGL_Graphics'
base=root/'Resources/Modules/Appeal_Module_Graphics'/plugin
env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH',''))
with tempfile.TemporaryDirectory(prefix='Modern_GL_') as temp:
 folder=Path(temp)
 for name in ['Graphics_Animation_Render_Tests.cpp','Graphics_GUI_Image_Tests.cpp']:
  source=folder/name;exe=folder/(source.stem+'.exe')
  source.write_text((root/'Tests'/name).read_text().replace(legacy,plugin).replace('glReadBuffer(GL_BACK)','glReadBuffer(GL_FRONT)'))
  subprocess.run(['g++','-std=c++17',str(source),'-lopengl32','-o',str(exe)],check=True,cwd=root)
  result=subprocess.run([str(exe)],cwd=root,env=env,capture_output=True,text=True,timeout=25)
  assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
  print(result.stdout.strip())

 # Run the existing fullscreen/dock/paint input checks against core-profile rendering.
 source=folder/'Workspace.cpp';exe=folder/'Workspace.exe';workspace=folder/'Workspace';workspace.mkdir()
 source.write_text((root/'Tests/Graphics_Editor_Workspace_Tests.cpp').read_text().replace(legacy,plugin).replace('glReadBuffer(GL_BACK)','glReadBuffer(GL_FRONT)'))
 deps=base/'.deps/windows-x64'
 subprocess.run(['g++','-std=c++17','-I'+str(deps/'include/SDL2'),str(source),'-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],check=True,cwd=root)
 result=subprocess.run([str(exe),str(workspace)],cwd=root,env=env,capture_output=True,text=True,timeout=30)
 assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
 print(result.stdout.strip())
 resources=folder/'Resources';shaders=resources/'Graphics/Shaders';meshes=resources/'Graphics/Meshes'
 shaders.mkdir(parents=True);meshes.mkdir(parents=True)
 for path in (root/'Resources/Graphics/Shaders/Modern_OpenGL').iterdir():shutil.copy2(path,shaders/path.name)
 reference=Path(r'C:/Users/Billy/Documents/Bin/C++/Archive/SDL2_GL_Test/Resources/Shaders')
 # Reference fixtures are also kept in Tests for portability to other checkouts.
 if not reference.exists():reference=root/'Tests/Fixtures/SDL2_GL_Test_Shaders'
 shutil.copytree(reference,shaders/'Reference')
 vertex=(shaders/'Default.vs').read_text()
 config=(shaders/'Default.shader').read_text()
 (shaders/'Tint.fs').write_text('#version 330 core\nout vec4 LFragment;\nvoid main(){LFragment=vec4(1,0,1,1);}')
 (shaders/'Tint.shader').write_text(config.replace('Default.fs','Tint.fs'))
 (shaders/'Invalid.fs').write_text('#version 330 core\nthis is invalid GLSL')
 (shaders/'Invalid.shader').write_text(config.replace('Default.fs','Invalid.fs'))
 (shaders/'Broken_Link.fs').write_text('#version 330 core\nin vec3 v_Color;\nout vec4 LFragment;\nvoid main(){LFragment=vec4(v_Color,1);}')
 (shaders/'Broken_Link.shader').write_text(config.replace('Default.fs','Broken_Link.fs'))
 (meshes/'Triangle.obj').write_text('v -1 -1 0\nv 1 -1 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvt 0.5 1\nvn 0 0 -1\nf 1/1/1 2/2/1 3/3/1\n')
 exe=folder/'Render.exe';artifacts=root/'Tests/Artifacts';artifacts.mkdir(exist_ok=True)
 subprocess.run(['g++','-std=c++17','-I'+str(deps/'include'),str(root/'Tests/Modern_OpenGL_Render_Tests.cpp'),'-L'+str(deps/'lib'),'-lSDL2','-lopengl32','-o',str(exe)],check=True,cwd=root)
 result=subprocess.run([str(exe),str(resources),str(artifacts/'Modern_OpenGL.ppm')],cwd=root,env=env,capture_output=True,text=True,timeout=30)
 assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
 print(result.stdout.strip())

 # Keep the script/native Graphics API identical across the two plugin builds.
 import ctypes,re
 source=(base/'src/main.cpp').read_text()
 exports=re.findall(r'extern "C"\s+[^\n(]*?\b([A-Za-z_]\w*)\s*\(',source)
 for backend in [legacy,plugin]:
  library=ctypes.WinDLL(str(root/'Resources/Modules/Appeal_Module_Graphics'/backend/(backend+'.dll')),winmode=0)
  for name in exports:assert getattr(library,name),f'{backend}: missing {name}'
 print(f'GRAPHICS_API_PARITY_PASSED ({len(exports)} exports)')
 # Exercise the new example and the existing game/physics/scripts through the real host.
 for name in ['Examples/Graphics/Modern_OpenGL','Examples/World/Sprite_Garden']:
  body=(root/'Resources/Appeal'/Path(name+'.Appeal')).read_text()
  config=(root/'Resources/Appeal/Setup/Config.Appeal').read_text().replace('\nReturn\n','\n').replace(legacy,plugin)
  body=body.replace('Run Setup/Config',config)
  marker='Loop While Var Modern_Open == 1' if name.endswith('Modern_OpenGL') else 'Loop While Var Scene_Open == 1'
  body=body.replace(marker,'Set_Var Check_Frame 0\nLoop While Var Check_Frame < 5\n')
  insert='    Graphics Update' if name.endswith('Modern_OpenGL') else '    Input Update Default'
  body=body.replace(insert,'    Set_Var Check_Frame (Add Var Check_Frame 1)\n'+insert,1)
  script=root/'Resources/Appeal/Test'/('Modern_Host_'+uuid.uuid4().hex+'.Appeal')
  try:
   script.write_text(body)
   result=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,env=dict(env,APPEAL_RESOURCE_FOLDER=str(root/'Resources')),capture_output=True,text=True,timeout=30)
   assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
  finally:script.unlink(missing_ok=True)
 print('MODERN_OPENGL_APPEAL_HOST_PASSED')
