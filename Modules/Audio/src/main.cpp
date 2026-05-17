#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"


    Plugin Module;
    bool Module_Debug = true;

    void (*Back)();
    void (*Debug)(bool Enable);
    void (*Load_Play_List)(std::string Play_List);
    void (*Load_SFX)(std::string Name, std::string SFX_File);
    void (*Load_Song)(std::string Name, std::string Music_File);
    void (*Mute)();
    void (*Next)();
    void (*Play)();
    void (*Play_SFX)();
    void (*Play_SFX_Name)(std::string Name);
    void (*Play_Song)(std::string Name);
    void (*Play_Track)(int Track);
    void (*Set_Paths)(std::string Resources);
    void (*Stop)();
    void (*UnMute)();
    void (*Update)();
    void (*Set_Volume)(int Volume);
    int (*Get_Volume)();
    void (*Set_Volume_Percent)(int Percent);
    int (*Get_Volume_Percent)();
    void (*Set_Current_Loop)(int Loop);
    bool (*Current_Looping)();
    void (*Set_Auto_Play)(int Auto_Play);
    bool (*Auto_Playing)();
    std::string (*Current_Song)();
    bool (*Is_Muted)();

    Named_List<Module_Data> *Globals = nullptr;

    void Set_Global(std::string Name, std::string Type, std::string Value)
    {
        if(Globals == nullptr) return;

        std::string Full_Name = "Audio." + Name;
        Module_Data Data;
        Data.Module_Name = "Audio";
        Data.Name = Name;
        Data.Type = Type;
        Data.Value = Value;

        if(Globals->Find(Full_Name))
        {
            Globals->Current->Value = Data;
        }
        else
        {
            Globals->Add_Node(Full_Name, Data);
        }
    }

    void Update_Audio_Globals()
    {
        if(Current_Song != nullptr) Set_Global("Current_Song", "string", Current_Song());
        if(Get_Volume != nullptr) Set_Global("Volume", "int", std::to_string(Get_Volume()));
        if(Get_Volume_Percent != nullptr) Set_Global("Volume_Percent", "int", std::to_string(Get_Volume_Percent()));
        if(Current_Looping != nullptr) Set_Global("Current_Looping", "bool", Current_Looping() ? "1" : "0");
        if(Auto_Playing != nullptr) Set_Global("Auto_Playing", "bool", Auto_Playing() ? "1" : "0");
        if(Is_Muted != nullptr) Set_Global("Muted", "bool", Is_Muted() ? "1" : "0");
    }

 extern "C" void Init_Globals(std::string Name, Named_List<Module_Data> *Shared_Globals)
    {
        Globals = Shared_Globals;
        Module.Load(Name);
        Module.Assign("Back", Back);
        Module.Assign("Debug", Debug);
        Module.Assign("Load_Play_List", Load_Play_List);
        Module.Assign("Load_SFX", Load_SFX);
        Module.Assign("Load_Song", Load_Song);
        Module.Assign("Mute", Mute);
        Module.Assign("Next", Next);
        Module.Assign("Play", Play);
        Module.Assign("Play_SFX", Play_SFX);
        Module.Assign("Play_SFX_Name", Play_SFX_Name);
        Module.Assign("Play_Song", Play_Song);
        Module.Assign("Play_Track", Play_Track);             
        Module.Assign("Set_Paths", Set_Paths);
        Module.Assign("Stop", Stop);
        Module.Assign("UnMute", UnMute);
        Module.Assign("Update", Update);
        Module.Assign("Set_Volume", Set_Volume);
        Module.Assign("Get_Volume", Get_Volume);
        Module.Assign("Set_Volume_Percent", Set_Volume_Percent);
        Module.Assign("Get_Volume_Percent", Get_Volume_Percent);
        Module.Assign("Set_Current_Loop", Set_Current_Loop);
        Module.Assign("Current_Looping", Current_Looping);
        Module.Assign("Set_Auto_Play", Set_Auto_Play);
        Module.Assign("Auto_Playing", Auto_Playing);
        Module.Assign("Current_Song", Current_Song);
        Module.Assign("Is_Muted", Is_Muted);
        Update_Audio_Globals();
    }

 extern "C" void Init(std::string Name)
    {
        Init_Globals(Name, nullptr);
    }

