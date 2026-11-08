#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <windows.h>
#include <GL/gl.h>
#include <string>
#include <iostream>
#include <cstdlib>
#define CHECK(x) do{if(!(x)){std::cerr<<"Failed: " #x <<" line "<<__LINE__<<'\n';std::exit(1);}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
 CHECK(argc==4);
 auto p=LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
 Fn<void(*)(std::string)>(p,"Set_Paths")(argv[1]);
 Fn<void(*)(std::string,std::string,int,int)>(p,"Add_Window")("Main","Screen blend",800,600);
 Fn<void(*)(bool)>(p,"Set_GUI_Docking")(false);Fn<void(*)(bool)>(p,"Set_GUI_Visible")(true);SDL_SetHint("APPEAL_INPUT_FOCUS","GUI");
 auto begin=Fn<void(*)(std::string)>(p,"GUI_Begin_Fullscreen");auto end=Fn<void(*)()>(p,"GUI_End_Window");
 auto blend=Fn<bool(*)(std::string,std::string,std::string,float)>(p,"GUI_Image_Blend");
 auto render=Fn<void(*)()>(p,"Render"),update=Fn<void(*)()>(p,"Update");
 auto clock=Fn<double(*)()>(p,"Milliseconds");double t=clock();SDL_Delay(10);CHECK(clock()>=t+5);
 CHECK(!blend("Bad","missing.png",argv[3],0));
 for(float alpha:{0.0f,0.5f,1.0f}){
  for(int i=0;i<4;++i){begin("Screen Preview");CHECK(blend("Screen",argv[2],argv[3],alpha));end();render();update();}
  unsigned char rgb[3];glFinish();glReadBuffer(GL_FRONT);glReadPixels(400,300,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
  CHECK(std::abs(int(rgb[0])-int(255*(1-alpha)))<=3);CHECK(rgb[1]<3);CHECK(std::abs(int(rgb[2])-int(255*alpha))<=3);
 }

 auto audio=LoadLibraryA("Resources/Modules/Appeal_Module_Audio/Appeal_SDL2_Audio/Appeal_SDL2_Audio.dll");CHECK(audio);
 Fn<void(*)()>(audio,"Constructor")();
 auto sfx=std::string(argv[1])+"/Splash.wav";
 Fn<void(*)(std::string,std::string)>(audio,"Load_SFX")("Splash",sfx);
 Fn<void(*)(std::string)>(audio,"Play_SFX_Name")("Splash");
 auto mixer=GetModuleHandleA("SDL2_mixer.dll");CHECK(mixer);auto playing=Fn<int(*)(int)>(mixer,"Mix_Playing");CHECK(playing(-1)>0);
 Fn<void(*)()>(audio,"Stop")();CHECK(!Fn<bool(*)()>(audio,"Is_Playing")());
 Fn<void(*)()>(audio,"Destructor")();FreeLibrary(audio);
 Fn<void(*)()>(p,"Close_All")();Fn<void(*)()>(p,"Destructor")();FreeLibrary(p);
 std::cout<<"SCREEN_EDITOR_RENDER_PASSED\n";
}
