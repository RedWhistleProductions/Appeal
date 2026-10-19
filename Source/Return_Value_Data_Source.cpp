#include "Return_Value_Data_Source.h"

void Return_Value_Data_Source::Add_Value(std::string Value)
{
    Values.push(Value);
}

std::string Return_Value_Data_Source::Get_Data()
{
    std::string Data;
	if(Values.empty()) return "Return";
	else
	{
		Data = Values.front();
		Values.pop();
		return Data;
	}
}

void Return_Value_Data_Source::Reset()
{
	//Make sure Values is cleared
	while(!Values.empty()){Values.pop();};
}
