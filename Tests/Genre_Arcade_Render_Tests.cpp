#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <GL/gl.h>
#include <windows.h>
#include "Appeal.h"
#include "imgui_internal.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#define CHECK(x) do {if(!(x)){std::cerr<<"Failed: " #x<<" line "<<__LINE__<<'\n';std::exit(1);}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
    CHECK(argc==5);Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    auto p=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
    auto ctx=Fn<ImGuiContext*(*)()>(p,"_ZN5ImGui17GetCurrentContextEv")();CHECK(ctx);
    auto getid=Fn<ImGuiID(*)(const char*)>(p,"_ZN5ImGui5GetIDEPKc");
    auto update=Fn<void(*)()>(p,"Update");
    auto frame=[&](){app.Run(argv[2]);};frame();frame();
    auto window=SDL_GetWindowFromID(1);CHECK(window);
    auto panel=[&](){ImGuiWindow*w=nullptr;for(auto*v:ctx->Windows)if(std::strcmp(v->Name,"Genre_Arcade")==0)w=v;CHECK(w);return w;};
    auto id=[&](const char*label){auto old=ctx->CurrentWindow;ctx->CurrentWindow=panel();auto value=getid(label);ctx->CurrentWindow=old;return value;};
    auto key=[&](SDL_Scancode code,bool down){SDL_Event e{};e.type=down?SDL_KEYDOWN:SDL_KEYUP;
        e.key.windowID=SDL_GetWindowID(window);e.key.state=down?SDL_PRESSED:SDL_RELEASED;
        e.key.keysym.scancode=code;e.key.keysym.sym=SDL_GetKeyFromScancode(code);CHECK(SDL_PushEvent(&e)==1);SDL_Delay(20);update();frame();};
    auto mouse=[&](int x,int y,int down){SDL_WarpMouseInWindow(window,x,y);SDL_Event e{};
        e.type=SDL_MOUSEMOTION;e.motion.windowID=SDL_GetWindowID(window);e.motion.x=x;e.motion.y=y;CHECK(SDL_PushEvent(&e)==1);
        if(down>=0){e={};e.type=down?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;e.button.windowID=SDL_GetWindowID(window);
            e.button.button=SDL_BUTTON_LEFT;e.button.state=down?SDL_PRESSED:SDL_RELEASED;e.button.x=x;e.button.y=y;CHECK(SDL_PushEvent(&e)==1);}
        update();frame();};
    auto click=[&](const char*label){auto desired=id(label);int found=-1,x=int(panel()->WorkRect.Min.x)+35;
        for(int y=int(panel()->WorkRect.Min.y);y<780;y+=10){mouse(x,y,-1);if(ctx->HoveredId==desired){found=y;break;}}
        if(found<0)std::cerr<<"Missing button "<<label<<" page="<<app.Data_Manager.Variables["A_Page"]<<'\n';
        CHECK(found>=0);mouse(x,found,1);mouse(x,found,0);frame();frame();};
    auto script=[&](const std::string&body){std::ofstream out(argv[3]);out<<body<<"\nReturn\n";out.close();app.Run(argv[3]);frame();frame();};
    auto shot=[&](const char*name){frame();glFinish();int width=1100,height=800;
        std::vector<unsigned char> rgb(width*height*3);glPixelStorei(GL_PACK_ALIGNMENT,1);unsigned long long best=0;
        for(GLenum buffer:{GL_FRONT,GL_BACK}){std::vector<unsigned char>pixels(width*height*3);glReadBuffer(buffer);
            glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());CHECK(glGetError()==GL_NO_ERROR);
            unsigned long long sum=0;for(auto value:pixels)sum+=value;if(sum>best){best=sum;rgb=std::move(pixels);}}
        CHECK(best>100000);std::ofstream out(std::string(argv[4])+name+".ppm",std::ios::binary);out<<"P6\n"<<width<<' '<<height<<"\n255\n";
        for(int y=height-1;y>=0;--y)out.write(reinterpret_cast<const char*>(rgb.data()+y*width*3),width*3);
    };
    shot("Hub");
    // Native keyboard navigation starts the first room.
    auto target=id("01  The Signal");key(SDL_SCANCODE_TAB,true);key(SDL_SCANCODE_TAB,false);
    for(int i=0;i<18&&ctx->NavId!=target;++i){key(SDL_SCANCODE_TAB,true);key(SDL_SCANCODE_TAB,false);}
    CHECK(ctx->NavId==target);key(SDL_SCANCODE_RETURN,true);key(SDL_SCANCODE_RETURN,false);frame();frame();
    CHECK(app.Data_Manager.Variables["A_Page"]=="Play");
    click("Read / search");click("Take brass key");click("Unlock lighthouse");
    CHECK(app.Data_Manager.Variables["A_Result"]=="Win");shot("Story");
    click("Replay");CHECK(app.Data_Manager.Variables["A_Result"].empty());
    click("Pause / resume");CHECK(app.Data_Manager.Variables["Game_Paused"]=="1");
    click("Pause / resume");CHECK(app.Data_Manager.Variables["Game_Paused"]=="0");
    click("Return to hub");click("Previous page");click("Previous page");click("Previous page");CHECK(app.Data_Manager.Variables["A_Hub_Page"]=="1");
    click("09  Lane Patrol");CHECK(app.Data_Manager.Variables["A_Index"]=="9");
    click("Lane left");click("Fire");CHECK(app.Data_Manager.Variables["A_Score"]=="1");shot("Shooter");
    // Render every catalog entry through real Graphics, including unavailable rooms.
    for(int i=1;i<=31;++i){script("Set_Var A_Index "+std::to_string(i)+"\nCall Arcade_Start");
        bool gap=i==21||i==23||i==24||i==25||i==26||i==30||i==31;
        CHECK(app.Data_Manager.Variables["A_Page"]==(gap?"Gap":"Play"));
        if(i==18)shot("Ruins");if(i==31)shot("Gap");
    }
    click("Return to hub");click("Quit");CHECK(app.Data_Manager.Variables["A_Quit"]=="1");
    Fn<void(*)()>(p,"Close_All")();std::cout<<"GENRE_ARCADE_RENDER_TESTS_PASSED\n";
}
