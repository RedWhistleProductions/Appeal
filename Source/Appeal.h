#pragma once

#include <thread>
#include <chrono>
#include <fstream>

#include "Module_Data.h"
#include "File_IO.h"
#include "Named_List.h"
#include "Dynamic_Module.h"
#include "Data_Source_Manager.h"
#include "Data_Source_Preprocessor.h"
#include "File_Data_Source.h"
#include "Appeal_Function.h"

/****NOTE********************************************
 * Appeal uses a File_Data_Source by default,       *
 * but if you add the Network module you can        *
 * run scripts from a server over TCP.              *
 ***************************************************/

using Appeal_Dictionary= void (*)(Data_Source *Data);

class Appeal
{
    public:
    bool Done = false;
    bool Debug_Mode = false;
    ~Appeal();

    //Default Folder Paths
    std::string Resource_Folder = "./Resources";
    std::string Appeal_Folder = Resource_Folder + "/Appeal";
    std::string Modules_Folder = Resource_Folder + "/Modules";
    std::string Extensions_Folder = Resource_Folder + "/Extensions";
    void Set_Resource_Folder(std::string Folder);

    Data_Source_Manager Data_Manager;

    //Stores Funtions written in Appeal
    Named_List<Appeal_Function> Appeal_Functions;

    Named_List<Module_Data>Globals;

    //Used to switch between loaded modules
    Named_List<Dynamic_Module> Dynamic_Modules;
    void Add_Module(std::string Name, void (*Interpreter)(Data_Source *Data));
    void Add_Dynamic_Module(std::string Name, std::string Plugin_Name);
    void Add_Extension(std::string Name);

    bool Add_Script(std::string Name);
    void Run();
    void Run(std::string Name);


    //Functions for evaluating data
    bool Evaluate(std::string V1, std::string Opperator, std::string V2);
    std::string Read_Condition_Value();
    std::string Resolve_Value(std::string Value);

    void Run_Callback(std::string Function);
    void Run_Analog_Callback(std::string Function, int Value);
    void Run_Module_Callback(std::string Module, std::string Command, bool Has_Value, int Value);
    void Capture_Block(std::string Function);
    bool Find_Global(std::string Name);
    std::string Get_Global_Value(std::string Name);
    void Run_Function(std::string Function);

    private:
    Named_List<File_Data_Source> Appeal_Scripts;

    // The active reader belongs to a Run_Context stack frame. Nested calls
    // temporarily replace it, sharing modules/variables but not input buffers.
    Data_Source_Preprocessor *Preprocessor = nullptr;
    void Run_Context(Return_Value_Data_Source *Results);
    void Run_Commands();
    void Execute_Expression(const std::vector<std::string> &Script,
                            Return_Value_Data_Source &Results);
    Named_List<Appeal_Dictionary> Dictionary_List;
    void (*Interpreter)(Data_Source *Data);


    // The App terminates if Command_Error == Command_Error_Limit
    const int Command_Error_Limit = 3;
    int Command_Error = 0;

};

