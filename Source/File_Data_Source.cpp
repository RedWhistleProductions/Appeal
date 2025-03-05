#include "File_Data_Source.h"


File_Data_Source::File_Data_Source()
{
    Valid = false;
}

File_Data_Source::File_Data_Source(std::string File_Name)
{
    Valid = File.Open(File_Name);
}

void File_Data_Source::Load(std::string File_Name)
{
    Valid = File.Open(File_Name);
}

std::string File_Data_Source::Get_Data()
{
    std::string Data;
    if(Valid)
    {   
        File >> Data;
    }
    else
    {
        Data = "Error: File Not Found";
    }
    return Data;
}

File_Data_Source::~File_Data_Source()
{
    if(Valid){File.Close();}
}