#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <windows.h>
#include <GL/gl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#define CHECK(x) do {if(!(x)){std::cerr<<"Failed: " #x <<" line "<<__LINE__<<'\n';std::exit(1);}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
 CHECK(argc>1);
 auto p=LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
 Fn<void(*)(std::string)>(p,"Set_Paths")(argv[1]);
 Fn<void(*)(std::string,int)>(p,"Add_Full_Screen_Window")("Main",0);
 SDL_Window* w=SDL_GetWindowFromID(1);CHECK(w);CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN_DESKTOP);
 Fn<void(*)(bool)>(p,"Set_GUI_Editor_Mode")(true);Fn<void(*)(bool)>(p,"Set_GUI_Docking")(true);Fn<void(*)(bool)>(p,"Set_GUI_Visible")(true);
 auto start=Fn<void(*)()>(p,"GUI_Begin_Frame");auto begin=Fn<void(*)(std::string)>(p,"GUI_Begin_Window");
 auto text=Fn<void(*)(std::string,std::string)>(p,"GUI_Text");auto end=Fn<void(*)()>(p,"GUI_End_Window");
 auto canvas=Fn<void(*)(std::string,int,int,int,int)>(p,"GUI_Editor_Viewport");auto value=Fn<int(*)(std::string,std::string)>(p,"GUI_Viewport_Value");
 auto rect=Fn<void(*)(std::string,float,float,float,float,int,int,int,int)>(p,"Draw_Rect");auto render=Fn<void(*)()>(p,"Render");auto update=Fn<void(*)()>(p,"Update");
 auto frame=[&](){
  start();begin("Components");text("Tile","Tile Map Editor");end();begin("Editing View");canvas("Map",20,40,64,64);end();begin("Tile Editor Controls");text("Brush","Brush preview and layer controls");end();
  rect("Tile",20,40,64,64,244,67,54,255);render();update();
 };
 // Alt+Enter works in GUI and game focus, ignores repeats, and preserves the window.
 auto enter=[&](SDL_Keymod modifiers,int repeat,Uint32 id){
  SDL_Event event{};event.type=SDL_KEYDOWN;event.key.windowID=id;event.key.state=SDL_PRESSED;
  event.key.repeat=repeat;event.key.keysym.sym=SDLK_RETURN;event.key.keysym.scancode=SDL_SCANCODE_RETURN;event.key.keysym.mod=modifiers;
  CHECK(SDL_PushEvent(&event)==1);update();
  event.type=SDL_KEYUP;event.key.state=SDL_RELEASED;CHECK(SDL_PushEvent(&event)==1);update();
 };
 SDL_SetHint("APPEAL_INPUT_FOCUS","GUI");
 auto toggle=Fn<bool(*)(std::string)>(p,"Toggle_Fullscreen");
 CHECK(!toggle("Missing"));CHECK(toggle("Main"));CHECK(!(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN));
 CHECK(toggle("Main"));CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN);
 enter(KMOD_NONE,0,SDL_GetWindowID(w));CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN);
 enter(KMOD_ALT,1,SDL_GetWindowID(w));CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN);
 enter(KMOD_ALT,0,SDL_GetWindowID(w));CHECK(!(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN));
 int windowW,windowH;SDL_GetWindowSize(w,&windowW,&windowH);CHECK(windowW>0&&windowH>0);
 SDL_SetWindowSize(w,windowW-40,windowH-40);SDL_GetWindowSize(w,&windowW,&windowH);
 for(int cycle=0;cycle<2;++cycle){
  enter(KMOD_ALT,0,SDL_GetWindowID(w));CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN);
  for(int frameIndex=0;frameIndex<5;++frameIndex)frame();CHECK(value("Map","Width")>0);
  SDL_SetHint("APPEAL_INPUT_FOCUS","Game");
  enter(KMOD_ALT,0,SDL_GetWindowID(w));CHECK(!(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN));
  int restoredW,restoredH;SDL_GetWindowSize(w,&restoredW,&restoredH);CHECK(restoredW==windowW&&restoredH==windowH);
 }
 SDL_SetHint("APPEAL_INPUT_FOCUS","GUI");
 enter(KMOD_ALT,0,SDL_GetWindowID(w));CHECK(SDL_GetWindowFlags(w)&SDL_WINDOW_FULLSCREEN);
 CHECK(SDL_GetWindowFromID(SDL_GetWindowID(w))==w);
 for(int i=0;i<5;++i)frame();
 const int x=value("Map","Origin_X"),y=value("Map","Origin_Y"),width=value("Map","Width"),height=value("Map","Height");
 CHECK(width>100&&height>100);int dw,dh;SDL_GL_GetDrawableSize(w,&dw,&dh);
 std::vector<unsigned char> pixels(dw*dh*3);glFinish();glReadBuffer(GL_FRONT);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,dw,dh,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());CHECK(glGetError()==GL_NO_ERROR);
 if(argc>2){std::ofstream image(argv[2],std::ios::binary);image<<"P6\n"<<dw<<' '<<dh<<"\n255\n";for(int row=dh-1;row>=0;--row)image.write(reinterpret_cast<char*>(pixels.data()+row*dw*3),dw*3);}
 int offset=((dh-1-y-height/2)*dw+x+width/2)*3;CHECK(offset>=0&&offset+2<int(pixels.size()));CHECK(pixels[offset]>200&&pixels[offset+1]<100&&pixels[offset+2]<100);
 SDL_WarpMouseInWindow(w,x+width/2,y+height/2);for(int i=0;i<3;++i)frame();
 CHECK(value("Map","Hovered")==1);CHECK(value("Map","X")>=30&&value("Map","X")<=33);CHECK(value("Map","Y")>=30&&value("Map","Y")<=33);
 SDL_Event middle{};middle.type=SDL_MOUSEBUTTONDOWN;middle.button.windowID=SDL_GetWindowID(w);middle.button.button=SDL_BUTTON_MIDDLE;middle.button.state=SDL_PRESSED;middle.button.x=x+width/2;middle.button.y=y+height/2;
 CHECK(SDL_PushEvent(&middle)==1);update();frame();CHECK(value("Map","Middle")==1);
 middle.type=SDL_MOUSEBUTTONUP;middle.button.state=SDL_RELEASED;CHECK(SDL_PushEvent(&middle)==1);update();frame();CHECK(value("Map","Middle")==0);
 // Docking/menu regions must never generate canvas paint or brush-wheel input.
 SDL_WarpMouseInWindow(w,5,5);for(int i=0;i<3;++i)frame();CHECK(value("Map","Hovered")==0);CHECK(value("Map","Left")==0&&value("Map","Middle")==0&&value("Map","Wheel")==0);
 Fn<void(*)()>(p,"GUI_Reset_Editor_Layout")();frame();
 Fn<void(*)()>(p,"Close_All")();Fn<void(*)()>(p,"Destructor")();FreeLibrary(p);
 auto ini=std::filesystem::path(argv[1])/"Graphics/GUI/Editor_Layout.ini";CHECK(std::filesystem::exists(ini));
 std::ifstream in(ini);std::string content((std::istreambuf_iterator<char>(in)),{});CHECK(content.find("Docking")!=std::string::npos);CHECK(content.find("Editing View")!=std::string::npos);
 std::cout<<"GRAPHICS_EDITOR_WORKSPACE_PASSED\n";
}


