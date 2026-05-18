#include <iostream>
extern "C" void Debug(bool Enable) { (void)Enable; }

#include <string>

extern "C" void Constructor() { std::cout << "Test_World Constructor()" << std::endl; }
extern "C" void Destructor() { std::cout << "Test_World Destructor()" << std::endl; }
extern "C" void Set_Paths(std::string Resources) { std::cout << "Set_Paths(" << Resources << ")" << std::endl; }
extern "C" void Create(std::string Name) { std::cout << "Create(" << Name << ")" << std::endl; }
extern "C" void Open_Scene(std::string Name) { std::cout << "Open_Scene(" << Name << ")" << std::endl; }
extern "C" void Load(std::string Name) { std::cout << "Load(" << Name << ")" << std::endl; }
extern "C" void Load_Scene(std::string Name) { std::cout << "Load_Scene(" << Name << ")" << std::endl; }
extern "C" void Save(std::string Name) { std::cout << "Save(" << Name << ")" << std::endl; }
extern "C" void Save_Scene(std::string Name) { std::cout << "Save_Scene(" << Name << ")" << std::endl; }
extern "C" void Switch(std::string Name) { std::cout << "Switch(" << Name << ")" << std::endl; }
extern "C" void Use_Scene(std::string Name) { std::cout << "Use_Scene(" << Name << ")" << std::endl; }
extern "C" void Close_Scene(std::string Name) { std::cout << "Close_Scene(" << Name << ")" << std::endl; }
extern "C" void Clear_Scene(std::string Name) { std::cout << "Clear_Scene(" << Name << ")" << std::endl; }
extern "C" void Update(std::string Name) { std::cout << "Update(" << Name << ")" << std::endl; }
extern "C" void Draw(std::string Name) { std::cout << "Draw(" << Name << ")" << std::endl; }
extern "C" void Entity_Create(std::string Name) { std::cout << "Entity_Create(" << Name << ")" << std::endl; }
extern "C" void Create_Entity(std::string Name) { std::cout << "Create_Entity(" << Name << ")" << std::endl; }
extern "C" void Entity_Destroy(std::string Name) { std::cout << "Entity_Destroy(" << Name << ")" << std::endl; }
extern "C" void Delete_Entity(std::string Name) { std::cout << "Delete_Entity(" << Name << ")" << std::endl; }
extern "C" void Clone_Entity(std::string Source, std::string Target) { std::cout << "Clone_Entity(" << Source << ", " << Target << ")" << std::endl; }
extern "C" void Entity_Set_Active(std::string Name, bool Active) { std::cout << "Entity_Set_Active(" << Name << ", " << Active << ")" << std::endl; }
extern "C" void Set_Active_Entity(std::string Name) { std::cout << "Set_Active_Entity(" << Name << ")" << std::endl; }
extern "C" void Entity_Set_Tag(std::string Name, std::string Tag) { std::cout << "Entity_Set_Tag(" << Name << ", " << Tag << ")" << std::endl; }
extern "C" void Set_Position(std::string Name, float X, float Y, float Z) { std::cout << "Set_Position(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Location(std::string Name, float X, float Y, float Z) { std::cout << "Set_Location(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Rotation(std::string Name, float X, float Y, float Z) { std::cout << "Set_Rotation(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Orientation(std::string Name, float X, float Y, float Z) { std::cout << "Set_Orientation(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Set_Scale(std::string Name, float X, float Y, float Z) { std::cout << "Set_Scale(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Move(std::string Name, float X, float Y, float Z) { std::cout << "Move(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Rotate(std::string Name, float X, float Y, float Z) { std::cout << "Rotate(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl; }
extern "C" void Add_Component(std::string Entity, std::string Component) { std::cout << "Add_Component(" << Entity << ", " << Component << ")" << std::endl; }
extern "C" void Remove_Component(std::string Entity, std::string Component) { std::cout << "Remove_Component(" << Entity << ", " << Component << ")" << std::endl; }
extern "C" void Has_Component(std::string Entity, std::string Component) { std::cout << "Has_Component(" << Entity << ", " << Component << ")" << std::endl; }
extern "C" void List_Components(std::string Entity) { std::cout << "List_Components(" << Entity << ")" << std::endl; }
extern "C" void Set(std::string Entity, std::string Key, std::string Value) { std::cout << "Set(" << Entity << ", " << Key << ", " << Value << ")" << std::endl; }
extern "C" void Get(std::string Entity, std::string Key) { std::cout << "Get(" << Entity << ", " << Key << ")" << std::endl; }
extern "C" void List_Entities() { std::cout << "List_Entities()" << std::endl; }
extern "C" void Query(std::string Tag) { std::cout << "Query(" << Tag << ")" << std::endl; }
extern "C" void Pause() { std::cout << "Pause()" << std::endl; }
extern "C" void Resume() { std::cout << "Resume()" << std::endl; }
extern "C" void Set_Time_Scale(float Scale) { std::cout << "Set_Time_Scale(" << Scale << ")" << std::endl; }
extern "C" void Emit_Event(std::string Event) { std::cout << "Emit_Event(" << Event << ")" << std::endl; }
extern "C" void Clear_Events() { std::cout << "Clear_Events()" << std::endl; }

