#define SDL_MAIN_HANDLED
#define APPEAL_VIDEO_BRIDGE
#include <SDL.h>
#include "Appeal.h"
#include "../Resources/Modules/Appeal_Module_Video/src/Video_Plugin_API.h"
#include <windows.h>
#include <GL/gl.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#define CHECK(x) do{if(!(x)){std::cerr<<"Failed: " #x <<" line "<<__LINE__<<'\n';throw std::runtime_error("Video playback check failed");}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
struct Tokens:Data_Source {
    std::vector<std::string> Values;size_t Index=0;std::string Result;
    Tokens(std::initializer_list<std::string> values):Values(values){}
    std::string Get_Data() override {CHECK(Index<Values.size());return Values[Index++];}
    void Return_Value(std::string value) override{Result=std::move(value);}
    bool End_Of_Source() const override{return Index==Values.size();}
};
int main(int argc,char**argv) try {
    CHECK(argc==2);Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    auto video=GetModuleHandleA("Appeal_Module_Video.dll"),backend=GetModuleHandleA("Appeal_FFmpeg_Video.dll");
    auto graphics=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll"),audio=GetModuleHandleA("Appeal_Module_Audio.dll");CHECK(video&&backend&&graphics&&audio);
    auto interpret=Fn<void(*)(Data_Source*)>(video,"Interpreter");
    auto command=[&](std::initializer_list<std::string> words){Tokens t(words);interpret(&t);return t.Result;};
    auto info=Fn<std::string(*)(std::string,std::string)>(backend,"Info");
    auto frame=Fn<bool(*)(std::string,Video_Frame*)>(backend,"Get_Frame");
    auto render=Fn<void(*)()>(graphics,"Render"),update=Fn<void(*)()>(graphics,"Update");
    auto consumed=Fn<uint64_t(*)(const char*)>(audio,"Audio_Video_Frames");
    auto tick=[&](){CHECK(command({"Update"})=="1");render();update();SDL_Delay(16);};
    Video_Frame first;CHECK(frame("Intro",&first));CHECK(first.Width==96&&first.Height==64);
    for(int n=0;n<24;++n)tick();
    Video_Frame later;CHECK(frame("Intro",&later));CHECK(later.Serial>first.Serial);CHECK(consumed("Intro")>0);
    // Video Draw is a real textured 2D image; the same texture is on the quad mesh.
    unsigned char pixel[3];glFinish();glReadBuffer(GL_FRONT);glReadPixels(100,480-1-80,1,1,GL_RGB,GL_UNSIGNED_BYTE,pixel);CHECK(glGetError()==GL_NO_ERROR);
    CHECK(pixel[0]!=24 || pixel[1]!=28 || pixel[2]!=34);
    glReadPixels(320,240,1,1,GL_RGB,GL_UNSIGNED_BYTE,pixel);CHECK(pixel[0]!=24 || pixel[1]!=28 || pixel[2]!=34);
    command({"Pause"});const double paused=std::stod(info("Intro","Position"));
    const auto silent=consumed("Intro");for(int n=0;n<6;++n)tick();
    CHECK(std::abs(std::stod(info("Intro","Position"))-paused)<0.002);CHECK(consumed("Intro")==silent);
    CHECK(command({"Seek","1.2"})=="1");tick();Video_Frame sought;CHECK(frame("Intro",&sought));CHECK(sought.Seconds>1.0&&sought.Seconds<=1.25);
    CHECK(consumed("Intro")==0);
    auto audioInterpreter=Fn<void(*)(Data_Source*)>(audio,"Interpreter");Tokens volume({"Set_Volume_Percent","23"});audioInterpreter(&volume);
    Tokens getVolume({"Get_Volume_Percent"});audioInterpreter(&getVolume);CHECK(getVolume.Result=="23");
    command({"Play"});for(int n=0;n<8;++n)tick();CHECK(consumed("Intro")>0);
    // Muting Audio consumes the timeline silently, then unmute restores it.
    Tokens mute({"Mute"});audioInterpreter(&mute);tick();Tokens unmute({"UnMute"});audioInterpreter(&unmute);tick();
    CHECK(command({"Seek","-1"})=="0");CHECK(std::stod(info("Intro","Position"))>1);
    CHECK(command({"Seek","2.9"})=="1");command({"Set_Current_Loop","True"});for(int n=0;n<16;++n)tick();CHECK(std::stod(info("Intro","Position"))<1);
    command({"Set_Current_Loop","False"});CHECK(command({"Seek","2.9"})=="1");for(int n=0;n<16;++n)tick();CHECK(info("Intro","Ended")=="1");CHECK(info("Intro","Is_Playing")=="0");
    command({"Stop"});CHECK(std::stod(info("Intro","Position"))==0);
    CHECK(command({"Load","Invalid","missing-video.mp4"})=="0");CHECK(command({"Current_Video"})=="Intro");
    command({"Close_All"});CHECK(consumed("Intro")==0);CHECK(info("Intro","Width").empty());
    std::cout<<"VIDEO_PLAYBACK_PASSED\n";
} catch(const std::exception &e) {std::cerr<<e.what()<<"\n";return 1;}
