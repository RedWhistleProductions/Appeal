#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#define CHECK(x) do{if(!(x))throw std::runtime_error("HTTPS driver check failed at line "+std::to_string(__LINE__));}while(false)
struct Library {
#ifdef _WIN32
    HMODULE Handle=nullptr;
#else
    void *Handle=nullptr;
#endif
    void (*Destroy)()=nullptr;
    explicit Library(const char *name){
#ifdef _WIN32
        Handle=LoadLibraryW(std::filesystem::u8path(name).c_str());
#else
        Handle=dlopen(name,RTLD_NOW|RTLD_LOCAL);
#endif
        CHECK(Handle);Get<void(*)()>("Constructor")();Destroy=Get<void(*)()>("Destructor");
    }
    ~Library(){if(Destroy)Destroy();
#ifdef _WIN32
        if(Handle)FreeLibrary(Handle);
#else
        if(Handle)dlclose(Handle);
#endif
    }
    template<class T>T Get(const char *name){
#ifdef _WIN32
        auto raw=GetProcAddress(Handle,name);T result=nullptr;static_assert(sizeof(result)==sizeof(raw));std::memcpy(&result,&raw,sizeof(raw));
#else
        auto result=reinterpret_cast<T>(dlsym(Handle,name));
#endif
        CHECK(result);return result;
    }
};
int main(int argc,char **argv)try{
    CHECK(argc==7);Library plugin(argv[1]);
    auto configure=plugin.Get<void(*)(std::string,std::string,int)>("Configure");
    auto tls=plugin.Get<bool(*)(std::string,std::string)>("Configure_TLS");auto disable=plugin.Get<void(*)(std::string)>("Disable");
    auto server=plugin.Get<void(*)()>("Server"),stop=plugin.Get<void(*)()>("Stop_Server");auto running=plugin.Get<bool(*)()>("Is_Server_Running");
    CHECK(tls(argv[2],argv[3]));plugin.Get<void(*)(std::string)>("Set_HTML_Path")(argv[4]);
    for(auto name:{"ECHO","CHAT","APPEAL"})disable(name);
    configure("HTTPS","127.0.0.1",std::stoi(argv[5]));
    if(std::stoi(argv[6])==0)disable("HTTP");else configure("HTTP","127.0.0.1",std::stoi(argv[6]));
    server();CHECK(running());std::cout<<"READY"<<std::endl;
    std::string command;while(std::getline(std::cin,command)){
        if(command=="stop"){stop();CHECK(!running());std::cout<<"STOPPED"<<std::endl;}
        else if(command=="restart"){stop();server();CHECK(running());std::cout<<"RESTARTED"<<std::endl;}
        else if(command=="quit")break;
    }
    stop();return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<std::endl;return 1;}
