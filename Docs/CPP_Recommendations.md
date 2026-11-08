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

## 9. Save validation, recovery, and migration

- **Evidence:** existing World snapshots and Storage transactions can save a
  complete scene, but the script cannot catch native import/hook failures or
  validate a snapshot's embedded scene name before lifecycle cleanup. Numeric
  and snapshot corruption can still stop execution; no format migration API exists.
- **MVP approach:** versioned slots, basic metadata checks, atomic writes, and
  explicit hooks; reject missing/unsupported slots before touching live state.
  Keep the previous World scene until import succeeds. Document corrupted data
  and external-resource recovery as limitations.
- **Recommended change:** nonmutating snapshot validation with returned metadata,
  explicit operation results, staged restoration, and version migration hooks.
- **Acceptance:** a malformed or incompatible save produces a useful error
  without damaging a running game's scene or external resources.

## 10. Runtime/module state serialization

- **Evidence:** Step 4 can restore World data and selected script runtime fields,
  but not enumerate Appeal variables, pending events/timers, or module-owned
  animation, physics, audio, AI, and input state through one existing API.
- **MVP approach:** explicit Save_On_Save/Save_On_Load hooks and custom string
  values. Clear stale events/timers on load; rebuild resources and schedules
  through the load hook instead of serializing native pointers.
- **Recommended change:** explicit opt-in serializers for registered variables,
  timer schedules, and modules, with stable IDs and ownership/version contracts.
- **Acceptance:** a representative game resumes the needed animation, movement,
  abilities and timed actions without duplicating or reviving stale work.

## 11. Higher-level save data operations if script complexity warrants them

- **Evidence:** generic slot replacement currently needs temporary key/value
  and metadata tables plus static SQL/savepoint plumbing. This is hidden inside
  Game/Saves; game-facing use is a database, slot, and short progress hooks.
- **MVP approach:** retain the tested reusable script; use bound field values,
  avoid string-built SQL, and keep save I/O off the normal per-frame path.
- **Recommended change:** consider a compatible high-level Storage save-bundle
  API or script record/map facility after evaluating real-game requirements.
  No performance bottleneck has been established for this save path.
- **Acceptance:** equivalent atomic replacement/error handling with substantially
  less script plumbing; profile real saves before adding asynchronous/batch APIs.

## 12. Consistent numeric comparison semantics for rules

- **Evidence:** Appeal equality uses text/boolean normalization, while ordering
  uses Try_Float. Rule_Check deliberately reuses these existing semantics, so
  differently formatted numeric strings can compare unequal and ordering has
  the current float precision/validation limitations.
- **MVP approach:** consistent text/flag representations, valid numeric inputs,
  and documented condition groups; no alternate numeric parser in scripts.
- **Recommended change:** define explicit typed/strict numeric comparisons and
  evaluate double-precision ordering consistently with arithmetic, without
  silently changing existing string/boolean comparison behavior.
- **Acceptance:** numeric comparison handles equivalent representations and
  large/fractional values predictably; invalid inputs have useful diagnostics.

## 13. Structured event payload/context

- **Evidence:** Events Emit carries one string value. Multiple interactions can
  run before dispatch, so global last-interaction actor/action/reason values
  cannot safely describe every queued notification.
- **MVP approach:** queued interaction notices carry only a captured target;
  a synchronous result hook receives the full current context. No delimiter
  encoding or duplicate database registry is added.
- **Recommended change:** consider immutable structured payloads or per-event
  records with typed fields and clear lifetime/ownership.
- **Acceptance:** queued results preserve each request's actor, target, action
  and reason across multiple attempts, scene transitions and deferred delivery.

## 14. Audio groups and explicit playback lifetime

- **Evidence:** SDL Audio Set_Volume adjusts music/video gain rather than a
  separate SFX group; the current script interface lacks general stop/pause/
  unload-by-effect operations. Suppressing new effects does not stop active ones.
- **MVP approach:** short loaded-once cues, a script enable flag, and deliberate
  music stop/select. Do not claim sample-accurate pause/resume or scene unloading.
- **Recommended change:** explicit effects/music buses, per-instance/group
  stop/pause/fade/unload and returned playback/load status.
- **Acceptance:** group volume and scene cleanup affect the intended sounds
  without changing unrelated playback; errors and lifecycle states are queryable.

## 15. Unified 2D viewport clipping and draw ordering

- **Evidence:** float Draw_Rect/Paste_Rect can implement camera mapping without
  new native code, but fully off-screen culling does not clip partial objects.
  Sprite, rectangle and text rendering are separate native command passes.
- **MVP approach:** full-window camera viewports, explicit coordinate conversion,
  host-owned presentation and floating-point rectangle/sprite drawing.
- **Recommended change:** explicit 2D viewport/scissor and a predictable unified
  draw layer/order path if split-screen, embedded views or mixed layers require it.
- **Acceptance:** partial objects stay inside their viewport and mixed primitive/
  sprite/text layers preserve the requested order.

## 16. Animation clock, completion and markers

- **Evidence:** pose selection can reuse per-asset Graphics playback, but exposed
  commands do not provide an explicit gameplay-delta advance, per-instance pause,
  completion/marker events or hit-frame ownership contract.
- **MVP approach:** state-to-clip selection only, native clip FPS/mode and explicit
  gameplay flags. Do not derive deterministic combat timing from renderer time.
- **Recommended change:** explicit animation advancement/pause/time-scale,
  completion and markers with actor-local ownership and save/load support.
- **Acceptance:** pause/slow motion affect clip progress consistently and each
  marker fires once at the intended simulation point, including across save/load.

## Performance watch list — no bottleneck established yet

- Native timer collection currently scans every active timer each update.
  A deadline-based structure may help if actual game profiles show timer cost.
- Script dispatch and per-object module calls should be measured before adding
  batch C++ functions. No speculative replacement of simple gameplay scripts.
- Step 1's small sample was about 0.19 seconds for 1,000 updates with one event
  each, including process startup. This is not a large-scene performance result.

Add measured scene/object costs and new recommendations as implementation and
full games expose them. Keep utilities and packaging work on separate lists.
