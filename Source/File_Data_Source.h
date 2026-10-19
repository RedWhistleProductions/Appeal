#pragma once

#include "Data_Source.h"
#include "File_IO.h"
#include <cctype>

class File_Data_Source: public Data_Source
{
    private:
        bool Valid;
        bool Exhausted = false;
    public:
        File_IO File;
        File_Data_Source();
        File_Data_Source(std::string File_Name);
        void Load(std::string File_Name);
        std::string Get_Data() override;
        bool End_Of_Source() const override { return Exhausted; }
        ~File_Data_Source();
};
