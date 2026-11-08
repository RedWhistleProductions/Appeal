#include "Appeal.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace
{
    int Getter_Calls = 0;
    int Tick = 0;
    Appeal *Callback_App = nullptr;

    // A real C++ dictionary using the same interface as a dynamically loaded
    // module. This verifies result publication without requiring mouse hardware.
    void Test_Interpreter(Data_Source *Data)
    {
        std::string Command;
        *Data >> Command;
        if(Command == "Mouse_X")
        {
            ++Getter_Calls;
            Data->Return_Value("125");
        }
        else if(Command == "Add")
        {
            int A, B;
            *Data >> A;
            *Data >> B;
            Data->Return_Value(std::to_string(A + B));
        }
        else if(Command == "Pair")
        {
            Data->Return_Value("3");
            Data->Return_Value("4");
        }
        else if(Command == "Echo")
        {
            std::string Value;
            *Data >> Value;
            Data->Return_Value(Value);
        }
        else if(Command == "Nothing") {}
        else if(Command == "Tick")
        {
            if(++Tick > 5) throw std::runtime_error("Loop did not terminate");
            Data->Return_Value(std::to_string(Tick));
        }
        else if(Command == "Callback")
        {
            Callback_App->Run_Module_Callback("Test", "Mouse_X", false, 0);
            Data->Return_Value("9");
        }
        else throw std::runtime_error("Unknown test command");
    }

    struct Capture_Output
    {
        std::ostringstream Output, Errors;
        std::streambuf *Old_Output = std::cout.rdbuf(Output.rdbuf());
        std::streambuf *Old_Errors = std::cerr.rdbuf(Errors.rdbuf());
        ~Capture_Output()
        {
            std::cout.rdbuf(Old_Output);
            std::cerr.rdbuf(Old_Errors);
        }
    };

    void Check(bool Condition, const std::string &Message)
    {
        if(!Condition) throw std::runtime_error(Message);
    }

    void Run_Tokens(const std::vector<std::string> &Tokens,
                    const std::string &Expected, const std::string &Error = "")
    {
        Appeal App;
        Appeal_Function Source;
        Source.Command_Vector = Tokens;
        App.Add_Module("Test", Test_Interpreter);
        Callback_App = &App;
        App.Data_Manager.Add_Source("test", &Source);
        Capture_Output Captured;
        App.Run();
        Check(Captured.Output.str() == Expected, "Unexpected output: " + Captured.Output.str());
        if(Error.empty()) Check(Captured.Errors.str().empty(), Captured.Errors.str());
        else
        {
            Check(Captured.Errors.str().find(Error) != std::string::npos,
                  "Missing diagnostic: " + Error + "; got: " + Captured.Errors.str());
            Check(App.Data_Manager.Data_Sources.Nodes == 1,
                  "Failed expression left temporary sources behind");
            Check(App.Data_Manager.Current_Source() == &Source, "Caller source was not restored");
        }
    }
}

