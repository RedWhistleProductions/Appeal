# Collision/physics integration — Step 10 MVP

Load Runtime, World, Physics, then `Libraries/Game/Bodies`. Attach/configure
bodies with existing Physics Use, shape, gravity and filtering commands. World
entities must exist. Set Body_Entity before per-body calls. There is no second
entity registry; call helpers explicitly for each known active actor.

- Set Body_Delta_Seconds from Game_Step_Seconds and call Bodies_Step once in the
  fixed-step loop. It steps all loaded Physics plugins when running/unpaused.
  Deltas outside (0,0.1] are rejected through Body_Stepped=0/Body_Status; they are
  not silently clamped. Don't also call Physics Update/Step for the same interval.
- Body_Sync copies Physics position/grounded to Body_X/Y/Z/Grounded and World.
  Body_With_Mechanics=1 additionally copies into an attached Mechanics entity.
  Set Mechanics External_Physics=1 when Physics owns motion. Body_Foot_Offset
  subtracts the capsule center-to-foot distance from Mechanics Y, default 0.
- Body_Send_Velocity copies Mechanics X/Z velocity while preserving Physics Y
  velocity. Jump/swim/flying vertical control remains game-specific, avoiding
  overwriting gravity/jumps each frame. Call before the global physics step.
- Body_Read_Events supplies Body_Event_Entity/Other/Type/Tag/Sensor synchronously
  to Body_On_Event for each event. Call once per body **after each physics step**.
  Don't destroy/reconfigure bodies, step physics or recursively read events from
  this callback. Queue game actions and apply them after all readers finish.

Body_Read_Events does not consume native events: calling twice in one step repeats
them. Contact/state queries can use existing Physics Contact/Touching/Get. The
existing Examples/Physics/Update_Entity_Mode illustrates water/ladder/climbing/
vehicle mode policy; the new bridge does not impose one on every game.

Physics/world coordinates remain unchanged (often +Y up); map Y for Camera_2D
presentation explicitly. Scene exit/load hooks still release/rebuild Physics and
Mechanics resources. The library cannot infer ownership or serialize bodies.
Tests cover real Box2D/Jolt floors, sensor begin/end, pause and World/Mechanics sync:
`python Tests/Game_Bodies_Tests.py`.
