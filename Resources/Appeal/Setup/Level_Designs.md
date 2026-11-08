# Level design databases

`Test_Game.appealdb` now stores level designs separately from player save slots.
Every level gets exactly two tables, plus one entry in the shared `Levels` catalog:

- **Tile-map table:** one row per named `Tile_Map_Section`.
- **Entity table:** one row per level entity, keyed by entity type and stable ID.

The Sprite Garden level contains its three screens: nine tile-map sections in
one level. Additional levels get their own pair of tables in the same game file.
`Setup/Test_Game` creates the empty Sprite_Garden table pair without overwriting
an existing design. It upgrades Test_Game's schema marker to version 4 and leaves
existing settings, controller data, scene metadata, and save slots intact.

## Saving and loading

After loading modules and building/editing a World scene:

```text
Run Setup/Test_Game
Storage Save_Level Sprite_Garden (World Export_Level)
```

Restore a complete saved World scene:

```text
Storage Use Test_Game
World Import_Level (Storage Load_Level Sprite_Garden)
```

For another level, choose another name; its tables are created on first save:

```text
Storage Save_Level Level_2 (World Export_Level)
```

To create an empty table pair without saving a scene:

```text
Storage Create_Level Level_2
```

The current scene's name is saved independently of the database level name.
A load stages the whole scene before replacing a live scene of that name.
Loading a level that has never been saved produces an error rather than clearing
World. Saving replaces that level's previous map/entity rows atomically. A failed
save restores the previous rows; an outer Storage transaction can still roll back
a successful save. Repeated database setup never resets design records.

Ready-made scripts (caller loads Setup/Config first):

- `Examples/World/Save_Sprite_Garden_Level`: save the currently active World scene
  under the Sprite_Garden level name.
- `Examples/World/Load_Sprite_Garden_Level`: restore the scene and the demo's screen
  selection variables, without creating a graphics window or owning a loop.
- `Examples/World/Create_Sprite_Garden_Level`: standalone initial save of the
  bundled script-built scene; run explicitly to replace the stored design.

The game demo still builds its scene from scripts at startup; these commands are
explicit design save/load operations. Loading World data does not rebuild Graphics
or reset Mechanics. The caller must bind the loaded sections and synchronize game
mechanics when changing designs in a running game.

## Data layout

The `Levels` catalog contains the friendly Name, Tile_Map_Table and Entity_Table
names, saved World_Name, Active_Section, Paused, Time_Scale, Entity_Lists (including
next-ID counters for each type), Format_Version and Updated_At. Table names use a
hex encoding of the UTF-8 level name with `_Tile_Maps` and `_Entities` suffixes.
This supports spaces/apostrophes and distinguishes names differing only by case
without SQL injection or SQLite's case-insensitive table-name collisions. Level
names may contain 1..128 UTF-8 bytes. Find the actual table names with:

```text
Print (Storage Get_Field Levels Name Sprite_Garden Tile_Map_Table)
Print (Storage Get_Field Levels Name Sprite_Garden Entity_Table)
```

Each **tile-map row** stores Name, Columns, Rows, Orientation, Origin_X/Y/Z,
Tile_Size_X/Y/Z, Neighbor_Left/Right/Above/Below/Front/Back, and Cells. Cells is
row-major text: `0` for an empty cell, or `1 "Entity_Type" ID` for an occupied
cell, one record per cell. Multi-cell scenery occupies one cell at its origin;
its graphic footprint remains independent of the grid.

Each **entity row** stores Entity_Type, Entity_ID, Name, Immutable, and Data.
Entity_ID is decimal TEXT so all supported unsigned 64-bit IDs survive SQLite
without numeric precision loss. Data is the existing versioned World entity
payload, including transform, tag, active state, components, properties, and door
destination/entry position. Custom entity factories must be registered before
loading; subtype-specific persistent fields must use the existing Properties or
Components mechanism.

Repeated immutable rocks, trees and ground tiles reference one entity row.
Mutable entities cannot occupy multiple cells. Free-positioned entities such as
Adam and Eve are saved even if no tile references them. No memory addresses or
copies of graphics are written to the database. On load, entity registries are
created first, then cell pointers are resolved, then section and door links.
Names for currently unresolved neighbors/doors remain pending, matching World's
existing behavior. Runtime display caches and input state are not design data.

## Validation

`python Tests/Level_Database_Tests.py` checks full scene round trips, separate
levels, case-sensitive level identity, shared prototypes, doors, maximum entity
IDs, repeated setup, overwrite rollback and rejection of corrupt references.
World's native tile tests check derived entity factories, all six neighbor links,
free placements, properties, next-ID counters and failed snapshot import safety.
