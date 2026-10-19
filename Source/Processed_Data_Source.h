#pragma once
#include <queue>
#include <string>
#include "Data_Source.h"

class Processed_Data_Source : public Data_Source
{
	public:
	std::queue<std::string> Values;

    void Add_Value(std::string Value);
    std::string Get_Data() override;
};
