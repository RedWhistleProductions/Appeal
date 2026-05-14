#include "Data_Source.h"

std::string Data_Source::Get_Data()
{
    return "";
}


void Data_Source::operator >> (std::string &Data)
{
    Data = Get_Data();
}

void Data_Source::operator >> (int &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stoi(Str);
}

void Data_Source::operator >> (float &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stof(Str);
}

void Data_Source::operator >> (double &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stod(Str);
}

void Data_Source::operator >> (bool &Data)
{
    std::string Str;
    *this >> Str;
    Data = std::stoi(Str);
}
