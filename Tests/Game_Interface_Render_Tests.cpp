#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Appeal.h"
#include "imgui_internal.h"
#include <windows.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
template<class T> T Fn(HMODULE p,const char* name) { auto f=GetProcAddress(p,name); CHECK(f); return reinterpret_cast<T>(f); }
int main(int argc,char**argv) {
    CHECK(argc==3);
    Appeal app; app.Set_Resource_Folder("./Resources"); app.Run(argv[1]);
    auto plugin=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll"); CHECK(plugin);
    auto context=Fn<ImGuiContext*(*)()>(plugin,"_ZN5ImGui17GetCurrentContextEv")(); CHECK(context);
    auto frame=[&](){app.Run(argv[2]);};
    frame(); frame(); frame();
    ImGuiWindow* panel=nullptr;
    for(auto* w:context->Windows) if(std::strcmp(w->Name,"Menu Test")==0) panel=w;
    CHECK(panel);
    auto window=SDL_GetWindowFromID(1); CHECK(window);
    auto update=Fn<void(*)()>(plugin,"Update");
    int x=static_cast<int>(panel->WorkRect.Min.x)+50;
    int y=static_cast<int>(panel->WorkRect.Min.y+context->FontSize+2*context->Style.FramePadding.y+context->Style.ItemSpacing.y+8);
    auto mouse=[&](int down) {
        SDL_WarpMouseInWindow(window,x,y);
        SDL_Event e{};e.type=SDL_MOUSEMOTION;e.motion.windowID=SDL_GetWindowID(window);e.motion.x=x;e.motion.y=y;
        CHECK(SDL_PushEvent(&e)==1);
        if(down>=0) {
            e={};e.type=down?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;
            e.button.windowID=SDL_GetWindowID(window);e.button.button=SDL_BUTTON_LEFT;
            e.button.state=down?SDL_PRESSED:SDL_RELEASED;e.button.x=x;e.button.y=y;
            CHECK(SDL_PushEvent(&e)==1);
        }
        update();frame();
    };
    mouse(-1); mouse(1); mouse(0); frame(); frame();
    CHECK(app.Data_Manager.Variables["Chosen"]=="second");
    CHECK(app.Data_Manager.Variables["Actions"]=="1");
    for(int i=0;i<4;++i) frame();
    CHECK(app.Data_Manager.Variables["Actions"]=="1");
    // Native GUI navigation: Tab to a button and activate it with Enter.
    auto key=[&](SDL_Scancode code,bool down) {
        SDL_Event e{};e.type=down?SDL_KEYDOWN:SDL_KEYUP;e.key.windowID=SDL_GetWindowID(window);
        e.key.state=down?SDL_PRESSED:SDL_RELEASED;e.key.keysym.scancode=code;e.key.keysym.sym=SDL_GetKeyFromScancode(code);
        CHECK(SDL_PushEvent(&e)==1);update();frame();
    };
    key(SDL_SCANCODE_TAB,true);key(SDL_SCANCODE_TAB,false);frame();
    key(SDL_SCANCODE_RETURN,true);key(SDL_SCANCODE_RETURN,false);frame();frame();
    CHECK(app.Data_Manager.Variables["Actions"]=="2");
    Fn<void(*)()>(plugin,"Close_All")();
    std::cout << "GAME_INTERFACE_RENDER_TESTS_PASSED\n";
}
