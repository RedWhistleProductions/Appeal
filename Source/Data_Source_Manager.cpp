#include "Data_Source_Manager.h"
#include <iostream>

namespace
{
    int To_Int(const std::string &Str)
    {
        try
        {
            return std::stoi(Str);
        }
        catch(const std::exception &)
        {
            std::cerr << "Warning: expected integer but got '" << Str << "'. Using 0." << std::endl;
            return 0;
        }
    }

    float To_Float(const std::string &Str)
    {
        try
        {
            return std::stof(Str);
        }
        catch(const std::exception &)
        {
            std::cerr << "Warning: expected float but got '" << Str << "'. Using 0." << std::endl;
            return 0.0f;
        }
    }

    double To_Double(const std::string &Str)
    {
        try
        {
            return std::stod(Str);
        }
        catch(const std::exception &)
        {
            std::cerr << "Warning: expected double but got '" << Str << "'. Using 0." << std::endl;
            return 0.0;
        }
    }
}

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
    Data = To_Int(Str);
}

void Data_Source_Manager::operator >> (float &Data)
{
    std::string Str;
    *this >> Str;
    Data = To_Float(Str);
}

void Data_Source_Manager::operator >> (double &Data)
{
    std::string Str;
    *this >> Str;
    Data = To_Double(Str);
}
