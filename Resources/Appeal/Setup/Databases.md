# Database setup

Load the modules once with `Run Setup/Config`, then call either setup independently:

```text
Run Setup/User_Setings
Run Setup/Test_Game
```

These create `Resources/Storage/User_Setings.appealdb` and
`Resources/Storage/Test_Game.appealdb` with the standard resource path.
`User_Setings` uses the requested spelling. Each setup opens/selects its own
database and leaves it open. Switch later with `Storage Use User_Setings` or
`Storage Use Test_Game`. Do not run setup inside an existing transaction.

Both scripts use transactions, `CREATE TABLE IF NOT EXISTS`, and
`INSERT OR IGNORE`: repeated calls preserve user edits and saved game data.
`Schema_Info` records User_Setings version 2 and Test_Game version 4; future changes need migrations.
For another game, copy Test_Game.Appeal, change its database name and defaults.

## Shared User_Setings tables

- `User_Settings`: global setting key/value pairs, initially the F1 focus key.
- `Controllers`: controller profiles and their dead zones; default is 24%.
- `Input_Bindings`: Player1/Player2 keyboard bindings and Player1 controller
  bindings, using generic Move/Run/Duck/Jump actions rather than character names.
  Scale is a signed multiplier in -100..100; analog Move uses scale 100.
- `Control_Sets`: named button function sets and release callbacks, excluding Capture.
  See [controller GUI](../GUIs/CONFIG_Controller.md).
- `Schema_Info`: schema version.

## Test_Game tables

- `Game_Settings`: player-to-character mappings and starting scene.
- `Characters`: reusable mutable Character templates, keyed by character folder name.
- `Character_Sprite_Sheets`: each character's sheets, sprite-relative file and metadata paths, frame sizes, grid dimensions, and projection. Seeded by `Setup/Test_Game_Characters` inside the main setup transaction; repeated setup preserves catalog edits. These are available templates, not new placements in an existing level.
- `Levels` and two tables per level: persisted tile-map and entity design records.
  See [level design save/load](Level_Designs.md).
- `Playlists`: one Audio-compatible playlist file per level, keyed by `Level_Name`. Paths are relative to `Audio/Playlist`.
- `Scenes`: Garden, Trail and Camp section names and world camera offsets.
- `Save_Slots`: named saves referencing a scene, initially empty.
- `Entity_State`: per-save entity identity, world position and mode, initially empty.
- `Schema_Info`: schema version.

The Test_Game setup enables foreign keys for its open connection. Enable
`PRAGMA foreign_keys = ON` again after reopening that database in a new session.

## SQL and existing storage commands

`Storage Execute_SQL "SQL text"` executes schema or write statements on the
selected database. It throws on failure and rolls back any active transaction.
For example:

```text
Storage Use User_Setings
Storage Execute_SQL "UPDATE Controllers SET Deadzone = 30 WHERE ID = 'Default';"
```

Execute_SQL does not return SELECT results. Get_Field and Set_Field provide
parameterized access to fields in these tables (see the controller GUI documentation). Existing Set/Get and Insert/Find
commands still address the plugin's original `records` table, which remains
available in both files. They do not address these new tables. These scripts
initialize storage; the controller GUI now loads and saves Control_Sets.
Saving live game state will be connected separately.

Validation: `python Tests/Storage_Setup_Tests.py` checks independent setup,
repeated execution, separate schemas, retained values and transaction rollback.
