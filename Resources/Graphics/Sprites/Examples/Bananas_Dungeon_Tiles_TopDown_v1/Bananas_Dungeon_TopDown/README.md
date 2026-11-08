# Bananas — top-down dungeon pack

Three PNG RGBA8 atlases, 64 × 64 pixel cells, no spacing or outer margin. Use nearest-neighbor filtering and integer scaling. Transparent pixels have zero RGB. Pixel coordinates start at the upper left.

- `dungeon_surfaces.png`: 16 opaque floor/material tiles. The four variants within each floor family share matching repeat edges. Other materials repeat with themselves.
- `dungeon_walls.png`: 32 transparent wall overlays: stone and sandstone, each with all 16 adjacency combinations. N=1, E=2, S=4, W=8. Sum bits for neighboring wall cells and select that number. The central wall and connection ports are 32 pixels wide. Paint over floor tiles. Collision should follow wall alpha rather than block the entire cell if using narrow passages.
- `dungeon_props.png`: 16 static objects including closed/open chests and doors, up/down stairs, barrel, crate, columns, rubble, spikes, braziers, table, and lever. Door frames and the intact column use 64 × 128 rectangles. All other objects use 64 × 64. These are state variants, not animation strips.

Matching JSON files provide named rectangles, footprints and pivots using `bananas.sprite-atlas.v1`. Props sort by their bottom-center pivot; floor and wall tiles use the top-left corner. Collision and interaction behavior must be implemented by the game.

`example_map.json` and `example_dungeon.png` demonstrate a 16 × 12 layout with walls selected from adjacency. Prop coordinates refer to the top-left grid cell of their sprite rectangle. `validation.json` records packing and repeat-edge checks. Not tested inside the Bananas engine.

Artwork generated with the built-in image generator, then cropped, scaled with nearest-neighbor sampling, packed, and given shared repeat edges. `generation_prompts.json` records the prompts; `build_pack.py` is the reproducible packing script (takes texture and prop source image paths).
