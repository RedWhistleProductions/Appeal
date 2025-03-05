#include "Dynamic_Module.h"

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