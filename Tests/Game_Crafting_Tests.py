from game_script_test import run
setup='''Add_Dynamic_Module Storage Appeal_SQLite_Storage
Run Libraries/Game/Runtime
Run Libraries/Game/Inventory
Run Libraries/Game/Crafting
Storage Open_Memory Craft_Test
Set_Var Inventory_Database Craft_Test
Call Inventory_Init
Call Craft_Init
Set_Var Inventory_Item Wood
Set_Var Inventory_Limit 10
Call Inventory_Define
Set_Var Inventory_Amount 4
Call Inventory_Add
Set_Var Inventory_Item Coin
Call Inventory_Define
Set_Var Inventory_Amount 2
Call Inventory_Add
Set_Var Inventory_Item Sword
Set_Var Inventory_Limit 1
Set_Var Inventory_Slot Hand
Call Inventory_Define
Set_Var Craft_Recipe Forge
Set_Var Craft_Output_Item Sword
Call Craft_Define
Set_Var Craft_Ingredient_Item Wood
Set_Var Craft_Ingredient_Quantity 2
Call Craft_Ingredient
Set_Var Craft_Ingredient_Item Coin
Set_Var Craft_Ingredient_Quantity 1
Call Craft_Ingredient
'''
run(setup+'''
Call Craft_Run
Print Var Craft_OK
Call Craft_Run
Print Var Craft_OK
Set_Var Inventory_Item Wood
Call Inventory_Read
Print Var Inventory_Quantity
Set_Var Inventory_Item Sword
Set_Var Inventory_Amount 1
Call Inventory_Take
Call Craft_Run
Print Var Craft_OK
Set_Var Inventory_Item Wood
Call Inventory_Read
Print Var Inventory_Quantity
Set_Var Inventory_Owner NPC
Call Craft_Run
Print Var Craft_OK
''',['1','0','2','1','0','0'])
run(setup+'''
Storage Execute_SQL "CREATE TEMP TRIGGER Craft_Fail BEFORE INSERT ON Inv_Stacks BEGIN SELECT RAISE(ABORT,'craft write failure'); END;"
Call Craft_Run
Print Var Craft_OK
Print Var Craft_Status
Set_Var Inventory_Item Wood
Call Inventory_Read
Print Var Inventory_Quantity
Set_Var Inventory_Item Coin
Call Inventory_Read
Print Var Inventory_Quantity
''',['0','craft write failure','4','2'])
run(setup+'''
Set_Var Craft_Recipe Recycle
Set_Var Craft_Output_Item Wood
Set_Var Craft_Output_Quantity 3
Call Craft_Define
Set_Var Craft_Ingredient_Item Wood
Set_Var Craft_Ingredient_Quantity 2
Call Craft_Ingredient
Call Craft_Run
Set_Var Inventory_Item Wood
Call Inventory_Read
Print Var Inventory_Quantity
''',['5'])
print('GAME_CRAFTING_TESTS_PASSED')
