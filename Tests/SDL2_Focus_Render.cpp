#include <windows.h>
#include <SDL.h>
#include "imgui.h"
#include <cstdlib>
#include <iostream>
#include <string>
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
template<class T> T Function(HMODULE Library, const char *Name)
{
    auto Address = GetProcAddress(Library, Name);
    CHECK(Address);
    return reinterpret_cast<T>(Address);
}
int main()
{
    auto Input = LoadLibraryA("Resources/Modules/Appeal_Module_Input/Appeal_SDL2_Input/Appeal_SDL2_Input.dll");
    auto Graphics = LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics.dll");
    CHECK(Input && Graphics);
    Function<void(*)()>(Input, "Constructor")();
    auto Focus = Function<bool(*)(std::string)>(Input, "Set_Focus");
    auto Render = Function<void(*)()>(Graphics, "Render");
    auto Update = Function<void(*)()>(Graphics, "Update");
    auto Get_IO = Function<ImGuiIO&(*)()>(Graphics, "_ZN5ImGui5GetIOEv");
    auto Get_Viewport = Function<ImGuiViewport*(*)()>(Graphics, "_ZN5ImGui15GetMainViewportEv");
    auto Get_Draw_Data = Function<ImDrawData*(*)()>(Graphics, "_ZN5ImGui11GetDrawDataEv");
    Function<void(*)(std::string,std::string,int,int)>(Graphics, "Add_Window")("Focus", "Input focus render check", 240, 160);
    Function<void(*)(bool)>(Graphics, "Set_GUI_Visible")(true);
    Function<void(*)(std::string)>(Graphics, "GUI_Begin")("Focus visibility test");
    Function<void(*)(std::string,std::string)>(Graphics, "GUI_Text")("Focus_Text", "Visible only in GUI focus");
    Function<void(*)()>(Graphics, "GUI_End_Window")();
    Render();
    CHECK(Get_Draw_Data()->CmdListsCount == 0);
    CHECK(!Get_IO().AppAcceptingEvents);
    CHECK(Get_IO().ConfigFlags & ImGuiConfigFlags_NoMouse);
    CHECK(!(Get_IO().ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard));
    CHECK(Focus("GUI")); Render(); Render();
    CHECK(Get_Draw_Data()->CmdListsCount > 0);
    CHECK(Get_IO().AppAcceptingEvents);
    CHECK(!(Get_IO().ConfigFlags & ImGuiConfigFlags_NoMouse));
    CHECK(Get_IO().ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard);
    CHECK(Get_IO().ConfigFlags & ImGuiConfigFlags_NavEnableGamepad);
    SDL_Event Event{};
    Event.type = SDL_KEYDOWN;
    Event.key.windowID = static_cast<Uint32>(reinterpret_cast<intptr_t>(Get_Viewport()->PlatformHandle));
    CHECK(Event.key.windowID != 0);
    SDL_Event Focus_Event{};
    Focus_Event.type = SDL_WINDOWEVENT;
    Focus_Event.window.windowID = Event.key.windowID;
    Focus_Event.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
    CHECK(SDL_PushEvent(&Focus_Event) == 1);
    Event.key.keysym.scancode = SDL_SCANCODE_SPACE;
    Event.key.keysym.sym = SDLK_SPACE;
    CHECK(SDL_PushEvent(&Event) == 1);
    Update(); Render(); Render();
    CHECK(Get_IO().KeysData[ImGuiKey_Space - ImGuiKey_NamedKey_BEGIN].Down);
    CHECK(Focus("Game")); Render();
    CHECK(Get_Draw_Data()->CmdListsCount == 0);
    CHECK(!Get_IO().AppAcceptingEvents);
    CHECK(!Get_IO().KeysData[ImGuiKey_Space - ImGuiKey_NamedKey_BEGIN].Down);
    CHECK(SDL_PushEvent(&Event) == 1);
    Update(); Render();
    CHECK(!Get_IO().KeysData[ImGuiKey_Space - ImGuiKey_NamedKey_BEGIN].Down);
    CHECK(Focus("GUI")); Render();
    CHECK(Get_Draw_Data()->CmdListsCount > 0);
    CHECK(!Get_IO().KeysData[ImGuiKey_Space - ImGuiKey_NamedKey_BEGIN].Down);
    Function<void(*)()>(Graphics, "Close_All")();
    Function<void(*)()>(Input, "Destructor")();
    std::cout << "SDL2_FOCUS_RENDER_PASSED\n";
}
