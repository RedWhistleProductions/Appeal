# Many-agent scheduling — Step 20 partial/MVP

Load Runtime then `Libraries/Game/Agents`. Set integer Agents_Count and
Agents_Budget (maximum decisions per pass, default 32). Each host frame:

```text
Call Agents_Begin
Loop While (Call Agents_Next) == 1
{
    // Load actor context at Agents_Index, think/FSM_Tick, store actor context.
}
```

Indices are 0..Count-1. The cursor persists between passes, so stable actors are
visited fairly in round-robin order. At most min(Budget,Count) are selected per
pass. Paused/stopped/disabled/empty/zero-budget passes yield none. Count shrinkage
normalizes an out-of-range cursor on the next Begin. Don't change population or
begin another pass inside the current loop; apply spawn/destruction afterward.
Skip stale/disabled actors deliberately in your context loader.

This budgets decision **count**, not milliseconds. A single heavy hook can still
stall. Use Game_Time minus each actor's stored last-think timestamp for decision
cadence. Do not update movement/physics only when a brain is selected: simulation
still runs every fixed step using retained commands. Keep actor state outside
the scheduler, e.g. the explicit two-field FSM context and game-owned records.

This is not a complete crowd simulator, unit selection/order system, pathfinding
or multithreaded AI. Those parts of Step 20 remain open and need representative
many-actor profiles before new native batching is justified. No C++ was added.

`python Tests/Game_Agents_Tests.py` checks fairness, shrinkage, pause, and budget
limits, and prints a scheduler-only cost sample (not complete NPC cost).
