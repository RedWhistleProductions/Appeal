from game_script_test import run
setup='''Add_Dynamic_Module Storage Appeal_SQLite_Storage
Storage Open_Memory Inv_Test
Run Libraries/Game/Inventory
Set_Var Inventory_Database Inv_Test
Call Inventory_Init
Set_Var Inventory_Item Potion
Set_Var Inventory_Limit 5
Call Inventory_Define
Set_Var Inventory_Item Sword
Set_Var Inventory_Limit 1
Set_Var Inventory_Slot Hand
Call Inventory_Define
'''
run(setup+'''
Set_Var Inventory_Item Potion
Set_Var Inventory_Amount 3
Call Inventory_Add
Print Var Inventory_OK
Print Var Inventory_Quantity
Call Inventory_Add
Print Var Inventory_OK
Set_Var Inventory_Amount 1.5
Call Inventory_Take
Print Var Inventory_OK
Set_Var Inventory_Amount 2
Call Inventory_Take
Print Var Inventory_Quantity
Set_Var Inventory_Owner NPC
Call Inventory_Read
Print Var Inventory_Quantity
Set_Var Inventory_Item Missing
Call Inventory_Add
Print Var Inventory_OK
''',['1','3','0','0','1','0','0'])
run(setup+'''
Set_Var Inventory_Item Sword
Set_Var Inventory_Amount 1
Call Inventory_Equip
Print Var Inventory_OK
Call Inventory_Add
Call Inventory_Equip
Set_Var Inventory_Slot Hand
Print (Call Inventory_Equipped)
Set_Var Inventory_Snapshot (Call Inventory_Export)
Call Inventory_Take
Print (Call Inventory_Equipped)
Call Inventory_Import
Print Var Inventory_OK
Print (Call Inventory_Equipped)
Set_Var Inventory_Snapshot "{}"
Call Inventory_Import
Print Var Inventory_OK
Call Inventory_Read
Print Var Inventory_Quantity
Print (Call Inventory_Equipped)
Call Inventory_Unequip
Print (Call Inventory_Equipped)
''',['0','Sword','','1','Sword','0','1','Sword',''])
run(setup+'''
Add_Dynamic_Module World Appeal_Standard_World
Run Libraries/Game/Runtime
Run Libraries/Game/Scenes
Run Libraries/Game/Saves
Set_Var Save_Database Inv_Test
Call Save_Init
Define_Function Collect
    Set_Var Save_Key Inventory
    Set_Var Save_Value (Call Inventory_Export)
    Call Save_Put
End_Function
Define_Function Restore
    Set_Var Save_Key Inventory
    Set_Var Inventory_Snapshot (Call Save_Get)
    Call Inventory_Import
End_Function
Set_Var Save_On_Save Collect
Set_Var Save_On_Load Restore
Call Game_Start
Set_Var Scene_Next Room
Call Scene_Request
Call Scene_Apply
Set_Var Inventory_Item Potion
Set_Var Inventory_Amount 4
Call Inventory_Add
Set_Var Save_Slot One
Call Save_To_Slot
Call Inventory_Take
Call Save_Load_Slot
Call Inventory_Read
Print Var Inventory_Quantity
Print Var Inventory_OK
''',['4','1'])
run(setup+'''
Set_Var Inventory_Item Potion
Set_Var Inventory_Amount 5
Call Inventory_Add
Set_Var Inventory_Snapshot (Call Inventory_Export)
Set_Var Inventory_Amount 1
Call Inventory_Set
Set_Var Inventory_Limit 2
Set_Var Inventory_Slot ""
Call Inventory_Define
Call Inventory_Import
Print Var Inventory_OK
Call Inventory_Read
Print Var Inventory_Quantity
Set_Var Inventory_Snapshot "not JSON"
Call Inventory_Import
Print Var Inventory_OK
Call Inventory_Read
Print Var Inventory_Quantity
''',['0','1','0','1'])
print('GAME_INVENTORY_TESTS_PASSED')
