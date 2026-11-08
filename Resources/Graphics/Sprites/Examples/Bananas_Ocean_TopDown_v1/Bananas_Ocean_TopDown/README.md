# Bananas — ocean top-down sprite pack

PNG atlases use straight-alpha RGBA8 and a 64 × 64 pixel grid with no external margin or spacing. Use nearest-neighbor filtering and integer scaling. Coordinates start at the upper left. Fully transparent pixels contain zero RGB.

- `ocean_surfaces.png`: 16 opaque repeating materials. Four variants within each main material family share matching repeat edges; each final-row material repeats with itself.
- `ocean_transitions.png`: 48 corner-based material blends in three families. Assign material bits to shared map vertices: NW=1, NE=2, SE=4, SW=8. Sum each tile's four bits to select a mask. A set bit selects the first material in the family name. Neighboring cells must share their common vertex bits. Boundaries are crisp and angled.
- `ocean_props.png`: 16 named static objects. Read JSON rectangles to import multi-cell objects. Transparent atlas space outside those rectangles is unused.

All matching JSON files use `bananas.sprite-atlas.v1`. Named entries contain pixel rectangles, grid footprints and pivots. Material tiles use top-left pivots; props use bottom-center pivots. Footprints describe image size, not collision or buoyancy. Configure water, walkability, interactions and vessel movement in the game. These packs contain static sprites, not animation strips or sailing simulations.

`example_level.png` and `example_map.json` demonstrate a 20 × 14 scene. Prop positions identify the upper-left grid cell of the sprite rectangle. `validation.json` records exact grid, bounds, occupancy, nonoverlap and repeat/connection-edge checks. Not tested inside the Bananas engine.

Artwork created with the built-in image generator, cropped and scaled with nearest-neighbor sampling, then packed. Repeating material edges were normalized. Prompts are in `generation_prompts.json`.

Contains 16 surfaces, 48 transitions (deep/shallow, deep/sand, shallow/sand), and 16 coastal props. The three water/sand families share material-corner rules, but transitions from different families should not be joined arbitrarily. Wave crest and foam sprites are static. The foam surface is an opaque texture; the separate wave crest is a transparent overlay.
