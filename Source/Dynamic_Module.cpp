#include "Dynamic_Module.h"

#ifdef __linux__
void Dynamic_Module::Load(std::string Module_Name)
    {
        if(handle != 0){
            Unload();
        }
        

        handle = dlopen (Module_Name.c_str(), RTLD_LAZY);
        if (!handle) 
        {
            fputs (dlerror(), stderr);
            exit(1);
        }
        else
        {
            if(Debug) std::cout << "Loaded: " << Module_Name << std::endl;
            Assign("Init", Init);
            Assign("Init_Globals", Init_Globals);
            Assign("Set_Function_Runner", Set_Function_Runner);
            Assign("Interpreter", Interpreter);
            Assign("Shutdown", Shutdown);
        }
        
        
    }

    void Dynamic_Module::Unload()
    {
        if(handle != 0)
        {
            if(Shutdown != nullptr) Shutdown();
            dlclose(handle);
            handle = 0;
        }
        Init = nullptr;
        Init_Globals = nullptr;
        Set_Function_Runner = nullptr;
        Interpreter = nullptr;
        Shutdown = nullptr;
    }

    Dynamic_Module::~Dynamic_Module()
    {
        Unload();
    }
#endif

#ifdef __MINGW32__
void Dynamic_Module::Load(std::string Module_Name)
{
    if(handle != 0){
        Unload();
    }
    
    handle = LoadLibraryW(std::wstring(Module_Name.begin(), Module_Name.end()).c_str());
    if (!handle) 
    {
        if(Debug) std::cout << "Error: " << Module_Name << " not found" << std::endl;
        exit(1);
    }
    else
    {
        if(Debug) std::cout << "Loaded: " << Module_Name << std::endl;
        Assign("Init", Init);
        Assign("Init_Globals", Init_Globals);
        Assign("Set_Function_Runner", Set_Function_Runner);
        Assign("Interpreter", Interpreter);
        Assign("Shutdown", Shutdown);
    } 
}

void Dynamic_Module::Unload()
{
    if(handle != 0)
    {
        if(Shutdown != nullptr) Shutdown();
        FreeLibrary(handle);
        handle = 0;
    }
    Init = nullptr;
    Init_Globals = nullptr;
    Set_Function_Runner = nullptr;
    Interpreter = nullptr;
    Shutdown = nullptr;
}

Dynamic_Module::~Dynamic_Module()
{
    Unload();
}
#endif
