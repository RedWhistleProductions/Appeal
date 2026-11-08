# NPC finite state machine — Step 12 MVP

Start NPC AI with a simple FSM, as requested. Genetic algorithms/neural networks
are later directions. This reusable script needs Runtime, not a new AI plugin.
It supplies lifecycle/state timing; games supply behavior and perception.

Set FSM_Entity (hook context), FSM_Next_State and hook function names, then call
FSM_Start. It exits a previous state when present and enters the requested state
at Game_Time. FSM_On_Enter/Exit/Update default to empty. Hooks branch on FSM_State.

Call FSM_Tick once per actor update. It runs only when enabled, game running and
unpaused. FSM_Age is gameplay milliseconds since entry. Inside FSM_On_Update,
set FSM_Next_State to request another state; after that hook finishes, one
transition runs exit -> state change -> enter. The new state's update waits
until the next tick. Same-state/empty requests do not transition. Lifecycle hooks
must not request another transition or mutate the state/context directly.
Reentrant Start/Tick calls are ignored while FSM_Busy=1. FSM_Change is internal.

For multiple NPCs, load each actor's FSM_State and FSM_Entered_At before calling,
set Entity/hooks/Enabled, then store State/Entered_At back into that actor's script
variables. No other per-actor memory is required by this helper. This explicit
two-field context swapping is MVP boilerplate, not an automatic actor registry.
Game-owned perception/position/health likewise stays in its own actor variables.
Save these fields through normal progress hooks; restore the same gameplay clock.

Use Game_Delta_Seconds (or Game_Step_Seconds in a fixed-step host) for behavior
movement. The FSM does not supply navigation, collision, attacks, threading or
automatic Mechanics actions; hooks call those existing systems deliberately.
Game pause freezes state age; disabling a specific FSM skips its updates but does
not stop the global game clock, so its age can increase while disabled.

Example: `Appeal.exe Examples/Game/NPC_FSM`. Tests:
`python Tests/Game_FSM_Tests.py` covers lifecycle order, deferred transition,
pause/age, reentrancy and independent actor contexts.
