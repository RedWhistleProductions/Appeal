#pragma once
#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"

#ifdef __linux__
    class Dynamic_Module
    {
    private:
        void *handle = 0;
        char *error = nullptr;
    public:
        bool Debug = false;
        Plugin Module_Plugin;
        void (*Init)(std::string Plugin_Name);
        void (*Init_Globals)(std::string Plugin_Name, Named_List<Module_Data> *Globals) = nullptr;
        void (*Set_Function_Runner)(void (*Run_Function)(std::string), void (*Run_Analog_Function)(std::string, int)) = nullptr;
        void (*Interpreter)(Data_Source *Data);
        void (*Shutdown)() = nullptr;
        void Load(std::string Module_Name);
        void Unload();
        template<typename T>void Assign(std::string Key, T &func);

        ~Dynamic_Module();
    };

    template<typename T>void Dynamic_Module::Assign(std::string Key, T &func)
    {
        void *Value = dlsym(handle, Key.c_str());
        if ((error = dlerror()) != NULL)  
        {
            if(Debug) std::cout << "Error: " << Key << " not found" << std::endl;
        } 
        else
        {
            func = reinterpret_cast<decltype(func)>(Value);
        }
    }
#endif

#ifdef _WIN32
    class Dynamic_Module
    {
    private:
        HINSTANCE handle = 0;
    public:
        bool Debug = false;
        Plugin Module_Plugin;
        void (*Init)(std::string Plugin_Name);
        void (*Init_Globals)(std::string Plugin_Name, Named_List<Module_Data> *Globals) = nullptr;
        void (*Set_Function_Runner)(void (*Run_Function)(std::string), void (*Run_Analog_Function)(std::string, int)) = nullptr;
        void (*Interpreter)(Data_Source *Data);
        void (*Shutdown)() = nullptr;
        void Load(std::string Module_Name);
        void Unload();
        template<typename T>void Assign(std::string Key, T &func);

        ~Dynamic_Module();
    };

    template<typename T>void Dynamic_Module::Assign(std::string Key, T &func)
    {
        func = reinterpret_cast<T>(GetProcAddress(handle, Key.c_str()));
    }


#endif
