#include "Appeal.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>


static Appeal *Active_Appeal = nullptr;

namespace
{
    double Arithmetic_Number(const std::string &Text)
    {
        std::istringstream Input(Text);
        Input.imbue(std::locale::classic());
        double Value;
        if(!(Input >> Value) || !std::isfinite(Value))
            throw std::runtime_error("Arithmetic requires a finite number: " + Text);
        Input >> std::ws;
        if(!Input.eof())
            throw std::runtime_error("Arithmetic requires a finite number: " + Text);
        return Value;
    }

    std::string Arithmetic_Result(double Value)
    {
        if(!std::isfinite(Value)) throw std::runtime_error("Arithmetic result out of range");
        if(Value == 0) return "0";
        std::ostringstream Output;
        Output.imbue(std::locale::classic());
        // Whole results remain usable by integer arguments such as Paste.
        if(std::trunc(Value) == Value) Output << std::fixed << std::setprecision(0);
        else Output << std::setprecision(std::numeric_limits<double>::max_digits10);
        Output << Value;
        return Output.str();
    }

    bool Normalize_Boolean(std::string Value, std::string &Result)
    {
        if(Value == "True" || Value == "true" || Value == "On" || Value == "on" || Value == "1")
        {
            Result = "1";
            return true;
        }
        if(Value == "False" || Value == "false" || Value == "Off" || Value == "off" || Value == "0")
        {
            Result = "0";
            return true;
        }
        return false;
    }

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

    bool File_Exists(const std::string &File_Name)
    {
        std::ifstream File(File_Name.c_str());
        return File.good();
    }

    bool Ends_With(const std::string &Value, const std::string &Suffix)
    {
        if(Value.length() < Suffix.length())
        {
            return false;
        }

        return Value.compare(Value.length() - Suffix.length(), Suffix.length(), Suffix) == 0;
    }

