# Tile Editor

Run from the Appeal directory:

```text
Appeal.exe "Utilities/Game_Editor/Level_Editor/Tile_Editor/Main"
```

The utility is entirely Appeal script and uses the installed Graphics, Input,
Storage and World plugins. It creates its own `Resources/Storage/Tile_Editor.appealdb`;
it does not change Test_Game or the main startup script.

Choose the **right mouse brush / new map fill** and map dimensions (1–64 columns and rows), then
press **New Map**. The right brush starts as `gray_100`. Every cell references that
sprite. Changing the right brush also changes the fill for the next new map.

The editor starts in a borderless fullscreen window using the existing ImGui
Docking build. Three panels form the workspace: **Components**, **Editing View**,
and **Editor Controls**. Drag panel tabs to rearrange or float them and drag
splitters to resize them. Layout changes are saved in
`Resources/Graphics/GUI/Editor_Layout.ini`. **Reset layout** restores the initial
three-panel arrangement. **Exit editor** closes the utility.

The Components toolbar currently selects Tile Map Editor; other editors will join
it as they are built. Its controls and viewport remain separate, reusable parts.

Choose the **left mouse brush** and **right mouse brush** independently, then
click or drag inside **Editing View** with the corresponding button to paint.
Middle-click or middle-drag erases by replacing cells with `Transparent`, revealing
lower layers. These actions affect only the selected drawing layer. The second
selector doubles as the fill sprite for new maps.

Each selector has a 96x96 preview to its right, over a transparency checkerboard.
Dropdowns stay within the selector width so they do not cover those previews.
Scroll the wheel over the view to cycle the left brush (up: previous, down: next),
wrapping at either end. F1 switches focus between the controls and view; middle-click
is reserved for erasing.

The map automatically fits the viewport with its aspect ratio preserved. Painting
coordinates follow panel resizing and docking; the controls and toolbar cannot
paint the map. The scene uses a separate 1100x800 render target, so fullscreen
resolution and panel size do not change saved map coordinates.

## Layers

New maps start with one layer, **Layer 0**, filled with the chosen default sprite.
Use **Drawing layer** to choose where painting and transparency are applied.
**Add layer** appends a transparent layer at the top and selects it.
**Remove selected layer** deletes that layer and its tiles, then renumbers the
remaining layers from 0. The last remaining layer cannot be removed.

All layers remain visible: layer 0 draws first, layer 1 above it, then layer 2,
and so on. Transparent cells reveal lower layers. Save/Load retains every layer;
older single-layer designs load as layer 0. Removing a layer affects the working
map immediately; use Save when you want to replace the stored design.

Enter a map name and press **Save**. Saving again replaces that named design.
Enter an existing name and press **Load** to restore it. New/Load require saving
first or checking **Discard unsaved changes** when the current map has edits.
Closing the window does not automatically save.

## Tile catalog and filters

**Tile filters...** opens a separate window for the currently selected layer.
Check the source folders to include, choose **Opaque tiles** and/or **Tiles with
transparency**, and optionally search names, source sheets, categories, or tags.
Both brushes and mouse-wheel cycling use the resulting list. An empty result
keeps both selectors and their retained brush previews visible, with a No matching
tiles placeholder, and disables painting until a filter is enabled;
middle-button erase remains available. Filters never delete or hide painted tiles.

Choices are independent per layer and are saved with the map. New layers and
older maps start with all folders and tile types enabled. For example, turn off
**Tiles with transparency** on layer 0, then turn off **Opaque tiles** on an upper
layer. Alpha is measured from the selected sprite rectangle, including both
halves of an oak sprite, rather than inferred from its folder or filename.

The editor scans `Resources/Graphics/Sprites` recursively on startup. **Scan for
new tiles** and **Refresh sprite database list** repeat the scan. The initial 285
seed definitions are kept, and new catalog entries are added without resetting
existing definitions or custom categories/tags.

The database has three main catalog tables:

- `Tile_Files`: discovered image/metadata files, source folders, metadata JSON,
  and whether the source was present at the last scan.
- `Sprites`: stable tile names, image paths, frame dimensions, and frame numbers.
- `Tile_Attributes`: source folder/sheet, measured opacity, editable Category/Tags,
  and reserved Animated/Animation_Group fields.

Named `bananas.sprite-atlas.v1` JSON files register their sprite rectangles.
Aligned regular frames and the existing two-part horizontal rectangles are
supported. Unsupported rectangles remain in the file inventory. Character
animation sheets with their own metadata are inventoried for later tools.
An image without metadata is registered as one whole-image tile; add atlas JSON
to describe its individual tiles. Invalid JSON and missing images are skipped.
Removed files are marked absent; saved tile definitions are retained.

Existing file/frame definitions and custom attributes are preserved on scans.
Source folder/sheet and opacity are refreshed from the source. For example:

```text
Storage Use Tile_Editor
Storage Set_Field Tile_Attributes Name Terrain/Props/tree_oak Category Scenery
Storage Set_Field Tile_Attributes Name Terrain/Props/tree_oak Tags "forest tree"
```

Animated tiles are planned for later: their frames should appear as one selectable
tile. The animation fields reserve that information; grouping and playback are
not implemented yet.

Saved designs use one World section per layer, shared immutable tile prototypes,
and the existing level format. Editor_Level_Layers stores layer order;
Editor_Level_Folders, Editor_Level_Palettes, and Editor_Level_Search preserve each
layer's folder, opacity, and attribute-search filters. Keep source images and the
catalog with the database; level snapshots refer to stable tile names.

Validation: `python Tests/Tile_Editor_Tests.py` exercises the actual editor script
against temporary databases, including painting, default fill, transparency,
independent named maps, fresh-process load, continued editing, catalog refresh,
and preservation of catalog changes. `python Tests/Tile_Catalog_Tests.py` checks
recursive discovery, new atlas/image registration, malformed JSON, actual opacity,
custom attributes, rescans, and independent saved layer filters.

The parent [Level Editor](../README.md) embeds these controls and can open the
tile map selected in its thumbnail grid without creating another window.
