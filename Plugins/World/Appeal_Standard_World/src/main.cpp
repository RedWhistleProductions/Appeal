#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    struct Vec3
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
    };

    struct Entity
    {
        std::string Name;
        std::string Tag;
        bool Active = true;
        Vec3 Position;
        Vec3 Rotation;
        Vec3 Scale = {1.0f, 1.0f, 1.0f};
        std::set<std::string> Components;
        std::map<std::string, std::string> Properties;
    };

    struct Scene
    {
        std::string Name;
        bool Paused = false;
        float Time_Scale = 1.0f;
        std::map<std::string, Entity> Entities;
        std::vector<std::string> Events;
    };

    bool Debug_Output = true;
    std::filesystem::path Resources_Path = "./Resources";
    std::filesystem::path World_Path = Resources_Path / "World";
    std::map<std::string, Scene> Scenes;
    std::string Current_Scene;
    std::string Active_Entity;

    void Print(const std::string &Message)
    {
        if(Debug_Output)
        {
            std::cout << Message << std::endl;
        }
    }

    void Ensure_Parent_Directory(const std::filesystem::path &Path)
    {
        auto Parent = Path.parent_path();
        if(!Parent.empty())
        {
            std::filesystem::create_directories(Parent);
        }
    }

    std::filesystem::path Scene_File(const std::string &Name)
    {
        std::filesystem::path Path(Name);
        if(Path.has_parent_path() || Path.has_extension())
        {
            return Path;
        }
        return World_Path / "Scenes" / (Name + ".world");
    }

    std::string Escape_Field(const std::string &Text)
    {
        std::string Result;
        for(char Character : Text)
        {
            if(Character == '\\' || Character == '\t' || Character == '\n' || Character == '\r')
            {
                Result += '\\';
                if(Character == '\t') Result += 't';
                else if(Character == '\n') Result += 'n';
                else if(Character == '\r') Result += 'r';
                else Result += Character;
            }
            else
            {
                Result += Character;
            }
        }
        return Result;
    }

    std::string Unescape_Field(const std::string &Text)
    {
        std::string Result;
        for(size_t i = 0; i < Text.size(); ++i)
        {
            if(Text[i] == '\\' && i + 1 < Text.size())
            {
                ++i;
                if(Text[i] == 't') Result += '\t';
                else if(Text[i] == 'n') Result += '\n';
                else if(Text[i] == 'r') Result += '\r';
                else Result += Text[i];
            }
            else
            {
                Result += Text[i];
            }
        }
        return Result;
    }

    Scene *Current()
    {
        if(Current_Scene.empty())
        {
            std::cerr << "World error: no scene is active." << std::endl;
            return nullptr;
        }

        auto Found = Scenes.find(Current_Scene);
        if(Found == Scenes.end())
        {
            std::cerr << "World error: active scene is missing." << std::endl;
            return nullptr;
        }
        return &Found->second;
    }

    Scene &Ensure_Scene(const std::string &Name)
    {
        auto [Found, Inserted] = Scenes.emplace(Name, Scene{});
        if(Inserted)
        {
            Found->second.Name = Name;
        }
        Current_Scene = Name;
        return Found->second;
    }

    Entity *Find_Entity(const std::string &Name)
    {
        Scene *Scene_Data = Current();
        if(Scene_Data == nullptr)
        {
            return nullptr;
        }

        auto Found = Scene_Data->Entities.find(Name);
        if(Found == Scene_Data->Entities.end())
        {
            std::cerr << "World error: entity not found: " << Name << std::endl;
            return nullptr;
        }
        return &Found->second;
    }

    Entity &Ensure_Entity(const std::string &Name)
    {
        Scene *Scene_Data = Current();
        if(Scene_Data == nullptr)
        {
            Ensure_Scene("Main");
            Scene_Data = Current();
        }

        auto [Found, Inserted] = Scene_Data->Entities.emplace(Name, Entity{});
        if(Inserted)
        {
            Found->second.Name = Name;
        }
        return Found->second;
    }

    void Set_Property(Entity &Entity_Data, const std::string &Key, const std::string &Value)
    {
        Entity_Data.Properties[Key] = Value;
    }

    std::vector<std::string> Split_Tab_Line(const std::string &Line)
    {
        std::vector<std::string> Fields;
        std::stringstream Parser(Line);
        std::string Field;
        while(std::getline(Parser, Field, '\t'))
        {
            Fields.push_back(Unescape_Field(Field));
        }
        return Fields;
    }
}

extern "C" void Constructor()
{
    Print("Appeal_Standard_World Constructor()");
}

