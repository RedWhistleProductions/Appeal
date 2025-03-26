#include "Plugin.h"

#ifdef __linux__
    void Plugin::Load(std::string File)
    {
        if(handle != 0)
        {
            Destructor();
            dlclose(handle);
        }
    
        handle = dlopen (File.c_str(), RTLD_LAZY);
        if (!handle) 
        {
            fputs (dlerror(), stderr);
            exit(1);
        }
        else
        {
            std::cout << "Loaded: " << File << std::endl;
        }

        Assign("Constructor", Constructor);
        Assign("Destructor", Destructor);
        if(Constructor != nullptr) Constructor();
    }

    Plugin::~Plugin()
    {
        if(Destructor != nullptr)
        {
            Destructor();
        } 
        if(handle != 0)
        {
            dlclose(handle);
        }   
    }

#endif

#ifdef __MINGW32__
    void Plugin::Load(std::string File)
    {    
        std::wstring wFile(File.begin(), File.end());
        handle = LoadLibraryW(wFile.c_str());
        if (!handle)
        {
            std::cout << wFile.c_str() << std::endl;
            std::cerr << "Failed to load Plugin: " << GetLastError() << std::endl;
            
            exit(1);
        }  
    }

    Plugin::~Plugin()
    {
        FreeLibrary(handle);
    }
#endif