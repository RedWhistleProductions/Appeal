#pragma once

#include <thread>
#include <chrono>

#include "File_IO.h"
#include "Named_List.h"
#include "Dynamic_Module.h"
#include "Data_Source_Manager.h"
#include "File_Data_Source.h"
#include "Appeal_Function.h"

/****NOTE********************************************
 * Appeal uses a File_Data_Source by default,       *
 * but if you add the Net_Client module you can     *
 * run scripts from a server over TCP.              *
 ***************************************************/

using Appeal_Dictionary= void (*)(Data_Source *Data);

class Appeal
{
    public:
    bool Done = false;
    std::string Resource_Folder = "./Resources";
    std::string Appeal_Folder = Resource_Folder + "/Appeal";
    std::string Modules_Folder = Resource_Folder + "/Modules";
    
    Data_Source_Manager Data_Manager;
    Named_List<Appeal_Function> Appeal_Functions;
    Named_List<Dynamic_Module> Dynamic_Modules;

    void Add_Module(std::string Name, void (*Interpreter)(Data_Source *Data));
    void Add_Dynamic_Module(std::string Name, std::string Plugin_Name);
    void Add_Script(std::string Name);
    void Run();
    void Run(std::string Name);
    
    private: 
    Named_List<File_Data_Source> Appeal_Scripts;
     
    std::string Command;
    Named_List<Appeal_Dictionary> Dictionary_List;
    void (*Interpreter)(Data_Source *Data);
    
    // The App terminates if Command_Error == Command_Error_Limit
    const int Command_Error_Limit = 3;
    int Command_Error = 0;
};

