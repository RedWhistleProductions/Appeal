#pragma once
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include "Data_Source_Manager.h"
#include "Return_Value_Data_Source.h"

// Pipeline: selected file/socket/function -> DSM -> this source -> interpreter.
// Each interpreter invocation has its own instance, so a nested expression
// cannot consume tokens belonging to the command waiting for its result.
class Data_Source_Preprocessor : public Data_Source
{
public:
    Data_Source_Manager *Raw_Data = nullptr;
    Return_Value_Data_Source *Results = nullptr;
    std::function<void(const std::vector<std::string>&, Return_Value_Data_Source&)> Execute;

    std::string Get_Data() override;
    std::string Get_Command();
    void Return_Value(std::string Value) override;

    // Capture uses unexpanded tokens: function bodies must run later, not now.
    std::string Get_Raw_Data();
    std::string Get_Condition_Value();

    // A loop retains the expression itself so getters run again each iteration.
    // A string value alone would freeze a changing mouse/button query forever.
    struct Condition_Value
    {
        std::string Literal;
        std::string Variable;
        std::vector<std::string> Expression;
    };
    Condition_Value Capture_Condition();
    std::string Resolve_Condition(const Condition_Value &Condition);

private:
    // Keep split tokens with their source when Run temporarily selects a file.
    std::map<Data_Source*, std::deque<std::string>> Pending;
    std::deque<std::string> Values;
    std::string Read_Value(std::string Token);
    std::vector<std::string> Collect_Expression();
    void Split_Token(const std::string &Token, std::deque<std::string> &Output);
};
