#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <windows.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#define CHECK(x) do{if(!(x)){std::cerr<<"Failed: " #x <<" line "<<__LINE__<<'\n';std::exit(1);}}while(false)
template<class T>T Fn(HMODULE p,const char*n){auto f=GetProcAddress(p,n);CHECK(f);return reinterpret_cast<T>(f);}
int main(int argc,char**argv){
 CHECK(argc==4);auto p=LoadLibraryA(argv[2]);CHECK(p);
 Fn<void(*)(std::string)>(p,"Set_Paths")(argv[1]);
 Fn<void(*)(std::string,std::string,int,int)>(p,"Add_Window")("Main","IDE font test",800,600);
 Fn<void(*)(bool)>(p,"Set_GUI_Docking")(false);Fn<void(*)(bool)>(p,"Set_GUI_Visible")(true);SDL_SetHint("APPEAL_INPUT_FOCUS","GUI");
 auto style=Fn<bool(*)(std::string,std::string,int,int,int,int)>(p,"GUI_Text_Editor_Style");
 auto begin=Fn<void(*)(std::string)>(p,"GUI_Begin_Fullscreen");auto end=Fn<void(*)()>(p,"GUI_End_Window");
 auto editor=Fn<void(*)(std::string,std::string,std::string)>(p,"GUI_Text_Editor");
 auto render=Fn<void(*)()>(p,"Render"),update=Fn<void(*)()>(p,"Update");
 auto frame=[&](){begin("Font rendering");editor("Source","Source","MMMMMMMMM");end();render();update();};
 auto height=[&](){int best=0;for(GLenum buffer:{GL_FRONT,GL_BACK}){std::vector<unsigned char> pixels(800*600*3);glFinish();glReadBuffer(buffer);glReadPixels(0,0,800,600,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());CHECK(glGetError()==GL_NO_ERROR);int lo=600,hi=-1;for(int y=0;y<600;++y)for(int x=0;x<800;++x){int i=(y*800+x)*3;if(pixels[i]>150&&pixels[i+1]<80&&pixels[i+2]<80){lo=std::min(lo,y);hi=std::max(hi,y);}}best=std::max(best,hi>=lo?hi-lo+1:0);}return best;};
 CHECK(style("Source",argv[3],13,255,0,0));for(int i=0;i<4;++i)frame();int small=height();CHECK(small>=6);
 CHECK(style("Source",argv[3],32,255,0,0));for(int i=0;i<4;++i)frame();int large=height();CHECK(large>small*1.5);
 CHECK(!style("Source","missing-font.ttf",22,255,0,0));CHECK(!style("Source",argv[3],0,255,0,0));
 for(int i=0;i<3;++i)frame();CHECK(height()==large);
 CHECK(Fn<std::string(*)(std::string)>(p,"GUI_Text_Value")("Source")=="MMMMMMMMM");
 CHECK(style("Source","",13,230,230,230));for(int i=0;i<4;++i)frame();CHECK(height()==0);
 Fn<void(*)()>(p,"Close_All")();Fn<void(*)()>(p,"Destructor")();FreeLibrary(p);std::cout<<"IDE_FONT_RENDER_PASSED\n";
}
