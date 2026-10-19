#include "Appeal.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace
{
    bool File_Exists(const std::string &File_Name)
    {
        std::ifstream File(File_Name.c_str());
        return File.good();
    }

    std::string Default_User_Resource_Folder()
    {
        const char* User_Profile = std::getenv("USERPROFILE");
        if(User_Profile != nullptr)
        {
            return std::string(User_Profile) + "/Documents/Appeal/Resources";
        }

        const char* Home = std::getenv("HOME");
        if(Home != nullptr)
        {
            return std::string(Home) + "/Documents/Appeal/Resources";
        }

        return "./Resources";
    }
}

int main(int argc, char* argv[])
{
    Appeal App;

    // Determine location of the Resource_Folder
    const char* Resource_Folder = std::getenv("APPEAL_RESOURCE_FOLDER");

    if(Resource_Folder != nullptr)
    {
        App.Set_Resource_Folder(Resource_Folder);
    }
    else if(File_Exists("./Resources/Appeal/Main.Appeal"))
    {
        App.Set_Resource_Folder("./Resources");
    }
    else
    {
        App.Set_Resource_Folder(Default_User_Resource_Folder());
    }

    if(argc > 1)
    {
        App.Run(argv[1]);
    }
    else
    {
        App.Run("Main");
    }
    
    return 0;
}
