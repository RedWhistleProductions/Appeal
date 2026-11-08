# Action timing windows — Step 22 partial

Load Runtime and `Libraries/Game/Action_Window`. Set Window_ID and nonnegative
Windup_MS/Active_MS/Recovery_MS, then Window_Start. It accepts only running,
unpaused games and rejects restarting an active window unless Interruptible=1.
All names use Window_ prefix. Tick once per simulation step to derive Phase
Windup/Active/Recovery/Finished from Game_Time. Apply hit/contact sampling only
while Phase=Active. Window_Cancel closes an active window explicitly.

Started/Opened/Closed/Finished/Cancelled notifications are queued Action_ events
with the ID payload. State transitions are sampled at ticks, not an audio or
animation frame. A long step that crosses the entire active interval sets
Window_Missed=1 while emitting open/close notifications once. Do not retroactively
apply damage from those late notifications. Use sufficiently small fixed steps
for required hit precision, plus per-target attack deduplication in the game.

Cancel windows on scene/restart cleanup. Each actor needs its own active window
context (including start time, flags and revision) if swapping this shared helper.
Persist through save hooks only when the game defines how attacks resume. Snapshot
configuration before starting; don't change duration/ID mid-window.

This adds gameplay timing phases, not animation markers, input-buffered combos,
ball/team behavior or a full fighting/sports implementation. Those Step 22/native
requirements remain open. Test: `python Tests/Game_Action_Window_Tests.py`.
