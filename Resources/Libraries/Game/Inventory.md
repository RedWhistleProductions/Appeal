# Inventory/equipment — Step 13 MVP

Load Storage and `Libraries/Game/Inventory`, set Inventory_Database to an open or
named connection and call Inventory_Init. Runtime data lives in connection-local
SQLite TEMP tables, not in every frame's save file. Reuse the save-game connection
or a host-owned memory connection; keep it open and do not change it during play.
No new module/plugin is required. The SQLite JSON functions are used by snapshots.

Define each item once: set Inventory_Item, positive integer Inventory_Limit,
Inventory_Slot (empty for none), then Inventory_Define. Counts/limits are bounded
to 1,000,000 in this MVP to stay within exact integer comparison range. Do not lower a definition's
limit beneath existing stacks or change slots while equipped. Names are case
sensitive; use nonempty owner/item/slot identities. Definitions are game content
and must be loaded before importing saved inventory.

- Set Inventory_Owner (default Player), Inventory_Item and nonnegative integer
  Inventory_Amount, then call Inventory_Add, Inventory_Take or Inventory_Set.
  Limits/insufficient quantity/fractional counts reject through Inventory_OK=0.
  Zero is a valid no-op amount for Add/Take and clears a stack through Set.
- Inventory_Read returns Quantity, Stack_Limit, Item_Slot and Known (all with the
  Inventory_ prefix). Unknown items return Known=0 and quantity/limit=0.
- Inventory_Equip requires ownership and a configured item slot. One item per
  owner/slot; another equip replaces it. Equipping does not consume quantity.
- Set Inventory_Slot and call `(Call Inventory_Equipped)` to return its item or
  empty. Inventory_Unequip clears it. Taking the last copy automatically unequips.
- `(Call Inventory_Export)` returns a versioned JSON string containing stacks and
  equipment for all owners. Set Inventory_Snapshot and call Inventory_Import to
  restore it. Import stages/validates known definitions, limits, ownership and
  equipment slots, then replaces data atomically. Invalid snapshots preserve live
  inventory and report Inventory_OK=0/Inventory_Status.

Save hook pattern: Save_Key=Inventory, Save_Value=(Call Inventory_Export), Save_Put.
Load hook: Inventory_Snapshot=(Call Save_Get), Inventory_Import. Check import status
in a game needing recovery; Save_Load_Slot doesn't automatically interpret a custom
hook's Inventory_OK. Empty/missing custom data should follow the game's migration
policy, not be imported blindly.

The host validates text/numeric inputs, coordinates transfers and item effects
through Interactions, and applies equipment stats through combat/game rules.
Arbitrary malformed numeric text can still trigger the interpreter's arithmetic
error. Definitions/low-level database errors can stop execution. No automatic
weight limit, trade atomicity between owners, durability, equipment bonuses or UI
is imposed. Public helpers select Inventory_Database and leave it selected.

Inventory_ inputs/outputs are shared scratch state. Inv_ tables/views and
Inventory_Write/Change/Check_Amount are internal. Don't edit their schema/context
or wrap library calls in a host transaction. SQLite work is action-driven; avoid
querying an entire inventory each render frame without profiling/caching.

The internal record/snapshot SQL is more involved than the game-facing calls;
a future record/map or high-level Storage API is already recommended in the C++
backlog. Test: `python Tests/Game_Inventory_Tests.py` covers counts, caps, owner
isolation, equipment, invalid-import rollback and integration with save slots.