int main(int argc, char **argv)
{
    try
    {
        Run_Tokens({"Set_Var", "I", "0", "Set_Var", "Total", "0",
            "Loop", "While", "Var", "I", "<", "3", "{",
              "Set_Var", "J", "0", "Loop", "While", "Var", "J", "<", "2", "{",
                "If", "1", "==", "1", "{", "If", "1", "==", "1", "{",
                  "Set_Var", "Total", "(Add", "Var", "Total", "1)", "}", "}",
                "Set_Var", "J", "(Add", "Var", "J", "1)", "}",
              "Set_Var", "I", "(Add", "Var", "I", "1)", "}",
            "Print", "Var", "Total"}, "6\n");
        Run_Tokens({"Define_Function", "F", "Print", "old", "End_Function",
                    "Define_Function", "F", "Print", "new", "End_Function", "Call", "F"}, "new\n");
        {
            Appeal Memory_App;
            Appeal_Function Body;
            Body.Command_Vector = {"Set_Var", "I", "0", "Loop", "While", "Var", "I", "<", "10000", "{",
                "If", "1", "==", "1", "{", "Set_Var", "I", "(Add", "Var", "I", "1)", "}", "}"};
            Memory_App.Data_Manager.Add_Source("memory", &Body);
            Memory_App.Run();
            Check(Memory_App.Appeal_Functions.Nodes == 0, "Anonymous blocks leaked into function registry");
        }
        Getter_Calls = 0;
        Run_Tokens({"Print(Test", "Mouse_X)", "Print", "after"}, "125\nafter\n");
        Check(Getter_Calls == 1, "Getter executed more than once");
        Run_Tokens({"Print(Test", "Add(Test", "Mouse_X)(Test", "Add", "2", "3))"}, "130\n");
        Run_Tokens({"Print(Test", "Add(Test", "Pair))"}, "7\n");
        Run_Tokens({"Set_Var", "X", "8", "Print(Test", "Add", "Var", "X", "2)"}, "10\n");
        Run_Tokens({"Print(Test", "Echo", "\"(literal Var)\")"}, "(literal Var)\n");
        Run_Tokens({"Print(Test", "Echo", "\"Return\")"}, "Return\n");
        Run_Tokens({"Print(Test", "Echo", "\"\")"}, "\n");
        Run_Tokens({"Define_Function", "F", "Return_Value(Test", "Mouse_X)", "Return",
                    "End_Function", "Print(Call", "F)", "Print(Call", "F)"}, "125\n125\n");
        Run_Tokens({"Define_Function", "F", "Print", "Var", "X", "End_Function",
                    "Set_Var", "X", "1", "Call", "F", "Set_Var", "X", "2", "Call", "F"}, "1\n2\n");
        Run_Tokens({"If", "1", "==", "2", "{", "Print(Test", "Mouse_X)", "}",
                    "Print", "after"}, "after\n");
        Run_Tokens({"Set_Var", "X", "0", "Loop", "While", "Var", "X", "<", "3", "{",
                    "Set_Var", "X", "(Test", "Add", "Var", "X", "1)", "}", "Print", "Var", "X"}, "3\n");
        Run_Tokens({"Print(Add", "2", "3)", "Print(Subtract", "2", "5)",
                    "Print(Multiply", "-2", "3)", "Print(Divide", "5", "2)"}, "5\n-3\n-6\n2.5\n");
        Run_Tokens({"Set_Var", "X", "176", "Set_Var", "Speed", "10",
                    "Set_Var", "X", "(Add", "Var", "X", "Var", "Speed)",
                    "Print", "Var", "X", "Print(Multiply(Add", "1", "2)", "4)"}, "186\n12\n");
        Run_Tokens({"Print(Add", "1.25", "2.5)", "Print(Add", "1e6", "2)",
                    "Print(Multiply", "-0", "3)"}, "3.75\n1000002\n0\n");
        Run_Tokens({"Add", "1", "2", "Print", "after"}, "after\n");
        Run_Tokens({"Print(Divide", "1", "0)"}, "", "Division by zero");
        Run_Tokens({"Print(Divide", "1", "-0)"}, "", "Division by zero");
        Run_Tokens({"Print(Add", "12oops", "1)"}, "", "finite number");
        Run_Tokens({"Print(Add", "nan", "1)"}, "", "finite number");
        Run_Tokens({"Print(Multiply", "1e308", "1e308)"}, "", "out of range");
        Run_Tokens({"Print(Subtract", "1)"}, "", "Missing argument");
        Tick = 0;
        Run_Tokens({"Loop", "While", "(Test", "Tick)", "<", "3", "{", "Print", "tick", "}"}, "tick\ntick\n");
        Check(Tick == 3, "Loop expression was not reevaluated");
        Run_Tokens({"Print(Test", "Callback)"}, "9\n");

        Run_Tokens({"Print", "(Test", "Mouse_X"}, "", "Missing )");
        Run_Tokens({"Print", ")"}, "", "Unexpected )");
        Run_Tokens({"Print", "()"}, "", "Empty expression");
        Run_Tokens({"Print(Test", "Nothing)"}, "", "returned no values");
        Run_Tokens({"Print(Test", "Add", "1)", "999"}, "", "Missing argument");
        Run_Tokens({"Print(Unknown)"}, "", "Unknown expression command");
        Run_Tokens({"Print(Test", "Add(Test", "Nothing)", "1)"}, "", "returned no values");
        Run_Tokens({"If", "(Test", "Pair)", "==", "3", "{", "}"}, "", "exactly one value");
        Run_Tokens({"Print(Test", "Pair)"}, "3\n", "Unused expression return values");
        Run_Tokens({"Define_Function", "Missing", "Print", "hello"}, "", "Missing End_Function");
        Run_Tokens({"If", "1", "==", "1", "{", "Print", "hello"}, "", "Missing }");

        // A forward-only adapter follows the same contract a TCP source uses.
        // No seeking, line buffering, or Appeal_Function cursor is required.
        {
            class Token_Stream : public Data_Source
            {
            public:
                std::queue<std::string> Tokens;
                std::string Get_Data() override
                {
                    if(Tokens.empty()) throw std::runtime_error("Read past stream terminator");
                    std::string Token = Tokens.front();
                    Tokens.pop();
                    return Token;
                }
            } Stream;
            for(const char *Token : {"Print(Test", "Add", "2", "3)", "Return"})
                Stream.Tokens.push(Token);
            Appeal Stream_App;
            Stream_App.Add_Module("Test", Test_Interpreter);
            Stream_App.Data_Manager.Add_Source("stream", &Stream);
            Capture_Output Stream_Output;
            Stream_App.Run();
            Check(Stream_Output.Output.str() == "5\n", "Forward-only stream failed");
            Check(Stream_Output.Errors.str().empty(), Stream_Output.Errors.str());
        }

        // Optional real-DLL check: pass the rebuilt Input module path. Inject a
        // deterministic getter through its exported pointer, without hardware.
        if(argc > 1)
        {
            Dynamic_Module Input;
            Input.Load(argv[1]);
            using Mouse_Function = int (*)();
            Mouse_Function *Getter = nullptr;
            Input.Assign("Mouse_X", Getter);
            Check(Getter != nullptr, "Input module does not export Mouse_X pointer");
            *Getter = +[]() { return 321; };
            Appeal Input_App;
            Input_App.Add_Module("Input", Input.Interpreter);
            Appeal_Function Script;
            Script.Command_Vector = {"Input", "Mouse_X", "Print(Input", "Mouse_X)"};
            Input_App.Data_Manager.Add_Source("input-test", &Script);
            Capture_Output Input_Output;
            Input_App.Run();
            Check(Input_Output.Output.str() == "321\n", "DLL return value failed");
            Check(Input_Output.Errors.str().empty(), Input_Output.Errors.str());
        }

        // File input exercises quotes, adjacent parentheses, physical EOF,
        // source switching, and returning from a script called in an expression.
        Appeal App;
        App.Add_Module("Test", Test_Interpreter);
        Capture_Output Captured;
        App.Run("Tests/Fixtures/Preprocessor.Appeal");
        Check(Captured.Errors.str().empty(), Captured.Errors.str());
        Check(Captured.Output.str() == "125\ntext (not code)\n42\nchild\nparent\n",
              "File/source-switch output: " + Captured.Output.str());
    }
    catch(const std::exception &Error)
    {
        std::cerr << "FAIL: " << Error.what() << std::endl;
        return 1;
    }
    std::cout << "Preprocessor integration tests passed" << std::endl;
    return 0;
}
