# Combat/damage/abilities — Step 14 MVP

Load Runtime then `Libraries/Game/Combat`. The host explicitly loads/stores
target state; no duplicate entity registry or World property getter is assumed.

Combat_Hit uses Target, Health, Armor, Damage, Active, Protected_Until and Grace_MS
(all prefixed Combat_). It rejects paused/stopped/inactive/dead/protected targets.
Accepted damage is max(Damage-max(Armor,0),0), capped at remaining health. Health
never becomes negative. Even an armor-blocked hit counts as accepted and refreshes
the configured grace period. Negative grace acts as 0. Combat_Accepted, Dealt,
Health, Protected_Until and Reason are outputs. Store updated health/protection
immediately before another hit against that target.

Combat_On_Hit is an optional synchronous feedback hook. It must not recursively
hit or modify the active Combat_ context. Nested Combat_Hit calls are ignored
while Combat_Busy=1. Combat_Resolve_Hit is internal. Queued Combat_Hit/Combat_Defeated events
carry only the target name. A dead target cannot emit repeated defeat events
unless the host incorrectly reloads stale health. The host owns target/team/range
validation and attack-to-target deduplication; a grace window isn't a per-attack
hit set. Native collision callbacks should queue hits, not destroy bodies while
their contact lists are being read.

Combat_Heal uses positive Heal_Amount, caps living Health to Max_Health, and respects
game pause. It does not revive dead/inactive targets automatically; spawning/
respawning is a game policy. Use finite, nonnegative health/armor values and a
positive Max_Health, matching existing arithmetic/comparison limits.

Ability_Try accepts Name, Enabled, Resource, Cost, Cooldown_MS and Ready_At (all
prefixed Ability_). It rejects invalid costs/cooldowns, insufficient resource,
cooldown and paused/stopped games. On acceptance it deducts cost, advances Ready_At
from Game_Time and queues Ability_Used with the name. The host validates targets
**before** spending resources, then spawns/applies an ability only if Accepted=1.
Persist resource/deadlines per actor/ability through save hooks. No projectile,
hitbox, network authority, status-effect stack or animation marker is implied.

Events use the existing immutable string payload boundary; synchronous hooks can
read full current context, delayed handlers cannot assume it hasn't changed.
All inputs are shared scratch state. Tests: `python Tests/Game_Combat_Tests.py`.
