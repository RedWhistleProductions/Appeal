#include "Appeal.h"
#include <iostream>

int main(int argc, char* argv[])
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
    
    return 0;
}
