#pragma once
#include <string>
#include <map>

#include "Named_List.h"
#include "Data_Source.h"

class Data_Source_Manager
{
    public:
    std::map<std::string, std::string> Variables;
    Named_List<Data_Source*> Data_Sources;
    void Add_Source(std::string Name, Data_Source *Source);
    void Remove_Source(std::string Name);
    std::string Get_Data();
    void operator >> (std::string &Data);
    void operator >> (int &Data);
    void operator >> (float &Data);
    void operator >> (double &Data);
};

