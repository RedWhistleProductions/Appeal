#include "Plugin.h"

void Plugin::Load(std::string File)
{
    if(handle != 0){
        Destructor();
        dlclose(handle);
    }
    
    
    handle = dlopen (File.c_str(), RTLD_LAZY);
	if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }
    else
    {
        std::cout << "Loaded: " << File << std::endl;
    }
    
    Assign("Constructor", Constructor);
    Assign("Destructor", Destructor);
    if(Constructor != nullptr) Constructor();
}

Plugin::~Plugin()
{
    if(Destructor != nullptr)
    {
        Destructor();
    } 
    if(handle != 0)
    {
        dlclose(handle);
    }   
}