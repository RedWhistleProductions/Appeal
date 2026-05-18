#include <iostream>
#include <string>

bool Debug_Output = true;

void Print(const std::string &Message)
{
    if(Debug_Output)
    {
        std::cout << Message << std::endl;
    }
}

extern "C" void Constructor() { Print("Test_Mechanics Constructor()"); }
extern "C" void Destructor() { Print("Test_Mechanics Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }
extern "C" void Set_Paths(std::string Resources) { Print("Set_Paths(" + Resources + ")"); }
extern "C" void Enter(std::string Entity) { Print("Enter(" + Entity + ")"); }
extern "C" void Exit(std::string Entity) { Print("Exit(" + Entity + ")"); }
extern "C" void Update(std::string Entity) { Print("Update(" + Entity + ")"); }
extern "C" void Digital(std::string Entity, std::string Action) { Print("Digital(" + Entity + ", " + Action + ")"); }
extern "C" void Analog(std::string Entity, std::string Action, int Value) { Print("Analog(" + Entity + ", " + Action + ", " + std::to_string(Value) + ")"); }
extern "C" void Set(std::string Entity, std::string Key, std::string Value) { Print("Set(" + Entity + ", " + Key + ", " + Value + ")"); }
