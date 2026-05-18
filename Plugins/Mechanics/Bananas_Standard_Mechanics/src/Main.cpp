#include <iostream>
#include <map>
#include <string>

namespace
{
    struct Entity_State
    {
        std::string Mode = "OnFoot";
        std::map<std::string, std::string> Values;
    };

    bool Debug_Output = true;
    std::string Resources_Path = "./Resources";
    std::map<std::string, Entity_State> Entities;

    void Print(const std::string &Message)
    {
        if(Debug_Output) std::cout << Message << std::endl;
    }

    Entity_State &State(const std::string &Entity)
    {
        return Entities[Entity];
    }
}

extern "C" void Constructor() { Print("Bananas_Standard_Mechanics Constructor()"); }
extern "C" void Destructor() { Print("Bananas_Standard_Mechanics Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }
extern "C" void Set_Paths(std::string Resources) { Resources_Path = Resources; Print("Mechanics resources: " + Resources_Path); }

extern "C" void Enter(std::string Entity)
{
    State(Entity);
    Print("Mechanics enter: " + Entity);
}

extern "C" void Exit(std::string Entity)
{
    Print("Mechanics exit: " + Entity);
}

extern "C" void Update(std::string Entity)
{
    Entity_State &Entity_State_Data = State(Entity);
    Print("Mechanics update: " + Entity + " mode=" + Entity_State_Data.Mode);
}

extern "C" void Digital(std::string Entity, std::string Action)
{
    State(Entity).Values["Digital." + Action] = "1";
    Print("Mechanics digital: " + Entity + "." + Action);
}

extern "C" void Analog(std::string Entity, std::string Action, int Value)
{
    State(Entity).Values["Analog." + Action] = std::to_string(Value);
    Print("Mechanics analog: " + Entity + "." + Action + " = " + std::to_string(Value));
}

extern "C" void Set(std::string Entity, std::string Key, std::string Value)
{
    Entity_State &Entity_State_Data = State(Entity);
    if(Key == "Mode") Entity_State_Data.Mode = Value;
    Entity_State_Data.Values[Key] = Value;
    Print("Mechanics set: " + Entity + "." + Key + " = " + Value);
}
