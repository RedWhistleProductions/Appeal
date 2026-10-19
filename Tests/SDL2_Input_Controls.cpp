#include <SDL.h>
#include <iostream>
#include <string>
#include <cstdlib>
extern "C" void Constructor();
extern "C" void Destructor();
extern "C" void Update(std::string);
extern "C" bool Set_Controller_Deadzone(int);
extern "C" void Open_Controller(int);
extern "C" void Close_Controller(int);
extern "C" int Controller_Axis(int, std::string);
extern "C" void Set_Controller_Axis_For(int, std::string, std::string);
extern "C" void Set_Controller_Button_Down_For(int, std::string, std::string);
extern "C" void Set_Controller_Button_Repeat_For(int, std::string, bool);
extern "C" void Set_Function_Runner(void(*)(std::string), void(*)(std::string,int));
int Map_Controller_Axis(int);
bool Repeat_Due(const std::string&, bool, bool, bool, Uint32);
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << std::endl; std::exit(1); } } while(false)
int Presses = 0, Analog = 999;
void Digital(std::string Name) { if(Name == "TestPress") ++Presses; }
void Axis(std::string Name, int Value) { if(Name == "TestAxis") Analog = Value; }
int main() {
    CHECK(Set_Controller_Deadzone(0));
    CHECK(Map_Controller_Axis(-32768) == -100 && Map_Controller_Axis(32767) == 100);
    CHECK(Map_Controller_Axis(16384) == 50 && Map_Controller_Axis(-16384) == -50);
    CHECK(Set_Controller_Deadzone(25));
    CHECK(Map_Controller_Axis(8000) == 0 && Map_Controller_Axis(-8000) == 0);
    CHECK(Map_Controller_Axis(20480) == 50);
    CHECK(!Set_Controller_Deadzone(-1) && !Set_Controller_Deadzone(100));
    CHECK(Map_Controller_Axis(8000) == 0);
    CHECK(Repeat_Due("test",true,true,false,0));
    CHECK(!Repeat_Due("test",true,false,false,500));
    CHECK(Repeat_Due("test",true,false,true,500));
    CHECK(!Repeat_Due("test",true,false,true,550));
    CHECK(Repeat_Due("test",true,false,true,600));
    CHECK(!Repeat_Due("test",false,false,true,700));
    CHECK(Repeat_Due("other",true,true,false,700));
    CHECK(!Repeat_Due("other",true,false,false,900));
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
    Constructor();
    Set_Function_Runner(Digital,Axis);
    int Device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER,6,21,0);
    CHECK(Device >= 0 && SDL_IsGameController(Device));
    SDL_Joystick *Virtual = SDL_JoystickOpen(Device);
    CHECK(Virtual);
    Open_Controller(Device);
    Set_Controller_Axis_For(Device,"Left_Stick_X","TestAxis");
    Set_Controller_Button_Down_For(Device,"DPad_Up","TestPress");
    Update("Default");
    CHECK(SDL_JoystickSetVirtualAxis(Virtual,SDL_CONTROLLER_AXIS_LEFTX,32767) == 0);
    Update("Default");
    CHECK(Analog == 100 && Controller_Axis(Device,"Left_Stick_X") == 100);
    CHECK(SDL_JoystickSetVirtualAxis(Virtual,SDL_CONTROLLER_AXIS_LEFTX,1000) == 0);
    Update("Default");
    CHECK(Analog == 0 && Controller_Axis(Device,"Left_Stick_X") == 0);
    CHECK(SDL_JoystickSetVirtualButton(Virtual,SDL_CONTROLLER_BUTTON_DPAD_UP,1) == 0);
    Update("Default");
    CHECK(Presses == 1);
    SDL_Delay(110);
    Update("Default");
    CHECK(Presses == 1);
    Set_Controller_Button_Repeat_For(Device,"DPad_Up",true);
    Update("Default");
    CHECK(Presses == 2);
    CHECK(SDL_JoystickSetVirtualButton(Virtual,SDL_CONTROLLER_BUTTON_DPAD_UP,0) == 0);
    Update("Default");
    SDL_Delay(110);
    Update("Default");
    CHECK(Presses == 2);
    CHECK(SDL_JoystickDetachVirtual(Device) == 0);
    Update("Default");
    CHECK(Controller_Axis(Device,"Left_Stick_X") == 0);
    SDL_JoystickClose(Virtual);
    Close_Controller(-1);
    Destructor();
    std::cout << "SDL2_INPUT_CONTROLS_PASSED\n";
}
