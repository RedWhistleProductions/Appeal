"""Persist real World designs into per-level table pairs and restore them safely."""
from pathlib import Path
from contextlib import closing
import os,sqlite3,subprocess,tempfile,uuid
root=Path(__file__).resolve().parent.parent
script=root/'Resources/Appeal/Test'/('Level_DB_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body,error=None):
    script.write_text('Add_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'
                      +f'Storage Set_Paths "{folder.as_posix()}"\n'+body+'\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,
        env=dict(os.environ,APPEAL_RESOURCE_FOLDER=str(root/'Resources')),
        capture_output=True,text=True,timeout=12)
    assert result.returncode==0,(result.returncode,result.stdout,result.stderr)
    if error:assert error in result.stderr,(result.stdout,result.stderr)
    else:assert not result.stderr,result.stderr
    return result.stdout.strip().splitlines()
def table(level,suffix):return 'Level_'+level.encode().hex().upper()+'_'+suffix
try:
 with tempfile.TemporaryDirectory(prefix='Level_DB_',dir=root/'Tests') as temp:
    folder=Path(temp);file=folder/'Storage/Test_Game.appealdb'
    assert run(folder,'''Run Setup/Test_Game
Run Examples/World/Setup_Sprite_Garden
World Create_Typed_Entity Door Door 1 0
World Set_Door_Destination Door Camp 25 50 0
World Set Door Description "door with spaces"
World Create_Typed_Entity Huge_ID Prototype 18446744073709551614 1
World Set_Tile Garden 0 2 Huge_ID
Run Examples/World/Save_Sprite_Garden_Level
Storage Save_Level "Level's Two" (World Export_Level)
Storage Save_Level garden (World Export_Level)
Storage Save_Level Garden (World Export_Level)
World Close_Scene Sprite_Garden
Run Examples/World/Load_Sprite_Garden_Level
Print (World Get_Tile Garden 0 2 ID)
Print (World Get_Tile Scenery 0 6 ID)
Print (World Get_Tile Scenery 8 6 ID)
Print (World Get_Door_Destination Door)
''') == ['18446744073709551614','1','1','Camp']
    maps=table('Sprite_Garden','Tile_Maps');entities=table('Sprite_Garden','Entities');other_maps=table("Level's Two",'Tile_Maps')
    with closing(sqlite3.connect(file)) as db:
        assert db.execute(f'SELECT COUNT(*) FROM "{maps}"').fetchone()[0]==9
        assert db.execute(f'SELECT COUNT(*) FROM "{entities}" WHERE Name=?',('Pine',)).fetchone()[0]==1
        assert db.execute(f'SELECT Entity_ID,typeof(Entity_ID) FROM "{entities}" WHERE Name=?',('Huge_ID',)).fetchone()==('18446744073709551614','text')
        assert db.execute(f'SELECT Neighbor_Right FROM "{maps}" WHERE Name=?',('Orchard',)).fetchone()[0]=='Trail'
        assert db.execute('SELECT COUNT(DISTINCT Tile_Map_Table) FROM Levels').fetchone()[0]==4
        assert db.execute('SELECT Version FROM Schema_Info WHERE Name=?',('Test_Game',)).fetchone()[0]==4
    output=run(folder,'''Run Setup/Test_Game
Run Setup/Test_Game
Run Examples/World/Load_Sprite_Garden_Level
Print (World Get_Tile Garden 0 2 ID)
World Get Door Description
World Set_Position Adam 12.25 338 0
Storage Save_Level "Level's Two" (World Export_Level)
World Delete_Section Camp_Scenery
Storage Save_Level Sprite_Garden (World Export_Level)
''')
    assert output==['18446744073709551614','door with spaces'],output
    with closing(sqlite3.connect(file)) as db,db:
        assert db.execute(f'SELECT COUNT(*) FROM "{maps}"').fetchone()[0]==8
        assert db.execute(f'SELECT COUNT(*) FROM "{other_maps}"').fetchone()[0]==9
        # Fail halfway through an overwrite: old rows must survive atomically.
        before=db.execute(f'SELECT * FROM "{entities}" ORDER BY Name').fetchall()
        db.execute(f'CREATE TRIGGER reject_save BEFORE INSERT ON "{entities}" BEGIN SELECT RAISE(ABORT, \'forced save failure\'); END;')
    run(folder,'''Run Setup/Test_Game
Run Examples/World/Setup_Sprite_Garden
Storage Save_Level Sprite_Garden (World Export_Level)
''',error='forced save failure')
    with closing(sqlite3.connect(file)) as db,db:
        assert db.execute(f'SELECT * FROM "{entities}" ORDER BY Name').fetchall()==before
        assert db.execute(f'SELECT COUNT(*) FROM "{maps}"').fetchone()[0]==8
        db.execute('DROP TRIGGER reject_save')
        # Corrupt a cell reference; loading must fail, rather than making a dangling pointer.
        db.execute(f'UPDATE "{maps}" SET Cells=? WHERE Name=?',('1 "Missing" 99\n','Garden'))
    run(folder,'''Run Setup/Test_Game
World Import_Level (Storage Load_Level Sprite_Garden)
''',error='Unresolved tile entity')
    with closing(sqlite3.connect(file)) as db,db:
        data=db.execute(f'SELECT Data FROM "{table("Garden","Entities")}" WHERE Name=?',('Door',)).fetchone()[0]
        db.execute(f'UPDATE "{table("Garden","Entities")}" SET Data=? WHERE Name=?',(data.replace('door with spaces','door\x00with spaces'),'Door'))
    assert run(folder,'''Run Setup/Test_Game
World Import_Level (Storage Load_Level Garden)
World Get Door Description
Storage Save_Level Garden (World Export_Level)
''')==['door\x00with spaces']
    assert run(folder,'''Run Setup/Test_Game
World Import_Level (Storage Load_Level "Level\'s Two")
Print (World Get_Neighbor Orchard Right)
''')==['Trail']
    run(folder,'''Run Setup/Test_Game
World Create Blank
Storage Begin
Storage Save_Level Rolled_Back_Level (World Export_Level)
Storage Rollback
Storage Save_Level Empty_Level (World Export_Level)
World Import_Level (Storage Load_Level Empty_Level)
Storage Create_Level Never_Saved
''')
    with closing(sqlite3.connect(file)) as db:
        assert not db.execute("SELECT 1 FROM Levels WHERE Name='Rolled_Back_Level'").fetchone()
        assert db.execute(f'SELECT COUNT(*) FROM "{table("Empty_Level","Entities")}"').fetchone()[0]==0
    run(folder,'''Run Setup/Test_Game
World Import_Level (Storage Load_Level Never_Saved)
''',error='Level has not been saved')
 print('LEVEL_DATABASE_PASSED')
finally:script.unlink(missing_ok=True)
