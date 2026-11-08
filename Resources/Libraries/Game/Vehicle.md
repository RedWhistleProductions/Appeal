# Arcade vehicle motion — Step 21 partial

Load Runtime and `Libraries/Game/Vehicle`. Supply a unit Forward_X/Y/Z vector,
mapped Throttle -100..100 and Brake 0..100, and a fixed Delta_Seconds in (0,.1].
Vehicle_Update integrates scalar Speed with acceleration, braking/coasting drag
and separate forward/reverse caps, then returns Velocity_X/Y/Z. All variables use
the Vehicle_ prefix. Rates/caps must be finite and nonnegative. Defaults are arcade
units, not a real vehicle model. State is explicit per vehicle; persist Speed.

When paused/stopped/disabled, velocity output is neutral while retained Speed is
unchanged. Consume neutral output every frame or stop the body's retained motion
when disabling it. The physics host must still honor game pause. Vehicle_Apply
copies the result to an attached Physics body; Horizontal_Only=1 preserves its
gravity-driven Y velocity. Set it to 0 for a rail/top-down/full-vector controller.

Use Input's existing mapped analog values/dead zones, not raw controller values.
The host supplies steering/heading/vector normalization; this helper does not
compute trig, tire grip, suspension, flight aerodynamics or a chase camera.
It's a reusable acceleration/brake foundation, so Step 21 remains partial for
full racing/flight/sports requirements. No new Mechanics/Physics plugin was added.

Test: `python Tests/Game_Vehicle_Tests.py` checks rates/caps, reverse/braking,
pause-neutral output and applying motion to a real Box2D body.
