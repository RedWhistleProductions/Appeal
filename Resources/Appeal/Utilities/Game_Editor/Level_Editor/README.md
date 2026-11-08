# Level Editor

Run from the Appeal directory:

```text
Appeal.exe "Utilities/Game_Editor/Level_Editor/Main"
```

This first Level Editor uses the existing fullscreen docked workspace. The
Components toolbar switches between Level Editor and Tile Map Editor in the same
application window, Editing View, and Editor Controls dock. Panel arrangements
remain configurable and are saved as before.

## Arrange tile maps

A new level starts with a 4-by-3 grid. Click a cell in Editing View or use the
Selected map cell slider. Choose a saved tile map and press **Assign map to cell**.
Its thumbnail shows the complete saved map with all layers composited in order.
Maps fill their cells without padding or grid borders, forming
one continuous preview. A small corner marker identifies the selected cell. Different map aspect ratios are scaled to fit the cells.
The selected-cell slider and map name in Editor Controls stay synchronized with
grid clicks. Switching components scrolls the shared controls dock back to the
top so its selectors remain visible.
**Clear selected cell** removes the placement without deleting the tile map.
The same map can occupy multiple cells; placements refer to the reusable saved map.

Type a new level name, or choose one from **Existing levels**, then use
**Save level** / **Load level**. The list is read automatically from the database;
selecting a saved name fills the name field without replacing unsaved layout data. New
layout dimensions range from 1 to 8 maps in either direction. **New level layout**
uses those dimensions and clears placements; save first or explicitly check
Discard level changes when replacing an unsaved layout.

## Edit a selected map

Select a grid cell, then choose **Tile Map Editor** in Components. An assigned
map opens with its existing layers and tile filters. An empty cell opens a new
map draft named from the level and cell; change its name in Tile Map Editor if
desired. Save the map, then choose **Level Editor** to attach it to that cell and
refresh the thumbnails. Save the level layout separately to retain the placement.

Switching to Level Editor preserves an unsaved tile-map draft. Thumbnails show
saved map data. Select the same cell and return to Tile Map Editor to continue the
draft, including one renamed or loaded while that cell was being edited. To open a different map, save first or explicitly allow discarding tile-map
changes. Saving changes to a shared map updates all its thumbnail placements.
**Refresh saved maps** picks up maps saved by another instance.

The Tile Map Editor also remains independently runnable:

```text
Appeal.exe "Utilities/Game_Editor/Level_Editor/Tile_Editor/Main"
```

## Component structure and data

`Main.Appeal` owns the single application window/loop. `Editor.Appeal` provides
level operations, controls, thumbnail composition, selection and component
switching. It embeds Tile Editor's existing controls and renderer; the tile
renderer accepts a draw origin, cell scale and drawable offset for thumbnail use.
Thumbnail imports restore the original World snapshot, preserving the live draft.

Both tools use the existing Tile_Editor database. `Editor_Game_Levels` stores level
names and grid dimensions; `Editor_Game_Level_Cells` stores references to named
saved tile maps. Existing saved maps and their data format are preserved. Saving also updates
`Editor_Game_Level_Links`: occupied cells link bidirectionally Left/Right and
Above/Below. Empty cells and grid boundaries have no exit. Links identify the
placement cell, so repeated instances of the same map may have different neighbors.
Music, characters, and doors are future Level Editor tools. Playlist and Character
Editors are not implemented here.

## Game runtime

`Runtime.Appeal` exposes map transitions without creating a window or GUI. After
loading the Storage/World modules, opening the Tile_Editor database, and running
that script, set:

```text
Set_Var Runtime_Level_Name My_Level
Set_Var Runtime_Cell 0
Set_Var Runtime_Player_X 32
Set_Var Runtime_Player_Y 32
Call Runtime_Level_Load
```

