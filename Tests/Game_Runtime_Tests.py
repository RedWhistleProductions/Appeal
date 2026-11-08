"""Exercise real interpreter scripts: clocks, lifecycle, events and timed delivery."""
from pathlib import Path
import os
import subprocess
import tempfile
import time

root = Path(__file__).resolve().parents[1]
env = dict(os.environ, APPEAL_RESOURCE_FOLDER=str(root / 'Resources'))

def run(body, expected=None, error=None):
    with tempfile.NamedTemporaryFile(mode='w', suffix='.Appeal', dir=root/'Tests',
                                     delete=False, encoding='utf-8') as f:
        path = Path(f.name)
        f.write('Run Libraries/Game/Runtime\n' + body + '\nDone\n')
    try:
        p = subprocess.run([str(root/'Appeal.exe'), str(path)], cwd=root, env=env,
                           capture_output=True, text=True, timeout=20)
        if error:
            assert error in p.stderr, (p.stdout, p.stderr)
        else:
            assert not p.stderr and p.returncode == 0, (p.stdout, p.stderr)
            if expected is not None:
                assert p.stdout.splitlines() == expected, (p.stdout, expected)
        return p.stdout.splitlines()
    finally:
        path.unlink()

# A real round: timer advances only during play, UI timer still advances paused.
run('''
Set_Var Hits 0
Define_Function Tick
    Set_Var Hits (Add Var Hits 1)
    Print Var Event_Value
End_Function
Events Bind Expired Tick
Call Game_Start
Events After Gameplay 100 Expired game
Events After_Real Interface 100 Expired real
Set_Var Game_Elapsed 40
Call Game_Advance
Call Game_Pause
Set_Var Game_Elapsed 100
Call Game_Advance
Call Game_Dispatch_Events
Print Var Game_Time
Print Var Game_Real_Time
Print Var Game_Delta
Call Game_Resume
Set_Var Game_Elapsed 60
Call Game_Advance
Call Game_Dispatch_Events
Print Var Hits
Print Var Game_Time
Print Var Game_Delta_Seconds
Call Game_Stop
Set_Var Game_Elapsed 40
Call Game_Advance
Print Var Game_Time
Print Var Game_State
''', ['real', '40', '140', '0', 'game', '2', '100', '0.059999999999999998', '100', 'Stopped'])

# Restart clears transient events/timers while preserving handlers and settings.
run('''
Define_Function Started
    Print started
End_Function
Events Bind Game_Started Started
Call Game_Start
Call Game_Dispatch_Events
Events After Old 10 Old_Event ""
Events Emit Old_Event ""
Set_Var Game_Time_Scale 2
Call Game_Restart
Print (Events Timer_Count)
Call Game_Dispatch_Events
Set_Var Game_Elapsed 1000
Call Game_Advance
Print Var Game_Time
Print Var Game_Real_Time
Print Var Game_Delta
Call Game_Pause
Call Game_Start
Print Var Game_Paused
Print Var Game_Time
''', ['started', '0', 'started', '500', '1000', '500', '0', '0'])

# FIFO, multiple listeners, idempotent bind, context restoration, deferred emits.
run('''
Set_Var Event_Name outer
Set_Var Event_Value original
Define_Function First
    Print Var Event_Value
    Events Emit Later deferred
End_Function
Define_Function Second
    Print second
End_Function
Define_Function Later
    Print Var Event_Value
End_Function
Events Bind Message First
Events Bind Message First
Events Bind Message Second
Events Bind Later Later
Events Emit Message one
Events Emit Message two
Events Dispatch 1
Print (Events Pending)
Print Var Event_Name
Print Var Event_Value
Events Unbind Message Second
Events Dispatch 8
Print (Events Pending)
Events Dispatch 8
''', ['one', 'second', '2', 'outer', 'original', 'two', 'deferred', '1', 'deferred'])

# Handler-generated cycles are bounded, and resets cancel the active batch.
run('''
Define_Function Again
    Print once
    Events Emit Loop ""
End_Function
Events Bind Loop Again
Events Emit Loop ""
Events Dispatch 256
Print (Events Pending)
Events Clear
Define_Function Reset
    Events Reset
    Events Emit Fresh ""
End_Function
Define_Function Stale
    Print BAD
End_Function
Events Bind Reset Reset
Events Bind Stale Stale
Events Emit Reset ""
Events Emit Stale ""
Events Dispatch 256
Print (Events Pending)
''', ['once', '1', '1'])

# Named replacement/cancellation, zero delay, simultaneous expiry deterministic.
run('''
Define_Function Seen
    Print Var Event_Value
End_Function
Events Bind E Seen
Events After A 10 E old
Events After A 20 E replacement
Events After B 0 E cancelled
Events Cancel B
Events After C 20 E last
Events After Zero 0 E zero
Set_Var Game_Elapsed 0
Call Game_Advance
Events Dispatch 256
Call Game_Start
Events After A 20 E replacement
Events After C 20 E last
Set_Var Game_Elapsed 20
Call Game_Advance
Events Dispatch 256
Print (Events Timer_Count)
''', ['zero', 'replacement', 'last', '0'])

