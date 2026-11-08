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
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);if(!f)std::cerr<<n<<'\n';CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
    CHECK(argc==4);Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    auto p=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
    auto ctx=Fn<ImGuiContext*(*)()>(p,"_ZN5ImGui17GetCurrentContextEv")();CHECK(ctx);
    auto getid=Fn<ImGuiID(*)(const char*)>(p,"_ZN5ImGui5GetIDEPKc");
    auto update=Fn<void(*)()>(p,"Update");
    auto frame=[&](){app.Run(argv[2]);};frame();frame();
    auto window=SDL_GetWindowFromID(1);CHECK(window);
    auto panel=[&](){ImGuiWindow* w=nullptr;for(auto* v:ctx->Windows)if(std::strcmp(v->Name,"Five_Questions")==0)w=v;CHECK(w);return w;};
    auto id=[&](const char*label){auto old=ctx->CurrentWindow;ctx->CurrentWindow=panel();auto value=getid(label);ctx->CurrentWindow=old;return value;};
    auto key=[&](SDL_Scancode code,bool down){SDL_Event e{};e.type=down?SDL_KEYDOWN:SDL_KEYUP;
        e.key.windowID=SDL_GetWindowID(window);e.key.state=down?SDL_PRESSED:SDL_RELEASED;
        e.key.keysym.scancode=code;e.key.keysym.sym=SDL_GetKeyFromScancode(code);CHECK(SDL_PushEvent(&e)==1);SDL_Delay(20);update();frame();};
    auto target=id("New game");
    key(SDL_SCANCODE_TAB,true);key(SDL_SCANCODE_TAB,false);
    for(int i=0;i<12&&ctx->NavId!=target;++i){key(SDL_SCANCODE_TAB,true);key(SDL_SCANCODE_TAB,false);}
    CHECK(ctx->NavId==target);key(SDL_SCANCODE_RETURN,true);key(SDL_SCANCODE_RETURN,false);frame();frame();
    if(app.Data_Manager.Variables["Quiz_Page"]!="Play"){
        for(const char*k:{"Quiz_Page","Controls_Event","Controls_Context","Controls_Allowed","Quiz_Status"})std::cerr<<k<<'='<<app.Data_Manager.Variables[k]<<'\n';
    }
    CHECK(app.Data_Manager.Variables["Quiz_Page"]=="Play");
    auto mouse=[&](int x,int y,int down){SDL_WarpMouseInWindow(window,x,y);SDL_Event e{};
        e.type=SDL_MOUSEMOTION;e.motion.windowID=SDL_GetWindowID(window);e.motion.x=x;e.motion.y=y;CHECK(SDL_PushEvent(&e)==1);
        if(down>=0){e={};e.type=down?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;e.button.windowID=SDL_GetWindowID(window);
            e.button.button=SDL_BUTTON_LEFT;e.button.state=down?SDL_PRESSED:SDL_RELEASED;e.button.x=x;e.button.y=y;CHECK(SDL_PushEvent(&e)==1);}
        update();frame();};
    auto click=[&](const char*label){
        auto desired=id(label);int x=int(panel()->WorkRect.Min.x)+50,found=-1;
        for(int y=int(panel()->WorkRect.Min.y);y<600;y+=12){mouse(x,y,-1);if(ctx->HoveredId==desired){found=y;break;}}
        if(found<0)std::cerr<<"Button not found: "<<label<<" page="<<app.Data_Manager.Variables["Quiz_Page"]<<'\n';
        CHECK(found>=0);mouse(x,found,1);mouse(x,found,0);frame();frame();
    };
    click("56");CHECK(app.Data_Manager.Variables["Round_Score"]=="1");click("Next");
    click("Pause");CHECK(app.Data_Manager.Variables["Quiz_Page"]=="Pause");
    click("Save and return to title");click("Continue");
    CHECK(app.Data_Manager.Variables["Quiz_Page"]=="Pause");click("Resume");
    CHECK(app.Data_Manager.Variables["Round_Item"]=="2");
    for(const char*answer:{"Mercury","3","0 degrees","Pacific"}){click(answer);click("Next");}
    CHECK(app.Data_Manager.Variables["Quiz_Page"]=="Results");
    CHECK(app.Data_Manager.Variables["Round_Outcome"]=="Win");CHECK(app.Data_Manager.Variables["Round_Score"]=="5");
    // Capture the actual rendered result screen for visual inspection.
    frame();glFinish();int width=960,height=640;
    std::vector<unsigned char> rgb(width*height*3);glPixelStorei(GL_PACK_ALIGNMENT,1);
    unsigned long long best=0;
    for(GLenum buffer:{GL_FRONT,GL_BACK}){
        std::vector<unsigned char> pixels(width*height*3);glReadBuffer(buffer);
        glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());CHECK(glGetError()==GL_NO_ERROR);
        unsigned long long sum=0;for(auto value:pixels)sum+=value;
        if(sum>best){best=sum;rgb=std::move(pixels);}
    }
    CHECK(best>100000);
    std::ofstream out(argv[3],std::ios::binary);out<<"P6\n"<<width<<' '<<height<<"\n255\n";
    for(int y=height-1;y>=0;--y)out.write(reinterpret_cast<const char*>(rgb.data()+y*width*3),width*3);out.close();
    click("Play again");CHECK(app.Data_Manager.Variables["Round_Score"]=="0");
    for(const char*answer:{"54","Venus","4","10 degrees","Atlantic"}){click(answer);click("Next");}
    CHECK(app.Data_Manager.Variables["Round_Outcome"]=="Lose");CHECK(app.Data_Manager.Variables["Round_Score"]=="0");
    click("Back to title");click("Options");click("Toggle sound");CHECK(app.Data_Manager.Variables["Quiz_Sound"]=="0");
    click("Back to title");click("Quit");CHECK(app.Data_Manager.Variables["Quiz_Quit"]=="1");
    Fn<void(*)()>(p,"Close_All")();std::cout<<"FIVE_QUESTIONS_RENDER_TESTS_PASSED\n";
}