extern "C" void Destructor()
{
    Scenes.clear();
    Current_Scene.clear();
    Active_Entity.clear();
    Print("Appeal_Standard_World Destructor()");
}

extern "C" void Debug(bool Enable)
{
    Debug_Output = Enable;
}

extern "C" void Set_Paths(std::string Resources)
{
    Resources_Path = Resources;
    World_Path = Resources_Path / "World";
    std::filesystem::create_directories(World_Path / "Scenes");
    Print("World path: " + World_Path.string());
}

extern "C" void Create(std::string Name)
{
    Ensure_Scene(Name);
    Print("World scene created: " + Name);
}

extern "C" void Open_Scene(std::string Name)
{
    Create(Name);
}

extern "C" void Switch(std::string Name)
{
    Ensure_Scene(Name);
    Print("World scene active: " + Name);
}

extern "C" void Use_Scene(std::string Name)
{
    Switch(Name);
}

extern "C" void Close_Scene(std::string Name)
{
    Scenes.erase(Name);
    if(Current_Scene == Name)
    {
        Current_Scene = Scenes.empty() ? "" : Scenes.begin()->first;
    }
    Print("World scene closed: " + Name);
}

extern "C" void Clear_Scene(std::string Name)
{
    Scene &Scene_Data = Ensure_Scene(Name);
    Scene_Data.Entities.clear();
    Scene_Data.Events.clear();
    Print("World scene cleared: " + Name);
}

extern "C" void Save(std::string Name)
{
    auto Found = Scenes.find(Name);
    if(Found == Scenes.end())
    {
        std::cerr << "World save failed, scene not found: " << Name << std::endl;
        return;
    }

    std::filesystem::path Path = Scene_File(Name);
    Ensure_Parent_Directory(Path);
    std::ofstream Output(Path);
    if(!Output.is_open())
    {
        std::cerr << "World save failed: " << Path.string() << std::endl;
        return;
    }

    const Scene &Scene_Data = Found->second;
    Output << "Scene\t" << Escape_Field(Scene_Data.Name) << '\t' << Scene_Data.Paused << '\t' << Scene_Data.Time_Scale << '\n';
    for(const auto &Entity_Pair : Scene_Data.Entities)
    {
        const Entity &Entity_Data = Entity_Pair.second;
        Output << "Entity\t" << Escape_Field(Entity_Data.Name) << '\t' << Escape_Field(Entity_Data.Tag) << '\t' << Entity_Data.Active << '\t'
               << Entity_Data.Position.X << '\t' << Entity_Data.Position.Y << '\t' << Entity_Data.Position.Z << '\t'
               << Entity_Data.Rotation.X << '\t' << Entity_Data.Rotation.Y << '\t' << Entity_Data.Rotation.Z << '\t'
               << Entity_Data.Scale.X << '\t' << Entity_Data.Scale.Y << '\t' << Entity_Data.Scale.Z << '\n';
        for(const std::string &Component : Entity_Data.Components)
        {
            Output << "Component\t" << Escape_Field(Entity_Data.Name) << '\t' << Escape_Field(Component) << '\n';
        }
        for(const auto &Property : Entity_Data.Properties)
        {
            Output << "Property\t" << Escape_Field(Entity_Data.Name) << '\t' << Escape_Field(Property.first) << '\t'
                   << Escape_Field(Property.second) << '\n';
        }
    }

    Print("World scene saved: " + Path.string());
}

extern "C" void Save_Scene(std::string Name)
{
    Save(Name);
}

extern "C" void Load(std::string Name)
{
    std::filesystem::path Path = Scene_File(Name);
    std::ifstream Input(Path);
    if(!Input.is_open())
    {
        std::cerr << "World load failed: " << Path.string() << std::endl;
        return;
    }

    Scene Scene_Data;
    Scene_Data.Name = Name;
    std::string Line;
    while(std::getline(Input, Line))
    {
        auto Fields = Split_Tab_Line(Line);
        if(Fields.empty())
        {
            continue;
        }

        if(Fields[0] == "Scene" && Fields.size() >= 4)
        {
            Scene_Data.Name = Fields[1];
            Scene_Data.Paused = Fields[2] != "0";
            Scene_Data.Time_Scale = std::stof(Fields[3]);
        }
        else if(Fields[0] == "Entity" && Fields.size() >= 13)
        {
            Entity Entity_Data;
            Entity_Data.Name = Fields[1];
            Entity_Data.Tag = Fields[2];
            Entity_Data.Active = Fields[3] != "0";
            Entity_Data.Position = {std::stof(Fields[4]), std::stof(Fields[5]), std::stof(Fields[6])};
            Entity_Data.Rotation = {std::stof(Fields[7]), std::stof(Fields[8]), std::stof(Fields[9])};
            Entity_Data.Scale = {std::stof(Fields[10]), std::stof(Fields[11]), std::stof(Fields[12])};
            Scene_Data.Entities[Entity_Data.Name] = Entity_Data;
        }
        else if(Fields[0] == "Component" && Fields.size() >= 3)
        {
            Scene_Data.Entities[Fields[1]].Name = Fields[1];
            Scene_Data.Entities[Fields[1]].Components.insert(Fields[2]);
        }
        else if(Fields[0] == "Property" && Fields.size() >= 4)
        {
            Scene_Data.Entities[Fields[1]].Name = Fields[1];
            Scene_Data.Entities[Fields[1]].Properties[Fields[2]] = Fields[3];
        }
    }

    Current_Scene = Scene_Data.Name;
    Scenes[Scene_Data.Name] = Scene_Data;
    Print("World scene loaded: " + Path.string());
}

