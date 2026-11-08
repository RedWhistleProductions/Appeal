from pathlib import Path
import os,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps/windows-x64'
with tempfile.TemporaryDirectory(prefix='Controls_',dir=root/'Tests') as temp:
    folder=Path(temp);setup=folder/'Setup.Appeal';frame=folder/'Frame.Appeal';exe=folder/'Check.exe'
    setup.write_text('''
Add_Dynamic_Module Input Appeal_SDL2_Input
Run Libraries/Game/Runtime
Run Libraries/Game/Controls
Input Set_Focus Game
Input Open_Controller -1
Input Bind_Button Fire "0:A"
Input Bind_Axis Move "0:Left_Stick_X"
Set_Var Count 0
Define_Function Fired
    Set_Var Count (Add Var Count 1)
End_Function
Define_Function Pause
    Call Game_Pause
End_Function
Define_Function Resume
    Call Game_Resume
End_Function
Define_Function GUI
    Input Set_Focus GUI
End_Function
Define_Function Queue_UI
    Set_Var Controls_Context UI
    Call Controls_Queue
End_Function
Events Bind Fire Fired
Call Game_Start
Return
''')
    frame.write_text('''Input Update Default
Set_Var Controls_Context Game
Set_Var Controls_Action Move
Call Controls_Read_Axis
Set_Var Controls_Action Fire
Set_Var Controls_Event Fire
Set_Var Controls_Value Player
Call Controls_Queue_Pressed
Call Game_Dispatch_Events
Return
''')
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource','-I'+str(deps/'include/SDL2'),
        'Tests/Game_Controls_Tests.cpp',*objects,'-L'+str(deps/'lib'),'-lSDL2','-o',str(exe)],cwd=root,check=True)
    p=subprocess.run([str(exe),str(setup),str(frame)],cwd=root,
        env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH','')),capture_output=True,text=True,timeout=20)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    print(p.stdout.strip())
