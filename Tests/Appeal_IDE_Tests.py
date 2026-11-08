# Text editor IO, Unicode/large buffers, draft guards and real GUI integration.
from pathlib import Path
import subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Appeal_IDE'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body,host=False):
    setup='Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Input Appeal_SDL2_Input\nAdd_Dynamic_Module Files Appeal_Standard_Files\n'
    if host:setup+='Add_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'
    setup+=f'Files Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'
    if host:setup+=f'Storage Set_Paths "{folder.as_posix()}"\nRun Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog\nRun Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor\nRun Utilities/Game_Editor/Level_Editor/Editor\nRun Utilities/Game_Editor/Editor\n'
    script.write_text(setup+f'Run {base}/Editor\n'+body+'\nGraphics Close_All\nDone\n',encoding='utf-8')
    result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,encoding='utf-8',timeout=30)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    return result.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Appeal_IDE_',dir=root/'Tests') as temp:
    folder=Path(temp);source=folder/'Source.Appeal';target=folder/'Saved.Appeal';other=folder/'Existing.Appeal';large=folder/'Large.Appeal'
    original=('// caf'+chr(233)+' '+chr(26085)+chr(26412)+chr(35486)+'\r\nDefine_Function Hello\r\n\tPrint "Hi"\r\nEnd_Function\r\n').encode('utf-8')
    source.write_bytes(original);other.write_text('keep me');large_text=('// large '+chr(945)+chr(946)+'\n')*12000;large.write_text(large_text,encoding='utf-8')
    out=run(folder,f'''
Set_Var IDE_Path "{source.as_posix()}"
Call IDE_Open
Set_Var IDE_Path "{target.as_posix()}"
Call IDE_Save_As
Print Var IDE_Status
Set_Var IDE_Text Changed
Call IDE_Changed
Set_Var IDE_Path "{other.as_posix()}"
Call IDE_Open
Print Var IDE_Text
Call IDE_Save_As
Print Var IDE_Dirty
Set_Var IDE_Overwrite 1
Call IDE_Save_As
Print Var IDE_Status
Call IDE_New
Set_Var IDE_Path "{(folder/'missing.Appeal').as_posix()}"
Call IDE_Open
Print Var IDE_Status
''')
    assert out==['File saved.','Changed','1','File saved.','Could not open that file. The current document is unchanged.'],out
    assert target.read_bytes()==original
    assert other.read_text()=='Changed'
    out=run(folder,f'''
Set_Var IDE_Path "{source.as_posix()}"
Call IDE_Open
Set_Var IDE_Text Draft
Call IDE_Changed
Set_Var IDE_Path "{folder.as_posix()}"
Call IDE_Save_As
Print Var IDE_Dirty
Call IDE_New
Print Var IDE_Text
Set_Var IDE_Discard 1
Call IDE_New
Print Var IDE_Dirty
Set_Var IDE_Path "{folder.name}/Sub/New.Appeal"
Set_Var IDE_Text New_Content
Call IDE_Changed
Call IDE_Save
Print Var IDE_Status
''')
    assert out==['1','Draft','0','File saved.'],out
    assert (folder/'Sub/New.Appeal').read_text()=='New_Content'
    out=run(folder,f'''
Set_Var IDE_Text Keep_Draft
Call IDE_Changed
Set_Var IDE_Path "{other.as_posix()}/invalid/Child.Appeal"
Call IDE_Save_As
Print Var IDE_Dirty
Print Var IDE_Text
Print Var IDE_Status
''')
    assert out==['1','Keep_Draft','Could not save the file. Your document and unsaved-change flag are retained.'],out
    empty=folder/'Empty.Appeal';empty.write_bytes(b'')
    out=run(folder,f'''
Set_Var IDE_Path "{empty.as_posix()}"
Call IDE_Open
Call IDE_Save
Print Var IDE_Status
Print Var IDE_Dirty
''')
    assert out==['File saved.','0'],out
    assert empty.read_bytes()==b'' 
    out=run(folder,f'''
Call TE_New
Set_Var TE_Dirty 0
Call LE_New
Set_Var LE_Dirty 0
Set_Var TE_Running 1
Graphics Add_Window Main "Appeal IDE integration" 1100 800
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Docking True
Graphics Set_GUI_Visible True
Set_Var IDE_Path "{source.as_posix()}"
Call IDE_Open
Call GE_Show_IDE
Call LE_Workspace
Graphics Render
Graphics Update
Set_Var IDE_Font_Family 1
Set_Var IDE_Font_Size 20
Call IDE_Font_Apply
Call LE_Workspace
Graphics Render
Graphics Update
Print Var IDE_Dirty
Call IDE_Font_Reset
Call LE_Workspace
Graphics Render
Graphics Update
Print Var IDE_Dirty
Files Read_All "{large.as_posix()}"
Set_Var_Global Large_Text Files.File.Last_Read_All
Graphics GUI_Set_Text_Value IDE_Source Var Large_Text
Call IDE_Changed
Call GE_Show
Call LE_Workspace
Graphics Render
Graphics Update
Call GE_Show_IDE
Call LE_Workspace
Graphics Render
Graphics Update
Print Var IDE_Dirty
Call IDE_Fonts_Open
Call LE_Workspace
Graphics Render
Graphics Update
Set_Var IDE_Font_Family 1
Set_Var IDE_Font_Size 22
Set_Var IDE_Font_Bold 1
Set_Var IDE_Font_Italic 1
Set_Var IDE_Font_R 120
Set_Var IDE_Font_G 220
Set_Var IDE_Font_B 160
Call IDE_Font_Apply
Print Var IDE_Font_Applied
Print Var IDE_Font_File
Call LE_Workspace
Graphics Render
Graphics Update
Print Var IDE_Dirty
Set_Var IDE_Font_Family 5
Graphics GUI_Set_Text_Value IDE_Font_Custom_Control missing-font.ttf
Call IDE_Font_Apply
Print Var IDE_Font_Applied
Call IDE_Font_Reset
Call LE_Workspace
Graphics Render
Graphics Update
Print Var IDE_Font_Applied
Call IDE_Fonts_Close
Graphics GUI_Set_Text_Value IDE_Path_Control "{(folder/'GUI_Saved.Appeal').as_posix()}"
Call IDE_Save_As
Print Var IDE_Status
Set_Var IDE_Browse_Root "{folder.as_posix()}"
Call IDE_Browse
Call LE_Workspace
Graphics Render
Graphics Update
Graphics GUI_File_Picker_Select IDE_File_Picker "{source.as_posix()}"
Call IDE_Picked
Print Var IDE_File
Graphics GUI_Set_Text_Value IDE_Source Unsaved
Call IDE_Changed
Call GE_Request_Exit
Print Var GE_Mode
Print Var TE_Running
Print Var IDE_Exit_Pending
Call LE_Workspace
Graphics Render
Graphics Update
Call IDE_Exit_Cancel
Print Var TE_Running
Print Var IDE_Exit_Pending
Call GE_Request_Exit
Call IDE_Exit_Discard
Print Var TE_Running
Set_Var TE_Running 1
Set_Var IDE_Dirty 0
Call GE_Request_Exit
Print Var TE_Running
Set_Var TE_Running 1
Set_Var IDE_Discard 1
Call IDE_New
Graphics GUI_Set_Text_Value IDE_Source Exit_Save_Text
Call IDE_Changed
Graphics GUI_Set_Text_Value IDE_Path_Control "{folder.as_posix()}"
Call GE_Request_Exit
Call IDE_Exit_Save
Print Var TE_Running
Print Var IDE_Exit_Pending
Graphics GUI_Set_Text_Value IDE_Path_Control "{(folder/'Exit_Saved.Appeal').as_posix()}"
Call IDE_Exit_Save
Print Var TE_Running
Print Var IDE_Exit_Pending
''',True)
    assert out==['0','0','1','1','C:/Windows/Fonts/consolaz.ttf','1','0','1','File saved.',source.as_posix(),'IDE','1','1','1','0','0','0','1','1','0','0'],out
    assert (folder/'Exit_Saved.Appeal').read_text()=='Exit_Save_Text'
    assert (folder/'GUI_Saved.Appeal').read_text(encoding='utf-8')==large_text
    for path,counter,needle in [('Appeal_IDE','IDE','Run Utilities/Game_Editor/Appeal_IDE/Editor'),('', 'TE','Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"')]:
        main=(root/'Resources/Appeal/Utilities/Game_Editor'/path/'Main.Appeal').read_text()
        main=main.replace(needle,f'Files Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'+(f'Storage Set_Paths "{folder.as_posix()}"\n' if not path else '')+needle)
        loop=f'Loop While Var {counter}_Running == 1' if path else 'Loop While Var TE_Open == 1'
        main=main.replace(loop,'Set_Var IDE_Tick 0\nLoop While Var IDE_Tick < 2').replace('    Graphics Update','    Graphics Update\n    Set_Var IDE_Tick (Add Var IDE_Tick 1)')
        script.write_text(main,encoding='utf-8')
        result=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
        assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    print('APPEAL_IDE_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