extern "C" void Load_Scene(std::string Name)
{
    Load(Name);
}

extern "C" void Update(std::string Name)
{
    if(!Name.empty())
    {
        Switch(Name);
    }

    Scene *Scene_Data = Current();
    if(Scene_Data == nullptr)
    {
        return;
    }

    if(Scene_Data->Paused)
    {
        Print("World update skipped, scene paused: " + Scene_Data->Name);
        return;
    }

    Print("World updated: " + Scene_Data->Name);
}

extern "C" void Draw(std::string Name)
{
    if(!Name.empty())
    {
        Switch(Name);
    }
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Print("World draw request: " + Scene_Data->Name + " (" + std::to_string(Scene_Data->Entities.size()) + " entities)");
    }
}

extern "C" void Entity_Create(std::string Name)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Active_Entity = Name;
    Print("World entity created: " + Entity_Data.Name);
}

extern "C" void Create_Entity(std::string Name)
{
    Entity_Create(Name);
}

extern "C" void Entity_Destroy(std::string Name)
{
    Scene *Scene_Data = Current();
    if(Scene_Data == nullptr)
    {
        return;
    }
    Scene_Data->Entities.erase(Name);
    if(Active_Entity == Name)
    {
        Active_Entity.clear();
    }
    Print("World entity destroyed: " + Name);
}

extern "C" void Delete_Entity(std::string Name)
{
    Entity_Destroy(Name);
}

extern "C" void Clone_Entity(std::string Source, std::string Target)
{
    Entity *Source_Entity = Find_Entity(Source);
    if(Source_Entity == nullptr)
    {
        return;
    }

    Entity Copy = *Source_Entity;
    Copy.Name = Target;
    Current()->Entities[Target] = Copy;
    Print("World entity cloned: " + Source + " -> " + Target);
}

extern "C" void Entity_Set_Active(std::string Name, bool Active)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Active = Active;
    Print("World entity active: " + Name + " = " + std::to_string(Active));
}

extern "C" void Set_Active_Entity(std::string Name)
{
    Ensure_Entity(Name);
    Active_Entity = Name;
    Print("World active entity: " + Name);
}

extern "C" void Entity_Set_Tag(std::string Name, std::string Tag)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Tag = Tag;
    Print("World entity tag: " + Name + " = " + Tag);
}

extern "C" void Set_Position(std::string Name, float X, float Y, float Z)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Position = {X, Y, Z};
    Set_Property(Entity_Data, "Transform.X", std::to_string(X));
    Set_Property(Entity_Data, "Transform.Y", std::to_string(Y));
    Set_Property(Entity_Data, "Transform.Z", std::to_string(Z));
    Print("World position set: " + Name);
}

extern "C" void Set_Location(std::string Name, float X, float Y, float Z)
{
    Set_Position(Name, X, Y, Z);
}

extern "C" void Set_Rotation(std::string Name, float X, float Y, float Z)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Rotation = {X, Y, Z};
    Set_Property(Entity_Data, "Transform.RotationX", std::to_string(X));
    Set_Property(Entity_Data, "Transform.RotationY", std::to_string(Y));
    Set_Property(Entity_Data, "Transform.RotationZ", std::to_string(Z));
    Print("World rotation set: " + Name);
}

extern "C" void Set_Orientation(std::string Name, float X, float Y, float Z)
{
    Set_Rotation(Name, X, Y, Z);
}

