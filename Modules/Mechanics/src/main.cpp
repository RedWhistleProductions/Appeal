#include "Plugin.h"
#include "Data_Source.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace
{
    struct Mechanics_Functions
    {
        void (*Debug)(bool Enable) = nullptr;
        void (*Set_Paths)(std::string Resources) = nullptr;
        void (*Enter)(std::string Entity) = nullptr;
        void (*Exit)(std::string Entity) = nullptr;
        void (*Update)(std::string Entity) = nullptr;
        void (*Digital)(std::string Entity, std::string Action) = nullptr;
        void (*Analog)(std::string Entity, std::string Action, int Value) = nullptr;
        void (*Set)(std::string Entity, std::string Key, std::string Value) = nullptr;
    };

    struct Plugin_Slot
    {
        std::string Name;
        std::string File;
        std::unique_ptr<Plugin> Loaded_Plugin;
        Mechanics_Functions Functions;
        bool Should_Load = false;
        bool Is_Loaded = false;
        int Users = 0;
    };

    bool Module_Debug = true;
    std::string Resources_Path = "./Resources";
    std::string Load_Mode = "Manual";
    std::map<std::string, Plugin_Slot> Plugins;
    std::map<std::string, std::string> Entity_Modes;

    template<typename T>
    void Assign(Plugin_Slot &Slot, std::string Name, T &Function)
    {
        Slot.Loaded_Plugin->Assign(Name, Function);
    }

    void Print(const std::string &Message)
    {
        if(Module_Debug)
        {
            std::cout << Message << std::endl;
        }
    }

    Plugin_Slot *Find_Slot(const std::string &Name)
    {
        auto Found = Plugins.find(Name);
        if(Found == Plugins.end())
        {
            if(Module_Debug) std::cerr << "Mechanics plugin slot not found: " << Name << std::endl;
            return nullptr;
        }
        return &Found->second;
    }

    bool Load_Slot(Plugin_Slot &Slot)
    {
        if(Slot.Is_Loaded)
        {
            return true;
        }

        if(Slot.File.empty())
        {
            if(Module_Debug) std::cerr << "Mechanics plugin slot has no file: " << Slot.Name << std::endl;
            return false;
        }

        Slot.Loaded_Plugin = std::make_unique<Plugin>();
        Slot.Loaded_Plugin->Load(Slot.File);
        Assign(Slot, "Debug", Slot.Functions.Debug);
        Assign(Slot, "Set_Paths", Slot.Functions.Set_Paths);
        Assign(Slot, "Enter", Slot.Functions.Enter);
        Assign(Slot, "Exit", Slot.Functions.Exit);
        Assign(Slot, "Update", Slot.Functions.Update);
        Assign(Slot, "Digital", Slot.Functions.Digital);
        Assign(Slot, "Analog", Slot.Functions.Analog);
        Assign(Slot, "Set", Slot.Functions.Set);

        if(Slot.Functions.Debug != nullptr) Slot.Functions.Debug(Module_Debug);
        if(Slot.Functions.Set_Paths != nullptr) Slot.Functions.Set_Paths(Resources_Path);

        Slot.Is_Loaded = true;
        Print("Mechanics loaded plugin: " + Slot.Name);
        return true;
    }

    void Unload_Slot(Plugin_Slot &Slot, bool Force = false)
    {
        if(!Slot.Is_Loaded)
        {
            return;
        }

        if(!Force && Slot.Users > 0)
        {
            Print("Mechanics plugin still in use: " + Slot.Name);
            return;
        }

        Slot.Functions = Mechanics_Functions{};
        Slot.Loaded_Plugin.reset();
        Slot.Is_Loaded = false;
        Print("Mechanics unloaded plugin: " + Slot.Name);
    }

    void Apply_Plan()
    {
        for(auto &Pair : Plugins)
        {
            Plugin_Slot &Slot = Pair.second;
            bool Wanted = Slot.Should_Load;
            if(Load_Mode == "All")
            {
                Wanted = true;
            }
            else if(Load_Mode == "On_Demand")
            {
                Wanted = Slot.Users > 0;
            }

            if(Wanted)
            {
                Load_Slot(Slot);
            }
            else
            {
                Unload_Slot(Slot);
            }
        }
    }
}

extern "C" void Init(std::string File)
{
    Plugin_Slot &Slot = Plugins["Default"];
    Slot.Name = "Default";
    Slot.File = File;
    Slot.Should_Load = true;
    Load_Slot(Slot);
}

