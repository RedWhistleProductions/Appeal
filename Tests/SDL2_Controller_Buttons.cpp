#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
extern "C" {
void Constructor(); void Destructor(); void Update(std::string);
void Open_Controller(int); void Close_Controller(int);
void Set_Controller_Button_Down(std::string, std::string);
void Set_Controller_Button_Up(std::string, std::string);
void Set_Controller_Button_Repeat(std::string, bool);
void Set_Function_Runner(void(*)(std::string), void(*)(std::string,int));
std::string Focus(); bool Set_Focus(std::string);
}
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
std::vector<std::string> Calls;
void Digital(std::string Name) {
    Calls.push_back(Name);
    if(Name == "Close_During_Callback") {
        Close_Controller(-1);
        Set_Controller_Button_Down("A", "Rebound");
    }
}
void Analog(std::string, int) {}
int main()
{
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    Constructor(); Set_Function_Runner(Digital, Analog);
    CHECK(std::string(SDL_GetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS)) == "0");
    int Device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 21, 0);
    CHECK(Device >= 0);
    char *Mapping = SDL_GameControllerMappingForGUID(SDL_JoystickGetDeviceGUID(Device));
    CHECK(Mapping);
    CHECK(SDL_GameControllerAddMapping((std::string(Mapping) + ",touchpad:b20,").c_str()) >= 0);
    SDL_free(Mapping);
    SDL_Joystick *Pad = SDL_JoystickOpen(Device); CHECK(Pad);
    Open_Controller(Device);
    // Global button handlers are the same API used by the controller GUI.
    const std::vector<std::string> Buttons = {"A", "B", "X", "Y", "Back", "Guide", "Start",
        "Left_Stick", "Right_Stick", "Left_Shoulder", "Right_Shoulder", "DPad_Up", "DPad_Down",
        "DPad_Left", "DPad_Right", "Misc1", "Paddle1", "Paddle2", "Paddle3", "Paddle4", "Touchpad"};
    for(const auto &Button : Buttons) {
        Set_Controller_Button_Down(Button, Button + "_Press");
        Set_Controller_Button_Up(Button, Button + "_Release");
        Set_Controller_Button_Repeat(Button, false);
    }
    Update("Default");
    for(int I = 0; I < static_cast<int>(Buttons.size()); ++I) {
        Calls.clear();
        CHECK(SDL_JoystickSetVirtualButton(Pad, I, 1) == 0);
        Update("Default");
        if(Calls != std::vector<std::string>{Buttons[I] + "_Press"}) { std::cerr << "Button: " << Buttons[I] << " calls: "; for(auto &Call : Calls) std::cerr << Call << ' '; std::cerr << '\n'; }
        CHECK(Calls == std::vector<std::string>{Buttons[I] + "_Press"});
        Update("Default"); CHECK(Calls.size() == 1);
        CHECK(SDL_JoystickSetVirtualButton(Pad, I, 0) == 0);
        Update("Default");
        CHECK(Calls.size() == 2 && Calls.back() == Buttons[I] + "_Release");
        CHECK(Focus() == "Game");
    }
    for(auto Axis : {SDL_CONTROLLER_AXIS_TRIGGERLEFT, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}) {
        std::string Button = Axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ? "Left_Trigger" : "Right_Trigger";
        CHECK(SDL_JoystickSetVirtualAxis(Pad, Axis, -32768) == 0); Update("Default");
        Set_Controller_Button_Down(Button, "Trigger_Press");
        Set_Controller_Button_Up(Button, "Trigger_Release");
        Calls.clear();
        CHECK(SDL_JoystickSetVirtualAxis(Pad, Axis, 32767) == 0); Update("Default");
        CHECK(Calls == std::vector<std::string>{"Trigger_Press"});
        Update("Default"); CHECK(Calls.size() == 1);
        CHECK(SDL_JoystickSetVirtualAxis(Pad, Axis, -32768) == 0); Update("Default");
        CHECK(Calls.size() == 2 && Calls.back() == "Trigger_Release");
    }
    // Empty function names genuinely unbind; Minus can be focus or a custom action.
    Set_Controller_Button_Down("A", ""); Set_Controller_Button_Up("A", ""); Calls.clear();
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_A, 1); Update("Default");
    CHECK(Calls.empty());
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_A, 0); Update("Default");
    Set_Controller_Button_Down("Back", "Input.Toggle_Focus");
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_BACK, 1); Update("Default");
    CHECK(Focus() == "GUI");
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_BACK, 0); Update("Default");
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_BACK, 1); Update("Default");
    CHECK(Focus() == "Game");
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_BACK, 0); Update("Default");
    Set_Controller_Button_Down("A", "Close_During_Callback");
    Calls.clear();
    SDL_JoystickSetVirtualButton(Pad, SDL_CONTROLLER_BUTTON_A, 1); Update("Default");
    CHECK(Calls == std::vector<std::string>{"Close_During_Callback"});
    Close_Controller(-1); SDL_JoystickClose(Pad); SDL_JoystickDetachVirtual(Device);
    Destructor();
    std::cout << "SDL2_CONTROLLER_BUTTONS_PASSED\n";
}
