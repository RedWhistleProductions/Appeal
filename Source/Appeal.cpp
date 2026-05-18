#include "Appeal.h"

static Appeal *Active_Appeal = nullptr;

namespace
{
    bool Try_Float(const std::string &Value, float &Result)
    {
        try
        {
            Result = std::stof(Value);
            return true;
        }
        catch(const std::exception &)
        {
            std::cerr << "Evaluation error: expected number but got '" << Value << "'" << std::endl;
            Result = 0.0f;
            return false;
        }
    }
}

static void Appeal_Function_Callback(std::string Function)
{
    if(Active_Appeal != nullptr)
    {
        Active_Appeal->Run_Callback(Function);
    }
}

static void Appeal_Analog_Function_Callback(std::string Function, int Value)
{
    if(Active_Appeal != nullptr)
    {
        Active_Appeal->Run_Analog_Callback(Function, Value);
    }
}

class Runtime_Data_Source : public Data_Source
{
public:
    Data_Source_Manager *Manager = nullptr;

    Runtime_Data_Source(Data_Source_Manager *Source_Manager)
    {
        Manager = Source_Manager;
    }

    std::string Get_Data() override
    {
        if(Manager == nullptr)
        {
            return "";
        }
        return Manager->Get_Data();
    }
};

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
    
    Dynamic_Modules.Current->Value.Debug = Debug_Mode;
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
    
    if(Dynamic_Modules.Current->Value.Init_Globals != nullptr)
    {
        Dynamic_Modules.Current->Value.Init_Globals(Plugin_Path, &Globals);
    }
    else
    {
        Dynamic_Modules.Current->Value.Init(Plugin_Path);
    }

    if(Dynamic_Modules.Current->Value.Set_Function_Runner != nullptr)
    {
        Active_Appeal = this;
        Dynamic_Modules.Current->Value.Set_Function_Runner(Appeal_Function_Callback, Appeal_Analog_Function_Callback);
    }
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
    std::string Running_Source = Data_Manager.Data_Sources.Current->Name;
    if(Debug_Mode) std::cout << "Running: " << Running_Source << std::endl;
    
    Done = false;
    if(Data_Manager.Data_Sources.Nodes == 0)
    {
        std::cout << "Error: No Data Sources" << std::endl;
        Done = true;
    }

    try
    {
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
                    Shutdown();
                    if(Debug_Mode) std::cout << "Done Running: " << Running_Source << std::endl;
                    return;
                }
                else if(Command == "Debug")
                {
                    int Enable;
                    Data_Manager >> Enable;
                    Debug_Mode = Enable != 0;
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
                    if(Debug_Mode) std::cout << "Running: " << Name << std::endl;
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
                    Run_Function(Function_Name);
                }
                else if( Command == "Return" )
                {
                    std::string Returning_Source = Data_Manager.Data_Sources.Current->Name;
                    auto Previous_Source = Data_Manager.Data_Sources.Current->Back_Node;
                    if(Debug_Mode) std::cout << "Returning From: " << Returning_Source << std::endl;
                    // Remove removes the current Data Source and goes back to the previous one
                    Data_Manager.Data_Sources.Remove();
                    if(Data_Manager.Data_Sources.Nodes > 0 && Previous_Source != nullptr)
                    {
                        Data_Manager.Data_Sources.Current = Previous_Source;
                        Data_Manager.Data_Sources.Current_Name = Previous_Source->Name;
                    }
                    if(Returning_Source == Running_Source)
                    {
                        return;
                    }
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
                else if(Command == "Loop")
                {
                    std::string Loop_Type, Value_1, Op, Value_2, Loop_Target;
                    Data_Manager >> Loop_Type; //While or Until
                    Value_1 = Read_Condition_Value();
                    Data_Manager >> Op;
                    Value_2 = Read_Condition_Value();
                    Data_Manager >> Loop_Target;

                    bool Use_Function = false;
                    if(Loop_Target == "{")
                    {
                        Use_Function = true;
                        Capture_Block("Loop_Function");
                    }

                    if( Loop_Type == "While")
                    {
                        while(Evaluate(Value_1, Op, Value_2))
                        {
                            if(Use_Function)
                            {
                                Run_Function("Loop_Function");
                            }
                            else
                            {
                                Run(Loop_Target);
                            }
                        }
                    }
                    else if(Loop_Type == "Until")
                    {   
                        do
                        {
                            if(Use_Function)
                            {
                                Run_Function("Loop_Function");
                            }
                            else
                            {
                                Run(Loop_Target);
                            }
                        } while (Evaluate(Value_1, Op, Value_2) == false);
                    }

                }
                else if(Command == "If")
                {
                    std::string Value_1, Op, Value_2;
                    Value_1 = Read_Condition_Value();
                    Data_Manager >> Op;
                    Value_2 = Read_Condition_Value();
                    Data_Manager >> Command;
                    
                    if(Command == "{")
                    {
                        Capture_Block("If_Function");
                    }

                    if(Evaluate(Value_1, Op, Value_2))
                    {
                        Run_Function("If_Function");
                    }
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
                else if(Command == "Print_Global")
                {
                    std::string Name;
                    Data_Manager >> Name;
                    std::cout << Get_Global_Value(Name) << std::endl;
                }
                else if(Command == "Set_Var_Global")
                {
                    std::string Variable, Global;
                    Data_Manager >> Variable;
                    Data_Manager >> Global;
                    Data_Manager.Variables[Variable] = Get_Global_Value(Global);
                }
                //Check if the Command is the name of a Dictionary
                else 
                {
                    if(Dictionary_List.Find(Command))
                    {
                        Interpreter = Dictionary_List.Current->Value;
                        Runtime_Data_Source Module_Data(&Data_Manager);
                        Interpreter(&Module_Data);
                    }
                    else
                    {
                        if(Debug_Mode) std::cout << "\tCommand Not Found: " << Command << std::endl;
                        Command_Error++;
                        if(Command_Error > Command_Error_Limit)
                        {
                            Done = true;
                        }
                    }
                }   
            }
        }
    }
    catch(const std::exception &Error)
    {
        std::cerr << "Runtime error while running '" << Running_Source
                  << "' near command '" << Command << "': " << Error.what() << std::endl;
        Done = true;
        Shutdown();
    }
    if(Debug_Mode) std::cout << "Done Running: " << Running_Source << std::endl;
}

