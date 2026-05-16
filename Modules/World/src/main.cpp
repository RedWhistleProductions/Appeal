#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
void (*Create)(std::string Name);
void (*Load)(std::string Name);
void (*Save)(std::string Name);
void (*Switch)(std::string Name);
void (*Update)(std::string Name);
void (*Draw)(std::string Name);
void (*Entity_Create)(std::string Name);
void (*Entity_Destroy)(std::string Name);
void (*Entity_Set_Active)(std::string Name, bool Active);
void (*Entity_Set_Tag)(std::string Name, std::string Tag);
void (*Set_Position)(std::string Name, float X, float Y, float Z);
void (*Set_Rotation)(std::string Name, float X, float Y, float Z);
void (*Set_Scale)(std::string Name, float X, float Y, float Z);
void (*Move)(std::string Name, float X, float Y, float Z);
void (*Rotate)(std::string Name, float X, float Y, float Z);
void (*Add_Component)(std::string Entity, std::string Component);
void (*Remove_Component)(std::string Entity, std::string Component);
void (*Query)(std::string Tag);

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init(std::string Name)
{
    Module.Load(Name);
    Assign("Debug", Debug);
    Assign("Set_Paths", Set_Paths);
    Assign("Create", Create);
    Assign("Load", Load);
    Assign("Save", Save);
    Assign("Switch", Switch);
    Assign("Update", Update);
    Assign("Draw", Draw);
    Assign("Entity_Create", Entity_Create);
    Assign("Entity_Destroy", Entity_Destroy);
    Assign("Entity_Set_Active", Entity_Set_Active);
    Assign("Entity_Set_Tag", Entity_Set_Tag);
    Assign("Set_Position", Set_Position);
    Assign("Set_Rotation", Set_Rotation);
    Assign("Set_Scale", Set_Scale);
    Assign("Move", Move);
    Assign("Rotate", Rotate);
    Assign("Add_Component", Add_Component);
    Assign("Remove_Component", Remove_Component);
    Assign("Query", Query);
}

extern "C" void Interpreter(Data_Source *Data)
{
    std::string Command;
    *Data >> Command;

    if(Command == "Init")
    {
        std::string Name;
        *Data >> Name;
        Init(Name);
    }
    else if(Command == "Debug")
    {
        int Enable;
        *Data >> Enable;
        Module_Debug = Enable != 0;
        if(Debug != nullptr) Debug(Module_Debug);
    }
    else if(Command == "Set_Paths")
    {
        std::string Resources;
        *Data >> Resources;
        Set_Paths(Resources);
    }
    else if(Command == "Create")
    {
        std::string Name;
        *Data >> Name;
        Create(Name);
    }
    else if(Command == "Load")
    {
        std::string Name;
        *Data >> Name;
        Load(Name);
    }
    else if(Command == "Save")
    {
        std::string Name;
        *Data >> Name;
        Save(Name);
    }
    else if(Command == "Switch")
    {
        std::string Name;
        *Data >> Name;
        Switch(Name);
    }
    else if(Command == "Update")
    {
        std::string Name;
        *Data >> Name;
        Update(Name);
    }
    else if(Command == "Draw")
    {
        std::string Name;
        *Data >> Name;
        Draw(Name);
    }
    else if(Command == "Entity_Create")
    {
        std::string Name;
        *Data >> Name;
        Entity_Create(Name);
    }
    else if(Command == "Entity_Destroy")
    {
        std::string Name;
        *Data >> Name;
        Entity_Destroy(Name);
    }
    else if(Command == "Entity_Set_Active")
    {
        std::string Name;
        bool Active;
        *Data >> Name;
        *Data >> Active;
        Entity_Set_Active(Name, Active);
    }
    else if(Command == "Entity_Set_Tag")
    {
        std::string Name, Tag;
        *Data >> Name;
        *Data >> Tag;
        Entity_Set_Tag(Name, Tag);
    }
    else if(Command == "Set_Position")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Position(Name, X, Y, Z);
    }
    else if(Command == "Set_Rotation")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Rotation(Name, X, Y, Z);
    }
    else if(Command == "Set_Scale")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Scale(Name, X, Y, Z);
    }
    else if(Command == "Move")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Move(Name, X, Y, Z);
    }
    else if(Command == "Rotate")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Rotate(Name, X, Y, Z);
    }
    else if(Command == "Add_Component")
    {
        std::string Entity, Component;
        *Data >> Entity;
        *Data >> Component;
        Add_Component(Entity, Component);
    }
    else if(Command == "Remove_Component")
    {
        std::string Entity, Component;
        *Data >> Entity;
        *Data >> Component;
        Remove_Component(Entity, Component);
    }
    else if(Command == "Query")
    {
        std::string Tag;
        *Data >> Tag;
        Query(Tag);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in World Dictionary" << std::endl;
    }
}
