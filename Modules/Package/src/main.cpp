#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
void (*Add_Repo)(std::string Name, std::string URL);
void (*Remove_Repo)(std::string Name);
void (*Update_Repos)();
void (*Search)(std::string Query);
void (*Install)(std::string Name);
void (*Remove)(std::string Name);
void (*Upgrade)();
void (*List)();
void (*Info)(std::string Name);
void (*Verify)(std::string Name);
void (*Load_Module)(std::string Name);
void (*Unload_Module)(std::string Name);
void (*Load_Plugin)(std::string Module, std::string Plugin);
void (*Unload_Plugin)(std::string Module);

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
    Assign("Add_Repo", Add_Repo);
    Assign("Remove_Repo", Remove_Repo);
    Assign("Update_Repos", Update_Repos);
    Assign("Search", Search);
    Assign("Install", Install);
    Assign("Remove", Remove);
    Assign("Upgrade", Upgrade);
    Assign("List", List);
    Assign("Info", Info);
    Assign("Verify", Verify);
    Assign("Load_Module", Load_Module);
    Assign("Unload_Module", Unload_Module);
    Assign("Load_Plugin", Load_Plugin);
    Assign("Unload_Plugin", Unload_Plugin);
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
    else if(Command == "Add_Repo")
    {
        std::string Name, URL;
        *Data >> Name;
        *Data >> URL;
        Add_Repo(Name, URL);
    }
    else if(Command == "Remove_Repo")
    {
        std::string Name;
        *Data >> Name;
        Remove_Repo(Name);
    }
    else if(Command == "Update_Repos")
    {
        Update_Repos();
    }
    else if(Command == "Search")
    {
        std::string Query;
        *Data >> Query;
        Search(Query);
    }
    else if(Command == "Install")
    {
        std::string Name;
        *Data >> Name;
        Install(Name);
    }
    else if(Command == "Remove")
    {
        std::string Name;
        *Data >> Name;
        Remove(Name);
    }
    else if(Command == "Upgrade")
    {
        Upgrade();
    }
    else if(Command == "List")
    {
        List();
    }
    else if(Command == "Info")
    {
        std::string Name;
        *Data >> Name;
        Info(Name);
    }
    else if(Command == "Verify")
    {
        std::string Name;
        *Data >> Name;
        Verify(Name);
    }
    else if(Command == "Load_Module")
    {
        std::string Name;
        *Data >> Name;
        Load_Module(Name);
    }
    else if(Command == "Unload_Module")
    {
        std::string Name;
        *Data >> Name;
        Unload_Module(Name);
    }
    else if(Command == "Load_Plugin")
    {
        std::string Module_Name, Plugin_Name;
        *Data >> Module_Name;
        *Data >> Plugin_Name;
        Load_Plugin(Module_Name, Plugin_Name);
    }
    else if(Command == "Unload_Plugin")
    {
        std::string Module_Name;
        *Data >> Module_Name;
        Unload_Plugin(Module_Name);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Package Dictionary" << std::endl;
    }
}