bool Appeal::Evaluate(std::string V1, std::string Op, std::string V2)
{
    V1 = Resolve_Value(V1);
    V2 = Resolve_Value(V2);

    if(Op == "==") return V1 == V2;
    else if(Op == "!=") return V1 != V2;
    else if(Op == "<=" || Op == "<" || Op == ">=" || Op == ">")
    {
        float N1 = 0.0f;
        float N2 = 0.0f;
        if(!Try_Float(V1, N1) || !Try_Float(V2, N2)) return false;
        if(Op == "<=") return N1 <= N2;
        if(Op == "<") return N1 < N2;
        if(Op == ">=") return N1 >= N2;
        return N1 > N2;
    }
    else
    {
        if(Debug_Mode) std::cout << "Evaluation error: " << Op << " is not a valid operator!" << std::endl;
        return false;
    }
}

std::string Appeal::Read_Condition_Value()
{
    std::string Value = Data_Manager.Data_Sources.Current->Value->Get_Data();
    if(Value == "Var")
    {
        std::string Name = Data_Manager.Data_Sources.Current->Value->Get_Data();
        return "Var:" + Name;
    }
    return Value;
}

std::string Appeal::Resolve_Value(std::string Value)
{
    std::string Prefix = "Var:";
    if(Value.rfind(Prefix, 0) == 0)
    {
        std::string Name = Value.substr(Prefix.length());
        auto It = Data_Manager.Variables.find(Name);
        if(It != Data_Manager.Variables.end())
        {
            return It->second;
        }

        if(Debug_Mode) std::cout << "Error: Variable Not Found: " << Name << std::endl;
        return "";
    }
    return Value;
}

void Appeal::Run_Function(std::string Function)
{
    if(Appeal_Functions.Find(Function))
    {
        Data_Manager.Add_Source(Function, &Appeal_Functions.Current->Value);
        Run();
    }
    else
    {
        if(Debug_Mode) std::cout << "Function not found: " << Function << std::endl;
    }
}

void Appeal::Run_Callback(std::string Function)
{
    if(Function == "")
    {
        return;
    }

    size_t Dot = Function.find('.');
    if(Dot != std::string::npos)
    {
        Run_Module_Callback(Function.substr(0, Dot), Function.substr(Dot + 1), false, 0);
        return;
    }

    Run_Function(Function);
}

void Appeal::Run_Analog_Callback(std::string Function, int Value)
{
    if(Function == "")
    {
        return;
    }

    Data_Manager.Variables["Callback_Value"] = std::to_string(Value);

    size_t Dot = Function.find('.');
    if(Dot != std::string::npos)
    {
        Run_Module_Callback(Function.substr(0, Dot), Function.substr(Dot + 1), true, Value);
        return;
    }

    Run_Function(Function);
}

void Appeal::Run_Module_Callback(std::string Module, std::string Command, bool Has_Value, int Value)
{
    if(Dictionary_List.Find(Module))
    {
        Appeal_Function Callback_Source;
        Callback_Source.Add_Command(Command);
        if(Has_Value)
        {
            Callback_Source.Add_Command(std::to_string(Value));
        }
        Dictionary_List.Current->Value(&Callback_Source);
    }
    else
    {
        if(Debug_Mode) std::cout << "Callback module not found: " << Module << std::endl;
    }
}

void Appeal::Capture_Block(std::string Function)
{
    std::string Command;
    int Brace_Depth = 1;

    Appeal_Functions.Add_Node(Function);
    while(Brace_Depth > 0)
    {
        Command = Data_Manager.Data_Sources.Current->Value->Get_Data();

        if(Command == "{")
        {
            Brace_Depth++;
            Appeal_Functions.Current->Value.Add_Command(Command);
        }
        else if(Command == "}")
        {
            Brace_Depth--;
            if(Brace_Depth > 0)
            {
                Appeal_Functions.Current->Value.Add_Command(Command);
            }
        }
        else
        {
            Appeal_Functions.Current->Value.Add_Command(Command);
        }
    }
}

bool Appeal::Find_Global(std::string Name)
{
    return Globals.Find(Name);
}

std::string Appeal::Get_Global_Value(std::string Name)
{
    if(Find_Global(Name))
    {
        return Globals.Current->Value.Value;
    }

    if(Debug_Mode) std::cout << "Global not found: " << Name << std::endl;
    return "";
}

void Appeal::Shutdown()
{
    if(Shutting_Down)
    {
        return;
    }

    Shutting_Down = true;

    while(Dynamic_Modules.Nodes > 0)
    {
        Dynamic_Modules.Last();
        Dynamic_Modules.Current->Value.Unload();
        Dynamic_Modules.Remove();
    }
}
