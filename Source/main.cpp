#include "Appeal.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    Appeal App;
    const char* Resource_Folder = std::getenv("APPEAL_RESOURCE_FOLDER");

    if(Resource_Folder != nullptr)
    {
        App.Set_Resource_Folder(Resource_Folder);
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
