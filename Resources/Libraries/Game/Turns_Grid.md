# Turns, rounds and grids — Step 16 MVP

Load Runtime, `Libraries/Game/Turns` and optionally `Libraries/Game/Grid`.

Turn_Count is a positive integer; actors use indices 0..Count-1. Set nonnegative
integer Turn_Budget, then Turn_Start. It initializes owner 0, round number 1 and
AP=Budget. Turn_Revision increments on start, every accepted spend, and turn end.
Queued player commands should capture this revision and their actor index.

Set Turn_Actor/Turn_Expected_Revision and Turn_Cost, then Turn_Spend. It rejects
wrong/stale actors, paused/stopped/inactive turns, negative cost or insufficient
AP. On success it deducts AP and sets Turn_Accepted=1. Validate legal moves and
targets **before** spending. Apply effects only after accepted spending; no
automatic effect rollback is provided. Free actions (cost 0) are permitted.

Turn_End uses the same actor/revision gates, advances cyclic owner order, resets
AP and increments Turn_Number on wrap. Ending is explicit, not automatic at AP=0.
To end after an accepted final action, use the newly updated revision. Set
Turn_Active=0 when the game is over. Turn_Began/Turn_Action carry the owner index
only; delayed handlers must not assume mutable turn globals describe an old event.
Persist owner, AP, number, active and revision through save hooks; don't restart
turns after restoring those fields.

Grid_Check takes integer From_X/Y, To_X/Y, Columns/Rows, Blocked, Diagonal,
Corner_Clear and Max_Distance (all Grid_). It validates endpoints and nonzero
range using Manhattan distance, or Chebyshev distance when diagonal movement is
enabled. Host-supplied occupancy and corner checks prevent illegal destinations.
Default range is one adjacent cell. **Longer range checks do not find a path or
test intervening obstacles**; the host must validate a path separately.

Grid_To_World returns the destination cell's top-left using Origin_X/Y and positive
Cell_Width/Height, for Camera_2D or other presentation. No integer floor/pixel-pick,
A*, line of sight, occupancy registry or mandatory Physics scene is added.
Use finite, reasonable integer indices and explicit game rules for special moves.

Both helpers use shared scratch state; the host owns board content, action rules,
opponents, timers and UI. Test: `python Tests/Game_Turns_Grid_Tests.py`.
