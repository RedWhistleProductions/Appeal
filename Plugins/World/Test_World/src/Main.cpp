#include <iostream>
#include <string>

extern "C" void Constructor() { std::cout << "Test_World Constructor()" << std::endl; }
extern "C" void Destructor() { std::cout << "Test_World Destructor()" << std::endl; }
extern "C" void Set_Paths(std::string Resources) { std::cout << "Set_Paths(" << Resources << ")" << std::endl; }
extern "C" void Create(std::string Name) { std::cout << "Create(" << Name << ")" << std::endl; }
extern "C" void Load(std::string Name) { std::cout << "Load(" << Name << ")" << std::endl; }
extern "C" void Save(std::string Name) { std::cout << "Save(" << Name << ")" << std::endl; }
extern "C" void Switch(std::string Name) { std::cout << "Switch(" << Name << ")" << std::endl; }
extern "C" void Update(std::string Name) { std::cout << "Update(" << Name << ")" << std::endl; }
extern "C" void Draw(std::string Name) { std::cout << "Draw(" << Name << ")" << std::endl; }
extern "C" void Entity_Create(std::string Name) { std::cout << "Entity_Create(" << Name << ")" << std::endl; }
extern "C" void Entity_Destroy(std::string Name) { std::cout << "Entity_Destroy(" << Name << ")" << std::endl; }
extern "C" void Entity_Set_Active(std::string Name, bool Active) { std::cout << "Entity_Set_Active(" << Name << ", " << Active << ")" << std::endl; }
extern "C" void Entity_Set_Tag(std::string Name, std::string Tag) { std::cout << "Entity_Set_Tag(" << Name << ", " << Tag << ")" << std::endl; }
extern "C" void Set_Position(std::string Name, float X, float Y, float Z) { std::cout << "Set_Position(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Rotation(std::string Name, float X, float Y, float Z) { std::cout << "Set_Rotation(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Scale(std::string Name, float X, float Y, float Z) { std::cout << "Set_Scale(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Move(std::string Name, float X, float Y, float Z) { std::cout << "Move(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Rotate(std::string Name, float X, float Y, float Z) { std::cout << "Rotate(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Add_Component(std::string Entity, std::string Component) { std::cout << "Add_Component(" << Entity << ", " << Component << ")" << std::endl; }
extern "C" void Remove_Component(std::string Entity, std::string Component) { std::cout << "Remove_Component(" << Entity << ", " << Component << ")" << std::endl; }
extern "C" void Query(std::string Tag) { std::cout << "Query(" << Tag << ")" << std::endl; }
