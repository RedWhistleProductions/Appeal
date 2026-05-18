#include <iostream>
#include <map>
#include <string>

namespace
{
    struct Agent
    {
        std::string State = "Idle";
        std::string Target;
        std::map<std::string, std::string> Values;
    };

    bool Debug_Output = true;
    std::string Resources_Path = "./Resources";
    std::map<std::string, Agent> Agents;

    void Print(const std::string &Message)
    {
        if(Debug_Output) std::cout << Message << std::endl;
    }

    Agent &Get_Agent(const std::string &Entity)
    {
        return Agents[Entity];
    }
}

extern "C" void Constructor() { Print("Appeal_Recast_BehaviorTree_AI Constructor()"); }
extern "C" void Destructor() { Print("Appeal_Recast_BehaviorTree_AI Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }
extern "C" void Set_Paths(std::string Resources) { Resources_Path = Resources; Print("AI resources: " + Resources_Path); }

extern "C" void Enter(std::string Entity)
{
    Get_Agent(Entity);
    Print("AI enter: " + Entity);
}

extern "C" void Exit(std::string Entity)
{
    Print("AI exit: " + Entity);
}

extern "C" void Think(std::string Entity)
{
    Agent &Agent_Data = Get_Agent(Entity);
    Print("AI think: " + Entity + " state=" + Agent_Data.State);
}

extern "C" void Decide(std::string Entity)
{
    Agent &Agent_Data = Get_Agent(Entity);
    if(Agent_Data.Values.count("Threat") > 0) Agent_Data.State = "Engage";
    else if(!Agent_Data.Target.empty()) Agent_Data.State = "Navigate";
    else Agent_Data.State = "Idle";
    Print("AI decide: " + Entity + " -> " + Agent_Data.State);
}

extern "C" void Event(std::string Entity, std::string Event_Name)
{
    Agent &Agent_Data = Get_Agent(Entity);
    Agent_Data.Values["Event"] = Event_Name;
    if(Event_Name == "PlayerSeen") Agent_Data.Values["Threat"] = "Player";
    Print("AI event: " + Entity + "." + Event_Name);
}

extern "C" void Set(std::string Entity, std::string Key, std::string Value)
{
    Agent &Agent_Data = Get_Agent(Entity);
    if(Key == "State") Agent_Data.State = Value;
    else if(Key == "Target") Agent_Data.Target = Value;
    Agent_Data.Values[Key] = Value;
    Print("AI set: " + Entity + "." + Key + " = " + Value);
}
