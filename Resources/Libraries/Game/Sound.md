# Audio integration — Step 7 MVP

Load Audio and `Libraries/Game/Sound`. Load effects/music once using existing
Audio Load_SFX/Load_Song commands; the library doesn't reload assets per event.
Set Sound_Cue to a loaded effect name and call Sound_Play_Cue, or bind an event:

```text
Events Bind Sound_Cue Sound_Receive_Cue
Events Emit Sound_Cue Correct
```

Sound_Enabled defaults to 1; set it to 0 to suppress new cues. For music, set
Sound_Music to a loaded song name (or empty) and call Sound_Refresh_Music. This
stops existing music and selects the configured song when enabled; it restarts
from the beginning rather than preserving playback position. Call only on a
deliberate theme/settings change, not each frame.

Games decide which pause/menu events change audio and persist Sound_Enabled
through the existing settings helpers. UI feedback may continue while gameplay
is paused. Sound suppression does not halt an effect already playing. Existing
Audio volume controls affect music/video, not a separate SFX bus. Full channel
groups, fades, per-scene unloading and exact pause/resume remain native follow-ups.

The library owns only Sound_ variables/functions. The host owns asset lifetimes,
Audio initialization and event binding. Native backend errors are not converted
into success results. Test with `python Tests/Game_Sound_Tests.py`; this uses the
real SDL audio backend with a dummy output and checks mixer channel activity.