extern "C" void Audio_Back() { Back(); }
extern "C" void Audio_Next() { Next(); }
extern "C" void Audio_Play() { Play(); }
extern "C" void Audio_Stop() { Stop(); }
extern "C" void Audio_Mute() { Mute(); }
extern "C" void Audio_UnMute() { UnMute(); }
extern "C" void Audio_Update() { Update(); }
extern "C" void Audio_Set_Volume(int Volume) { Set_Volume(Volume); }
extern "C" int Audio_Get_Volume() { return Get_Volume(); }
extern "C" void Audio_Set_Volume_Percent(int Percent) { Set_Volume_Percent(Percent); }
extern "C" int Audio_Get_Volume_Percent() { return Get_Volume_Percent(); }
extern "C" void Audio_Set_Current_Loop(int Loop) { Set_Current_Loop(Loop); }
extern "C" bool Audio_Current_Looping() { return Current_Looping(); }
extern "C" void Audio_Set_Auto_Play(int Auto_Play) { Set_Auto_Play(Auto_Play); }
extern "C" bool Audio_Auto_Playing() { return Auto_Playing(); }
extern "C" std::string Audio_Current_Song() { return Current_Song(); }
extern "C" bool Audio_Is_Muted() { return Is_Muted(); }

extern "C" void Shutdown()
{
    if(Stop != nullptr)
    {
        Stop();
    }
    Module.Unload();
    Globals = nullptr;
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

        else if(Command == "Back")
        {
            Back();
            Update_Audio_Globals();
        }

        else if(Command == "Load_Play_List")
        {
            *Data >> Command;
            Load_Play_List(Command);
            Update_Audio_Globals();
        }

        else if(Command == "Load_SFX")
        {
            std::string Name;
            std::string SFX_File;
            *Data >> Name;
            *Data >> SFX_File;
            Load_SFX(Name, SFX_File);
        }

        else if(Command == "Load_Song")
        {
            std::string Name;
            std::string Music_File;
            *Data >> Name;
            *Data >> Music_File;
            Load_Song(Name, Music_File);
            Update_Audio_Globals();
        }

        else if(Command == "Mute")
        {
            Mute();
            Update_Audio_Globals();
        }

        else if(Command == "Next")
        {
            Next();
            Update_Audio_Globals();
        }

        else if(Command == "Play")
        {
            Play();
            Update_Audio_Globals();
        }

        else if(Command == "Play_SFX")
        {
            Play_SFX();
        }

        else if(Command == "Play_SFX_Name")
        {
            std::string Name;
            *Data >> Name;
            Play_SFX_Name(Name);
        }

        else if(Command == "Play_Song")
        {
            std::string Name;
            *Data >> Name;
            Play_Song(Name);
            Update_Audio_Globals();
        }

        else if(Command == "Play_Track")
        {
            int Track;
            *Data >> Track;
            Play_Track(Track);
            Update_Audio_Globals();
        }

        else if(Command == "Set_Paths")
        {
            std::string Resources;
            *Data >> Resources;
            Set_Paths(Resources);
        }

        else if(Command == "Stop")
        {
            Stop();
            Update_Audio_Globals();
        }

        else if(Command == "UnMute")
        {
            UnMute();
            Update_Audio_Globals();
        }
        else if(Command == "Update")
        {
            Update();
            Update_Audio_Globals();
        }
        else if(Command == "Toggle_Mute")
        {
            if(Is_Muted != nullptr && Is_Muted())
            {
                UnMute();
            }
            else
            {
                Mute();
            }
            Update_Audio_Globals();
        }
        else if(Command == "Set_Volume")
        {
            int Volume;
            *Data >> Volume;
            Set_Volume(Volume);
            Update_Audio_Globals();
        }
        else if(Command == "Set_Volume_Percent")
        {
            int Percent;
            *Data >> Percent;
            Set_Volume_Percent(Percent);
            Update_Audio_Globals();
        }
        else if(Command == "Set_Current_Loop")
        {
            int Loop;
            *Data >> Loop;
            Set_Current_Loop(Loop);
            Update_Audio_Globals();
        }
        else if(Command == "Set_Auto_Play")
        {
            int Auto_Play;
            *Data >> Auto_Play;
            Set_Auto_Play(Auto_Play);
            Update_Audio_Globals();
        }
        else if(Command == "Get_Volume")
        {
            if(Module_Debug) std::cout << Get_Volume() << std::endl;
        }
        else if(Command == "Get_Volume_Percent")
        {
            if(Module_Debug) std::cout << Get_Volume_Percent() << std::endl;
        }
        else if(Command == "Current_Looping")
        {
            if(Module_Debug) std::cout << Current_Looping() << std::endl;
        }
        else if(Command == "Auto_Playing")
        {
            if(Module_Debug) std::cout << Auto_Playing() << std::endl;
        }
        else if(Command == "Current_Song")
        {
            if(Module_Debug) std::cout << Current_Song() << std::endl;
        }
        else if(Command == "Is_Muted")
        {
            if(Module_Debug) std::cout << Is_Muted() << std::endl;
        }

        else
        {
          if(Module_Debug) std::cout << "\tError: "  + Command + " not found in Audio Dictionary" << std::endl;
        } 
    }   
