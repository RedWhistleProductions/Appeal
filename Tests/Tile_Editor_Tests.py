"""Exercise the actual Appeal editor operations against an isolated database."""
from contextlib import closing
from pathlib import Path
import sqlite3
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
script = root / 'Resources/Appeal/Utilities/Game_Editor/Level_Editor/Tile_Editor' / ('Test_' + uuid.uuid4().hex + '.Appeal')

def run(folder, body):
    script.write_text(
        'Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\n'
        'Add_Dynamic_Module Files Appeal_Standard_Files\n'
        'Add_Dynamic_Module Storage Appeal_SQLite_Storage\n'
        'Add_Dynamic_Module World Appeal_Standard_World\n'
        f'Storage Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\n'
        'Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"\n'
        'Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor"\n' + body + '\nDone\n')
    result = subprocess.run([str(root / 'Appeal.exe'), 'Utilities/Game_Editor/Level_Editor/Tile_Editor/' + script.stem],
                            cwd=root, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result
    assert not result.stderr, result.stderr
    return result.stdout.strip().splitlines()

try:
    with tempfile.TemporaryDirectory(prefix='Tile_Editor_', dir=root / 'Tests') as temp:
        folder = Path(temp)
        assert run(folder, '''
Set_Var TE_Brush Terrain/Props/boulder
Set_Var TE_Original Var TE_Brush
Set_Var TE_Wheel 1
Call TE_Cycle_Brush
Set_Var TE_Wheel -1
Call TE_Cycle_Brush
Print Var TE_Brush
Set_Var TE_Brush (Storage Get_Field Editor_Sprite_List Number 0 Name)
Set_Var TE_Wheel 1
Call TE_Cycle_Brush
Print Var TE_Brush_Index
Set_Var TE_Wheel -286
Call TE_Cycle_Brush
Print Var TE_Brush_Index
''') == ['Terrain/Props/boulder', '284', '0']
        assert run(folder, """
Call TE_New
Call TE_Add_Layer
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Left_Down 1
Set_Var TE_Right_Down 0
Set_Var TE_Middle_Down 0
Set_Var TE_Brush red_100
Call TE_Apply_Brush
Print (World Get_Tile Var TE_Section 0 0 Name)
Set_Var TE_Default blue_100
Set_Var TE_Left_Down 0
Set_Var TE_Right_Down 1
Call TE_Apply_Brush
Print (World Get_Tile Var TE_Section 0 0 Name)
Set_Var TE_Middle_Down 1
Call TE_Apply_Brush
Print (World Get_Tile Var TE_Section 0 0 Name)
Print (World Get_Tile Map 0 0 Name)
""") == ['red_100', 'blue_100', 'Transparent', 'gray_100']
        assert run(folder, '''
Set_Var TE_Name Missing
Call TE_Load_Map
Print Var TE_Status
Set_Var TE_New_Columns 4
Set_Var TE_New_Rows 3
Call TE_New
Print (World Get_Tile Map 0 0 Name)
Print (World Get_Tile Map 3 2 Name)
Set_Var TE_X 1
Set_Var TE_Y 2
Set_Var TE_Paint_Sprite red_100
Call TE_Paint
Set_Var TE_X 2
Call TE_Paint
Print (World Get_Tile Map 1 2 ID)
Print (World Get_Tile Map 2 2 ID)
Set_Var TE_Name "Map's One"
Call TE_Save_Map
Set_Var TE_Paint_Sprite Transparent
Call TE_Paint
Call TE_Request_New
Print (World Get_Tile Map 2 2 Name)
Set_Var TE_Name Second
Call TE_Save_Map
''') == ['No saved map with that name.', 'gray_100', 'gray_100', '2', '2', 'Transparent']
        # Reopen in a fresh interpreter: registry state and data must survive.
        assert run(folder, '''
Set_Var TE_Name "Map's One"
Call TE_Load_Map
Print (World Get_Section Map Columns)
Print (World Get_Section Map Rows)
Print (World Get_Tile Map 2 2 Name)
Set_Var TE_X 3
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite blue_100
Call TE_Paint
Set_Var TE_Paint_Sprite red_100
Call TE_Paint
Call TE_Save_Map
Set_Var TE_Name Second
Call TE_Load_Map
Print (World Get_Tile Map 2 2 Name)
Set_Var TE_Name Missing
Call TE_Load_Map
Print (World Get_Tile Map 2 2 Name)
Set_Var TE_New_Columns 2
Set_Var TE_New_Rows 1
Set_Var TE_Default Transparent
Call TE_New
Print (World Get_Section Map Columns)
Print (World Get_Tile Map 1 0 Name)
''') == ['4', '3', 'red_100', 'Transparent', 'Transparent', '2', 'Transparent']
        assert run(folder, """
Call TE_New
Print Var TE_Layer_Count
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite red_100
Call TE_Paint
Call TE_Add_Layer
Print Var TE_Layer
Print (World Get_Tile Var TE_Section 0 0 Name)
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite blue_100
Call TE_Paint
Call TE_Add_Layer
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite green_100
Call TE_Paint
Set_Var TE_Name Layered
Call TE_Save_Map
Print (World Get_Tile Map 0 0 Name)
Print (World Get_Tile 1 0 0 Name)
Print (World Get_Tile 2 0 0 Name)
""") == ['1', '1', '', 'red_100', 'blue_100', 'green_100']
        assert run(folder, """
Set_Var TE_Name Layered
Call TE_Load_Map
Print Var TE_Layer_Count
Set_Var Callback_Value 1
Call TE_Layer_Changed
Print (World Get_Tile Var TE_Section 0 0 Name)
Call TE_Remove_Layer
Print Var TE_Layer_Count
Print Var TE_Layer
Print (World Get_Tile Var TE_Section 0 0 Name)
Set_Var Callback_Value 0
Call TE_Layer_Changed
Call TE_Remove_Layer
Print Var TE_Layer_Count
Print (World Get_Tile Var TE_Section 0 0 Name)
Call TE_Remove_Layer
Print Var TE_Layer_Count
Set_Var TE_X 0
Set_Var TE_Y 0
Set_Var TE_Paint_Sprite Transparent
Call TE_Paint
Call TE_Save_Map
""") == ['3', 'blue_100', '2', '1', 'green_100', '1', 'green_100', '1']
        assert run(folder, """
Set_Var TE_Name Layered
Call TE_Load_Map
Print Var TE_Layer_Count
Print (World Get_Tile Var TE_Section 0 0 Name)
Call TE_Add_Layer
Print Var TE_Layer
Call TE_New
Print Var TE_Layer_Count
""") == ['1', 'Transparent', '1', '1']
        db_path = folder / 'Storage/Tile_Editor.appealdb'
        with closing(sqlite3.connect(db_path)) as db, db:
            names = [r[0] for r in db.execute('SELECT Name FROM Sprites ORDER BY Name')]
            assert len(names) == 285
            assert db.execute("SELECT Frame_Width,Frame_Height,Frame FROM Sprites WHERE Name='Terrain/Props/tree_oak'").fetchone() == (64,128,1)
            assert db.execute("SELECT Second_Frame FROM Sprite_Extras WHERE Name='Terrain/Props/tree_oak'").fetchone() == (2,)
            for sheet, count in [('Terrain_Props.png',12), ('Terrain_Props_Top_Down.png',16), ('Terrain_Tiles.png',16), ('Terrain_Transitions_Top_Down.png',48)]:
                assert db.execute('SELECT count(*) FROM Sprites WHERE File=?', ('Examples/Terrain/'+sheet,)).fetchone()[0] == count
            assert db.execute('SELECT Items FROM Editor_Catalog').fetchone()[0] == '|'.join(names)
            for file, in db.execute('SELECT DISTINCT File FROM Sprites'):
                assert (root / 'Resources/Graphics/Sprites' / file).is_file(), file
            # Simulate a map saved by the original single-layer editor.
            db.execute("DELETE FROM Editor_Level_Layers WHERE Level_Name=?", ("Map's One",))
            db.execute("UPDATE Sprites SET Frame=5 WHERE Name='red_100'")
            db.execute("INSERT INTO Sprites VALUES ('Custom', 'Examples/Transparent.png',64,64,0)")
        # Repeated setup preserves edits and lists newly registered sprites.
        assert run(folder, '''
Call TE_Refresh
Print (Storage Get_Field Sprites Name red_100 Frame)
Print (Storage Get_Field Editor_Sprite_List Number 0 Name)
Print (Storage Get_Field Editor_Catalog Name All Count)
''') == ['5', 'Custom', '286']
        assert run(folder, """
Set_Var TE_Name "Map's One"
Call TE_Load_Map
Print Var TE_Layer_Count
Print Var TE_Layer
Print (World Get_Tile Var TE_Section 2 2 Name)
""") == ['1', '0', 'red_100']
    print('TILE_EDITOR_TESTS_PASSED')
finally:
    script.unlink(missing_ok=True)
