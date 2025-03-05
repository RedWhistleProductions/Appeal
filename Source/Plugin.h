#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include <dlfcn.h>

class Plugin
{
private:
    void *handle;
	char *error;

    void (*Constructor)();
    void (*Destructor)();

public:

    void Load(std::string File);
    template<typename T>void Assign(std::string Key, T &func);

    ~Plugin();
};

template<typename T>
void Plugin::Assign(std::string Key, T &func)
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


