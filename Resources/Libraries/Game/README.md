# Reusable game runtime

Load `Run Libraries/Game/Runtime` once, then `Call Game_Start`.
Call `Game_Update` and `Game_Dispatch_Events` once per host frame, including
while paused. The host owns its input, simulation, rendering, and object reset.

```text
Define_Function Show_Message
    Print Var Event_Value
End_Function
Events Bind Message Show_Message
Events After Greeting 1000 Message "Hello after one gameplay second"
```

Use `Game_Delta_Seconds` for variable-step simulation, or the optional
`Game_Next_Step` expression and `Game_Step_Seconds` for fixed-step simulation.
Gate simulation on running/pause state. Existing module clocks are not
automatically synchronized by this library.

See [the full reference](../../../Docs/Game_Runtime.html) for lifecycle,
events, timers, configuration, ordering, and restart behavior.

Example: `Appeal.exe Examples/Game/State_Timing_Events`

Tests: `python Tests/Game_Runtime_Tests.py`

## Scene lifecycle

Load `Run Libraries/Game/Scenes` after Runtime and a World plugin for reusable
scene enter/exit/update/draw hooks and deferred transitions.
See [Scenes](Scenes.md) for usage, object commands, and ownership limitations.

Deferred native improvements are tracked in
[C++ recommendations](../../../Docs/CPP_Recommendations.md).

## Player interface and navigation

Load `Game/Menus` after Runtime for renderer-independent menu selection and
event actions. `Game/Interface` adds an optional Graphics GUI adapter.
See [Menus](Menus.md) for focus handling, host integration, and MVP limitations.
