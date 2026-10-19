#include "Processed_Data_Source.h"

void Processed_Data_Source::Add_Value(std::string Value)
{
    Values.push(Value);
}

std::string Processed_Data_Source::Get_Data()
{
	/* NOTE:
		The Data_Source_Manager insures there is Data
		in the queue before calling this function
	*/
    std::string Data;
	Data = Values.front();
	Values.pop();
	return Data;
}
