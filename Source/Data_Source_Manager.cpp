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
    Data_Source *Source = Current_Source();
    return Source == nullptr ? "Done" : Source->Get_Data();
}

Data_Source *Data_Source_Manager::Current_Source()
{
    return Data_Sources.Nodes == 0 ? nullptr : Data_Sources.Out();
}

void Data_Source_Manager::operator >> (std::string &Data)
{
    Data = Get_Data();
}

void Data_Source_Manager::operator >> (int &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Int(Str);
}

void Data_Source_Manager::operator >> (float &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Float(Str);
}

void Data_Source_Manager::operator >> (double &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Double(Str);
}
