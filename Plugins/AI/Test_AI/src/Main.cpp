#include <iostream>
#include <string>

bool Debug_Output = true;

void Print(const std::string &Message)
{
    if(Debug_Output) std::cout << Message << std::endl;
}

extern "C" void Constructor() { Print("Test_AI Constructor()"); }
extern "C" void Destructor() { Print("Test_AI Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }
extern "C" void Set_Paths(std::string Resources) { Print("Set_Paths(" + Resources + ")"); }
extern "C" void Enter(std::string Entity) { Print("Enter(" + Entity + ")"); }
extern "C" void Exit(std::string Entity) { Print("Exit(" + Entity + ")"); }
extern "C" void Think(std::string Entity) { Print("Think(" + Entity + ")"); }
extern "C" void Decide(std::string Entity) { Print("Decide(" + Entity + ")"); }
extern "C" void Event(std::string Entity, std::string Event_Name) { Print("Event(" + Entity + ", " + Event_Name + ")"); }
extern "C" void Set(std::string Entity, std::string Key, std::string Value) { Print("Set(" + Entity + ", " + Key + ", " + Value + ")"); }
