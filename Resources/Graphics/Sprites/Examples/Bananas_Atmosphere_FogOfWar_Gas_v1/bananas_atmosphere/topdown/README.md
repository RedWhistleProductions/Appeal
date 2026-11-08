# Top-down seamless atmosphere and fog of war

This companion set uses actual overhead cloud/fog/smoke textures. Each tile is64x64 RGBA8 on an exact grid. It is designed for placement on the same64px world grid as your levels. The default pivot is(0,0): draw at tile/world top-left.

## Files

- topdown_atmosphere.png/json:120 texture frames, 8 columns x 15 rows, 512 x 960. Five eight-frame animations (clouds, fog, smoke, green_gas, brown_gas) at25%,50%,75% intrinsic-alpha multipliers. Pick a consistent material/opacity across neighbouring cells. Opposite edges share exact RGBA pixels within each family; boundaries are shared across animation phases too, so phase offsets will not create edge mismatches. These are repeating fields, unlike the isolated billboards in the parent directory.
- visibility_masks.png/json:16 static64x64 soft coverage masks,4x4 atlas256x256. White RGB plus alpha for use in a shader/mask compositor if desired.
- fog_of_war.png/json:256 ready-composited frames,8columns x32rows,512x2048. Two fog-of-war states,16 corner masks per state,eight phases per mask. No separate mask shader is required for these sprites; standard alpha compositing is enough.

## Fog-of-war states

unexplored:dark charcoal textured fog, fully opaque where coverage is full, hiding the unknown map. explored_memory:dark translucent fog retaining partial visibility of previously seen terrain. Visible areas use mask0(empty) or skip drawing the overlay. These are presentation assets. Your game supplies discovery state, line-of-sight and visibility calculations; these PNGs do not calculate them.

Animation keys are explored_memory_mask_0 through explored_memory_mask_15 and unexplored_mask_0 through unexplored_mask_15. All loop. The first16 rows are explored_memory masks0..15; the next16 rows are unexplored masks0..15. Each row contains eight animation frames. Each final texture cycle closes on the same first/last frame to avoid a wrap discontinuity.

## Connecting tiles

Use shared corner states, with bits NW=1,NE=2,SE=4,SW=8. A fogged corner is1; a clear corner is0. Mask = NW +2*NE +4*SE +8*SW when corner variables are booleans. Assign visibility to shared map vertices so neighbouring cells use the same bits at their common corners. Select the matching mask from the chosen layer. Mask0 is fully clear;mask15 fully covered;other masks create soft straight/corner/diagonal boundaries.

Fog patterns are spatially periodic and all compatible corner-mask pairs have pixel-identical shared edges. Masks must agree:arbitrary incompatible masks will still make a seam. This16-mask system gives basic corner-based transitions;it is not a47-tile neighbour autotiler. Keep the map's visibility resolution fine enough for your gameplay. Increase vertex resolution or use your own visibility shader for detailed circular line-of-sight.

For three visibility states, draw explored_memory first for everything not currently visible, then draw unexplored on top for never-discovered regions. Build the two layers from independent shared-vertex coverage fields. At a completely unknown interior tile,unexplored is opaque. On exploration,mark it discovered and remove only the unexplored layer. On current visibility,clear both layers. Do not reveal never-discovered cells merely by fading the unknown layer to a lower opacity unless that is intended.

Top-down natural atmosphere can be drawn separately above or below the visibility layers. Clouds/fog/smoke have real graded alpha and25/50/75 variants. Fog-of-war unknown interiors intentionally reach alpha255 to conceal the map. explored_memory stays semitransparent. Keep consistent variants/materials along a connected region for exact colour/alpha continuity.

Use nearest-neighbour filtering and straight-alpha blending(SRC_ALPHA,ONE_MINUS_SRC_ALPHA). PNG alpha must be used;do not substitute brightness for alpha. Tiles have no extruded gutters,so use no mipmaps for an initial renderer. Fully transparent pixels have zero RGB. For screen coordinates that differ from64world pixels,scale all connected tiles identically and align their boundaries exactly.

The overhead source art was made using the built-in image-generation tool. Edge harmonization and mask compositing turn it into exact repeatable tiles. Exact spatial/temporal and alpha checks are recorded in validation.json. Previews show a renderer demonstration,not a Bananas engine implementation. See parent LICENSE.txt for the generic CC0-1.0 designation.

## Shaped green and brown gas regions

gas_area_tiles.png/json provides 256 ready-composited frames: two colours, 16 masks per colour, eight phases per mask. Atlas 512 x 2048. Names are green_gas_mask_0..15 and brown_gas_mask_0..15. Coverage follows the same shared-corner bit rules. These natural coloured fumes use a baked 75% intrinsic-alpha multiplier; multiply by a runtime opacity to make them lighter. They have tested matching edges across compatible masks and every phase combination. Use these sprites when you want soft-edged poison/gas regions without a separate mask shader. The ordinary repeating field atlas also offers 25%, 50%, and 75% versions of both colours.

To avoid a repeated grid of separate puffs, the coloured overhead gas samples are overlapped with periodically shifted copies before edge matching. The separate parent gas_fumes sheet retains isolated puff shapes. Natural atmosphere and gas areas are visual layers; the game decides whether a gas-covered tile applies poison or other gameplay effects.
