#include "Data_Source_Preprocessor.h"

void Data_Source_Preprocessor::Split_Token(const std::string &Token, std::deque<std::string> &Output)
{
    // Sources retain quotes. Only parentheses outside quoted text are syntax.
    // This accepts Print(Input Mouse_X) without spaces at either bracket.
    std::string Part;
    bool Quoted = false;
    for(char Character : Token)
    {
        if(Character == '"') Quoted = !Quoted;
        if(!Quoted && (Character == '(' || Character == ')'))
        {
            if(!Part.empty()) Output.push_back(Part);
            Part.clear();
            Output.push_back(std::string(1, Character));
        }
        else Part += Character;
    }
    if(Quoted) throw std::runtime_error("Unterminated quoted string");
    if(!Part.empty() || Token.empty()) Output.push_back(Part);
}

std::string Data_Source_Preprocessor::Get_Raw_Data()
{
    if(Raw_Data == nullptr) return "Done";
    auto &Tokens = Pending[Raw_Data->Current_Source()];
    if(Tokens.empty()) Split_Token(Raw_Data->Get_Data(), Tokens);
    std::string Token = Tokens.front();
    Tokens.pop_front();
    return Token;
}

std::string Data_Source_Preprocessor::Get_Data()
{
    // Returned properties are consumed in FIFO order. Do not parse them again:
    // a returned string containing '(' or 'Var' is literal data.
    if(!Values.empty())
    {
        std::string Value = Values.front();
        Values.pop_front();
        return Value;
    }
    std::string Token = Get_Raw_Data();
    if(Token == "Return" || Token == "Done")
        throw std::runtime_error("Missing argument before end of source");
    return Read_Value(Token);
}

std::string Data_Source_Preprocessor::Get_Command()
{
    // Source terminators are commands, never substitute values for a missing
    // argument. Leftover properties also must not become executable commands.
    if(!Values.empty()) throw std::runtime_error("Unused expression return values");
    return Read_Value(Get_Raw_Data());
}

std::string Data_Source_Preprocessor::Read_Value(std::string Token)
{
    if(Token.size() >= 2 && Token.front() == '"' && Token.back() == '"')
        return Token.substr(1, Token.size() - 2);
    if(Token == "Var")
    {
        std::string Name = Get_Raw_Data();
        auto It = Raw_Data->Variables.find(Name);
        if(It == Raw_Data->Variables.end())
            throw std::runtime_error("Variable not found: " + Name);
        return It->second;
    }
    if(Token == ")") throw std::runtime_error("Unexpected )");
    if(Token != "(") return Token;

    std::vector<std::string> Script = Collect_Expression();
    if(!Execute) throw std::runtime_error("No expression interpreter configured");

    Return_Value_Data_Source Returned;
    Execute(Script, Returned);
    // Returning from Execute signals completion. An empty result is an error,
    // not a request to read the next argument from the outer script.
    if(Returned.Values.empty()) throw std::runtime_error("Expression returned no values");
    while(!Returned.Values.empty()) Values.push_back(Returned.Get_Data());
    return Get_Data();
}

std::vector<std::string> Data_Source_Preprocessor::Collect_Expression()
{
    // Collect a balanced expression without executing it during capture.
    // The temporary interpreter's preprocessor resolves its nested arguments.
    // Return/Done cannot cross this boundary into the caller's source.
    std::vector<std::string> Script;
    int Depth = 1;
    while(Depth > 0)
    {
        std::string Token = Get_Raw_Data();
        if(Token == "Return" || Token == "Done")
            throw std::runtime_error("Missing ) before end of source");
        if(Token == "(") ++Depth;
        if(Token == ")") --Depth;
        if(Depth > 0) Script.push_back(Token);
    }
    if(Script.empty()) throw std::runtime_error("Empty expression");
    return Script;
}

Data_Source_Preprocessor::Condition_Value Data_Source_Preprocessor::Capture_Condition()
{
    Condition_Value Condition;
    std::string Token = Get_Raw_Data();
    if(Token == "Var") Condition.Variable = Get_Raw_Data();
    else if(Token == "(") Condition.Expression = Collect_Expression();
    else Condition.Literal = Read_Value(Token);
    return Condition;
}

std::string Data_Source_Preprocessor::Resolve_Condition(const Condition_Value &Condition)
{
    if(!Condition.Variable.empty())
    {
        auto It = Raw_Data->Variables.find(Condition.Variable);
        if(It == Raw_Data->Variables.end())
            throw std::runtime_error("Variable not found: " + Condition.Variable);
        return It->second;
    }
    if(Condition.Expression.empty()) return Condition.Literal;
    Return_Value_Data_Source Returned;
    if(!Execute) throw std::runtime_error("No expression interpreter configured");
    Execute(Condition.Expression, Returned);
    if(Returned.Values.size() != 1)
        throw std::runtime_error("Condition expression must return exactly one value");
    return Returned.Get_Data();
}

void Data_Source_Preprocessor::Return_Value(std::string Value)
{
    if(Results != nullptr) Results->Add_Value(Value);
}

std::string Data_Source_Preprocessor::Get_Condition_Value()
{
    // If uses this immediate form; loops retain a Condition_Value instead.
    // Both require one result, so a second property cannot become the operator.
    Condition_Value Condition = Capture_Condition();
    if(!Condition.Variable.empty()) return "Var:" + Condition.Variable;
    return Resolve_Condition(Condition);
}
