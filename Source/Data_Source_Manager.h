#pragma once
#include <string>
#include <map>

#include "Named_List.h"
#include "Data_Source.h"



class Data_Source_Manager : public Data_Source
{
    public:
    std::map<std::string, std::string> Variables;

    //Raw Data Sources
    Named_List<Data_Source*> Data_Sources;

    void Add_Source(std::string Name, Data_Source *Source);
    void Remove_Source(std::string Name);

    // DSM supplies raw tokens only; preprocessing is the next pipeline stage.
    std::string Get_Data() override;
    Data_Source *Current_Source();

    void operator >> (std::string &Data);
    void operator >> (int &Data);
    void operator >> (float &Data);
    void operator >> (double &Data);

};

