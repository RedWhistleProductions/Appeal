"""Real SDL/ImGui mouse and keyboard activation through reusable Appeal menus."""
from pathlib import Path
import os, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
deps=root/'Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/.deps'
with tempfile.TemporaryDirectory(prefix='Game_UI_',dir=root/'Tests') as temp:
    folder=Path(temp); setup=folder/'Setup.Appeal'; frame=folder/'Frame.Appeal'; exe=folder/'Check.exe'
    setup.write_text('''
Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics
Add_Dynamic_Module Input Appeal_SDL2_Input
Run Libraries/Game/Runtime
Run Libraries/Game/Menus
Run Libraries/Game/Interface
Graphics Add_Window Main "Menu integration test" 640 480
Graphics Set_GUI_Docking False
Graphics Set_GUI_Visible True
Input Set_Focus GUI
Set_Var Chosen ""
Set_Var Actions 0
Define_Function Chosen
    Set_Var Chosen Var Event_Value
    Set_Var Actions (Add Var Actions 1)
End_Function
Define_Function First
    Set_Var UI_Selected_Item 1
    Call UI_Select_Item
End_Function
Define_Function Second
    Set_Var UI_Selected_Item 2
    Call UI_Select_Item
End_Function
Events Bind Choose Chosen
Define_Function Build
    Call Menu_Begin
    Graphics GUI_Begin "Menu Test"
    Set_Var UI_Item_ID First
    Set_Var UI_Item_Label First
    Set_Var UI_Item_Callback First
    Set_Var Menu_Item_Event Choose
    Set_Var Menu_Item_Value first
    Call UI_Menu_Item
    Set_Var UI_Item_ID Second
    Set_Var UI_Item_Label Second
    Set_Var UI_Item_Callback Second
    Set_Var Menu_Item_Value second
    Call UI_Menu_Item
    Graphics GUI_End
    Call Menu_End
End_Function
Call Game_Start
Set_Var Menu_Next Title
Call Menu_Show
Return
''',encoding='utf-8')
    frame.write_text('Input Update Default\nCall Game_Update\nCall Game_Dispatch_Events\nCall Build\nGraphics Render\nGraphics Update\nReturn\n',encoding='utf-8')
    objects=[str(p) for p in (root/'Obj/Windows').glob('*.o') if p.name!='main.o']
    subprocess.run(['g++','-std=c++17','-pthread','-ISource',
        '-I'+str(deps/'windows-x64/include/SDL2'),'-I'+str(deps/'imgui'),
        'Tests/Game_Interface_Render_Tests.cpp',*objects,'-L'+str(deps/'windows-x64/lib'),'-lSDL2','-o',str(exe)],cwd=root,check=True)
    p=subprocess.run([str(exe),str(setup),str(frame)],cwd=root,
        env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH','')),
        capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    print(p.stdout.strip())
