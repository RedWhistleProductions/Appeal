// Deterministic event sharing test: compile with the plugin source and SDL2.
#include <SDL.h>
#include <iostream>
#include <string>
#include <cstdlib>
extern "C" void Constructor();
extern "C" void Destructor();
extern "C" void Update(std::string);
extern "C" int Mouse_Wheel();
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << std::endl; std::exit(1); } } while(false)
int main() {
    CHECK(SDL_Init(SDL_INIT_VIDEO) == 0);
    auto Window = SDL_CreateWindow("Input events",0,0,320,240,SDL_WINDOW_HIDDEN);
    CHECK(Window);
    Constructor();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    SDL_Event Wheel{}; Wheel.type = SDL_MOUSEWHEEL; Wheel.wheel.y = 3;
    SDL_Event Quit{}; Quit.type = SDL_QUIT;
    CHECK(SDL_PushEvent(&Wheel) == 1);
    CHECK(SDL_PushEvent(&Quit) == 1);
    Update("Default");
    CHECK(Mouse_Wheel() == 3);
    SDL_Event Event{};
    bool Saw_Wheel = false, Saw_Quit = false;
    while(SDL_PollEvent(&Event)) {
        if(Event.type == SDL_MOUSEWHEEL) Saw_Wheel = true;
        if(Event.type == SDL_QUIT) Saw_Quit = true;
    }
    CHECK(Saw_Wheel && Saw_Quit); // Graphics can still consume both.
    Update("Default");
    CHECK(Mouse_Wheel() == 0); // no repeated input without another event
    Wheel.wheel.y = -2;
    CHECK(SDL_PushEvent(&Wheel) == 1);
    while(SDL_PollEvent(&Event)) {} // Graphics pumps first instead.
    Update("Default");
    CHECK(Mouse_Wheel() == -2);
    Destructor();
    Constructor();
    Update("Default");
    CHECK(Mouse_Wheel() == 0);
    Destructor();
    SDL_DestroyWindow(Window);
    SDL_Quit();
    std::cout << "SDL2_INPUT_EVENTS_PASSED\n";
}
