# Deferred C++ recommendations

This is the backlog for native changes discovered while building reusable Appeal
game components. Prefer existing module APIs and reusable scripts for the MVP.
Record evidence, the current workaround, and an acceptance check here before
expanding the C++ implementation. These are recommendations, not implemented work.

## 1. Return World properties and query results to scripts

- **Evidence:** `Appeal_Module_World/src/main.cpp` forwards `Get`,
  `Has_Component`, `List_Entities`, `List_Components`, and `Query` to void plugin
  functions. `Appeal_Standard_World/src/main.cpp` prints their results.
  Parenthesized calls therefore cannot supply the values to gameplay scripts.
- **Impact:** generic object updates, inventory, AI, and gameplay conditions
  cannot inspect these results using the normal expression syntax.
- **MVP approach:** use known object names, typed IDs, and explicit scene hooks;
  keep game-specific mutable state in script variables. Do not parse console
  output or introduce a second database registry merely to work around this.
- **Recommended change:** add compatible returned-value property/component
  accessors, nonthrowing existence checks, and script-friendly indexed iteration
  or snapshots of query results. Retain diagnostic printing separately.
- **Acceptance:** read an object's health in an If expression; enumerate active
  tagged entities without parsing text; missing-object checks return a defined
  result without creating an object or throwing.

## 2. An enclosing-function return and simpler conditions

- **Evidence:** Return inside an If returns from its anonymous block rather
  than its enclosing function. Step 1's Game_Next_Step required nested guards
  and a single final Return_Value to avoid returning multiple values.
- **MVP approach:** keep this nesting inside reusable library functions.
- **Recommended change:** design an explicit enclosing-function return with
  compatible existing block behavior; consider boolean conjunction/disjunction
  so simple guards need fewer nested blocks.
- **Acceptance:** a guarded function exits once and returns one value from
  inside nested conditions, without changing existing script behavior.

## 3. One authoritative simulation delta across modules

- **Evidence:** the game runtime exposes elapsed/fixed-step time, but existing
  modules are not automatically driven by that clock. World Update currently
  provides no general script object-update dispatch.
- **MVP approach:** gate scene hooks on Game_Running/Game_Paused and use the
  documented delta inside game scripts. Module-specific adapters remain explicit.
- **Recommended change:** audit/update Mechanics, Physics, Graphics animation,
  World and AI for an explicit delta path and consistent pause/time-scale behavior.
- **Acceptance:** one movement/animation/collision example behaves consistently
  across frame rates, pauses without advancing, and follows the selected scale.

## 4. Scene ownership of resources and pending work

- **Evidence:** World owns its entities/sections, but scene closure cannot by
  itself release unrelated Graphics/Physics/Mechanics/Input resources. Events
  and timers currently have global names without owner-scoped cancellation.
- **MVP approach:** Scene_On_Exit synchronously releases game-owned external
  resources and cancels its named timers/bindings. Global events are not cleared
  during a transition. Event handlers must validate the scene they target.
- **Recommended change:** consider explicit owner scopes and bulk cleanup for
  scene resources, subscriptions, and timers, plus generation-safe entity handles.
- **Acceptance:** repeat scene transitions with queued work; no stale callbacks
  affect replacement objects, and resource counts return to their baseline.

## 5. Explicit lifecycle operation results

- **Evidence:** several World operations are void; setters can implicitly create
  objects and Switch creates a missing scene. There is no general script-level
  exception recovery for a failed enter/exit hook. Calls to unknown function
  names can also be silently ignored when debugging is off.
- **MVP approach:** document ownership and preconditions; enter scenes through
  the lifecycle library; use existing mutation semantics deliberately. Runtime
  errors stop execution rather than pretend a transition succeeded.
- **Recommended change:** expose success/error results and nonmutating queries;
  consider staged scene construction/rollback once game requirements justify it.
- **Acceptance:** failed scene construction reports a useful result and can
  leave the previous scene usable without partially committing resources.

## 6. Consistent boolean arguments

- **Evidence:** lifecycle tests using `World Entity_Set_Active Copy False` and
  `World Create_Typed_Entity Hero Character 1 False` produced an integer-parser
  warning before using 0. These module arguments currently require numeric flags.
- **MVP approach:** pass 0 or 1 to World boolean arguments.
- **Recommended change:** consistently accept documented boolean spellings
  across interpreter/module argument conversion without warnings.
- **Acceptance:** equivalent 0/1 and False/True inputs produce identical behavior
  without diagnostics; malformed values report a useful error.

## 7. GUI callback payloads and stable control IDs

- **Evidence:** Graphics button callbacks receive a function name without the
  control ID/payload. The menu adapter therefore needs a small per-row callback
  to select the matching menu entry. The legacy backend renders buttons using
  ImGui::Button(Command.Text), so identical visible labels share an ImGui ID
  even when the script control names differ.
- **MVP approach:** keep callbacks short and route selection through the reusable
  library; use unique labels or ImGui's hidden ## suffix for duplicate labels.
- **Recommended change:** expose callback control ID/payload context and use
  the script control name as a stable native widget ID independently of its label.
- **Acceptance:** one generic handler can identify any activated menu row;
  two buttons with the same visible label activate their own actions correctly.

## 8. Explicit input contexts for custom interface navigation

- **Evidence:** Input gameplay getters are suppressed in GUI focus, while
  Graphics provides native GUI keyboard/gamepad navigation in that focus.
- **MVP approach:** use native GUI navigation for GUI menus, or script menu
  navigation with Game focus and custom drawing. Avoid per-frame focus toggling.
- **Recommended change:** consider explicit menu/game input contexts if custom
  scripted GUI navigation needs independent bindings without enabling gameplay.
- **Acceptance:** menu actions can be read while GUI is focused and gameplay
  actions remain suppressed; transitions neither leak nor repeat held actions.

## Performance watch list — no bottleneck established yet

- Native timer collection currently scans every active timer each update.
  A deadline-based structure may help if actual game profiles show timer cost.
- Script dispatch and per-object module calls should be measured before adding
  batch C++ functions. No speculative replacement of simple gameplay scripts.
- Step 1's small sample was about 0.19 seconds for 1,000 updates with one event
  each, including process startup. This is not a large-scene performance result.

Add measured scene/object costs and new recommendations as implementation and
full games expose them. Keep utilities and packaging work on separate lists.
