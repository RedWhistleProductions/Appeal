# Bananas Animated Atmosphere v1

One production PNG atlas with separate animations for clouds, fog and smoke, in the existing pixel-art style. Format: true transparent RGBA8 PNG, 64 x 64 frames, eight columns, zero spacing and zero outer margin. Matching `bananas.sprite-sheet.v1` JSON supplies frame rectangles, millisecond timing, pivots and animation names.

The atlas is 512 x 1152: 18 rows x 8 columns =144 frames. It contains six eight-frame animations at three opacity multipliers. Rows0-5 use25%, rows6-11 use50%, rows12-17 use75%. Within each block, rows are cloud_soft, cloud_wispy, fog_thin, fog_dense, smoke_rising and smoke_puff. Names use effect_opacity, for example fog_thin_50 or smoke_rising_75.

These percentages multiply the artwork's intrinsic alpha. A75% variant is capped at75% alpha; its thin wisps remain much more transparent. RGB brightness is not substituted for transparency. Alpha also varies across each silhouette and subtly during looping cycles; smoke_puff fades to a completely transparent final frame. Pixel-stepped soft edges preserve the pixel-art style. Fully transparent pixels have zeroed RGB.

Cloud/fog and smoke_rising animations loop. smoke_puff plays once and should be removed on completion; its last frame is deliberately empty. Cloud/fog pivot is(32,32); smoke pivot is(32,58). Draw each source frame at effect_position minus its local pivot. Source-cell alignment is retained across all frames so expansion/contraction is not cancelled by individually resizing each puff.

Use straight-alpha blending with SRC_ALPHA,ONE_MINUS_SRC_ALPHA for conventional OpenGL. Do not use additive blending for ordinary fog or dark smoke. The shader must preserve the sampled texture alpha. Multiplying by a runtime opacity is supported; do not overwrite every pixel's alpha with one constant. Use nearest-neighbour sampling and preferably integer scaling. Mipmaps/linear sampling can bleed across atlas cells because these sheets have no extruded filtering gutters.

For wide fog, draw several overlapping instances, or stretch a fog sprite horizontally. Use different frame offsets and slow world movement to avoid synchronized repetition. The sheet supplies billowing shapes; long-distance drift/parallax belongs to the engine. Multiple semitransparent layers accumulate opacity: two50% pixels overlapping produce75% coverage, not50%. Draw background cloud layers before terrain/actors; foreground mist after them when you want partial occlusion. Use depth-aware sorting for smoke sources where appropriate.

These are isolated atmospheric sprites, not promised seamless repeating texture strips. For top-down or side-view scenes, use them as screen-facing billboards and tune scale/anchor/depth to the camera. No magic, sorcery, symbols or supernatural effects are included.

Art was generated with the built-in image-generation tool, mechanically placed onto the exact frame grid, and given deterministic alpha variants/fade envelopes as requested. The original alpha is retained and multiplied by the edge/opacity treatment. Prompt records are included. Previews demonstrate compositing and animation outside Bananas; engine runtime integration has not been tested.

preview_opacity.png uses visible preview-only backgrounds to make transparency differences easy to inspect. preview_animations.gif displays all animations/variants. preview_level.gif shows selected instances over the existing level preview. These are demonstrations with captions/backgrounds, not production atlases. The PNG atlas itself contains no baked background, checkerboard or labels.

Generic example assets follow the existing CC0-1.0 designation; see LICENSE.txt. Company names, trademarks and proprietary mascot assets are excluded.

## Added overhead and coloured tiles

The topdown folder adds seamless overhead cloud, fog, smoke, green gas and brown gas fields, soft visibility masks, ready-composited fog-of-war tiles, and shaped green/brown gas regions. See topdown/README.md for corner-mask rules and discovery-state layering. The parent atmosphere.png remains an isolated sprite sheet; use the topdown atlases for exact repeating joins.

gas_fumes.png/json is a second isolated sprite sheet for green and brown fumes, 512 x 384 pixels, 48 frames. It uses 64 x 64 cells, eight columns, and six rows: green/brown at 25%, then green/brown at 50%, then green/brown at 75%. Animation keys are green_gas_25, brown_gas_25, and the corresponding 50/75 variants. These loop and use the centre pivot (32,32). Colours are stylized natural-gas/poison visuals; hazard rules and damage belong to the game.

The fog-of-war and gas masks are rendering assets. The included visibility preview demonstrates discovery layering; it does not install a line-of-sight implementation in Bananas.
