# Bananas Terrain, Effects and UI v1

Pixel-art companion assets for Adam and Eve. Production atlases use PNG RGBA8 with real alpha, exact pixel rectangles, zero packing spacing, and matching JSON. Generic assets are designated CC0-1.0 under the same policy as the character packs. No magic, sorcery or occult content.

| Atlas | Dimensions | Grid | Contents |
| --- | --- | --- | --- |
| terrain_tiles.png | 256 x 256 | 64 x 64 | 16 ground/material tiles |
| terrain_props.png | 512 x 256 | 64 x 64 | 12 scenery objects with multi-cell footprints |
| effects.png | 512 x 384 | 64 x 64 | 48 frames across 6 animations |
| ui_panels.png | 512 x 352 | 32 x 32 | 12 panels/controls and 3 separately extractable fills |
| ui_icons.png | 256 x 64 | 32 x 32 | 16 icons/controls |

## Loading and drawing

Coordinates start at the top left; x grows right, y grows down. JSON `rect` is the exact source rectangle in pixels. Draw at world/screen position minus the listed local pivot. Use nearest-neighbour filtering and integer scale/positions for crisp pixels. Textures are sRGB colour with straight alpha. Conventional OpenGL blend factors: SRC_ALPHA, ONE_MINUS_SRC_ALPHA. Use no mipmaps for the initial atlas renderer; linear filtering can sample neighbouring sprites because these sheets have no extruded gutters.

Transparent space outside prop/effect/icon artwork is preserved. Fully filled ground tiles intentionally have opaque interiors and edges; transparent ground edges would make unwanted gaps. All sheets still use RGBA8.

## Terrain

World cell size is 64 pixels, matching character frame size. Narrow and pine tree rectangles are 64 x 128 (1 x 2 cells). Oak and apple tree rectangles are 128 x 128 (2 x 2 cells). The ladder also spans 1 x 2 cells, fence 2 x 1, and smaller props 1 x 1. Footprints describe packing and logical placement, not exact silhouette dimensions or collision shapes. Each sprite is scaled proportionally into its footprint and anchored at its base. Empty cells in the prop atlas are intentionally unused.

Ground tile groups: grass_0 through grass_3, soil_0 through soil_3, stone_0 through stone_3; sand, wood_planks, water_deep and water_shallow. Adjacent tiles within each group share identical horizontal edge pixels. Fill materials also repeat vertically; grass repeats horizontally only and its bottom edge matches the soil group's top. This is a base repeatable tileset, not a full corner/slope/autotile or cross-material transition set. Different materials require a level boundary or a future transition tile; edge checks do not claim matching unrelated material families.

The water tiles are static material textures. Effects include an animated water splash. Collision fields are suggested categories only; props are marked none so the loader does not accidentally make an entire canopy solid. Define actual collision geometry in your scene/game.

## Effects

Rows: fire, explosion, smoke, dust, sparks, water_splash. Eight frames per row. Each JSON frame specifies duration_ms and pivot. Fire loops. Other effects are one-shot and should be removed on completion instead of freezing their final frame. Fire and splash use the lower anchor; free bursts use the centre. For impact placement, align the pivot with the event position. Effects are separate overlays and are not baked into character sprites. They use regular alpha blending, not mandatory additive blending.

Expansion and contraction were retained by applying a fixed size mapping to each source cell. Initial animation timings may be adjusted to fit gameplay. Fire, explosions, smoke, dust, metal-impact sparks and water splashes are physical/natural effects.

## UI

Buttons provide normal, hover, pressed and disabled states, blank for runtime text. Panels provide parchment dialogue, wood-trimmed dialogue, inventory and tooltip surfaces. `nine_slice` specifies left/right/top/bottom border sizes in pixels. Split at those margins, keep corners unchanged, and stretch or tile the centre/edge pieces. `min_size` prevents negative centre dimensions. Do not uniformly scale a panel when you want the border thickness to stay fixed. `content_inset` supplies a starting text padding; add more padding when your text needs it.

The UI packing grid is 32 pixels. Buttons/bars span 128 x 32; dialogue panels span 256 x 128; inventory is 128 x 128; tooltip source is 256 x 128. Icons each occupy 32 x 32.

The three coloured bar images in the main bar row are static previews. For variable values, use bar_track plus bar_fill_health, bar_fill_stamina or bar_fill_experience. Each fill is a 104 x 12 payload inside a grid-aligned 128 x 32 container. Those payload rectangles deliberately do not cover the whole container. Draw a left-anchored crop of width floor(104 * clamp(value,0,1)), at track origin + (12,10); at zero draw no fill. JSON supplies both payload and container coordinates. This allows smooth values without stretching the whole frame. No mana bar or magical resource is included.

Icons: full/half/empty heart, coin, inventory bag, settings, sound on/off, four directional arrows, checkbox off/on and radio off/on. Hover/pressed state selection, text, input handling and focus are runtime responsibilities.

## Validation and previews

Validation passed for RGBA format, atlas dimensions, rectangle bounds, occupied sprites, non-overlapping extraction rectangles, terrain edge equality, nine-slice geometry and effect frame indices. Nine-slice corner pixels were verified unchanged after resizing; the included UI preview checks dynamic bars at 0/25/50/75/100 percent. Assets were inspected in a composed scene alongside Adam and Eve. An animated preview exercises all six effects, including one-shot removal.

preview_scene.png, preview_ui_scaling.png and preview_effects.gif are demonstrations, not atlases. Their backgrounds and captions are baked into previews only. Do not load them as game sheets. The GIF repeats the demonstration; each non-looping effect still disappears at completion within that cycle.

Art was created with the built-in image-generation tool and then mechanically cropped, scaled, packed and normalized to the required grid. Prompt records are in generation_prompts.json. No engine runtime has been tested in this task; loaders must interpret this JSON schema. As with the character sets, generated artwork can have minor phase-to-phase variation. The delivered PNG geometry is exact.
