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
    Exhausted = false;
    if(Valid)
    {
        File.Close();
    }
    File.Mode = "Input";
    Valid = File.Open(File_Name);
}

std::string File_Data_Source::Get_Data()
{
    if(!Valid) throw std::runtime_error("File data source is not open");
    // Preserve quotes for the preprocessor, including quotes attached to a
    // command's parentheses. File_IO's general data reader strips them too early.
    std::string Data;
    bool Quoted = false;
    char Character;
    while(File.iFile.get(Character))
    {
        if(!Quoted && std::isspace(static_cast<unsigned char>(Character)))
        {
            if(!Data.empty()) return Data;
            continue;
        }
        // Keep the existing script convention for whole-line comments.
        if(Data.empty() && (Character == '#' || Character == '/'))
        {
            std::getline(File.iFile, Data);
            Data.clear();
            continue;
        }
        if(Character == '"') Quoted = !Quoted;
        Data += Character;
    }
    if(Quoted) throw std::runtime_error("Unterminated quoted string in " + File.File);
    if(!Data.empty()) return Data;
    Exhausted = true;
    return "Return";
}

File_Data_Source::~File_Data_Source()
{
    if(Valid){File.Close();}
}
