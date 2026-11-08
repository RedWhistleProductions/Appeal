# Interactions and rules — Step 5 MVP

`Game/Rules` evaluates reusable condition groups without any module dependency.
`Game/Interactions` connects those rules to synchronous game-defined effects and
queued success/denial notifications. Load Runtime, Rules, then Interactions once.
The libraries do not add a database, native functions, or a fixed gameplay model.

## A guarded action

```text
Run Libraries/Game/Runtime
Run Libraries/Game/Rules
Run Libraries/Game/Interactions
Set_Var Has_Key 0

Define_Function Check_Door
    Set_Var Rule_Left Var Has_Key
    Set_Var Rule_Operator ==
    Set_Var Rule_Right 1
    Set_Var Rule_Message "You need a key."
    Call Rule_Check
End_Function
Define_Function Open_Door
    // Apply game effects here using Interact_Current_Target.
End_Function
Set_Var Interact_Actor Player
Set_Var Interact_Target Door
Set_Var Interact_Action Open
Set_Var Interact_Check Check_Door
Set_Var Interact_Apply Open_Door
Call Game_Start
Call Interact_Try
Print Var Interact_OK
Print Var Interact_Reason
```

Call from a deliberate gameplay/event boundary, not a Graphics rendering callback.
For a menu button, queue a game event and let its handler configure/try the action.
World operations are optional: a rule can change a puzzle variable, score, quest
flag, or scene request as easily as an object's property.

## Rules

| Function | Behavior |
|---|---|
| Rule_Begin_All | Reset a group that succeeds only if every check matches. An empty All group succeeds. |
| Rule_Begin_Any | Reset a group that succeeds if at least one check matches. An empty Any group fails. |
| Rule_Check | Compare Rule_Left and Rule_Right using Rule_Operator. Add that result to the group and record failure feedback from Rule_Message. |
| Rule_End | Finalize Rule_Passed and Rule_Reason. Always call this before consuming a standalone group result. |

Supported operators: `==`, `!=`, `<`, `<=`, `>`, `>=`. Invalid operators make the
whole group fail with `Unsupported rule operator.`, including an Any group with
other successful checks. All groups retain the first failed condition's message;
failed Any groups retain the first alternative's message. A successful group has
an empty reason. Every supplied check is evaluated; this is not short-circuiting.

Comparison follows existing Appeal semantics: equality compares text with existing
boolean normalization, while ordering compares numeric values through the current
interpreter. Use consistent representations for equality (`5` and `5.0` are not
guaranteed equal); ordering requires valid numeric inputs. This library adds no
new numeric parser/precision behavior. These limitations are recorded for future
C++ work rather than silently changing the language.

Standalone use: begin a group, set the four input variables, call Rule_Check for
each condition, call Rule_End, then read Rule_Passed (0/1) and Rule_Reason.
There is one shared group, not nested named groups. Save a result into a game
variable if another group will be evaluated later.

## Interaction flow

Interact_Try snapshots actor/target/action into Interact_Current_Actor/Target/Action.
It requires Game_Running=1, Game_Paused=0, Interact_Enabled!=0, nonempty target/action,
and a configured apply hook. Actor may be empty for environmental actions.
Failed built-in gates report `Interaction unavailable.` without running check/effect.

It then begins an All group and calls Interact_Check, when configured. The check
hook supplies rules and must have no gameplay side effects. To use alternatives,
the hook can explicitly start an Any group instead. Rule_End runs automatically.
Only a passing group executes Interact_Apply, once per accepted attempt.

Interact_On_Result runs synchronously for every nonnested attempt, after the
effect when accepted. It can display feedback/read full current context and result.
Then the library queues Interaction_Succeeded or Interaction_Denied with **only
the target name** as Event_Value. Use the synchronous result hook when you need
the actor, action, or detailed reason. Do not read the last Interact_ variables in
a delayed event handler and assume they belong to that queued event.

| Configuration/input | Meaning |
|---|---|
| Interact_Actor / Interact_Target / Interact_Action | Request identity strings; initially empty. |
| Interact_Enabled | Global host gate, initially 1. |
| Interact_Check | Optional existing check function name; empty means no additional rules. |
| Interact_Apply | Required existing effect function name. |
| Interact_On_Result | Optional existing synchronous feedback function name. |
| Rule_Left / Rule_Operator / Rule_Right / Rule_Message | Current check inputs; set explicitly before each check. Default operator == and default message Condition not met. |

| Output/internal state | Meaning |
|---|---|
| Interact_OK / Interact_Reason | Last accepted-attempt result, 0/1 and denial text; a successful effect clears the reason. |
| Interact_Current_Actor/Target/Action | Captured active/last request. Hooks should use these and not modify them. |
| Interact_Busy / Interact_Reentered | Nested calls are ignored while busy and set Reentered=1. They do not emit another result or execute an effect. |
| Interact_Allowed | Internal gate flag. |
| Rule_Mode / Rule_Count / Rule_Passed / Rule_Reason | Shared condition group status. |
| Rule_Invalid / Rule_Matched / Rule_Valid_Operator | Internal evaluation flags. |

Set up the request and hooks before calling. Do not change library-owned context,
results or hooks from within a check/effect/result hook. Functions must exist:
the current interpreter can silently ignore a missing function name. A native or
hook error stops execution; there is no automatic rollback of partial effects.

## State, repeated actions, and persistence

Games own availability, range/target checks, item counts, locked/open flags,
cooldowns and actor permissions. Re-evaluate these in the check hook for every
attempt; do not rely on an earlier prompt having said an action was available.
The effect must update the relevant state, so a second pickup/open request can
be denied. There is no automatic inventory, collision, target search, or cooldown
scheduler in this component.

For a cooldown, store its next allowed Game_Time, check current time against it,
and advance the stored deadline in the effect. Game pause/time scale then apply
through the runtime clock. Persist those game-owned values and flags using
Save_On_Save/Save_On_Load when needed. The libraries do not automatically save
actor/target references or reconstruct interaction configuration after loading.

World Get/query return limitations still apply: use game-owned variables for
conditions until script-friendly World access is available. Effects can use
existing World create/destroy/set/activate commands and Scene_Request.

## Validation

`python Tests/Game_Interactions_Tests.py` checks All/Any groups, comparisons,
invalid operators, first-failure feedback, pause/stopped/disabled/invalid requests,
one effect per accepted attempt, reentrancy, event/result context, and a real
World pickup whose repeated collection is denied.

`Appeal.exe Examples/Game/Interactions` demonstrates a locked door, key pickup,
repeated-action denial and door opening using real World objects.

Production C++ is unchanged. Remaining native recommendations are in
[CPP_Recommendations.md](../../../Docs/CPP_Recommendations.md).
