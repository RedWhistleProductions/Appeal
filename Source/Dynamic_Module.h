#pragma once
#include "Plugin.h"
#include "Data_Source.h"


class Dynamic_Module
{
private:
    void *handle;
	char *error;
public:
    Plugin Module_Plugin;
    void (*Init)(std::string Plugin_Name);
    void (*Interpreter)(Data_Source *Data);
    void Load(std::string Module_Name);
    template<typename T>void Assign(std::string Key, T &func);

    ~Dynamic_Module();
};

template<typename T>void Dynamic_Module::Assign(std::string Key, T &func)
{
    void *Value = dlsym(handle, Key.c_str());
    if ((error = dlerror()) != NULL)  
    {
        std::cout << "Error: " << Key << " not found" << std::endl;
    } 
    else
    {
        func = reinterpret_cast<decltype(func)>(Value);
    }
}




