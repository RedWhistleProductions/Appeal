"""Exercise both independently callable setup scripts in isolated SQLite files."""
from pathlib import Path
from contextlib import closing
import os
import sqlite3
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
script = root / 'Resources/Appeal/Test' / ('Storage_Setup_' + uuid.uuid4().hex + '.Appeal')

def run(body):
    script.write_text('Add_Dynamic_Module Storage Appeal_SQLite_Storage\n' + body + '\nDone\n')
    result = subprocess.run([str(root / 'Appeal.exe'), 'Test/' + script.stem], cwd=root,
                            env=dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root / 'Resources')),
                            capture_output=True, text=True, timeout=10)
    assert result.returncode == 0 and not result.stderr, result.stdout + result.stderr
    return result.stdout

try:
    with tempfile.TemporaryDirectory(prefix='Storage_Setup_', dir=root / 'Tests') as temp:
        path = Path(temp)
        prefix = 'Storage Set_Paths "' + path.as_posix() + '"\n'
        # Each script works on its own, without executing the other one first.
        run(prefix + 'Run Setup/Test_Game')
        assert not (path / 'Storage/User_Setings.appealdb').exists()
        run(prefix + 'Run Setup/User_Setings')
        user = path / 'Storage/User_Setings.appealdb'
        game = path / 'Storage/Test_Game.appealdb'
        with closing(sqlite3.connect(user)) as db, db:
            assert db.execute('SELECT COUNT(*) FROM Input_Bindings').fetchone()[0] == 14
            assert db.execute('SELECT Deadzone FROM Controllers').fetchone()[0] == 24
            db.execute("UPDATE Controllers SET Deadzone=33 WHERE ID='Default'")
        with closing(sqlite3.connect(game)) as db, db:
            assert db.execute('SELECT COUNT(*) FROM Scenes').fetchone()[0] == 3
            assert db.execute('SELECT COUNT(*) FROM Characters').fetchone()[0] == 12
            assert db.execute('SELECT COUNT(*) FROM Character_Sprite_Sheets').fetchone()[0] == 28
            assert not db.execute('PRAGMA foreign_key_check').fetchall()
            for file, metadata in db.execute('SELECT File, Metadata_File FROM Character_Sprite_Sheets'):
                assert (root / 'Resources/Graphics/Sprites' / file).is_file(), file
                if metadata:
                    assert (root / 'Resources/Graphics/Sprites' / metadata).is_file(), metadata
            db.execute("UPDATE Characters SET Default_Sprite_Sheet='Eve_Actions_Top_Down' WHERE Name='Eve'")

            db.execute("INSERT INTO Save_Slots(ID, Name, Scene) VALUES(1, 'Check', 'Camp')")
            db.execute("INSERT INTO Entity_State VALUES(1, 'Adam', 'Character', 1, 2100, 338, 0, 'Default')")
        # Repeat setup twice in one interpreter session; saved data must survive.
        run(prefix + 'Run Setup/User_Setings\nRun Setup/Test_Game\nRun Setup/User_Setings\nRun Setup/Test_Game')
        with closing(sqlite3.connect(user)) as db, db:
            assert db.execute('SELECT Deadzone FROM Controllers').fetchone()[0] == 33
            assert db.execute('SELECT COUNT(*) FROM Input_Bindings').fetchone()[0] == 14
            assert not db.execute("SELECT 1 FROM sqlite_master WHERE name='Save_Slots'").fetchone()
        with closing(sqlite3.connect(game)) as db, db:
            assert db.execute('SELECT Position_X FROM Entity_State').fetchone()[0] == 2100
            assert db.execute("SELECT Default_Sprite_Sheet FROM Characters WHERE Name='Eve'").fetchone()[0] == 'Eve_Actions_Top_Down'
            assert db.execute('SELECT COUNT(*) FROM Character_Sprite_Sheets').fetchone()[0] == 28

            assert db.execute('SELECT Scene FROM Save_Slots').fetchone()[0] == 'Camp'
            assert not db.execute("SELECT 1 FROM sqlite_master WHERE name='Controllers'").fetchone()
        # Failed SQL inside a transaction rolls back the entire batch.
        failure = 'Storage Use Test_Game\nStorage Execute_SQL "BEGIN; INSERT INTO Game_Settings VALUES (\'Rollback_Check\', \'1\'); INSERT INTO missing_table VALUES (1); COMMIT;"'
        try:
            run(prefix + failure)
            raise AssertionError('Invalid SQL unexpectedly succeeded')
        except AssertionError as error:
            assert 'missing_table' in str(error), str(error)
        with closing(sqlite3.connect(game)) as db, db:
            assert not db.execute("SELECT 1 FROM Game_Settings WHERE Key='Rollback_Check'").fetchone()
    print('STORAGE_SETUP_PASSED')
finally:
    script.unlink(missing_ok=True)
