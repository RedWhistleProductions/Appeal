#pragma once

#include "Data_Source.h"
#include "File_IO.h"

class File_Data_Source: public Data_Source
{
    private:
        bool Valid;
    public:
        File_IO File;
        File_Data_Source();
        File_Data_Source(std::string File_Name);
        void Load(std::string File_Name);
        std::string Get_Data() override;
        ~File_Data_Source();
};
