# Slime

Load Slime.png and Slime.json together. This enemy is independent of the other folders.

512 x 512 RGBA8 transparent PNG; 8 columns x 8 rows; every frame 64 x 64; pivot (32,58). Zero spacing and margin. Right-facing side view.

Rows: idle, walk, run, jump, fall, attack, hurt, die. Each row has eight frames. Index = row * 8 + column. Timings and loop flags are in the JSON. Idle/walk/run/fall loop; die holds its final frame. Other animations return to your chosen gameplay state after completion.

Use nearest-neighbour sampling and straight-alpha blending. Vertical motion and hitboxes belong to the game. CC0-1.0; see LICENSE.txt.
