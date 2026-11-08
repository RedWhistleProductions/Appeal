"""Brush controls remain present when a layer filter matches no tiles."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
script = root / 'Resources/Appeal/Utilities/Game_Editor/Level_Editor/Tile_Editor' / ('Brush_Visibility_' + uuid.uuid4().hex + '.Appeal')
try:
    with tempfile.TemporaryDirectory(prefix='Brush_Visibility_', dir=root / 'Tests') as temp:
        folder = Path(temp)
        images = folder / 'Graphics/Sprites/Examples'
        images.mkdir(parents=True)
        shutil.copyfile(root / 'Resources/Graphics/Sprites/Examples/Place_Holder_Tiles.png', images / 'Place_Holder_Tiles.png')
        script.write_text(
            'Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\n'
            'Add_Dynamic_Module Files Appeal_Standard_Files\n'
            'Add_Dynamic_Module Storage Appeal_SQLite_Storage\n'
            'Add_Dynamic_Module World Appeal_Standard_World\n'
            f'Graphics Set_Paths "{folder.as_posix()}"\n'
            f'Files Set_Paths "{folder.as_posix()}"\n'
            f'Storage Set_Paths "{folder.as_posix()}"\n' + '''
Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"
Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor"
Graphics Add_Window Main "Brush visibility check" 1280 800
Graphics Set_GUI_Editor_Mode True
Graphics Set_GUI_Docking True
Graphics Set_GUI_Visible True
Call TE_New
Set_Var TE_Brush red_100
Set_Var TE_Default blue_100
Storage Execute_SQL "UPDATE Editor_Layer_Folders SET Enabled='0';"
Call TE_Update_Palette
Print Var TE_Palette_Count
Graphics GUI_Set_Selection_Value TE_Brush_Menu 123
Graphics GUI_Set_Selection_Value TE_Default_Menu 124
Set_Var Check 0
Loop While Var Check < 3
{
    Graphics GUI_Begin_Frame
    Call TE_GUI
    Graphics Render
    Graphics Update
    Set_Var Check (Add Var Check 1)
}
Graphics GUI_Selection_Text TE_Brush_Menu
Set_Var_Global Text Graphics.GUI.TE_Brush_Menu.Text
Print Var Text
Graphics GUI_Selection_Text TE_Default_Menu
Set_Var_Global Text Graphics.GUI.TE_Default_Menu.Text
Print Var Text
Set_Var Callback_Value 0
Call TE_Brush_Changed
Call TE_Default_Changed
Print Var TE_Brush
Print Var TE_Default
Storage Execute_SQL "UPDATE Editor_Layer_Folders SET Enabled='1';"
Call TE_Update_Palette
Graphics GUI_Begin_Frame
Call TE_GUI
Graphics Render
Graphics Update
Graphics GUI_Selection_Text TE_Brush_Menu
Set_Var_Global Text Graphics.GUI.TE_Brush_Menu.Text
Print Var Text
Graphics GUI_Selection_Text TE_Default_Menu
Set_Var_Global Text Graphics.GUI.TE_Default_Menu.Text
Print Var Text
Graphics Close_All
Done
''')
        result = subprocess.run([str(root / 'Appeal.exe'), 'Utilities/Game_Editor/Level_Editor/Tile_Editor/' + script.stem],
                                cwd=root, capture_output=True, text=True, timeout=30)
        assert result.returncode == 0 and not result.stderr, (result.stdout, result.stderr)
        assert result.stdout.strip().splitlines() == [
            '0', 'No matching tiles', 'No matching tiles', 'red_100', 'blue_100', 'red_100', 'blue_100'
        ], result.stdout
        print('BRUSH_VISIBILITY_TESTS_PASSED')
finally:
    script.unlink(missing_ok=True)
