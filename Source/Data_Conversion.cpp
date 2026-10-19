#include "Data_Conversion.h"
#include <iostream>

namespace Data_Conversion
{
    int To_Int(const std::string &Str)
    {
        try
        {
            size_t Parsed = 0;
            int Value = std::stoi(Str, &Parsed);
            if(Parsed != Str.find_last_not_of(" \t\r\n") + 1)
            {
                std::cerr << "Warning: extra characters after integer value: '" << Str << "'" << std::endl;
            }
            return Value;
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

    bool To_Bool(const std::string &Str)
    {
        return To_Int(Str) != 0;
    }
}
