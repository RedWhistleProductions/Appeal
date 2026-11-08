# Female guard

This NPC is independent of the others. Keep each PNG with its matching JSON: guard_female_side and guard_female_topdown.

Side:512x256,32frames,eight frames each for idle,walk,talk,interact. Top-down:512x512,64frames,four frames per action in down,left,right,up. All cells64x64 RGBA8,zero spacing and margin,pivot(32,58). JSON supplies timing and animation indices. Idle/walk/talk loop;interact plays once then returns to the game-selected state.

Role:guard. Interaction:salute. No dialogue text,trade system,AI or audio is embedded in these images. Use nearest-neighbour sampling and straight-alpha blending. CC0-1.0;see LICENSE.txt.
