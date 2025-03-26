#include "Appeal.h"


void Appeal::Add_Module(std::string Name, void (*Interpreter)(Data_Source *Data))
{
    Dictionary_List.Add_Node(Name, Interpreter);
}

void Appeal::Add_Dynamic_Module(std::string Name, std::string Plugin_Name)
{
    std::string Module_Path, Plugin_Path;
    // Load the Dynamic Module
    Dynamic_Modules.Add_Node(Name);
    #ifdef __linux__
        Module_Path = Modules_Folder + "/" + Name + "/" + Name + ".so";
    #endif
    #ifdef __MINGW32__
        Module_Path = Modules_Folder + "/" + Name + "/" + Name + ".dll";
    #endif
    
    Dynamic_Modules.Current->Value.Load(Module_Path);
    
    // Load the Interpreter
    Add_Module(Name, Dynamic_Modules.Current->Value.Interpreter);

    // Load the Plugin
    #ifdef __linux__
        Plugin_Path = Modules_Folder + "/" + Name + "/" + Plugin_Name + "/" + Plugin_Name + ".so";
    #endif
    #ifdef __MINGW32__
        Plugin_Path = Modules_Folder + "/" + Name + "/" + Plugin_Name + "/" + Plugin_Name + ".dll";
    #endif
    
    Dynamic_Modules.Current->Value.Init(Plugin_Path);
}

void Appeal::Add_Script(std::string Name)
{
    Appeal_Scripts.Add_Node();
    Appeal_Scripts.Current->Name = Name;
    Appeal_Scripts.Current->Value.Load(Appeal_Folder + "/" + Name + ".Appeal");
    
    Data_Manager.Add_Source(Name, &Appeal_Scripts.Current->Value);   
}

void Appeal::Run(std::string Script)
{
    if(Data_Manager.Data_Sources.Find(Script))
    {
        Run();
    }
    else
    {
        Add_Script(Script);
        Run();
    }
}

void Appeal::Run()
{
    std::cout << "Running: " << Data_Manager.Data_Sources.Current->Name << std::endl;
    
    Done = false;
    if(Data_Manager.Data_Sources.Nodes == 0)
    {
        std::cout << "Error: No Data Sources" << std::endl;
        Done = true;
    }

    //while(not Done or Command != "Return")
    while(not Done)
    {
        
        Command = Data_Manager.Data_Sources.Current->Value->Get_Data();
        if(Command != "")
        {
            //If the Command is "Done" the program will exit
            if(Command == "Done")
            {
                Done = true;
            }
            else if(Command == "Add_Dynamic_Module")
            {
                std::string Name;
                std::string Plugin_Name;
                Data_Manager >> Name;
                Data_Manager >> Plugin_Name;
                Add_Dynamic_Module(Name, Plugin_Name);
            }
            else if(Command == "Print")
            {
                std::string Str;
                Data_Manager >> Str;
                std::cout << Str << std::endl;
            }
            else if( Command == "Run" )
            {
                std::string Name;
                Data_Manager >> Name;
                Add_Script(Name);
                std::cout << "Running: " << Name << std::endl;
            }
            else if( Command == "Define_Function" )
            {
                std::string Function_Name;
                Data_Manager >> Function_Name;
                Appeal_Functions.Add_Node(Function_Name);
                while( Command != "End_Function" and Command != "Done")
                {
                    Data_Manager >> Command;
                    if( Command == "End_Function" or Command == "Done")
                    {
                        break;
                    }
                    Appeal_Functions.Current->Value.Add_Command(Command);
                }
            }
            else if( Command == "Call" )
            {
                std::string Function_Name;
                Data_Manager >> Function_Name;
                Appeal_Functions.Find("Function_Name");
                Data_Manager.Add_Source("Function_Name", &Appeal_Functions.Current->Value);
            }
            else if( Command == "Return" )
            {
                std::cout << "Returning From: " << Data_Manager.Data_Sources.Current->Name << std::endl;
                // Remove removes the current Data Source and goes back to the previous one
                Data_Manager.Data_Sources.Remove();
            }
            else if( Command == "Add_Module" )
            {
                std::string Name;
                Data_Manager >> Name;
                Add_Module(Name, Dynamic_Modules.Current->Value.Interpreter);
            }
            else if( Command == "Add_Data_Source" )
            {
                std::string Name;
                Data_Manager >> Name;
                Data_Manager.Add_Source(Name, &Appeal_Scripts.Current->Value);
            }
            else if(Command == "Delay")
            {
                int seconds;
                Data_Manager >> seconds;
                std::this_thread::sleep_for(std::chrono::seconds(seconds));
            }
            else if( Command == "Set_Var")
            {
                //Sets a variable in the Data_Manager 
                std::string Name, Value;
                Data_Manager >> Name;
                Data_Manager >> Value;
                Data_Manager.Variables[Name] = Value;   
            }
            else if( Command == "Del_Var")
            {
                std::string Name;
                Data_Manager >> Name;
                Data_Manager.Variables.erase(Name);
            }
            //Check if the Command is the name of a Dictionary
            else 
            {
                if(Dictionary_List.Find(Command))
                {
                    Interpreter = Dictionary_List.Current->Value;
                    Interpreter(Data_Manager.Data_Sources.Current->Value);
                }
                else
                {
                    std::cout << "\tCommand Not Found: " << Command << std::endl;
                    Command_Error++;
                    if(Command_Error > Command_Error_Limit)
                    {
                        Done = true;
                    }
                }
            }   
        }
    }
    std::cout << "Done Running: " << Data_Manager.Data_Sources.Current->Name << std::endl;
}
