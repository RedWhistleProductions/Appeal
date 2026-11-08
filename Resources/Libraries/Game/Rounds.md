# Progression and outcomes — Step 8 MVP

`Libraries/Game/Rounds` is a reusable sequential question/round policy. Load
after Runtime. Set positive integer Round_Total, nonnegative Round_Points (default
1), and Round_Pass_Score, then call Round_Start. The host separately starts the
game runtime and supplies content for Round_Item (1-based).

Set Round_Correct to 0 or 1 and call Round_Submit. While running/unpaused and
Round_State=Question, it accepts once, adds points if correct, increments
Round_Answered and changes state to Feedback. Repeated submissions cannot score
again. Round_Accepted reports whether that call was accepted.

Call Round_Next from Feedback to advance or finish. The final outcome is Win
when Round_Score >= Round_Pass_Score, otherwise Lose. Finishing does not stop
Game automatically: the host chooses menus, sound and persistence. States are
Idle, Invalid (nonpositive total), Question, Feedback and Finished.

Queued events: Round_Answered carries the item number; Round_Finished carries
Win/Lose. Like other events, handlers must not assume mutable global state still
matches an earlier event. UI commands should carry the item number and recheck
it before submission/advancement to reject stale clicks.

Persist item/answered/score/state/outcome and any game-specific selection through
save hooks. Restoring these variables resumes the round; do not call Round_Start
after loading. This library doesn't own timers, unlocks, inventory, randomization
or question data. Applications can compose those with Rules and Events.

Tests: `python Tests/Game_Rounds_Tests.py` covers wins/losses, duplicate/paused
submissions, finalization, invalid total and restart.