    std::string Script_File_Name(const std::string &Appeal_Folder, const std::string &Name)
    {
        if(File_Exists(Name))
        {
            return Name;
        }

        if(Ends_With(Name, ".Appeal"))
        {
            return Name;
        }

        std::string Named_File = Name + ".Appeal";
        if(File_Exists(Named_File))
        {
            return Named_File;
        }

        return Appeal_Folder + "/" + Name + ".Appeal";
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

void Appeal::Set_Resource_Folder(std::string Folder)
{
    Resource_Folder = Folder;
    Appeal_Folder = Resource_Folder + "/Appeal";
    Modules_Folder = Resource_Folder + "/Modules";
    Extensions_Folder = Resource_Folder + "/Extensions";
}

void Appeal::Add_Module(std::string Name, void (*Interpreter)(Data_Source *Data))
{
    Dictionary_List.Add_Node(Name, Interpreter);
}

void Appeal::Add_Dynamic_Module(std::string Name, std::string Plugin_Name)
{
    std::string Module_Path, Plugin_Path, Appeal_Prefix;
    Appeal_Prefix = "Appeal_Module_";
    Dynamic_Modules.Add_Node(Name);
    #ifdef __linux__
        Module_Path = Modules_Folder + "/" + Appeal_Prefix + Name + "/" + Appeal_Prefix + Name + ".so";
    #endif
    #ifdef __MINGW32__
        Module_Path = Modules_Folder + "/" + Appeal_Prefix + Name + "/" + Appeal_Prefix + Name + ".dll";
    #endif

    Dynamic_Modules.Current->Value.Debug = Debug_Mode;
    Dynamic_Modules.Current->Value.Load(Module_Path);

    // Load the Interpreter
    Add_Module(Name, Dynamic_Modules.Current->Value.Interpreter);

    // Load the Plugin
    #ifdef __linux__
        Plugin_Path = Modules_Folder + "/" + Appeal_Prefix + Name + "/" + Plugin_Name + "/" + Plugin_Name + ".so";
    #endif
    #ifdef __MINGW32__
        Plugin_Path = Modules_Folder + "/" + Appeal_Prefix + Name + "/" + Plugin_Name + "/" + Plugin_Name + ".dll";
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

void Appeal::Add_Extension(std::string Name)
{
    std::string Extension_Path;

    if(Dictionary_List.Find(Name))
    {
        if(Debug_Mode) std::cout << "Extension already loaded: " << Name << std::endl;
        return;
    }

    #ifdef __linux__
        Extension_Path = Extensions_Folder + "/" + Name + "/" + Name + ".so";
    #endif
    #ifdef __MINGW32__
        Extension_Path = Extensions_Folder + "/" + Name + "/" + Name + ".dll";
    #endif

    if(!File_Exists(Extension_Path))
    {
        std::cout << "Error: " << Extension_Path << " is missing." << std::endl;
        return;
    }

    Dynamic_Modules.Add_Node(Name);
    Dynamic_Modules.Current->Value.Debug = Debug_Mode;
    Dynamic_Modules.Current->Value.Load(Extension_Path);

    if(Dynamic_Modules.Current->Value.Interpreter == nullptr)
    {
        std::cout << "Error: Extension " << Name << " does not export Interpreter." << std::endl;
        Dynamic_Modules.Current->Value.Unload();
        Dynamic_Modules.Remove();
        return;
    }

    Add_Module(Name, Dynamic_Modules.Current->Value.Interpreter);

    if(Dynamic_Modules.Current->Value.Init_Globals != nullptr)
    {
        Dynamic_Modules.Current->Value.Init_Globals("", &Globals);
    }
    else if(Dynamic_Modules.Current->Value.Init != nullptr)
    {
        Dynamic_Modules.Current->Value.Init("");
    }

    if(Dynamic_Modules.Current->Value.Set_Function_Runner != nullptr)
    {
        Active_Appeal = this;
        Dynamic_Modules.Current->Value.Set_Function_Runner(Appeal_Function_Callback, Appeal_Analog_Function_Callback);
    }
}

bool Appeal::Add_Script(std::string Name)
{
    std::string File_Name = Script_File_Name(Appeal_Folder, Name);
    // First Check if the Script is already loaded
    if(Appeal_Scripts.Find(Name))
    {
        Appeal_Scripts.Current->Value.Load(File_Name);
        Data_Manager.Add_Source(Name, &Appeal_Scripts.Current->Value);
        return true;
    }

    // Make sure the file exists before loading it
    if(File_Exists(File_Name))
    {
        Appeal_Scripts.Add_Node();
        Appeal_Scripts.Current->Name = Name;
        Appeal_Scripts.Current->Value.Load(File_Name);

        Data_Manager.Add_Source(Name, &Appeal_Scripts.Current->Value);
        return true;
    }
    else
    {
        std::cout << "Error: " << File_Name << " is missing." << std::endl;
        return false;
    }
}

void Appeal::Run(std::string Script)
{
    if(Data_Manager.Data_Sources.Find(Script))
    {
        Run();
    }
    else
    {
        if(Add_Script(Script))
        {
            Run();
        }
    }
}

void Appeal::Run()
{
    // Only the outer entry point reports an error. Inner contexts unwind first,
    // restoring the suspended reader and removing their temporary sources.
    try
    {
        Run_Context(Preprocessor == nullptr ? nullptr : Preprocessor->Results);
    }
    catch(const std::exception &Error)
    {
        if(Preprocessor != nullptr) throw;
        Done = true;
        std::cerr << "Appeal error: " << Error.what() << std::endl;
    }
}

void Appeal::Run_Context(Return_Value_Data_Source *Results)
{
    Data_Source_Preprocessor Input;
    Input.Raw_Data = &Data_Manager;
    Input.Results = Results;
    Input.Execute = [this](const std::vector<std::string> &Script, Return_Value_Data_Source &Output)
    {
        Execute_Expression(Script, Output);
    };
    Data_Source_Preprocessor *Previous = Preprocessor;
    Preprocessor = &Input;
    try { Run_Commands(); }
    catch(...)
    {
        Preprocessor = Previous;
        throw;
    }
    Preprocessor = Previous;
}

void Appeal::Execute_Expression(const std::vector<std::string> &Script,
                                Return_Value_Data_Source &Results)
{
    // The expression is a bounded, temporary script. Its exhausted source emits
    // Return, so it can never consume the caller's next argument. Each nested
    // expression owns a separate result queue until its execution completes.
    Appeal_Function Expression;
    Expression.Command_Vector = Script;
    int Source_Count = Data_Manager.Data_Sources.Nodes;
    auto Previous_Source = Data_Manager.Data_Sources.Current;
    int Previous_Index = Data_Manager.Data_Sources.Current_Node;
    bool Previous_Done = Done;
    int Previous_Error = Command_Error;
    Data_Manager.Add_Source("<expression>", &Expression);

    auto Restore = [&]()
    {
        // On failure, Run or Call may have left additional sources above ours.
        while(Data_Manager.Data_Sources.Nodes > Source_Count)
        {
            Data_Manager.Data_Sources.Last();
            Data_Manager.Data_Sources.Remove();
        }
        Data_Manager.Data_Sources.Current = Previous_Source;
        Data_Manager.Data_Sources.Current_Node = Previous_Index;
        Data_Manager.Data_Sources.Current_Name = Previous_Source == nullptr ? "" : Previous_Source->Name;
        Done = Previous_Done;
        Command_Error = Previous_Error;
    };
    try { Run_Context(&Results); }
    catch(...)
    {
        Restore();
        throw;
    }
    Restore();
}

void Appeal::Run_Commands()
{
    // Command is local: evaluating an argument may recursively execute another
    // command, and must not overwrite the suspended command's name.
    std::string Command;
    if(Data_Manager.Current_Source() == nullptr) return;
    std::string Running_Source = Data_Manager.Data_Sources.Current->Name;
    if(Debug_Mode) std::cout << "Running: " << Running_Source << std::endl;

    Done = false;
    if(Data_Manager.Data_Sources.Nodes == 0)
    {
        std::cout << "Error: No Data Sources" << std::endl;
        Done = true;
    }
    //while(not Done or Command != "Return")
    while(not Done)
    {
        Command = Preprocessor->Get_Command();
        if(Command != "")
        {
            //If the Command is "Done" the program will exit
            if(Command == "Done")
            {
                if(Preprocessor->Results != nullptr)
                    throw std::runtime_error("Done is not allowed inside an expression");
                Done = true;
                if(Debug_Mode) std::cout << "Done Running: " << Running_Source << std::endl;
                return;
            }
            else if(Command == "Debug")
            {
                std::string Value;
                *Preprocessor >> Value;
                if(Value == "1" || Value == "On" || Value == "on" || Value == "True" || Value == "true")
                {
                    Debug_Mode = true;
                }
                else if(Value == "0" || Value == "Off" || Value == "off" || Value == "False" || Value == "false")
                {
                    Debug_Mode = false;
                }
            }
            else if(Command == "Add_Dynamic_Module")
            {
                std::string Name;
                std::string Plugin_Name;
                *Preprocessor >> Name;
                *Preprocessor >> Plugin_Name;
                Add_Dynamic_Module(Name, Plugin_Name);
            }
            else if(Command == "Add_Extension" || Command == "Add_Dynamic_Extension")
            {
                std::string Name;
                *Preprocessor >> Name;
                Add_Extension(Name);
            }
            else if(Command == "Return_Value")
            {
                if(Preprocessor->Results == nullptr)
                    throw std::runtime_error("Return_Value requires an expression");
                // Appeal functions may publish several properties with repeated
                // Return_Value commands. Ordinary Return still ends the source.
                std::string Value;
                *Preprocessor >> Value;
                Preprocessor->Return_Value(Value);
            }
            else if(Command == "Add" || Command == "Subtract" ||
                    Command == "Multiply" || Command == "Divide")
            {
                std::string Left, Right;
                *Preprocessor >> Left;
                *Preprocessor >> Right;
                const double A = Arithmetic_Number(Left);
                const double B = Arithmetic_Number(Right);
                if(Command == "Divide" && B == 0)
                    throw std::runtime_error("Division by zero");
                double Result = 0;
                if(Command == "Add") Result = A + B;
                else if(Command == "Subtract") Result = A - B;
                else if(Command == "Multiply") Result = A * B;
                else Result = A / B;
                Preprocessor->Return_Value(Arithmetic_Result(Result));
            }
            else if(Command == "Print")
            {
                std::string Str;
                *Preprocessor >> Str;
                std::cout << Str << std::endl;
            }
            else if( Command == "Run" )
            {
                std::string Name;
                *Preprocessor >> Name;
                if(Name == "Var")
                {
                    std::string Variable;
                    *Preprocessor >> Variable;
                    auto Variable_It = Data_Manager.Variables.find(Variable);
                    if(Variable_It != Data_Manager.Variables.end())
                    {
                        Name = Variable_It->second;
                    }
                    else
                    {
                        if(Debug_Mode) std::cout << "Run variable not found: " << Variable << std::endl;
                        continue;
                    }
                }
                if(Add_Script(Name))
                {
                    if(Debug_Mode) std::cout << "Running: " << Name << std::endl;
                }
            }
            else if( Command == "Define_Function" )
            {
                std::string Function_Name;
                *Preprocessor >> Function_Name;
                Appeal_Functions.Add_Node(Function_Name);
                while( Command != "End_Function" and Command != "Done")
                {
                    Command = Preprocessor->Get_Raw_Data();
                    if(Data_Manager.Current_Source()->End_Of_Source() || Command == "Done")
                        throw std::runtime_error("Missing End_Function before end of source");
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
                *Preprocessor >> Function_Name;
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
                *Preprocessor >> Name;
                Add_Module(Name, Dynamic_Modules.Current->Value.Interpreter);
            }
            else if( Command == "Add_Data_Source" )
            {
                std::string Name;
                *Preprocessor >> Name;
                Data_Manager.Add_Source(Name, &Appeal_Scripts.Current->Value);
            }
            else if(Command == "Delay")
            {
                int seconds;
                *Preprocessor >> seconds;
                std::this_thread::sleep_for(std::chrono::seconds(seconds));
            }
            else if(Command == "Loop")
            {
                std::string Loop_Type, Op, Loop_Target;
                *Preprocessor >> Loop_Type; //While or Until
                auto Value_1 = Preprocessor->Capture_Condition();
                *Preprocessor >> Op;
                auto Value_2 = Preprocessor->Capture_Condition();
                *Preprocessor >> Loop_Target;

                auto Condition = [&]()
                {
                    // Separate reads define left-to-right evaluation order.
                    std::string Left = Preprocessor->Resolve_Condition(Value_1);
                    std::string Right = Preprocessor->Resolve_Condition(Value_2);
                    return Evaluate(Left, Op, Right);
                };

                bool Use_Function = false;
                if(Loop_Target == "{")
                {
                    Use_Function = true;
                    Capture_Block("Loop_Function");
                }

                if( Loop_Type == "While")
                {
                    while(!Done && Condition())
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
                    } while (!Done && !Condition());
                }

            }
            else if(Command == "If")
            {
                std::string Value_1, Op, Value_2;
                Value_1 = Read_Condition_Value();
                *Preprocessor >> Op;
                Value_2 = Read_Condition_Value();
                *Preprocessor >> Command;

                if(Command == "{")
                {
                    Capture_Block("If_Function");
                }

                if(Evaluate(Value_1, Op, Value_2))
                {
                    Run_Function("If_Function");
                }
            }

            else if(Command == "Read_Line")
            {
                std::string Name, Status, Line;
                *Preprocessor >> Name;
                *Preprocessor >> Status;
                bool Read = static_cast<bool>(std::getline(std::cin, Line));
                Data_Manager.Variables[Name] = Line;
                Data_Manager.Variables[Status] = Read ? "1" : "0";
            }
            else if( Command == "Set_Var")
            {
                //Sets a variable in the Data_Manager
                std::string Name, Value;
                *Preprocessor >> Name;
                *Preprocessor >> Value;
                Data_Manager.Variables[Name] = Value;
            }
            else if( Command == "Del_Var")
            {
                std::string Name;
                *Preprocessor >> Name;
                Data_Manager.Variables.erase(Name);
            }
            else if(Command == "Print_Global")
            {
                std::string Name;
                *Preprocessor >> Name;
                std::cout << Get_Global_Value(Name) << std::endl;
            }
            else if(Command == "Set_Var_Global")
            {
                std::string Variable, Global;
                *Preprocessor >> Variable;
                *Preprocessor >> Global;
                Data_Manager.Variables[Variable] = Get_Global_Value(Global);
            }
            //Check if the Command is the name of a Dictionary
            else
            {
                if(Dictionary_List.Find(Command))
                {
                    Interpreter = Dictionary_List.Current->Value;
                    Interpreter(Preprocessor);
                }
                else
                {
                    if(Debug_Mode) std::cout << "\tCommand Not Found: " << Command << std::endl;
                    if(Preprocessor->Results != nullptr)
                        throw std::runtime_error("Unknown expression command: " + Command);
                    Command_Error++;
                    if(Command_Error > Command_Error_Limit)
                    {
                        Done = true;
                    }
                }
            }
        }
    }
    if(Debug_Mode) std::cout << "Done Running: " << Running_Source << std::endl;
}

bool Appeal::Evaluate(std::string V1, std::string Op, std::string V2)
{
    V1 = Resolve_Value(V1);
    V2 = Resolve_Value(V2);

    if(Op == "==" || Op == "!=")
    {
        std::string B1, B2;
        if(Normalize_Boolean(V1, B1) && Normalize_Boolean(V2, B2))
        {
            if(Op == "==") return B1 == B2;
            return B1 != B2;
        }

        if(Op == "==") return V1 == V2;
        return V1 != V2;
    }
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
    return Preprocessor->Get_Condition_Value();
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
        // Execute a fresh cursor over the definition. Repeated and recursive
        // calls must not share Appeal_Function's mutable read position.
        Appeal_Function Invocation;
        Invocation.Command_Vector = Appeal_Functions.Current->Value.Command_Vector;
        int Source_Count = Data_Manager.Data_Sources.Nodes;
        Data_Manager.Add_Source(Function, &Invocation);
        try { Run(); }
        catch(...)
        {
            while(Data_Manager.Data_Sources.Nodes > Source_Count)
            {
                Data_Manager.Data_Sources.Last();
                Data_Manager.Data_Sources.Remove();
            }
            throw;
        }
        while(Data_Manager.Data_Sources.Nodes > Source_Count)
        {
            Data_Manager.Data_Sources.Last();
            Data_Manager.Data_Sources.Remove();
        }
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
        // Callbacks use the same dispatch and preprocessing as other scripts.
        // A private output queue also prevents a callback's incidental result
        // from being mistaken for the enclosing expression's return value.
        std::vector<std::string> Script = {Module, Command};
        if(Has_Value) Script.push_back(std::to_string(Value));
        Return_Value_Data_Source Ignored;
        Execute_Expression(Script, Ignored);
    }
    else
    {
        if(Debug_Mode) std::cout << "Callback module not found: " << Module << std::endl;
    }
}

//Reads { and }
void Appeal::Capture_Block(std::string Function)
{
    std::string Command;
    int Brace_Depth = 1;

    Appeal_Functions.Add_Node(Function);
    while(Brace_Depth > 0)
    {
        Command = Preprocessor->Get_Raw_Data();
        if(Command == "Done" || Data_Manager.Current_Source()->End_Of_Source())
            throw std::runtime_error("Missing } before end of source");

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

Appeal::~Appeal()
{

}
