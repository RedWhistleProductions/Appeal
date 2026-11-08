"""Real Appeal scripts: typed CRUD, composite keys, errors, paging and GUI embedding."""
from pathlib import Path
import sqlite3,subprocess,tempfile,uuid
from contextlib import closing
root=Path(__file__).resolve().parent.parent
base='Utilities/Game_Editor/Database_Editor'
script=root/'Resources/Appeal'/base/('Test_'+uuid.uuid4().hex+'.Appeal')
def run(folder,body):
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\n'+
        f'Files Set_Paths "{folder.as_posix()}"\nStorage Set_Paths "{folder.as_posix()}"\nRun {base}/Editor\nSet_Var DE_Folder "{folder.as_posix()}"\nCall DE_Refresh_Databases\nSet_Var DE_Next_Database "{(folder/"Sample.appealdb").as_posix()}"\nCall DE_Open_Database\n'+body+'\nDone\n')
    p=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    return p.stdout.strip().splitlines()
def table(name):
    return f'Set_Var Callback_Value (Storage Get_Field DE_Tables Name "{name}" Number)\nCall DE_Table_Changed\n'
def field(name,value,kind='text'):
    return f'Set_Var DE_Column (Storage Get_Field DE_Draft Name "{name}" Number)\nCall DE_Select_Field\nSet_Var DE_Value "{value}"\nSet_Var DE_Kind {kind}\nSet_Var DE_Pending 1\nCall DE_Stage\n'
