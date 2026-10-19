#pragma once
#include <string>
#include <stdexcept>
#include "Data_Conversion.h"


class Data_Source
{
    public:
    virtual std::string Get_Data();
    //Call Get_Data() using the >> operator
    void operator >>(std::string &Data);
    void operator >>(int &Data);
    void operator >>(float &Data);
    void operator >>(double &Data);
    void operator >>(bool &Data);

    // A command publishes its results through the source it reads arguments
    // from. A normal command discards them; an expression collects them.
    virtual void Return_Value(std::string Value)
    {
    }
    // Distinguish an exhausted source from a literal Return in a captured body.
    virtual bool End_Of_Source() const { return false; }
};

