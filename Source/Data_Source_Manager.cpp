#include "Data_Source_Manager.h"

void Data_Source_Manager::Add_Source(std::string Name, Data_Source *Source)
{
    Data_Sources.Add_Node(Name, Source);
}

void Data_Source_Manager::Remove_Source(std::string Name)
{
    Data_Sources.Remove(Name);
}

std::string Data_Source_Manager::Get_Data()
{
    std::string Value;
    if(Data_Sources.Nodes == 0)
    {
        std::cout << "Error: No Data Sources" << std::endl;
        return "";
    }

    Value = Data_Sources.Out()->Get_Data();
    if(Value == "Var")
    {
        std::string Var_Name;
        Var_Name = Data_Sources.Out()->Get_Data();
        auto it = Variables.find(Var_Name);
        if(it != Variables.end())
        {
            Value = it->second;
        }
        else
        {
            std::cout << "Error: Variable Not Found: " << Var_Name << std::endl;
            return "Done";
        }
    }
    return Value;
}

void Data_Source_Manager::operator >> (std::string &Data)
{
    Data = Get_Data();
}

void Data_Source_Manager::operator >> (int &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stoi(Str);
}

void Data_Source_Manager::operator >> (float &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stof(Str);
}

void Data_Source_Manager::operator >> (double &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stod(Str);
}