Update Runtime_Player_X/Y with the player's map-local position and call
`Runtime_Level_Update` each game frame. Crossing an edge loads the linked cell's
saved World snapshot and carries the player coordinates to the opposite edge,
preserving small overflow. Empty neighbors block movement at that boundary.
`Runtime_Cell`, `Runtime_Map_Name`, `Runtime_Section`, `Runtime_Width/Height`, and
`Runtime_Transitioned` expose the result. Coordinates use the editor's 64-unit
World tiles, independent of the thumbnail display scale.

Set `Runtime_On_Map_Loaded` to your Appeal callback function name to recreate
render bindings and restore the player entity from game-owned state after each
map load. The callback runs after entry coordinates are calculated, including
the initial load. This helper imports a whole map snapshot; player inventory,
mechanics, and other persistent gameplay state should be retained by the caller.
`Runtime_Loaded` is 0 if the initial cell has no map. Existing saved layouts get
their derived neighbor links refreshed when loaded by this helper.

Validation:

```text
python Tests/Level_Editor_Tests.py
python Tests/Level_Editor_Render_Tests.py
python Tests/Level_Editor_Links_Tests.py
```

These cover named layout persistence, repeated map references, map selection and
opening, new maps, draft preservation/discard guards, real thumbnail pixels, and
component switching through one docked host. The render check links the current
interpreter objects under Obj/Windows; build Appeal first if those are absent.

## Level playlists

Click **Playlist Editor** in Components to show the selected level's playlist and recursive Music Library. Drag songs into the list to save automatically; changing the selected level opens that level's playlist. Selecting another editor hides both playlist panels, and each panel has a title-bar X. See [Playlist Editor](../Playlist_Editor/README.md) for standalone use, file format, database links, and embedding.

## Dragging tile maps

Drag a placed tile map in Editing View to another grid cell. Empty destinations move the placement; occupied destinations swap the two maps. Releasing outside the view cancels the move. Moving a placement also moves its association with any live Tile Editor draft.

Select a **Saved tile map** in Editor Controls to see its thumbnail preview. Drag the preview onto a level cell to place that reusable saved map; an occupied destination is replaced. This places a reference to the saved map, without changing the map itself. Layout changes remain drafts until **Save level** is clicked.

All editor panels have title-bar X buttons. Use the **Windows** menu to reopen a closed panel or **Restore all windows** to restore them all. Closing a panel preserves its draft data; closing either playlist panel also stops playback.


## Playing a level test

Click **Test level** near the top of Editor Controls. The selected occupied cell
is the starting map; if the selection is empty, the test starts at the first
occupied cell. Editing View shows that map at playable scale with a yellow player
marker. Use **WASD** or **arrow keys** to move. Crossing an edge loads its linked
neighbor; empty cells and outer edges block exit. All saved tile-map layers draw
in their normal order. **F1** switches input focus as usual.

Click **Stop test**, press **F6** while game input has focus, or choose another
component to return to editing. Escape retains the application's existing exit
behavior. The previous input focus is restored when the test ends.

Tests use the **current layout draft** and the **saved versions of its tile maps**.
They do not save or overwrite level layouts, permanent links, or tile-map drafts.
Save a tile map before testing if its latest edits should appear in the test.
Missing map references and empty levels are reported before playback starts.

This is an exploration test for map composition and transitions. Character
placement, doors, collision rules, and gameplay mechanics are not configured by
this version; the marker can move across all tiles. Existing playlist playback
stops when testing begins.

`Test.Appeal` supplies the nonblocking test lifecycle. It snapshots and restores
the live World and shared drawing state. Runtime preview cells/links are temporary
SQLite tables, selected only when `Runtime_Use_Preview` is 1, so testing an unsaved
layout does not change the saved runtime data. The normal Runtime entry point
continues to load persisted layouts with its default `Runtime_Use_Preview = 0`.

Validation: `python Tests/Level_Play_Tests.py` checks empty/missing map handling,
links, blocked edges, draft preservation, and repeated play/stop cycles through
the real docked GUI.
