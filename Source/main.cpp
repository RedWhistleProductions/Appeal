#include "Appeal.h"

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
    
    while(App.Done == false){};
    
    return 0;
}