# Custom state is independent of pause; stopping freezes gameplay, not real time.
run('''
Call Game_Start
Set_Var Game_Next_State Boss
Call Game_Set_State
Call Game_Pause
Call Game_Pause
Call Game_Resume
Call Game_Resume
Print Var Game_State
Print Var Game_Previous_State
Set_Var Game_Time_Scale 0
Set_Var Game_Elapsed 50
Call Game_Advance
Print Var Game_Delta
Set_Var Game_Elapsed -1
Call Game_Advance
Print Var Game_Real_Time
''', ['Boss', 'Playing', '0', '50'])

# Equivalent elapsed durations split across frames yield identical fixed steps.
for frames, delta in [(10, 10), (4, 25)]:
    run(f'''
Call Game_Start
Set_Var Game_Fixed_Delta 20
Set_Var Steps 0
Set_Var Frame 0
Set_Var Game_Elapsed {delta}
Loop While Var Frame < {frames}
{{
    Call Game_Advance
    Loop While (Call Game_Next_Step) == 1
    {{
        Set_Var Steps (Add Var Steps 1)
    }}
    Set_Var Frame (Add Var Frame 1)
}}
Print Var Steps
Print Var Game_Time
''', ['5', '100'])
run('''
Call Game_Start
Set_Var Game_Fixed_Delta 10
Set_Var Game_Max_Steps 2
Set_Var Game_Elapsed 100
Call Game_Advance
Set_Var Steps 0
Loop While (Call Game_Next_Step) == 1
{
    Set_Var Steps (Add Var Steps 1)
}
Print Var Steps
Print Var Game_Accumulator
Call Game_Pause
Print (Call Game_Next_Step)
Call Game_Stop
Print (Call Game_Next_Step)
''', ['2', '0', '0', '0'])

run('Events Bind Missing No_Function', error='Events handler not found')
run('Events After Bad -1 E value', error='nonnegative delay')
run('Events After Bad nan E value', error='finite number')
run('Events Dispatch 0', error='limit must be an integer')
run('Events Dispatch 1.5', error='limit must be an integer')
run('Events Update 10 10\nEvents Update 9 10', error='nondecreasing')
run('''
Define_Function Nested
    Events Dispatch 1
End_Function
Events Bind E Nested
Events Emit E ""
Events Dispatch 1
''', error='cannot be nested')

# Use the actual monotonic clock through Game_Update, rather than injected deltas.
clock = run('''
Call Game_Start
Set_Var Begin (Clock_Milliseconds)
Set_Var Now Var Begin
Loop While (Subtract Var Now Var Begin) < 25
{
    Call Game_Update
    Set_Var Now (Clock_Milliseconds)
}
Print Var Game_Time
Print Var Game_Real_Time
''')
assert 0 < float(clock[0]) <= float(clock[1]), clock

# Libraries resolve from the configured resource root even outside the checkout,
# with nested folders, either path separator, and an optional .Appeal suffix.
with tempfile.TemporaryDirectory(prefix='Library_Lookup_') as temp:
    folder = Path(temp)
    resources = folder/'CustomResources'
    library = resources/'Libraries/Nested/Probe.Appeal'
    library.parent.mkdir(parents=True)
    library.write_text('Print LIBRARY_FOUND\n', encoding='utf-8')
    driver = folder/'Driver.Appeal'
    driver.write_text('Run Libraries/Nested/Probe\n'
                      'Run Libraries/Nested/Probe.Appeal\n'
                      'Run Libraries\\Nested\\Probe\nDone\n', encoding='utf-8')
    p = subprocess.run([str(root/'Appeal.exe'), str(driver)], cwd=folder,
                       env=dict(env, APPEAL_RESOURCE_FOLDER=str(resources)),
                       capture_output=True, text=True, timeout=10)
    assert p.returncode == 0 and not p.stderr, (p.stdout, p.stderr)
    assert p.stdout.splitlines() == ['LIBRARY_FOUND'] * 3, p.stdout

# Small representative cost sample: interpreted update plus one event per frame.
start = time.perf_counter()
run('''
Define_Function Count
    Set_Var Received (Add Var Received 1)
End_Function
Set_Var Received 0
Events Bind Frame Count
Call Game_Start
Set_Var Frame 0
Set_Var Game_Elapsed 16
Loop While Var Frame < 1000
{
    Call Game_Advance
    Events Emit Frame ""
    Call Game_Dispatch_Events
    Set_Var Frame (Add Var Frame 1)
}
Print Var Received
''', ['1000'])
elapsed = time.perf_counter() - start
print(f'GAME_RUNTIME_TESTS_PASSED (1000 update/event frames: {elapsed:.3f}s, includes process startup)')
