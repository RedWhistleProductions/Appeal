#include <iostream>
#include <string>

bool Debug_Output = true;

void Print(const std::string &Message)
{
    if(Debug_Output) std::cout << Message << std::endl;
}

std::string Vec(float X, float Y, float Z)
{
    return std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z);
}

extern "C" void Constructor() { Print("Test_Physics Constructor()"); }
extern "C" void Destructor() { Print("Test_Physics Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }
extern "C" void Set_Paths(std::string Resources) { Print("Set_Paths(" + Resources + ")"); }
extern "C" void Attach(std::string Entity) { Print("Attach(" + Entity + ")"); }
extern "C" void Detach(std::string Entity) { Print("Detach(" + Entity + ")"); }
extern "C" void Update(std::string Entity) { Print("Update(" + Entity + ")"); }
extern "C" void Set(std::string Entity, std::string Key, std::string Value) { Print("Set(" + Entity + ", " + Key + ", " + Value + ")"); }
extern "C" void Set_Body_Type(std::string Entity, std::string Type) { Print("Set_Body_Type(" + Entity + ", " + Type + ")"); }
extern "C" void Set_Position(std::string Entity, float X, float Y, float Z) { Print("Set_Position(" + Entity + ", " + Vec(X, Y, Z) + ")"); }
extern "C" void Set_Velocity(std::string Entity, float X, float Y, float Z) { Print("Set_Velocity(" + Entity + ", " + Vec(X, Y, Z) + ")"); }
extern "C" void Apply_Force(std::string Entity, float X, float Y, float Z) { Print("Apply_Force(" + Entity + ", " + Vec(X, Y, Z) + ")"); }
extern "C" void Apply_Impulse(std::string Entity, float X, float Y, float Z) { Print("Apply_Impulse(" + Entity + ", " + Vec(X, Y, Z) + ")"); }
extern "C" void Set_Gravity(float X, float Y, float Z) { Print("Set_Gravity(" + Vec(X, Y, Z) + ")"); }