try:
 with tempfile.TemporaryDirectory(prefix='Database_Editor_',dir=root/'Tests') as temp:
    folder=Path(temp)
    db=sqlite3.connect(folder/'Sample.appealdb')
    db.executescript("""CREATE TABLE Items(ID INTEGER PRIMARY KEY, Name TEXT NOT NULL UNIQUE, Amount REAL DEFAULT 1.5, Data BLOB, Note TEXT, Generated TEXT GENERATED ALWAYS AS (Name || '!') STORED);
INSERT INTO Items(Name,Data) VALUES('First',X'00FF');
CREATE TABLE Composite(A TEXT,B INTEGER,Value TEXT,PRIMARY KEY(A,B)) WITHOUT ROWID;
INSERT INTO Composite VALUES('key',1,'before');
CREATE TABLE Pages(ID INTEGER PRIMARY KEY,Value TEXT);
WITH RECURSIVE n(i) AS (SELECT 1 UNION ALL SELECT i+1 FROM n WHERE i<205) INSERT INTO Pages SELECT i,'value' || i FROM n;
CREATE TABLE EmptyFirst(Blank TEXT,Value TEXT); INSERT INTO EmptyFirst VALUES(NULL,'present');
CREATE TABLE Parent(ID INTEGER PRIMARY KEY); INSERT INTO Parent VALUES(1);
CREATE TABLE Child(ID INTEGER PRIMARY KEY,Parent_ID INTEGER REFERENCES Parent(ID)); INSERT INTO Child VALUES(1,1);
""")
    db.commit();db.close()
    out=run(folder,table('Items')+"""
Print Var DE_Table
Print Var DE_Row_Count
Set_Var Callback_Value 0
Call DE_Record_Changed
Print (Storage Get_Field DE_Draft Name Name Value)
Print (Storage Get_Field DE_Draft Name Data Value)
Print (Storage Get_Field DE_Draft Name Data Kind)
"""+field('Name',"O'Brien | quoted")+"""
Call DE_Update
Print Var DE_Status
Print Var DE_Row_Count
""")
    assert out==['Items','1','First','00FF','blob','Record saved. Select a record to read its stored values.','1'],out
    with closing(sqlite3.connect(folder/'Sample.appealdb')) as db:
        assert db.execute('SELECT Name,Amount,Data,Generated FROM Items').fetchone()==("O'Brien | quoted",1.5,b'\x00\xff',"O'Brien | quoted!")
    out=run(folder,table('Items')+'Call DE_New_Record\n'+field('Name','Second')+field('Note','ignored','null')+"""
Call DE_Create
Print Var DE_Row_Count
Set_Var Callback_Value 1
Call DE_Record_Changed
Print (Storage Get_Field DE_Draft Name Amount Value)
Print (Storage Get_Field DE_Draft Name Note Kind)
Call DE_Delete_Record
Print Var DE_Row_Count
Set_Var DE_Confirm_Delete 1
Call DE_Delete_Record
Print Var DE_Row_Count
""")
    assert out==['2','1.5','null','2','1'],out
    out=run(folder,table('Composite')+'Set_Var Callback_Value 0\nCall DE_Record_Changed\n'+field('Value','after')+'Call DE_Update\nPrint Var DE_Status\n')
    assert out==['Record saved. Select a record to read its stored values.'],out
    with closing(sqlite3.connect(folder/'Sample.appealdb')) as db: assert db.execute('SELECT Value FROM Composite').fetchone()==('after',)
    out=run(folder,table('Pages')+"""
Print Var DE_Row_Count
Call DE_Next
Print Var DE_Page
Print Var DE_Row_Count
Set_Var Callback_Value 0
Call DE_Record_Changed
Print (Storage Get_Field DE_Draft Name ID Value)
Call DE_Previous
Print Var DE_Page
""")
    assert out==['200','1','5','201','0'],out
    out=run(folder,table('EmptyFirst')+'Print Var DE_Row_Count\nPrint Var DE_Row_Items\n')
    assert out==['1',' [record 1]'],out
    out=run(folder,table('Items')+'Set_Var Callback_Value 0\nCall DE_Record_Changed\n'+field('Amount','abc','integer')+'Call DE_Update\nPrint Var DE_Status\n')
    assert out==['Invalid value: use a JSON number for integer/real or even-length hexadecimal for blob.'],out
    out=run(folder,table('Parent')+"""
Set_Var Callback_Value 0
Call DE_Record_Changed
Set_Var DE_Confirm_Delete 1
Call DE_Delete_Record
Print Var DE_Status
Print Var DE_Row_Count
""")
    assert out==['FOREIGN KEY constraint failed','1'],out
    out=run(folder,"""
Set_Var DE_New_Table New_Table
Set_Var DE_Definition "ID INTEGER PRIMARY KEY, Name TEXT"
Call DE_Create_Table
Print Var DE_Status
"""+table('New_Table')+field('Name','created')+"""
Call DE_Create
Print Var DE_Row_Count
Set_Var DE_New_Table Renamed_Table
Call DE_Rename_Table
Print Var DE_Status
"""+table('Renamed_Table')+"""
Set_Var DE_Confirm_Delete 1
Call DE_Drop_Table
Print Var DE_Status
Print (Storage Get_Field DE_Tables Name Renamed_Table Name)
Set_Var DE_New_Database Created_DB
Call DE_Create_Database
Print Var DE_Attached
Print Var DE_Table_Count
""")
    assert out==['Table operation completed. Select a table to browse it.','1','Table operation completed. Select a table to browse it.','Table operation completed. Select a table to browse it.','','1','0'],out
    assert (folder/'Created_DB.appealdb').exists()
    out=run(folder,table('Items')+'Set_Var Callback_Value 0\nCall DE_Record_Changed\n'+field('Name','pending')+table('Pages')+"""
Print Var DE_Table
Set_Var DE_Discard 1
"""+table('Pages')+'Print Var DE_Table\n')
    assert out==['Items','Pages'],out
    # Load the real Game Editor cores and render both the database and game panels.
    script.write_text('Add_Dynamic_Module Graphics Appeal_SDL2_Legacy_OpenGL_Graphics\nAdd_Dynamic_Module Files Appeal_Standard_Files\nAdd_Dynamic_Module Storage Appeal_SQLite_Storage\nAdd_Dynamic_Module World Appeal_Standard_World\n'+
        f'Files Set_Paths "{folder.as_posix()}"\nStorage Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\n'+
        'Run Utilities/Game_Editor/Level_Editor/Tile_Editor/Catalog\nRun Utilities/Game_Editor/Level_Editor/Tile_Editor/Editor\nRun Utilities/Game_Editor/Level_Editor/Editor\nRun Utilities/Game_Editor/Editor\n'+
        f'Run {base}/Editor\nSet_Var DE_Return_DB Tile_Editor\nStorage Use Tile_Editor\n'+
        'Graphics Add_Window Main "Database Editor integration" 1100 800\nGraphics Set_GUI_Docking True\nGraphics Set_GUI_Editor_Mode True\nGraphics Set_GUI_Visible True\nCall TE_New\nSet_Var TE_Dirty 0\nCall LE_New\nSet_Var LE_Dirty 0\nSet_Var GE_New_Name DB_Test_Game\nCall GE_Create\nCall GE_Show_Database\n'+
        f'Set_Var DE_Next_Database "{(folder/"Sample.appealdb").as_posix()}"\nCall DE_Open_Database\nCall DE_Begin\nSet_Var Callback_Value (Storage Get_Field DE_Tables Name Items Number)\nCall DE_End\nCall DE_Table_Changed\nSet_Var Callback_Value 0\nCall DE_Record_Changed\nCall LE_Workspace\nGraphics Render\nGraphics Update\n'+
        'Call DE_Begin\nSet_Var DE_Column (Storage Get_Field DE_Draft Name Name Number)\nCall DE_Select_Field\nCall DE_End\nGraphics GUI_Set_Text_Value DE_Value_Control GUI_Updated\nCall DE_Value_Changed\nCall DE_Update\nPrint Var DE_Status\nPrint (Storage Get_Field Editor_Games Name DB_Test_Game Name)\nPrint Var GE_Mode\nCall GE_Show_Level\nCall LE_Workspace\nGraphics Render\nGraphics Update\nPrint Var GE_Mode\nCall GE_Show_Database\nCall LE_Workspace\nGraphics Render\nGraphics Update\nPrint Var GE_Mode\nStorage Close Database_Editor_Workspace\nStorage Close Tile_Editor\nGraphics Close_All\nDone\n')
    gui=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert gui.returncode==0 and not gui.stderr,(gui.stdout,gui.stderr)
    assert gui.stdout.strip().splitlines()==['Record saved. Select a record to read its stored values.','DB_Test_Game','Database','Level','Database'],gui.stdout
    with closing(sqlite3.connect(folder/'Sample.appealdb')) as db:
        assert db.execute('SELECT Name FROM Items').fetchone()==('GUI_Updated',)
    standalone=(root/'Resources/Appeal'/base/'Main.Appeal').read_text()
    standalone=standalone.replace('Run Utilities/Game_Editor/Database_Editor/Editor',f'Files Set_Paths "{folder.as_posix()}"\nGraphics Set_Paths "{folder.as_posix()}"\nRun Utilities/Game_Editor/Database_Editor/Editor')
    standalone=standalone.replace('Loop While Var DE_Running == 1','Set_Var DE_Tick 0\nLoop While Var DE_Tick < 2').replace('    Graphics Update','    Graphics Update\n    Set_Var DE_Tick (Add Var DE_Tick 1)')
    script.write_text(standalone)
    gui=subprocess.run([str(root/'Appeal.exe'),base+'/'+script.stem],cwd=root,capture_output=True,text=True,timeout=30)
    assert gui.returncode==0 and not gui.stderr,(gui.stdout,gui.stderr)
    print('DATABASE_EDITOR_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
