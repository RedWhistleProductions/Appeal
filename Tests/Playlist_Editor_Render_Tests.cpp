#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Appeal.h"
#include "imgui_internal.h"
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " line " << __LINE__ << '\n'; std::exit(1); } } while(false)
template<class T> T Fn(HMODULE p, const char* name) { auto f=GetProcAddress(p,name); CHECK(f); return reinterpret_cast<T>(f); }
int main(int argc,char**argv) {
    CHECK(argc==3);
    Appeal app; app.Set_Resource_Folder("./Resources"); app.Run(argv[1]);
    auto plugin=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll"); CHECK(plugin);
    auto context=Fn<ImGuiContext*(*)()>(plugin,"_ZN5ImGui17GetCurrentContextEv")(); CHECK(context);
    auto pos=Fn<void(*)(const char*,const ImVec2&,int)>(plugin,"_ZN5ImGui12SetWindowPosEPKcRK6ImVec2i");
    auto size=Fn<void(*)(const char*,const ImVec2&,int)>(plugin,"_ZN5ImGui13SetWindowSizeEPKcRK6ImVec2i");
    pos("Music Library",ImVec2(30,30),0); size("Music Library",ImVec2(500,650),0);
    pos("Playlist Editor",ImVec2(560,30),0); size("Playlist Editor",ImVec2(650,650),0);
    auto frame=[&](){ app.Run(argv[2]); };
    auto update=Fn<void(*)()>(plugin,"Update");
    auto selected=Fn<std::string(*)(std::string)>(plugin,"GUI_Music_Picker_Selected_Path");
    auto count=Fn<int(*)(std::string)>(plugin,"GUI_Playlist_Item_Count");
    auto window=SDL_GetWindowFromID(1); CHECK(window);
    auto mouse=[&](int x,int y,int down) {
        SDL_WarpMouseInWindow(window,x,y);
        SDL_Event event{};
        event.type=SDL_MOUSEMOTION;event.motion.windowID=SDL_GetWindowID(window);event.motion.x=x;event.motion.y=y;
        CHECK(SDL_PushEvent(&event)==1);
        if(down>=0) {
            event={};event.type=down ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
            event.button.windowID=SDL_GetWindowID(window);event.button.button=SDL_BUTTON_LEFT;
            event.button.state=down ? SDL_PRESSED : SDL_RELEASED;event.button.x=x;event.button.y=y;
            CHECK(SDL_PushEvent(&event)==1);
        }
        update();frame();
    };
    frame(); frame();
    int sourceY=-1;
    for(int y=130;y<250 && sourceY<0;y+=3) {
        mouse(200,y,-1); mouse(200,y,1); mouse(200,y,0);
        if(selected("PE_Music")=="Sub Folder/Song One.wav") sourceY=y;
    }
    CHECK(sourceY>0); CHECK(count("PE_List")==0);
    // Drive real SDL/ImGui drag events, including dropping into an empty list.
    for(int targetY=180; targetY<330 && count("PE_List")==0;targetY+=15) {
        mouse(200,sourceY,-1);mouse(200,sourceY,1);
        mouse(230,sourceY,-1);mouse(800,targetY,-1);mouse(800,targetY,-1);mouse(800,targetY,0);
    }
    CHECK(count("PE_List")==1);
    CHECK(app.Data_Manager.Variables["PE_Status"]=="Playlist saved and linked to this level.");
    // Render several more frames: scanning and callbacks must not add duplicates.
    for(int i=0;i<5;++i) frame();
    CHECK(count("PE_List")==1);
    Fn<void(*)(std::string,std::string,std::string)>(plugin,"GUI_Playlist_Add")("PE_List","Second","Sub Folder/Song One.wav");
    frame();frame();
    ImGuiWindow *list=nullptr;
    for(auto *candidate : context->Windows)
        { if(std::strstr(candidate->Name,"Playlist") && (candidate->Flags & ImGuiWindowFlags_ChildWindow)) list=candidate; }
    CHECK(list);
    const int rowX=static_cast<int>(list->WorkRect.Min.x)+70;
    const int firstY=static_cast<int>(list->WorkRect.Min.y+context->FontSize/2);
    const int secondY=static_cast<int>(firstY+context->FontSize+context->Style.ItemSpacing.y);
    mouse(rowX,firstY,-1);mouse(rowX,firstY,1);mouse(rowX+20,firstY,-1);
    mouse(rowX,secondY,-1);mouse(rowX,secondY,-1);mouse(rowX,secondY,0);
    CHECK(count("PE_List")==2);
    const std::string saved=app.Data_Manager.Variables["Check_Resources"]+"/"+app.Data_Manager.Variables["PE_Resource_File"];
    { std::ifstream file(saved);std::string line;CHECK(std::getline(file,line));CHECK(line.rfind("\"Second\"",0)==0); }
    // The row's X must remove that song and persist even when it is the last one.
    const int removeX=static_cast<int>(list->WorkRect.Min.x)+6;
    auto click=[&](int x,int y){mouse(x,y,-1);mouse(x,y,1);mouse(x,y,0);};
    click(removeX,firstY);CHECK(count("PE_List")==1);
    click(removeX,firstY);CHECK(count("PE_List")==0);
    { std::ifstream file(saved);std::string line;CHECK(!std::getline(file,line)); }
    // Both close buttons hide both component windows; reopening retains the list.
    for(const char *name : {"Playlist Editor","Music Library"}) {
        app.Data_Manager.Variables["PE_Visible"]="1";
        Fn<void(*)(std::string,bool)>(plugin,"GUI_Set_Window_Open")("Playlist Editor",true);
        Fn<void(*)(std::string,bool)>(plugin,"GUI_Set_Window_Open")("Music Library",true);frame();frame();
        ImGuiWindow *panel=nullptr;
        for(auto *candidate : context->Windows) if(std::strcmp(candidate->Name,name)==0) panel=candidate;
        CHECK(panel);
        click(static_cast<int>(panel->Pos.x+panel->Size.x-context->Style.FramePadding.x-context->FontSize/2),static_cast<int>(panel->Pos.y+panel->TitleBarHeight/2));
        CHECK(app.Data_Manager.Variables["PE_Visible"]=="0");frame();
        for(auto *candidate : context->Windows)
            if(std::strcmp(candidate->Name,"Playlist Editor")==0 || std::strcmp(candidate->Name,"Music Library")==0) CHECK(!candidate->Active);
        // The persistent Windows menu can restore the closed component.
        click(30,8);
        ImGuiWindow *menu=nullptr;
        for(auto *candidate : context->Windows) if(candidate->Active && std::strstr(candidate->Name,"##Menu_00")) menu=candidate;
        CHECK(menu);
        click(static_cast<int>(menu->WorkRect.Min.x)+45,static_cast<int>(menu->WorkRect.Min.y+context->FontSize/2));
        CHECK(app.Data_Manager.Variables["PE_Visible"]=="1");frame();
    }
    std::cout<<"PLAYLIST_EDITOR_DRAG_DROP_PASSED\n";
}
