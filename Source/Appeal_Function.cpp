#include "Appeal_Function.h"

void Appeal_Function::Add_Command(std::string Command)
{
    Command_Vector.push_back(Command);
}

std::string Appeal_Function::Get_Data()
{
    std::string Data;
    if(Line_Number < Command_Vector.size())
    {
        Data = Command_Vector[Line_Number];
        Line_Number++;
    }
    else 
    {
        Line_Number = 0;
        Data = "Return";
    }
    return Data;
}