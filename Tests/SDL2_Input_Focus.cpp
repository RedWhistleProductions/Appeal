#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

extern "C" {
void Constructor(); void Destructor(); void Update(std::string);
bool Set_Focus_Mouse_Button(std::string);
bool Set_Focus(std::string); void Toggle_Focus(); std::string Focus();
void Open_Controller(int); void Close_Controller(int);
int Controller_Axis(int, std::string); bool Controller_Down(int, std::string);
bool Controller_Pressed(int, std::string);
void Set_Controller_Axis_For(int, std::string, std::string);
void Set_Controller_Button_Down_For(int, std::string, std::string);
void Set_Function_Runner(void(*)(std::string), void(*)(std::string,int));
void Bind_Key(std::string, std::string); bool Down(std::string);
int Mouse_X(); int Mouse_Wheel();
}
extern std::map<SDL_Scancode, bool> Key_Now;
int Count = 0, Axis_Count = 0;
void Digital(std::string) { ++Count; }
void Analog(std::string, int) { ++Axis_Count; }
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
void F1(int Repeat = 0)
{
    SDL_Event Event{};
    Event.type = SDL_KEYDOWN;
    Event.key.keysym.scancode = SDL_SCANCODE_F1;
    Event.key.repeat = Repeat;
    CHECK(SDL_PushEvent(&Event) == 1);
    Update("Default");
}
int main()
{
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    Constructor(); Set_Function_Runner(Digital, Analog);
    CHECK(Focus() == "Game");
    CHECK(!Set_Focus("Invalid") && Focus() == "Game");
    Bind_Key("Move", "D"); Key_Now[SDL_SCANCODE_D] = true;
    CHECK(Down("Move"));
    CHECK(Set_Focus("GUI"));
    CHECK(std::string(SDL_GetHint("APPEAL_INPUT_FOCUS")) == "GUI");
    CHECK(!Down("Move") && Mouse_X() == 0 && Mouse_Wheel() == 0);
    CHECK(!Set_Focus_Mouse_Button("Left"));
    CHECK(Set_Focus_Mouse_Button("Middle"));
    SDL_Event Middle{}; Middle.type = SDL_MOUSEBUTTONDOWN; Middle.button.button = SDL_BUTTON_MIDDLE;
    CHECK(SDL_PushEvent(&Middle) == 1); Update("Default");
    CHECK(Focus() == "Game" && Count == 0);
    CHECK(SDL_PushEvent(&Middle) == 1); Update("Default");
    CHECK(Focus() == "GUI" && Count == 0);
    CHECK(Set_Focus_Mouse_Button("None"));
    CHECK(SDL_PushEvent(&Middle) == 1); Update("Default");
    CHECK(Focus() == "GUI");
    const int Device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 21, 0);
    CHECK(Device >= 0);
    SDL_Joystick *Virtual = SDL_JoystickOpen(Device); CHECK(Virtual);
    Open_Controller(Device);
    Set_Controller_Axis_For(Device, "Left_Stick_X", "Axis");
    Set_Controller_Button_Down_For(Device, "A", "Jump");
    CHECK(SDL_JoystickSetVirtualAxis(Virtual, SDL_CONTROLLER_AXIS_LEFTX, 32767) == 0);
    CHECK(SDL_JoystickSetVirtualButton(Virtual, SDL_CONTROLLER_BUTTON_A, 1) == 0);
    Update("Default");
    CHECK(Count == 0 && Axis_Count == 0);
    CHECK(!Controller_Down(Device, "A") && !Controller_Pressed(Device, "A"));
    CHECK(Controller_Axis(Device, "Left_Stick_X") == 0);
    F1(1); CHECK(Focus() == "GUI"); // OS key repeats never toggle focus.
    F1(); CHECK(Focus() == "Game");
    CHECK(Count == 0 && Axis_Count == 0); // Toggle frame is consumed.
    Update("Default");
    CHECK(Controller_Down(Device, "A") && !Controller_Pressed(Device, "A"));
    CHECK(Count == 0 && Axis_Count > 0); // No synthetic jump from a held GUI button.
    CHECK(Controller_Axis(Device, "Left_Stick_X") == 100);
    CHECK(SDL_JoystickSetVirtualButton(Virtual, SDL_CONTROLLER_BUTTON_A, 0) == 0);
    Update("Default");
    CHECK(SDL_JoystickSetVirtualButton(Virtual, SDL_CONTROLLER_BUTTON_A, 1) == 0);
    Update("Default"); CHECK(Count == 1);
    CHECK(SDL_JoystickSetVirtualButton(Virtual, SDL_CONTROLLER_BUTTON_BACK, 1) == 0);
    Update("Default"); CHECK(Focus() == "GUI");
    CHECK(!Controller_Down(Device, "Back"));
    Toggle_Focus(); CHECK(Focus() == "Game");
    CHECK(SDL_JoystickDetachVirtual(Device) == 0); Update("Default");
    SDL_JoystickClose(Virtual); Close_Controller(-1); Destructor();
    CHECK(SDL_GetHint("APPEAL_INPUT_FOCUS") == nullptr);
    std::cout << "SDL2_INPUT_FOCUS_PASSED\n";
}
