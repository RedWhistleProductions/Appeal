# Character Editor

Choose **Character Editor** in the Game Editor's Components toolbar after
creating or opening a game. It shares Editing View and Editor Controls with
the other components.

Standalone launch:

```text
Appeal.exe "Utilities/Game_Editor/Character_Editor/Main"
```

The standalone editor uses the same game catalog and database. Select an
existing game or use **Create / open game** to start one.

## Names and characters

Use **New character**, enter its **Default name**, then **Save character**.
The character selector lists saved characters in the current game. Selecting
one loads its name and stats. A stable internal ID keeps its data attached
when the default name is changed; characters may share the same display name.

Changes remain in a draft until saved. Switching components retains that draft.
Save it or explicitly check **Discard character changes when switching** before
selecting another character/game or starting a new character.

## Game-defined numeric stats

**Create numeric stat...** opens a window for a stat name and numeric default.
Examples include Health = 100, Strength = 10, and Speed = 2.5. Stat names must
be nonempty and unique within the game. Stat definitions are saved immediately
when **Create stat** is pressed and are available to all characters in that game.
Other games can define different stats or use different defaults for the same
stat name.

Editing View lets you select a stat and enter this character's numeric value.
**Apply stat value to draft** stages it. Selecting another stat or saving the
character also stages the current value. **Save character** persists the name
and values together. New and existing characters use the stat's default until
they have a saved value of their own.

Numbers may be negative or fractional, using JSON number syntax. Empty values,
text, NaN, and infinity are rejected with a status message; the draft remains
available to correct. The list below the value box shows the staged values.

## Embedding and storage

`Editor.Appeal` defines reusable controls and functions without opening a window
or owning a loop. Load Graphics and Storage, select the host's database, then
run it. The Game Editor uses the existing `Tile_Editor.appealdb` database.
`Editor_Games` is shared with Game Editor. New tables are `Editor_Characters`,
`Editor_Character_Stats`, and `Editor_Character_Stat_Values`.

Set `CE_Next_Game` and call `CE_Use_Game` to select a saved game. Compose
`CE_View` inside Editing View and call `CE_GUI` to draw Editor Controls and the
optional stat-creation window. `CE_Dirty` indicates unsaved character changes.
The host retains ownership of its Storage connection and application loop.

Future requirements remain: per-character mechanics sets with gameplay-controlled
enable flags, and either a whole background story or story segments with
gameplay-controlled lock/unlock flags. Those controls are not implemented here.
