# Bananas Game Editor

Run from the Appeal directory:

```text
Appeal.exe "Utilities/Game_Editor/Main"
```

The fullscreen, dockable host starts on Game Editor. Press **Alt+Enter** to
switch between fullscreen and a resizable window. This shortcut also works in
the standalone component editors and restores the previous window size. Enter a new game name and
choose **Create new game**, or choose an entry in **Existing games** and click
**Open game**. Games are saved immediately in the existing Tile_Editor database.

**Level Editor** is the first game component. It uses the same Editing View and
Editor Controls windows, with its existing Tile Map Editor and playlist tools.
Return to **Game Editor** in Components to create or open another game. Save
pending level and tile-map edits before changing games.

Saving a level adds its reference to the current game. The level selector shows
that game's levels; opening a game restores its alphabetically first saved level,
or starts an empty layout. To reuse an existing standalone level, type its name
and load it, then save it to add it to this game. Levels and tile maps remain
shared resources: saving the same level name updates that level for every game
that references it. New games start with a proposed `<Game>_Level_1` name.

`Editor.Appeal` supplies the game catalog and GUI without owning a window or
loop. `Main.Appeal` loads the existing component cores and owns the shared host.
**Database Editor** is available from Components, including before opening a
game. It selects databases, tables and records, provides record CRUD and table
management, and uses the same dockable windows. See
[Database Editor](Database_Editor/README.md) for standalone launch and usage.
**Character Editor** edits each character's default name and numeric stat values.
Use **Create numeric stat...** to define named stats and defaults for the current
game. See [Character Editor](Character_Editor/README.md) for standalone launch,
draft handling, and storage details.
Future game components can join this host. Standalone Level and Tile Map Editor
entry points remain available.

**Splash / Title Screens** configures full-screen images, splash SFX, theme music,
and splash duration per game. Test previews the timed crossfade in Editing View.
See [Splash / Title Screen Editor](Screen_Editor/README.md) for usage and its
standalone entry point.

**Appeal IDE** starts as a text editor for creating, opening and saving scripts.
It is available without an open game and keeps its document across component
switches. See [Appeal IDE](Appeal_IDE/README.md) for usage and standalone launch.

Database additions: `Editor_Games` and `Editor_Game_Level_Membership`. Existing
levels and maps are preserved; no existing content is automatically assigned to
a new game.