extern "C" void Set_Scale(std::string Name, float X, float Y, float Z)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Scale = {X, Y, Z};
    Set_Property(Entity_Data, "Transform.ScaleX", std::to_string(X));
    Set_Property(Entity_Data, "Transform.ScaleY", std::to_string(Y));
    Set_Property(Entity_Data, "Transform.ScaleZ", std::to_string(Z));
    Print("World scale set: " + Name);
}

extern "C" void Move(std::string Name, float X, float Y, float Z)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Position.X += X;
    Entity_Data.Position.Y += Y;
    Entity_Data.Position.Z += Z;
    Set_Position(Name, Entity_Data.Position.X, Entity_Data.Position.Y, Entity_Data.Position.Z);
}

extern "C" void Rotate(std::string Name, float X, float Y, float Z)
{
    Entity &Entity_Data = Ensure_Entity(Name);
    Entity_Data.Rotation.X += X;
    Entity_Data.Rotation.Y += Y;
    Entity_Data.Rotation.Z += Z;
    Set_Rotation(Name, Entity_Data.Rotation.X, Entity_Data.Rotation.Y, Entity_Data.Rotation.Z);
}

extern "C" void Add_Component(std::string Entity_Name, std::string Component)
{
    Entity &Entity_Data = Ensure_Entity(Entity_Name);
    Entity_Data.Components.insert(Component);
    Print("World component added: " + Entity_Name + "." + Component);
}

extern "C" void Remove_Component(std::string Entity_Name, std::string Component)
{
    Entity &Entity_Data = Ensure_Entity(Entity_Name);
    Entity_Data.Components.erase(Component);
    Print("World component removed: " + Entity_Name + "." + Component);
}

extern "C" void Has_Component(std::string Entity_Name, std::string Component)
{
    Entity *Entity_Data = Find_Entity(Entity_Name);
    std::cout << (Entity_Data != nullptr && Entity_Data->Components.count(Component) > 0 ? "1" : "0") << std::endl;
}

extern "C" void List_Components(std::string Entity_Name)
{
    Entity *Entity_Data = Find_Entity(Entity_Name);
    if(Entity_Data == nullptr)
    {
        return;
    }

    std::cout << "World components for " << Entity_Name << ":" << std::endl;
    for(const std::string &Component : Entity_Data->Components)
    {
        std::cout << "  " << Component << std::endl;
    }
}

extern "C" void Set(std::string Entity_Name, std::string Key, std::string Value)
{
    Entity &Entity_Data = Ensure_Entity(Entity_Name);
    Set_Property(Entity_Data, Key, Value);
    Print("World property set: " + Entity_Name + "." + Key + " = " + Value);
}

extern "C" void Get(std::string Entity_Name, std::string Key)
{
    Entity *Entity_Data = Find_Entity(Entity_Name);
    if(Entity_Data == nullptr)
    {
        return;
    }

    auto Found = Entity_Data->Properties.find(Key);
    if(Found == Entity_Data->Properties.end())
    {
        Print("World property not found: " + Entity_Name + "." + Key);
        return;
    }

    std::cout << Found->second << std::endl;
}

extern "C" void List_Entities()
{
    Scene *Scene_Data = Current();
    if(Scene_Data == nullptr)
    {
        return;
    }

    std::cout << "World entities in " << Scene_Data->Name << ":" << std::endl;
    for(const auto &Pair : Scene_Data->Entities)
    {
        std::cout << "  " << Pair.first << " tag=" << Pair.second.Tag << " active=" << Pair.second.Active << std::endl;
    }
}

extern "C" void Query(std::string Tag)
{
    Scene *Scene_Data = Current();
    if(Scene_Data == nullptr)
    {
        return;
    }

    std::cout << "World query tag: " << Tag << std::endl;
    for(const auto &Pair : Scene_Data->Entities)
    {
        if(Pair.second.Tag == Tag)
        {
            std::cout << "  " << Pair.first << std::endl;
        }
    }
}

extern "C" void Pause()
{
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Scene_Data->Paused = true;
        Print("World paused: " + Scene_Data->Name);
    }
}

extern "C" void Resume()
{
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Scene_Data->Paused = false;
        Print("World resumed: " + Scene_Data->Name);
    }
}

extern "C" void Set_Time_Scale(float Scale)
{
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Scene_Data->Time_Scale = Scale;
        Print("World time scale: " + std::to_string(Scale));
    }
}

extern "C" void Emit_Event(std::string Event)
{
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Scene_Data->Events.push_back(Event);
        Print("World event: " + Event);
    }
}

extern "C" void Clear_Events()
{
    Scene *Scene_Data = Current();
    if(Scene_Data != nullptr)
    {
        Scene_Data->Events.clear();
        Print("World events cleared");
    }
}
