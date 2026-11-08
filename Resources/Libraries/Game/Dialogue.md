# Dialogue and branching narrative — Step 15 MVP

Load Runtime and `Libraries/Game/Dialogue`. Set Dialogue_On_Node to an existing
content function that branches on Dialogue_Node and supplies Speaker/Text. Set
Dialogue_Next_Node and call Dialogue_Start. An empty next node ends the dialogue.

Games draw the current speaker/text and authored choices using their UI. Button
callbacks queue game events; their payload should capture Dialogue_Revision.
In the handler, select a known choice, recompute its availability (quest flags,
inventory, locked/unlocked story segment, Rules, etc.), and set:

- Dialogue_Choice_ID: nonempty stable choice ID.
- Dialogue_Choice_Next: destination node, or empty to end.
- Dialogue_Choice_Available: current 0/1 permission.
- Dialogue_Expected_Revision: captured version of the displayed node.

Call Dialogue_Select. It accepts only an open/enabled/current revision, calls
Dialogue_On_Choice for game effects, changes node, increments revision and loads
new content. Dialogue_Accepted/Reason report the result. Old queued choices cannot
affect another visit even to the same node. The caller owns which choice IDs
belong to the node and which destination/effects they authorize.

Dialogue_On_Choice can read Chosen_ID/Chosen_Next and the outgoing Node; it must
not mutate library context or recursively advance dialogue. Busy guards ignore
nested Start/Select/Refresh. Hook errors are not rolled back. Queued Changed/Ended/
Choice_Made events carry new node / previous node / choice ID respectively, not a
full immutable conversation record. Delayed handlers must respect that boundary.

Dialogue can run while gameplay is paused; the host owns pause/focus policy and
whether a disabled dialogue should be hidden. Persist Node and Revision plus
game-owned quest/story flags with save hooks, then call Dialogue_Refresh to reload
content without replaying choice effects or changing revision. Text/content hooks
are scripts, not serialized executable code. Initial Speaker/Text are empty.

No typewriter text, localization database, voice playback, automatic inventory
effects, nested conversation stack or editor is imposed. This is a reusable
branch/state foundation, not all RPG quest features.

Test: `python Tests/Game_Dialogue_Tests.py` covers locked choices, branch effects,
stale/double choice rejection, lifecycle guards, closure and refresh after restore.
