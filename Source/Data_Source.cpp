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
    Data = Data_Conversion::To_Int(Str);
}

void Data_Source::operator >> (float &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Float(Str);
}

void Data_Source::operator >> (double &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Double(Str);
}

void Data_Source::operator >> (bool &Data)
{
    std::string Str;
    *this >> Str;
    Data = Data_Conversion::To_Bool(Str);
}
