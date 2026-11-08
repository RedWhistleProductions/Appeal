from game_script_test import run
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Combat
Call Game_Start
Set_Var Combat_Target Enemy
Set_Var Combat_Armor 5
Set_Var Combat_Damage 40
Set_Var Combat_Grace_MS 100
Call Combat_Hit
Print Var Combat_Health
Call Combat_Hit
Print Var Combat_Accepted
Set_Var Game_Elapsed 100
Call Game_Advance
Call Combat_Hit
Print Var Combat_Health
Call Game_Advance
Call Combat_Hit
Print Var Combat_Health
Call Combat_Hit
Print Var Combat_Accepted
Set_Var Combat_Heal_Amount 100
Call Combat_Heal
Print Var Combat_Health
Set_Var Combat_Health 50
Call Combat_Heal
Print Var Combat_Health
Call Game_Pause
Call Combat_Hit
Print Var Combat_Health
''',['65','0','30','0','0','0','100','100'])
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Combat
Call Game_Start
Set_Var Ability_Name Fireball
Set_Var Ability_Cost 30
Set_Var Ability_Cooldown_MS 100
Call Ability_Try
Print Var Ability_Accepted
Print Var Ability_Resource
Call Ability_Try
Print Var Ability_Accepted
Set_Var Game_Elapsed 100
Call Game_Advance
Call Ability_Try
Print Var Ability_Resource
Set_Var Ability_Cost 50
Call Game_Advance
Call Ability_Try
Print Var Ability_Accepted
Print Var Ability_Resource
''',['1','70','0','40','0','40'])
run('''Run Libraries/Game/Runtime
Run Libraries/Game/Combat
Call Game_Start
Set_Var Defeats 0
Define_Function Died
    Set_Var Defeats (Add Var Defeats 1)
End_Function
Define_Function Nested
    Call Combat_Hit
End_Function
Events Bind Combat_Defeated Died
Set_Var Combat_On_Hit Nested
Set_Var Combat_Target Enemy
Set_Var Combat_Damage 1000
Call Combat_Hit
Call Combat_Hit
Call Game_Dispatch_Events
Print Var Defeats
Set_Var Combat_Health 10
Set_Var Combat_Armor 100
Set_Var Combat_Damage 5
Call Combat_Hit
Print Var Combat_Dealt
Print Var Combat_Health
''',['1','0','10'])
print('GAME_COMBAT_TESTS_PASSED')
