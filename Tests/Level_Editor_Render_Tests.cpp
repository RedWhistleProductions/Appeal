#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Appeal.h"
#include <windows.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#define CHECK(x) do {if(!(x)){std::cerr<<"Failed: " #x <<" line "<<__LINE__<<'\n';std::exit(1);}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
 CHECK(argc>1);Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
 auto p=GetModuleHandleA("Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
 auto value=Fn<int(*)(std::string,std::string)>(p,"GUI_Viewport_Value");
 int x=value("TE_Canvas","Origin_X"),y=value("TE_Canvas","Origin_Y"),w=value("TE_Canvas","Width"),h=value("TE_Canvas","Height");CHECK(w>100&&h>100);
 GLint viewport[4];glGetIntegerv(GL_VIEWPORT,viewport);CHECK(viewport[2]>0&&viewport[3]>0);
 auto sample=[&](int column){unsigned char rgb[3];int px=x+(column*180+90)*w/720,py=y+90*h/540;glFinish();glReadBuffer(GL_FRONT);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(px,viewport[3]-1-py,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);CHECK(glGetError()==GL_NO_ERROR);return std::vector<unsigned char>(rgb,rgb+3);};
 auto red=sample(0),blue=sample(1);CHECK(red[0]>200&&red[1]<100&&red[2]<100);CHECK(blue[0]<100&&blue[1]>100&&blue[2]>200);
 // Samples immediately on both sides of the map seam must contain map pixels.
 for(int sourceX : {179,180,181}) {
    unsigned char rgb[3];int px=x+sourceX*w/720,py=y+90*h/540;
    glReadPixels(px,viewport[3]-1-py,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
    CHECK((rgb[0]>200&&rgb[1]<100&&rgb[2]<100)||(rgb[0]<100&&rgb[1]>100&&rgb[2]>200));
 }
 // Translucent cells must not darken at shared fractional tile boundaries.
 auto semi=sample(2);
 for(int source=5;source<175;++source) {
    for(int axis=0;axis<2;++axis) {
        unsigned char rgb[3];int sx=360+(axis==0?source:90),sy=axis==0?90:source;
        glReadPixels(x+sx*w/720,viewport[3]-1-y-sy*h/540,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
        for(int channel=0;channel<3;++channel) CHECK(std::abs(int(rgb[channel])-int(semi[channel]))<=1);
    }
 }
 {unsigned char marker[3];glReadPixels(x+5*w/720,viewport[3]-1-y-h/540,1,1,GL_RGB,GL_UNSIGNED_BYTE,marker);CHECK(marker[0]>240&&marker[1]>170&&marker[1]<210&&marker[2]<80);}
 CHECK(Fn<bool(*)(std::string)>(p,"Window_Open")("Main"));
 CHECK(argc>2);auto update=Fn<void(*)()>(p,"Update");SDL_Window *window=SDL_GetWindowFromID(1);CHECK(window);
 auto frame=[&](){app.Run(argv[2]);};
 auto click=[&](int y){
    y+=19; // Editor Windows menu bar.
    SDL_WarpMouseInWindow(window,90,y);update();frame();
    SDL_Event event{};event.type=SDL_MOUSEBUTTONDOWN;event.button.windowID=SDL_GetWindowID(window);event.button.button=SDL_BUTTON_LEFT;event.button.state=SDL_PRESSED;event.button.x=90;event.button.y=y;
    CHECK(SDL_PushEvent(&event)==1);update();frame();
    event.type=SDL_MOUSEBUTTONUP;event.button.state=SDL_RELEASED;CHECK(SDL_PushEvent(&event)==1);update();frame();
 };
 // Drag an existing placement into an empty cell, then swap with an occupied one.
 auto storage=GetModuleHandleA("Appeal_SQLite_Storage.dll");CHECK(storage);
 auto get=Fn<std::string(*)(std::string,std::string,std::string,std::string)>(storage,"Get_Field");
 auto moveMouse=[&](int mx,int my,int down){
    SDL_WarpMouseInWindow(window,mx,my);
    SDL_Event event{};event.type=SDL_MOUSEMOTION;event.motion.windowID=SDL_GetWindowID(window);event.motion.x=mx;event.motion.y=my;CHECK(SDL_PushEvent(&event)==1);
    if(down>=0){event={};event.type=down?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP;event.button.windowID=SDL_GetWindowID(window);event.button.button=SDL_BUTTON_LEFT;event.button.state=down?SDL_PRESSED:SDL_RELEASED;event.button.x=mx;event.button.y=my;CHECK(SDL_PushEvent(&event)==1);}
    update();frame();
 };
 auto drag=[&](int sx,int sy,int dx,int dy){moveMouse(sx,sy,-1);moveMouse(sx,sy,1);moveMouse(sx+15,sy,-1);moveMouse(dx,dy,-1);moveMouse(dx,dy,-1);moveMouse(dx,dy,0);};
 const int cy=y+90*h/540;
 drag(x+90*w/720,cy,5,5);
 CHECK(get("LE_Cells","Cell","0","Map_Name")=="Red");
 drag(x+90*w/720,cy,x+630*w/720,cy);
 CHECK(get("LE_Cells","Cell","0","Map_Name").empty());CHECK(get("LE_Cells","Cell","3","Map_Name")=="Red");
 drag(x+630*w/720,cy,x+270*w/720,cy);
 CHECK(get("LE_Cells","Cell","1","Map_Name")=="Red");CHECK(get("LE_Cells","Cell","3","Map_Name")=="Blue");
 // The saved-map GUI preview has real thumbnail pixels and copies onto the grid.
 frame();
 int px=value("LE_Map_Preview","Origin_X"),py=value("LE_Map_Preview","Origin_Y"),pw=value("LE_Map_Preview","Width"),ph=value("LE_Map_Preview","Height");CHECK(pw>0&&ph>0);
 { unsigned char rgb[3];glFinish();glReadBuffer(GL_FRONT);glReadPixels(px+pw/2,viewport[3]-1-py-ph/2,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
   for(int channel=0;channel<3;++channel) CHECK(std::abs(int(rgb[channel])-int(semi[channel]))<=2); }
 drag(px+pw/2,py+ph/2,x+90*w/720,cy);
 CHECK(get("LE_Cells","Cell","0","Map_Name")==app.Data_Manager.Variables["LE_Assign_Name"]);
 CHECK(app.Data_Manager.Variables["LE_Dirty"]=="1");
 for(int cycle=0;cycle<3;++cycle){click(60);CHECK(app.Data_Manager.Variables["GE_Mode"]=="Tile");click(37);CHECK(app.Data_Manager.Variables["GE_Mode"]=="Level");}
 CHECK(SDL_GetWindowFromID(2)==nullptr);
 app.Data_Manager.Variables["LE_Selected"]="1";frame();
 CHECK(Fn<int(*)(std::string)>(p,"GUI_Slider_Int_Value")("LE_Cell_Number")==1);
 // Playlist component shares the host and follows the selected level.
 click(83);CHECK(app.Data_Manager.Variables["PE_Visible"]=="1");frame();
 CHECK(app.Data_Manager.Variables["PE_Level"]==app.Data_Manager.Variables["LE_Name"]);
 Fn<void(*)(std::string,std::string)>(p,"GUI_Set_Text_Value")("LE_Name_Control","Playlist_Other");frame();
 CHECK(app.Data_Manager.Variables["PE_Level"]=="Playlist_Other");
 // Selecting either other editor closes both playlist component panels.
 click(37);CHECK(app.Data_Manager.Variables["PE_Visible"]=="0");
 click(83);CHECK(app.Data_Manager.Variables["PE_Visible"]=="1");
 click(60);CHECK(app.Data_Manager.Variables["PE_Visible"]=="0");
 CHECK(SDL_GetWindowFromID(2)==nullptr);
 std::cout<<"LEVEL_EDITOR_RENDER_PASSED\n";
}
