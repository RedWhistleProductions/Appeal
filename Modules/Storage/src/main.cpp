#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
void (*Open)(std::string Name);
void (*Close)(std::string Name);
void (*Use)(std::string Name);
void (*Read)(std::string File);
void (*Write)(std::string Name, std::string File);
void (*Exists)(std::string File);
void (*Delete)(std::string File);
void (*Set)(std::string Key, std::string Value);
void (*Get)(std::string Key);
void (*Insert)(std::string Collection, std::string Key, std::string Value);
void (*Update)(std::string Collection, std::string Key, std::string Value);
void (*Find)(std::string Collection, std::string Key);
void (*List)(std::string Collection);
void (*Remove)(std::string Collection, std::string Key);
void (*Begin)();
void (*Commit)();
void (*Rollback)();
void (*Backup)(std::string File);
void (*Restore)(std::string File);
void (*Export)(std::string File);
void (*Import)(std::string File);
void (*Save)(std::string Name);
void (*Load)(std::string Name);

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
    Assign("Open", Open);
    Assign("Close", Close);
    Assign("Use", Use);
    Assign("Read", Read);
    Assign("Write", Write);
    Assign("Exists", Exists);
    Assign("Delete", Delete);
    Assign("Set", Set);
    Assign("Get", Get);
    Assign("Insert", Insert);
    Assign("Update", Update);
    Assign("Find", Find);
    Assign("List", List);
    Assign("Remove", Remove);
    Assign("Begin", Begin);
    Assign("Commit", Commit);
    Assign("Rollback", Rollback);
    Assign("Backup", Backup);
    Assign("Restore", Restore);
    Assign("Export", Export);
    Assign("Import", Import);
    Assign("Save", Save);
    Assign("Load", Load);
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
    else if(Command == "Open")
    {
        std::string Name;
        *Data >> Name;
        Open(Name);
    }
    else if(Command == "Close")
    {
        std::string Name;
        *Data >> Name;
        Close(Name);
    }
    else if(Command == "Use")
    {
        std::string Name;
        *Data >> Name;
        Use(Name);
    }
    else if(Command == "Read")
    {
        std::string File;
        *Data >> File;
        Read(File);
    }
    else if(Command == "Write")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Write(Name, File);
    }
    else if(Command == "Exists")
    {
        std::string File;
        *Data >> File;
        Exists(File);
    }
    else if(Command == "Delete")
    {
        std::string File;
        *Data >> File;
        Delete(File);
    }
    else if(Command == "Set")
    {
        std::string Key, Value;
        *Data >> Key;
        *Data >> Value;
        Set(Key, Value);
    }
    else if(Command == "Get")
    {
        std::string Key;
        *Data >> Key;
        Get(Key);
    }
    else if(Command == "Insert")
    {
        std::string Collection, Key, Value;
        *Data >> Collection;
        *Data >> Key;
        *Data >> Value;
        Insert(Collection, Key, Value);
    }
    else if(Command == "Update")
    {
        std::string Collection, Key, Value;
        *Data >> Collection;
        *Data >> Key;
        *Data >> Value;
        Update(Collection, Key, Value);
    }
    else if(Command == "Find")
    {
        std::string Collection, Key;
        *Data >> Collection;
        *Data >> Key;
        Find(Collection, Key);
    }
    else if(Command == "List")
    {
        std::string Collection;
        *Data >> Collection;
        List(Collection);
    }
    else if(Command == "Remove")
    {
        std::string Collection, Key;
        *Data >> Collection;
        *Data >> Key;
        Remove(Collection, Key);
    }
    else if(Command == "Begin")
    {
        Begin();
    }
    else if(Command == "Commit")
    {
        Commit();
    }
    else if(Command == "Rollback")
    {
        Rollback();
    }
    else if(Command == "Backup")
    {
        std::string File;
        *Data >> File;
        Backup(File);
    }
    else if(Command == "Restore")
    {
        std::string File;
        *Data >> File;
        Restore(File);
    }
    else if(Command == "Export")
    {
        std::string File;
        *Data >> File;
        Export(File);
    }
    else if(Command == "Import")
    {
        std::string File;
        *Data >> File;
        Import(File);
    }
    else if(Command == "Save")
    {
        std::string Name;
        *Data >> Name;
        Save(Name);
    }
    else if(Command == "Load")
    {
        std::string Name;
        *Data >> Name;
        Load(Name);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Storage Dictionary" << std::endl;
    }
}