extern "C" void Interpreter(Data_Source *Data)
{
    std::string Command;
    *Data >> Command;

    if(Command == "Init")
    {
        std::string File;
        *Data >> File;
        Init(File);
    }
    else if(Command == "Debug")
    {
        int Enable;
        *Data >> Enable;
        Module_Debug = Enable != 0;
        for(auto &Pair : Plugins)
        {
            if(Pair.second.Functions.Debug != nullptr)
            {
                Pair.second.Functions.Debug(Module_Debug);
            }
        }
    }
    else if(Command == "Set_Paths")
    {
        *Data >> Resources_Path;
        for(auto &Pair : Plugins)
        {
            if(Pair.second.Functions.Set_Paths != nullptr)
            {
                Pair.second.Functions.Set_Paths(Resources_Path);
            }
        }
    }
    else if(Command == "Add_Plugin")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Plugin_Slot &Slot = Plugins[Name];
        Slot.Name = Name;
        Slot.File = File;
        Print("Mechanics plugin added: " + Name + " -> " + File);
    }
    else if(Command == "Remove_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Unload_Slot(*Slot, true);
            Plugins.erase(Name);
        }
    }
    else if(Command == "Load_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Slot->Should_Load = true;
            Load_Slot(*Slot);
        }
    }
    else if(Command == "Unload_Plugin")
    {
        std::string Name;
        *Data >> Name;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Slot->Should_Load = false;
            Unload_Slot(*Slot);
        }
    }
    else if(Command == "Set_Plugin_Wanted")
    {
        std::string Name;
        int Wanted;
        *Data >> Name;
        *Data >> Wanted;
        Plugin_Slot *Slot = Find_Slot(Name);
        if(Slot != nullptr)
        {
            Slot->Should_Load = Wanted != 0;
        }
    }
    else if(Command == "Apply_Load_Plan")
    {
        Apply_Plan();
    }
    else if(Command == "Set_Load_Mode")
    {
        *Data >> Load_Mode;
        Print("Mechanics load mode: " + Load_Mode);
        Apply_Plan();
    }
    else if(Command == "List_Plugins")
    {
        std::cout << "Mechanics plugins:" << std::endl;
        for(const auto &Pair : Plugins)
        {
            const Plugin_Slot &Slot = Pair.second;
            std::cout << "  " << Slot.Name
                      << " loaded=" << Slot.Is_Loaded
                      << " wanted=" << Slot.Should_Load
                      << " users=" << Slot.Users
                      << " file=" << Slot.File << std::endl;
        }
    }
    else if(Command == "Use")
    {
        std::string Entity, Plugin_Name;
        *Data >> Entity;
        *Data >> Plugin_Name;

        Plugin_Slot *New_Slot = Find_Slot(Plugin_Name);
        if(New_Slot == nullptr)
        {
            return;
        }
        Load_Slot(*New_Slot);

        auto Existing_Mode = Entity_Modes.find(Entity);
        if(Existing_Mode != Entity_Modes.end())
        {
            Plugin_Slot *Old_Slot = Find_Slot(Existing_Mode->second);
            if(Old_Slot != nullptr)
            {
                if(Old_Slot->Functions.Exit != nullptr) Old_Slot->Functions.Exit(Entity);
                if(Old_Slot->Users > 0) --Old_Slot->Users;
            }
        }

        Entity_Modes[Entity] = Plugin_Name;
        ++New_Slot->Users;
        if(New_Slot->Functions.Enter != nullptr) New_Slot->Functions.Enter(Entity);
        if(Load_Mode == "On_Demand") Apply_Plan();
    }
    else if(Command == "Release")
    {
        std::string Entity;
        *Data >> Entity;
        auto Existing_Mode = Entity_Modes.find(Entity);
        if(Existing_Mode != Entity_Modes.end())
        {
            Plugin_Slot *Slot = Find_Slot(Existing_Mode->second);
            if(Slot != nullptr)
            {
                if(Slot->Functions.Exit != nullptr) Slot->Functions.Exit(Entity);
                if(Slot->Users > 0) --Slot->Users;
            }
            Entity_Modes.erase(Existing_Mode);
            if(Load_Mode == "On_Demand") Apply_Plan();
        }
    }
    else if(Command == "Update")
    {
        std::string Entity;
        *Data >> Entity;
        if(Entity == "All")
        {
            for(const auto &Pair : Entity_Modes)
            {
                Plugin_Slot *Slot = Find_Slot(Pair.second);
                if(Slot != nullptr && Slot->Functions.Update != nullptr)
                {
                    Slot->Functions.Update(Pair.first);
                }
            }
        }
        else
        {
            auto Mode = Entity_Modes.find(Entity);
            if(Mode != Entity_Modes.end())
            {
                Plugin_Slot *Slot = Find_Slot(Mode->second);
                if(Slot != nullptr && Slot->Functions.Update != nullptr)
                {
                    Slot->Functions.Update(Entity);
                }
            }
        }
    }
    else if(Command == "Digital")
    {
        std::string Entity, Action;
        *Data >> Entity;
        *Data >> Action;
        auto Mode = Entity_Modes.find(Entity);
        if(Mode != Entity_Modes.end())
        {
            Plugin_Slot *Slot = Find_Slot(Mode->second);
            if(Slot != nullptr && Slot->Functions.Digital != nullptr)
            {
                Slot->Functions.Digital(Entity, Action);
            }
        }
    }
    else if(Command == "Analog")
    {
        std::string Entity, Action;
        int Value;
        *Data >> Entity;
        *Data >> Action;
        *Data >> Value;
        auto Mode = Entity_Modes.find(Entity);
        if(Mode != Entity_Modes.end())
        {
            Plugin_Slot *Slot = Find_Slot(Mode->second);
            if(Slot != nullptr && Slot->Functions.Analog != nullptr)
            {
                Slot->Functions.Analog(Entity, Action, Value);
            }
        }
    }
    else if(Command == "Set")
    {
        std::string Entity, Key, Value;
        *Data >> Entity;
        *Data >> Key;
        *Data >> Value;
        auto Mode = Entity_Modes.find(Entity);
        if(Mode != Entity_Modes.end())
        {
            Plugin_Slot *Slot = Find_Slot(Mode->second);
            if(Slot != nullptr && Slot->Functions.Set != nullptr)
            {
                Slot->Functions.Set(Entity, Key, Value);
            }
        }
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Mechanics Dictionary" << std::endl;
    }
}
