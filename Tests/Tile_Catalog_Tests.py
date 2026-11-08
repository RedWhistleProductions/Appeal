"""Discover new tiles and preserve attributes and independent layer filters."""
from pathlib import Path
from contextlib import closing
import json, shutil, sqlite3, subprocess, tempfile, uuid
root=Path(__file__).resolve().parent.parent
script=root/'Resources/Appeal/Utilities/Game_Editor/Level_Editor/Tile_Editor'/('Catalog_Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Graphics Set_Paths "{folder.as_posix()}"\nFiles Set_Paths "{folder.as_posix()}"\nStorage Set_Paths "{folder.as_posix()}"\n'+
        'Run "Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog"\nRun "Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor"\n'+body+'\nDone\n')
    p=subprocess.run([str(root/'Appeal.exe'),'Utilities/Game_Editor/Level_Editor/Tile_Editor/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    return p.stdout.strip().splitlines()
try:
 with tempfile.TemporaryDirectory(prefix='Tile_Catalog_',dir=root/'Tests') as temp:
    folder=Path(temp);ground=folder/'Graphics/Sprites/Ground';props=folder/"Graphics/Sprites/Props' Set";ground.mkdir(parents=True);props.mkdir(parents=True)
    shutil.copyfile(root/'Resources/Graphics/Sprites/Examples/Place_Holder_Tiles.png',ground/'Colors.png')
    shutil.copyfile(root/'Resources/Graphics/Sprites/Examples/Transparent.png',props/'Loose.png')
    atlas={'schema':'bananas.sprite-atlas.v1','image':'Colors.png','size':{'w':704,'h':1024},'grid':{'cell_width':64},'sprites':{
        name:{'rect':{'x':x,'y':0,'w':64,'h':64}} for name,x in [('base',0),('ghost',320),('blank',640)]}}
    (ground/'Colors.json').write_text(json.dumps(atlas),encoding='utf-8-sig');(ground/'Broken.json').write_text('{bad')
    assert run(folder,"""
Call TE_New
Print Var TE_Palette_Count
Storage Set_Field Editor_Layer_Palettes Section Map Transparent 0
Call TE_Update_Palette
Print Var TE_Palette_Count
Print (Storage Get_Field Editor_Brush_List Number 0 Name)
Storage Execute_SQL "UPDATE Editor_Layer_Folders SET Enabled='0' WHERE Section='Map';"
Call TE_Update_Palette
Print Var TE_Palette_Count
Print (World Get_Tile Map 0 0 Name)
Storage Execute_SQL "UPDATE Editor_Layer_Folders SET Enabled='1' WHERE Section='Map';"
Call TE_Update_Palette
Call TE_Add_Layer
Storage Set_Field Editor_Layer_Palettes Section 1 Opaque 0
Call TE_Update_Palette
Print Var TE_Palette_Count
Storage Set_Field Tile_Attributes Name Ground/Colors/ghost Tags water
Storage Set_Field Tile_Attributes Name Ground/Colors/ghost Category Magic
Storage Set_Field Editor_Layer_Search Section 1 Search water
Call TE_Update_Palette
Print Var TE_Palette_Count
Print (Storage Get_Field Editor_Brush_List Number 0 Name)
Set_Var TE_Name Filters
Call TE_Save_Map
""")==['4','1','Ground/Colors/base','0','Ground/Colors/base','3','1','Ground/Colors/ghost']
    shutil.copyfile(root/'Resources/Graphics/Sprites/Examples/Transparent.png',props/'New.png')
    assert run(folder,"""
Set_Var TE_Name Filters
Call TE_Load_Map
Print Var TE_Palette_Count
Set_Var Callback_Value 1
Call TE_Layer_Changed
Print Var TE_Palette_Count
Print (Storage Get_Field Tile_Attributes Name Ground/Colors/ghost Category)
Set_Var Callback_Value 0
Call TE_Layer_Changed
Call TE_Remove_Layer
Print Var TE_Palette_Count
Call TE_Add_Layer
Print Var TE_Palette_Count
Call TE_Discover_Tiles
Print Var TE_Palette_Count
""")==['1','1','Magic','1','5','5']
    with closing(sqlite3.connect(folder/'Storage/Tile_Editor.appealdb')) as db:
        assert db.execute('SELECT count(*) FROM Tile_Files WHERE Present=1').fetchone()[0]==5
        assert db.execute("SELECT count(*) FROM Sprites WHERE File='Ground/Colors.png'").fetchone()[0]==3
        assert db.execute("SELECT Opaque FROM Tile_Attributes WHERE Name='Ground/Colors/base'").fetchone()[0]==1
        assert db.execute("SELECT Opaque,Tags,Animated FROM Tile_Attributes WHERE Name='Ground/Colors/ghost'").fetchone()==(0,'water',0)
    print('TILE_CATALOG_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
