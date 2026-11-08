# Animation state integration — Step 11 MVP

Load `Libraries/Game/Poses` after Graphics when using Pose_Apply. Set Pose_Asset
and its flags before each actor update. Pose_Choose (no Graphics dependency)
selects a configured clip using priority Dead > Hurt > Attack > Jump > Run >
Walk > Idle. The flags are Pose_Dying/Hurting/Attacking/Airborne/Running/Moving,
default 0. Clip-name variables use Pose_Dead/Hurt/Attack/Jump/Run/Walk/Idle.
Empty selected clip names fall back to Idle.

Pose_Apply selects only when Graphics Current_Animation differs, preserving
playback instead of restarting each frame. Pose_OK reports native selection
success. Load/configure clips and FPS/mode once with existing Graphics calls.
Configure one-shot attacks/damage clips with the native animation mode as needed;
the game owns when their flags clear. Unknown clips keep native error behavior
and do not silently pretend to load. This uses Graphics' asset-local playback,
so the host can call the helper for multiple actors without a duplicate registry.

All Pose_ inputs are shared scratch state: set all flags/config that differ per
actor, rather than letting one actor's flags leak into another. Position, facing,
scaling and collision remain separate from pose choice. Physics/Mechanics can
supply flags through their existing getters.

Native animation time still follows the renderer's clock. Script game pause/
time scale does not automatically freeze it; exact gameplay-clock advancement,
animation markers/completion and hit-frame synchronization remain deferred native
work. Do not claim deterministic combat timing from this selector alone.

Test: `python Tests/Game_Poses_Tests.py` loads real sprite clips, checks priority,
per-asset isolation, missing clips, and that repeating a pose does not reset it.
