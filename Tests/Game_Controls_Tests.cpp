#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Appeal.h"
#include <iostream>
#include <cstdlib>
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
int main(int argc,char**argv) {
    CHECK(argc==3);SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
    CHECK(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_GAMECONTROLLER)==0);
    auto window=SDL_CreateWindow("Controls test",0,0,320,240,SDL_WINDOW_HIDDEN);CHECK(window);
    int device=SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER,6,21,0);CHECK(device==0);
    auto joystick=SDL_JoystickOpen(device);CHECK(joystick);
    Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    auto frame=[&](){app.Run(argv[2]);};frame();frame();
    auto key=[&](bool down){CHECK(SDL_JoystickSetVirtualButton(joystick,SDL_CONTROLLER_BUTTON_A,down)==0);frame();};
    key(true);CHECK(app.Data_Manager.Variables["Controls_Pressed"]=="1");
    CHECK(app.Data_Manager.Variables["Count"]=="1");
    frame();CHECK(app.Data_Manager.Variables["Count"]=="1");
    key(false);CHECK(app.Data_Manager.Variables["Controls_Released"]=="1");
    CHECK(SDL_JoystickSetVirtualAxis(joystick,SDL_CONTROLLER_AXIS_LEFTX,32767)==0);frame();
    CHECK(app.Data_Manager.Variables["Controls_Axis"]=="100.000000" || app.Data_Manager.Variables["Controls_Axis"]=="100");
    app.Run_Function("Pause");key(true);CHECK(app.Data_Manager.Variables["Controls_Down"]=="0");
    CHECK(app.Data_Manager.Variables["Count"]=="1");key(false);
    app.Run_Function("Resume");app.Run_Function("GUI");key(true);
    CHECK(app.Data_Manager.Variables["Controls_Down"]=="0");
    app.Run_Function("Queue_UI");frame();CHECK(app.Data_Manager.Variables["Count"]=="2");
    app.Data_Manager.Variables["Controls_Blocked"]="1";app.Run_Function("Queue_UI");frame();
    CHECK(app.Data_Manager.Variables["Count"]=="2");
    SDL_JoystickDetachVirtual(device);SDL_JoystickClose(joystick);
    SDL_DestroyWindow(window);std::cout<<"GAME_CONTROLS_TESTS_PASSED\n";
}
