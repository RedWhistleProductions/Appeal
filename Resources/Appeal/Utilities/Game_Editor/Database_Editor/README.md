# Database Editor

Run independently:

```text
Appeal.exe "Utilities/Game_Editor/Database_Editor/Main"
```

Or launch Game Editor and choose **Database Editor** in Components. It is
available even before a game is opened. Both entry points use the dockable
Components, Editing View, and Editor Controls windows.

## Browsing

The database selector lists `.appealdb`, `.sqlite`, `.sqlite3`, and `.db` files
in `Resources/Storage`. Enter another filesystem folder and click **Scan database
folder** to browse elsewhere. The scan lists files directly in that folder.
Select a database, table, and record. Records load in pages of 200; Previous and
Next navigate pages. Refresh rescans databases and reloads the current table and
its schema. Empty selectors show placeholders.

Editing View shows the record's fields and the selected table's SQL schema.
Use the field selector, value type selector, and value box to edit a field.
Selecting another field stages the current field automatically; **Stage field
value** stages it explicitly. Staging changes the draft, not the database.

## Record CRUD

- **New record draft** clears the draft; omitted fields use database defaults.
- **Create record** inserts a new record from the staged fields, including the
  current field if edited. When used on a loaded record, only edited fields are
  inserted; it does not clone unedited fields.
- **Read record** reloads the selected record.
- **Update record** writes only edited fields of the selected record.
- **Delete record** removes the selected record after **Confirm delete** is
  checked. The checkbox resets when a record/table is loaded or saved.

Text, integer, real, NULL, and hexadecimal blob values are supported. Integer
and real input uses JSON number syntax. NULL ignores the value box. Generated
fields are displayed but cannot be edited. Database defaults, keys, and
constraints are enforced; errors appear in the status message and preserve the
draft. Save or explicitly allow discarding field edits before changing record,
table, database, page, or refreshing.

Ordinary tables use SQLite row identity; WITHOUT ROWID tables use their primary
key, including composite keys. Update/Delete require exactly one matching row.
Tables with no usable identity cannot be edited as existing records. Views,
virtual tables, their shadow tables, and SQLite internal tables are excluded.

## Database and table controls

Enter a name and click **Create database** to create and open a database in the
scanned folder. Names accept letters, numbers, spaces, underscores and hyphens;
the editor adds `.appealdb`. Existing databases are selected from the list.

Enter a table name and SQL column definitions, then choose **Create table**.
The default example is `ID INTEGER PRIMARY KEY, Name TEXT`. **Rename selected
table** uses the table-name box. **Delete selected table** drops the table and
its records after **Confirm delete** is checked.

These operations edit the actual selected database. Edits to game-editor tables
can affect games that reference them. Return to another editor and reload its
data to see database changes; opening Database Editor preserves live drafts.

## Embedding

Load Graphics, Files and Storage, then run `Editor.Appeal`. It creates the named
`Database_Editor_Workspace` in-memory connection. Set `DE_Return_DB` to the host's
Storage connection and select that connection after loading. Each callback
temporarily selects the workspace and restores `DE_Return_DB` before returning.
The chosen database is attached to the workspace, so browsing does not initialize
application tables or replace the host's connection. Foreign keys are enabled.

The caller owns the main loop. Call `DE_View` inside Editing View, and `DE_GUI`
to compose Editor Controls. Close `Database_Editor_Workspace` during shutdown.
The two supporting Storage commands, `Open_Memory` and `Try_SQL`, are documented
in the Storage module reference.
