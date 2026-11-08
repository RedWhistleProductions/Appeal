"""Verify control-set GUI persistence and startup loading using isolated databases."""
from contextlib import closing
from pathlib import Path
import os
import sqlite3
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
script = root / 'Resources/Appeal/Test' / ('Controller_DB_' + uuid.uuid4().hex + '.Appeal')

def run(folder, body):
    prefix = f'''Run Setup/Config
Storage Set_Paths "{folder.as_posix()}"
Run Examples/Graphics/Setup_Character_Mechanics
Run Examples/Graphics/Character_Animation_Controls
Graphics Add_Window Main "Controller persistence check" 1024 704
Graphics Set_GUI_Docking False
Graphics Set_GUI_Visible True
Run Examples/Graphics/Setup_Characters
Run Examples/Graphics/Setup_Character_Input
Run GUIs/CONFIG_Controller
'''
    script.write_text(prefix + body + '\nInput Close_Controller -1\nGraphics Close_All\nDone\n')
    result = subprocess.run([str(root / 'Appeal.exe'), 'Test/' + script.stem], cwd=root,
                            env=dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root / 'Resources')),
                            capture_output=True, text=True, timeout=15)
    assert result.returncode == 0 and not result.stderr, f'exit={result.returncode}\n{result.stdout}{result.stderr}'
    return result.stdout.strip().splitlines()

try:
    with tempfile.TemporaryDirectory(prefix='Controller_DB_', dir=root / 'Tests') as temp:
        folder = Path(temp)
        output = run(folder, '''Print Var CONFIG_Controller_Name
Print Var CONFIG_Value_A
Print Var CONFIG_Value_B
Print Var CONFIG_Value_Minus
Graphics GUI_Set_Text_Value CONFIG_Control_Set_Name "Custom's Controls"
Graphics GUI_Set_Text_Value CONFIG_A Eve_Jump
Graphics GUI_Set_Text_Value CONFIG_A_Up ""
Graphics GUI_Set_Text_Value CONFIG_ZL Adam_Jump
Call CONFIG_Controller_Save
Print (Storage Get_Field Control_Sets Name "Custom's Controls" A)
Print (Storage Get_Field Control_Sets Name "Custom's Controls" ZL)
Input Set_Focus GUI
Call CONFIG_Controller_GUI
Graphics Render
Graphics Update
Input Set_Focus Game
Graphics Render
Graphics Update
''')
        assert output == ['Test_Game', 'Adam_Pad_Duck_Down', 'Adam_Jump', 'Input.Toggle_Focus', 'Eve_Jump', 'Adam_Jump'], output
        db_file = folder / 'Storage/User_Setings.appealdb'
        with closing(sqlite3.connect(db_file)) as db:
            columns = {row[1] for row in db.execute('PRAGMA table_info(Control_Sets)')}
            expected = {'A','B','X','Y','L','R','ZL','ZR','Left_Stick','Right_Stick','Minus','Plus','Home','DPad_Up','DPad_Down','DPad_Left','DPad_Right','Touchpad','Paddle1','Paddle2','Paddle3','Paddle4'}
            assert 'Capture' not in columns and 'Capture_Up' not in columns
            assert expected <= columns and {name+'_Up' for name in expected} <= columns
            assert db.execute("SELECT A, A_Up FROM Control_Sets WHERE Name=?", ("Custom's Controls",)).fetchone() == ('Eve_Jump', '')
            assert db.execute("SELECT Version FROM Schema_Info WHERE Name='User_Setings'").fetchone()[0] == 2
        output = run(folder, '''Print Var CONFIG_Controller_Name
Print Var CONFIG_Value_A
Print Var CONFIG_Value_ZL
Graphics GUI_Set_Text_Value CONFIG_Control_Set_Name Missing
Call CONFIG_Controller_Load
Print Var CONFIG_Controller_Name
Print Var CONFIG_Value_A
Graphics GUI_Set_Text_Value CONFIG_Control_Set_Name ""
Call CONFIG_Controller_Save
Print Var CONFIG_Controller_Name
''')
        assert output == ["Custom's Controls", 'Eve_Jump', 'Adam_Jump', "Custom's Controls", 'Eve_Jump', "Custom's Controls"], output
        with closing(sqlite3.connect(db_file)) as db:
            assert db.execute('SELECT COUNT(*) FROM Control_Sets').fetchone()[0] == 2
    print('CONTROLLER_DATABASE_GUI_PASSED')
finally:
    script.unlink(missing_ok=True)
