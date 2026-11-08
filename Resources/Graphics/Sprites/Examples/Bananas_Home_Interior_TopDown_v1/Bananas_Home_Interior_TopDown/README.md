# Bananas — home top-down sprite pack

Four PNG RGBA8 atlases on a 64 × 64 pixel grid, zero margin and spacing. Use nearest-neighbor filtering and integer scaling. Coordinates start at the upper left. Transparent pixels contain zero RGB.

- `home_surfaces.png`: 16 opaque material tiles. Four variants per floor family share repeat edges; the last four materials repeat with themselves.
- `home_transitions.png`: 48 floor transitions in three families. These use shared corner bits: NW=1, NE=2, SE=4, SW=8. Assign material bits to map vertices, then sum each tile's four corner bits. A set bit selects the first material in its family name. Match shared vertices between neighboring cells. Boundaries are crisp and angled.
- `home_walls.png`: 32 transparent connecting wall overlays, 16 masks for each of two materials. Primary/secondary refer to the two wall textures. Adjacency: N=1, E=2, S=4, W=8. Sum the bits for neighboring wall cells. Wall connection ports are 40 pixels wide. Paint over floors; configure collision to match the wall shape.
- `home_props.png`: 16 static objects. Props use rectangles from 64 × 64 to 192 × 192, packed in 192-pixel slots. Import the JSON rectangles rather than treating every cell as a separate object. Empty atlas space is unused.

Each sheet has matching `bananas.sprite-atlas.v1` JSON with names, pixel rectangles, grid footprints and pivots. Floors and walls use top-left pivots; props use bottom-center pivots for sorting. Footprints describe image size rather than collision geometry. Configure interaction, collisions and level transitions in your game. Open/closed props are state variants, not animations.

`example_level.png` and `example_map.json` show a 20 × 14 layout. Prop coordinates identify the upper-left cell of the sprite rectangle. `validation.json` records grid, bounds, occupancy, nonoverlap and repeat/connection edge checks. Not tested inside the Bananas engine.

Artwork was generated with the built-in image generator, then cropped, scaled with nearest-neighbor sampling and packed. Repeating material edges were normalized. `generation_prompts.json` records the prompts.

Primary walls use cream plaster; secondary walls use timber. Includes bedroom, living-room and kitchen furniture, rugs and door states.
