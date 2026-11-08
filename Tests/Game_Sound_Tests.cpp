#include "Appeal.h"
#include <windows.h>
#include <cstdlib>
#include <iostream>
#define CHECK(x) do {if(!(x)){std::cerr<<"Failed: " #x<<'\n';std::exit(1);}}while(false)
int main(int argc,char**argv){
    CHECK(argc==2);Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    auto mixer=GetModuleHandleA("SDL2_mixer.dll");CHECK(mixer);
    auto playing=reinterpret_cast<int(*)(int)>(GetProcAddress(mixer,"Mix_Playing"));
    auto halt=reinterpret_cast<int(*)(int)>(GetProcAddress(mixer,"Mix_HaltChannel"));CHECK(playing&&halt);
    CHECK(playing(-1)>0);halt(-1);
    app.Data_Manager.Variables["Sound_Enabled"]="0";app.Run_Function("Sound_Play_Cue");CHECK(playing(-1)==0);
    app.Data_Manager.Variables["Sound_Enabled"]="1";app.Run_Function("Sound_Play_Cue");CHECK(playing(-1)>0);
    halt(-1);std::cout<<"GAME_SOUND_TESTS_PASSED\n";
}
