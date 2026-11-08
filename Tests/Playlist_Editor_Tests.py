"""Exercise level playlist persistence and Audio loading in isolated resources."""
from pathlib import Path
from contextlib import closing
import os, sqlite3, subprocess, tempfile, uuid, wave

root = Path(__file__).resolve().parent.parent
base = 'Utilities/Game_Editor/Playlist_Editor'
script = root / 'Resources/Appeal' / base / ('Test_' + uuid.uuid4().hex + '.Appeal')

def run(folder, body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\n'
        + f'Graphics Set_Paths "{folder.as_posix()}"\nStorage Set_Paths "{folder.as_posix()}"\nStorage Use Tile_Editor\n'
        + f'Run {base}/Editor\n' + body + '\nDone\n')
    p = subprocess.run([str(root / 'Appeal.exe'), base + '/' + script.stem], cwd=root,
        env=dict(os.environ, SDL_AUDIODRIVER='dummy'), capture_output=True, text=True, timeout=20)
    assert p.returncode == 0 and not p.stderr, (p.stdout, p.stderr)
    return p.stdout.strip().splitlines()

try:
    with tempfile.TemporaryDirectory(prefix='Playlist_Editor_', dir=root / 'Tests') as temp:
        folder = Path(temp)
        music = folder / 'Audio/Music/Sub Folder'
        music.mkdir(parents=True)
        with wave.open(str(music / 'Song One.wav'), 'wb') as w:
            w.setparams((1, 2, 22050, 0, 'NONE', 'not compressed'))
            w.writeframes(b'\0\0' * 220500)
        output = run(folder, '''
Set_Var PE_Next_Level "Level's One"
Call PE_Select_Level
Graphics GUI_Playlist_Add PE_List "First Song" "Sub Folder/Song One.wav"
Graphics GUI_Playlist_Add PE_List "Encore" "Sub Folder/Song One.wav"
Call PE_Save
Set_Var PE_Next_Level Other
Call PE_Select_Level
Call PE_Clear
Set_Var PE_Next_Level "Level's One"
Call PE_Select_Level
Graphics GUI_Playlist_Item_Count PE_List
Print_Global Graphics.GUI.PE_List.Item_Count
Call PE_Save
''')
        assert output == ['2'], output
        dbfile = folder / 'Storage/Tile_Editor.appealdb'
        with closing(sqlite3.connect(dbfile)) as db:
            rows = dict(db.execute('SELECT Level_Name,File FROM Playlists'))
            assert len(rows) == 2
            filename = rows["Level's One"]
        playlist = folder / 'Audio/Playlist' / filename
        assert playlist.read_text().splitlines() == [
            '\"First Song\" Sub Folder/Song One.wav', '\"Encore\" Sub Folder/Song One.wav']
        assert (folder / 'Audio/Playlist' / rows['Other']).read_text() == ''
        # A new interpreter session restores the list; Audio reads the same file.
        output = run(folder, f'''
Set_Var PE_Next_Level "Level's One"
Call PE_Select_Level
Graphics GUI_Playlist_Item_Count PE_List
Print_Global Graphics.GUI.PE_List.Item_Count
Add_Dynamic_Module Audio Appeal_SDL2_Audio
Audio Set_Paths "{folder.as_posix()}"
Audio Load_Play_List {filename}
Print (Audio Current_Song)
Audio Next
Print (Audio Current_Song)
Audio Stop
''')
        assert output == ['2', 'First Song', 'Encore'], output
        # Exercise the actual editor playback callbacks, including fresh-load replacement.
        output = run(folder, f'''
Set_Var PE_Next_Level "Level's One"
Call PE_Select_Level
Call PE_Init_Audio
Audio Set_Paths "{folder.as_posix()}"
Call PE_Play
Print (Audio Current_Song)
Print (Audio Is_Playing)
Call PE_Next
Print (Audio Current_Song)
Call PE_Back
Print (Audio Current_Song)
Set_Var Callback_Value 27
Call PE_Volume_Changed
Audio Get_Volume_Percent
Print_Global Audio.Volume_Percent
Call PE_Stop
Print (Audio Is_Playing)
Call PE_Play
Print (Audio Current_Song)
Call PE_Close
Print (Audio Is_Playing)
Call PE_Play
Set_Var PE_Next_Level Other
Call PE_Select_Level
Print (Audio Is_Playing)
Call PE_Play
Print (Audio Is_Playing)
Set_Var PE_Next_Level "Level's One"
Call PE_Select_Level
Call PE_Play
Call PE_Next
Print (Audio Current_Song)
Call PE_Stop
''')
        assert output == ['First Song','1','Encore','First Song','27','0','First Song','0','0','0','Encore'], output
        # File errors must not create a database link claiming a successful save.
        blocked = folder / 'Audio/Playlist/Levels' / ('Level_' + b'Blocked'.hex().upper() + '.txt')
        blocked.mkdir()
        output = run(folder, '''
Set_Var PE_Next_Level Blocked
Call PE_Select_Level
Call PE_Save
Print Var PE_Status
''')
        assert 'Could not save' in output[0], output
        with closing(sqlite3.connect(dbfile)) as db:
            assert not db.execute("SELECT 1 FROM Playlists WHERE Level_Name='Blocked'").fetchone()
            assert db.execute('SELECT count(*) FROM Playlists').fetchone()[0] == 2
        assert playlist.read_text().count('Sub Folder/Song One.wav') == 2
    print('PLAYLIST_EDITOR_TESTS_PASSED')
finally:
    script.unlink(missing_ok=True)
