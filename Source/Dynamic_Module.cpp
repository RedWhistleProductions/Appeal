#include "Dynamic_Module.h"

#ifdef __linux__
void Dynamic_Module::Load(std::string Module_Name)
    {
        if(handle != 0){
            dlclose(handle);
        }
        

        handle = dlopen (Module_Name.c_str(), RTLD_LAZY);
        if (!handle) 
        {
            fputs (dlerror(), stderr);
            exit(1);
        }
        else
        {
            std::cout << "Loaded: " << Module_Name << std::endl;
            Assign("Init", Init);
            Assign("Interpreter", Interpreter);
        }
        
        
    }

    Dynamic_Module::~Dynamic_Module()
    {
        if(handle != 0)
        {
            dlclose(handle);
        }   
    }
#endif

#ifdef __MINGW32__
void Dynamic_Module::Load(std::string Module_Name)
{
    if(handle != 0){
        FreeLibrary(handle);
    }
    
    handle = LoadLibraryW(std::wstring(Module_Name.begin(), Module_Name.end()).c_str());
    if (!handle) 
    {
        std::cout << "Error: " << Module_Name << " not found" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Loaded: " << Module_Name << std::endl;
        Assign("Init", Init);
        Assign("Interpreter", Interpreter);
    } 
}

Dynamic_Module::~Dynamic_Module()
{
    if(handle != 0)
    {
        FreeLibrary(handle);
    }   
}
#endif