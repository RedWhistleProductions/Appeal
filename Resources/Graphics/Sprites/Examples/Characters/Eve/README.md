# Bananas generic female hero graphics test pack v1

Four transparent RGBA8 PNG atlases. Every frame is exactly 64 x 64 pixels. Eight columns, zero spacing, zero outer margin. No text, borders, baked backgrounds or visual effects. 152 frames total.

| Sheet | Dimensions | Rows |
| --- | --- | --- |
| Female_Hero_Movement.png | 512 x 320 | idle, walk, run, jump, fall |
| Female_Hero_Combat.png | 512 x 256 | sword_slash, sword_heavy, bow, block |
| Female_Hero_Actions.png | 512 x 384 | hurt, die, call, cheer, sit, interact |
| Female_Hero_Extras.png | 512 x 256 | directional idles, equipment, push, climb |

Each PNG has a matching JSON. Coordinates start at the top left; x grows right and y grows down. Index = row * 8 + column. The pixel-space drawing pivot is (32,58). Draw a frame at entity_position minus pivot. Frame duration is in milliseconds. Animation entries contain frame indices and loop flags. Non-looping animations stop at their last frame; return to idle using game state logic. Die explicitly holds its last frame.

Extras directions are down/front, left, right, up/back, two frames each. Equipment row columns 0-5 are unarmed, sword, shield, sword+shield, bow, plain wooden staff. Columns 6-7 are spare unarmed poses, not included in equipment animations. Push and climb occupy rows 2 and 3.

Use nearest-neighbour filtering. Load as an sRGB colour texture with straight alpha; for conventional OpenGL blending use SRC_ALPHA and ONE_MINUS_SRC_ALPHA. Disable mipmaps for basic atlas tests to avoid neighbour bleed. Flip ordinary side-view frames horizontally for leftward movement. Four directional views are idle views only, not full directional locomotion sets. Jump and fall poses are anchored: vertical motion is supplied by your engine, not baked into the sheet.

Animation timing is an initial test preset and can be adjusted. Artwork was generated with the built-in image-generation tool from the approved hero reference, then mechanically extracted, scaled with nearest-neighbour sampling, and placed into exact cells. Character pose/volume can vary slightly between generated frames; these are engine test assets, not a hand-polished animation set. Technical checks passed: RGBA, exact dimensions and grid, occupied frames, transparent cell margins, and no cell-edge overflow. No engine runtime playback has been tested.

Design specification: brown hair in a ponytail, blue practical tunic/armor and trousers, red scarf/cape, brown boots and gloves. Physical sword/bow/shield only; staff is plain wood without orb. No magic, sorcery, occult symbols, glowing projectiles, or separate effect sprites. Mascot and UI assets are outside this pack.

Generic examples are designated CC0-1.0 at the user's direction. See LICENSE.txt. This dedication does not include Red Whistle Productions trademarks or mascot assets.

This companion pack preserves the male set's animation names, frame indices, timings, dimensions and pivot. Filenames use female_hero_ to allow both packs in the same directory.
