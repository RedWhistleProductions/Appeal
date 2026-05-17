#include "Appeal.h"
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        Appeal App;

        if(argc > 1)
        {
            App.Run(argv[1]);
        }
        else
        {
            App.Run("Main");
        }
    }
    catch(const std::exception &Error)
    {
        std::cerr << "Fatal runtime error: " << Error.what() << std::endl;
        return 1;
    }
    
    return 0;
}
