#pragma once
#include <vector>
#include <string>
#include "Data_Source.h"

class Appeal_Function: public Data_Source
{
    private:
        unsigned int Line_Number = 0;
    public:
        std::vector<std::string>Command_Vector;

        void Add_Command(std::string Command);
        std::string Get_Data() override;
};

