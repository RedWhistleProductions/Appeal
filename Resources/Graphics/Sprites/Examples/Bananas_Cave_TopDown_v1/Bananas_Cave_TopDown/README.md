# Bananas — cave top-down level pack

All PNG atlases are straight-alpha RGBA8. Grid: 64 × 64 pixels, zero margin, zero spacing. Use nearest-neighbor filtering and integer scaling. Fully transparent pixels have zero RGB. Coordinates start at the upper left.

## Sheets

- `cave_surfaces.png`: 16 opaque repeating material tiles. Four variants per main family share matching repeat edges; the final four materials each repeat with themselves.
- `cave_transitions.png`: 48 four-corner Wang tiles across three material pairings. NW=1, NE=2, SE=4, SW=8. Assign material bits to shared map vertices; select the sum for each cell. A set bit uses the first material in the family name. Adjacent tiles must agree on their shared vertex bits. These transitions have crisp, angled boundaries.
- `cave_props.png`: 16 static objects, packed in 192-pixel slots. Import the named rectangles in JSON rather than treating every occupied grid cell as a separate object. Larger sprites span multiple grid cells. Transparent space between rectangles is unused.

Matching JSON files use `bananas.sprite-atlas.v1`. Each sprite includes a rectangle, grid footprint and pivot. Floor tiles use top-left pivots. Props use bottom-center pivots for sorting. Footprints describe image size, not collision geometry. Configure collisions, entrances and interactions in your game.

`example_level.png` and `example_map.json` show a 20 × 14 level. Prop positions identify the upper-left grid cell of their rectangle. The example is illustrative, not a complete game. `validation.json` records grid, bounds, occupancy, nonoverlap and compatible-edge checks. Not run inside the Bananas engine.

Art was created with the built-in image generator, then cropped, scaled with nearest-neighbor sampling and packed. Repeating edges were normalized. Prompts are in `generation_prompts.json`. These packs contain static sprites, not animation strips.

`cave_walls.png` adds 32 transparent wall overlays: gray and brown, each with 16 adjacency masks. N=1, E=2, S=4, W=8. Sum the bits of neighboring wall cells. Connections are 40 pixels wide. Place above floors and configure collision using the wall shape.
