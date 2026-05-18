#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
void (*Create)(std::string Name);
void (*Open_Scene)(std::string Name);
void (*Load)(std::string Name);
void (*Load_Scene)(std::string Name);
void (*Save)(std::string Name);
void (*Save_Scene)(std::string Name);
void (*Switch)(std::string Name);
void (*Use_Scene)(std::string Name);
void (*Close_Scene)(std::string Name);
void (*Clear_Scene)(std::string Name);
void (*Update)(std::string Name);
void (*Draw)(std::string Name);
void (*Entity_Create)(std::string Name);
void (*Create_Entity)(std::string Name);
void (*Entity_Destroy)(std::string Name);
void (*Delete_Entity)(std::string Name);
void (*Clone_Entity)(std::string Source, std::string Target);
void (*Entity_Set_Active)(std::string Name, bool Active);
void (*Set_Active_Entity)(std::string Name);
void (*Entity_Set_Tag)(std::string Name, std::string Tag);
void (*Set_Position)(std::string Name, float X, float Y, float Z);
void (*Set_Location)(std::string Name, float X, float Y, float Z);
void (*Set_Rotation)(std::string Name, float X, float Y, float Z);
void (*Set_Orientation)(std::string Name, float X, float Y, float Z);
void (*Set_Scale)(std::string Name, float X, float Y, float Z);
void (*Move)(std::string Name, float X, float Y, float Z);
void (*Rotate)(std::string Name, float X, float Y, float Z);
void (*Add_Component)(std::string Entity, std::string Component);
void (*Remove_Component)(std::string Entity, std::string Component);
void (*Has_Component)(std::string Entity, std::string Component);
void (*List_Components)(std::string Entity);
void (*Set)(std::string Entity, std::string Key, std::string Value);
void (*Get)(std::string Entity, std::string Key);
void (*List_Entities)();
void (*Query)(std::string Tag);
void (*Pause)();
void (*Resume)();
void (*Set_Time_Scale)(float Scale);
void (*Emit_Event)(std::string Event);
void (*Clear_Events)();

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
    Assign("Open_Scene", Open_Scene);
    Assign("Load", Load);
    Assign("Load_Scene", Load_Scene);
    Assign("Save", Save);
    Assign("Save_Scene", Save_Scene);
    Assign("Switch", Switch);
    Assign("Use_Scene", Use_Scene);
    Assign("Close_Scene", Close_Scene);
    Assign("Clear_Scene", Clear_Scene);
    Assign("Update", Update);
    Assign("Draw", Draw);
    Assign("Entity_Create", Entity_Create);
    Assign("Create_Entity", Create_Entity);
    Assign("Entity_Destroy", Entity_Destroy);
    Assign("Delete_Entity", Delete_Entity);
    Assign("Clone_Entity", Clone_Entity);
    Assign("Entity_Set_Active", Entity_Set_Active);
    Assign("Set_Active_Entity", Set_Active_Entity);
    Assign("Entity_Set_Tag", Entity_Set_Tag);
    Assign("Set_Position", Set_Position);
    Assign("Set_Location", Set_Location);
    Assign("Set_Rotation", Set_Rotation);
    Assign("Set_Orientation", Set_Orientation);
    Assign("Set_Scale", Set_Scale);
    Assign("Move", Move);
    Assign("Rotate", Rotate);
    Assign("Add_Component", Add_Component);
    Assign("Remove_Component", Remove_Component);
    Assign("Has_Component", Has_Component);
    Assign("List_Components", List_Components);
    Assign("Set", Set);
    Assign("Get", Get);
    Assign("List_Entities", List_Entities);
    Assign("Query", Query);
    Assign("Pause", Pause);
    Assign("Resume", Resume);
    Assign("Set_Time_Scale", Set_Time_Scale);
    Assign("Emit_Event", Emit_Event);
    Assign("Clear_Events", Clear_Events);
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
    else if(Command == "Open_Scene")
    {
        std::string Name;
        *Data >> Name;
        Open_Scene(Name);
    }
    else if(Command == "Load")
    {
        std::string Name;
        *Data >> Name;
        Load(Name);
    }
    else if(Command == "Load_Scene")
    {
        std::string Name;
        *Data >> Name;
        Load_Scene(Name);
    }
    else if(Command == "Save")
    {
        std::string Name;
        *Data >> Name;
        Save(Name);
    }
    else if(Command == "Save_Scene")
    {
        std::string Name;
        *Data >> Name;
        Save_Scene(Name);
    }
    else if(Command == "Switch")
    {
        std::string Name;
        *Data >> Name;
        Switch(Name);
    }
    else if(Command == "Use_Scene")
    {
        std::string Name;
        *Data >> Name;
        Use_Scene(Name);
    }
    else if(Command == "Close_Scene")
    {
        std::string Name;
        *Data >> Name;
        Close_Scene(Name);
    }
    else if(Command == "Clear_Scene")
    {
        std::string Name;
        *Data >> Name;
        Clear_Scene(Name);
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
    else if(Command == "Create_Entity")
    {
        std::string Name;
        *Data >> Name;
        Create_Entity(Name);
    }
    else if(Command == "Entity_Destroy")
    {
        std::string Name;
        *Data >> Name;
        Entity_Destroy(Name);
    }
    else if(Command == "Delete_Entity")
    {
        std::string Name;
        *Data >> Name;
        Delete_Entity(Name);
    }
    else if(Command == "Clone_Entity")
    {
        std::string Source, Target;
        *Data >> Source;
        *Data >> Target;
        Clone_Entity(Source, Target);
    }
    else if(Command == "Entity_Set_Active")
    {
        std::string Name;
        bool Active;
        *Data >> Name;
        *Data >> Active;
        Entity_Set_Active(Name, Active);
    }
    else if(Command == "Set_Active_Entity")
    {
        std::string Name;
        *Data >> Name;
        Set_Active_Entity(Name);
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
    else if(Command == "Set_Location")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Location(Name, X, Y, Z);
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
    else if(Command == "Set_Orientation")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Orientation(Name, X, Y, Z);
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
    else if(Command == "Has_Component")
    {
        std::string Entity, Component;
        *Data >> Entity;
        *Data >> Component;
        Has_Component(Entity, Component);
    }
    else if(Command == "List_Components")
    {
        std::string Entity;
        *Data >> Entity;
        List_Components(Entity);
    }
    else if(Command == "Set")
    {
        std::string Entity, Key, Value;
        *Data >> Entity;
        *Data >> Key;
        *Data >> Value;
        Set(Entity, Key, Value);
    }
    else if(Command == "Get")
    {
        std::string Entity, Key;
        *Data >> Entity;
        *Data >> Key;
        Get(Entity, Key);
    }
    else if(Command == "List_Entities")
    {
        List_Entities();
    }
    else if(Command == "Query")
    {
        std::string Tag;
        *Data >> Tag;
        Query(Tag);
    }
    else if(Command == "Pause")
    {
        Pause();
    }
    else if(Command == "Resume")
    {
        Resume();
    }
    else if(Command == "Set_Time_Scale")
    {
        float Scale;
        *Data >> Scale;
        Set_Time_Scale(Scale);
    }
    else if(Command == "Emit_Event")
    {
        std::string Event;
        *Data >> Event;
        Emit_Event(Event);
    }
    else if(Command == "Clear_Events")
    {
        Clear_Events();
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in World Dictionary" << std::endl;
    }
}
