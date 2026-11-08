# Memory and lifecycle audit - November 2, 2026

This audit covered first-party C++ in `Source`, `Resources/Modules`, and the legacy
root `Appeal_Module_Files` project: 154 source/header files in that inventory,
11 active modules, 14 installed functional plugins, and their 11 Test plugins.
Ownership and lifetime hotspots were reviewed across that inventory. Third-party
SDL, SQLite, ImGui and other `.deps` source code was not exhaustively audited.

## Findings corrected

| Area | Finding and change |
| --- | --- |
| Interpreter blocks | Each If/Loop capture permanently added another function node. Blocks now live in local invocation storage. Nested blocks no longer share global placeholder definitions. A 10,000-iteration regression checks that no anonymous functions remain registered. |
| Shared Named_List | A reproduced three-node destruction case leaked one value. Empty Remove/Next/Back and missing lookups could dereference null or return unrelated data. Lists now release every node, guard empty operations, return null for missing addresses, deep-copy their nodes and transfer ownership on moves. Cursor bookkeeping and node movement were corrected. All nine SDK copies match. |
| DLL ownership | Plugin and Dynamic_Module handles could be implicitly copied, risking multiple owners of one library handle. Copying is disabled. Linux symbol lookup clears stale function pointers before resolution. |
| Module lifecycle | Modules are destroyed explicitly in reverse load order, while globals still exist and before DLL teardown. All 11 modules provide Shutdown hooks. Plugins are unloaded before module static destruction; this is especially important for worker threads. |
| Network build | Its Makefile produced Network.dll while the interpreter loaded Appeal_Module_Network.dll. This left stale code active. Corrected the target; a live echo-server shutdown hang is now covered by a passing regression. |
| Module/function reload | Re-registering modules and functions accumulated duplicate registry entries and could retain obsolete dictionary pointers. Existing entries are replaced. |
| Audio | Sound effects lacked resource destruction and had an uninitialized channel before Load. Music/SFX are now single-owner objects. Resources are cleared before closing the mixer, after joining its worker. Empty SFX playback is guarded. |
| Input | Controller callbacks could close/rebind controllers while their containers were being traversed. Callbacks now run after polling, using saved function names/values. Windows controller close and SDL subsystem cleanup now release resources instead of the earlier leak workaround. |
| Graphics | GUI callbacks can rebuild the GUI command list during rendering. Rendering now traverses a stable snapshot. |
| Files | Open now retains RAII ownership during path resolution/opening, so an exception does not leak the allocated file handle. |
| Builds/tests | First-party object rules now track adjacent SDK headers. The core-module smoke test now runs an explicit test script instead of the interactive Main scene. |

## Validation completed

- Built the interpreter, all 11 active modules, all 14 functional plugins, all 11
  Test plugins, and the legacy root Files project.
- Interpreter/preprocessor tests, including nested blocks, redefinition and
  10,000 iterations without anonymous-function registry growth.
- Named_List destruction counts, empty operations, missing lookup, deep copy,
  move ownership, node reordering and cursor invariants; DLL-owner copy checks.
- GCC `-fanalyzer` on the list regression translation unit: no diagnostics in
  `Obj/Named_List_Analyzer.log` after the fixes. This is targeted static analysis.
- Core Audio/Graphics/Input/Network Test-plugin command smoke checks; seven other
  module Test backends: 161 calls including lifecycle hooks.
- Real AI, Files, Package, Storage, World and both Physics plugin smoke checks;
  extracted file contents verified.
- All three Mechanics orientations and character controls.
- World entity/tile-map native and script save/load tests.
- SQLite setup, persistence, control-set GUI save/load and transaction tests.
- Audio automatic advancement, disabling, stopping, looping, manual next and
  process shutdown.
- Graphics animation unit, script and OpenGL rendering checks.
- SDL Input events, controls, focus and digital controller/trigger callbacks,
  including closing/rebinding a controller inside its callback.
- Native GUI focus rendering checks.
- 50 SQLite DLL reloads, three live TCP echo sessions with implicit worker
  shutdown, and ten complete scene startup/render/shutdown sessions. Half of
  the scene sessions deliberately omit explicit cleanup to test destructors.

## Re-running key checks

```powershell
make -f Makefile -j4 all test
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/Build_All_Modules.ps1
python Tests/Memory_Lifecycle_Tests.py
python Tests/Controller_Database_Tests.py
python Tests/Storage_Setup_Tests.py
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/SDL2_Input_Events.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/Input_Focus_Tests.ps1 -CheckRendering
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/Graphics_Animation_Tests.ps1 -CheckRendering
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/Audio_Auto_Play_Tests.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/World_Tile_Map_Tests.ps1
```

## Limits

These results establish concrete fixes and regression coverage, not proof that
all memory errors are absent. The installed MinGW toolchain cannot link an
AddressSanitizer build (`cannot find -lasan`). No full ASan/LeakSanitizer run was
possible. The tests ran on Windows; Linux changes were reviewed but not executed.
Virtual controllers and available SDL/OpenGL behavior do not cover every physical
controller, driver, malformed asset, out-of-memory condition or third-party plugin.
Memory/error dialog text and a reproduction are still useful if another error
appears after restarting with the rebuilt binaries.
