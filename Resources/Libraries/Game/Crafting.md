# Economy/crafting resource foundation — Step 18 MVP

Load Runtime, Inventory and `Libraries/Game/Crafting`; initialize Inventory then
Craft_Init on the same connection. It extends Inventory's TEMP schema deliberately,
so keep these library versions together. Definitions are content, not save data.

Set Craft_Recipe, Craft_Output_Item and positive integer Craft_Output_Quantity,
then Craft_Define. For each ingredient set Craft_Ingredient_Item/Quantity and call
Craft_Ingredient. It replaces that recipe/item requirement, not appends duplicates.
Use integer definition quantities 1..1,000,000 and known item definitions. Recipe
names and values are bound; no game-authored string SQL is needed.

Set Inventory_Owner/Craft_Recipe, then Craft_Read for Craft_Ready. Craft_Run
rechecks all ingredient counts/output capacity and atomically consumes inputs,
unequips exhausted items and adds the output. Craft_OK/Status report the result.
If output is also an ingredient, capacity is checked against the final quantity.
Failed/disabled crafts leave resources alone; native write errors roll back.
Craft_Completed queues the recipe name only, not a mutable owner context.

Currency can be an ordinary inventory item used as a recipe cost. Building
placement can first validate grid/occupancy/rules, then craft/spend and place the
authored World object on success. Cross-module object placement isn't part of
the SQLite transaction: validate first and design game-specific recovery for
external resource failures. This is not a full construction/economy simulator.

Crafting doesn't impose pause/UI permission: game handlers must validate actor,
unlock/bench/range/turn state through Rules/Interactions before Craft_Run. Calls
are action-driven, not per-frame. Inventory snapshots persist resulting stacks;
recipe content is loaded again before restoring inventory. Shared Craft_/Inventory_
inputs are not reentrant context. Internal SQL plumbing remains hidden and is a
candidate for the deferred record/high-level Storage improvements.

Test: `python Tests/Game_Crafting_Tests.py` covers multiple ingredients, ownership,
capacity, self-consuming output and failure rollback.
