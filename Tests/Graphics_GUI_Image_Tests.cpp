#include <windows.h>
#include <GL/gl.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#define CHECK(x) do { if(!(x)) { std::cerr << "Failed: " #x << " at " << __LINE__ << '\n'; std::exit(1); } } while(false)
template<class T> T Fn(HMODULE p,const char* name) {auto f=GetProcAddress(p,name);CHECK(f);return reinterpret_cast<T>(f);}
int main() {
 auto p=LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics/Appeal_SDL2_Legacy_OpenGL_Graphics.dll");CHECK(p);
 Fn<void(*)(std::string,std::string,int,int)>(p,"Add_Window")("Main","GUI preview check",400,300);
 Fn<void(*)(bool)>(p,"Set_GUI_Docking")(false);
 Fn<void(*)(bool)>(p,"Set_GUI_Visible")(true);
 auto begin=Fn<void(*)(std::string)>(p,"GUI_Begin_Fullscreen");
 auto image=Fn<void(*)(std::string,std::string,int,int,int,int,int)>(p,"GUI_Image");
 auto end=Fn<void(*)()>(p,"GUI_End_Window");auto render=Fn<void(*)()>(p,"Render");
 auto draw=[&](int frame) {
  for(int i=0;i<3;++i) {begin("Preview");image("Brush","Examples/Place_Holder_Tiles.png",64,64,frame,1,96);end();render();}
  std::vector<unsigned char> data(400*300*3);glFinish();glReadBuffer(GL_FRONT);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,400,300,GL_RGB,GL_UNSIGNED_BYTE,data.data());CHECK(glGetError()==GL_NO_ERROR);return data;
 };
 auto red=draw(0),blue=draw(77),clear=draw(10);
 int reds=0,blues=0,checker=0;
 for(size_t i=0;i<red.size();i+=3) {
  if(red[i]>200&&red[i+1]<100&&red[i+2]<100) ++reds;
  if(blue[i]<100&&blue[i+1]>100&&blue[i+2]>200) ++blues;
  if(clear[i]==clear[i+1]&&clear[i+1]==clear[i+2]&&(clear[i]==100||clear[i]==160)) ++checker;
 }
 CHECK(reds>5000);CHECK(blues>5000);CHECK(checker>5000);
 // Both brush previews sit beside their selectors in a narrow controls panel.
 auto text=Fn<void(*)(std::string,std::string)>(p,"GUI_Text");
 auto width=Fn<void(*)(int)>(p,"GUI_Set_Next_Item_Width");
 auto combo=Fn<void(*)(std::string,std::string,std::string,int)>(p,"GUI_Combo");
 auto same=Fn<void(*)()>(p,"GUI_Same_Line");
 for(int i=0;i<3;++i) {
  begin("Preview");text("LeftLabel","Left mouse brush");width(-112);combo("Left","##Left","Red|Blue",0);same();image("LeftPreview","Examples/Place_Holder_Tiles.png",64,64,0,1,96);
  text("RightLabel","Right mouse brush");width(-112);combo("Right","##Right","Red|Blue",1);same();image("RightPreview","Examples/Place_Holder_Tiles.png",64,64,77,1,96);end();render();
 }
 std::vector<unsigned char> dual(400*300*3);glFinish();glReadBuffer(GL_FRONT);glReadPixels(0,0,400,300,GL_RGB,GL_UNSIGNED_BYTE,dual.data());
 int rightRed=0,rightBlue=0;
 for(int y=0;y<300;++y)for(int x=280;x<400;++x) {int i=(y*400+x)*3;if(dual[i]>200&&dual[i+1]<100&&dual[i+2]<100)++rightRed;if(dual[i]<100&&dual[i+1]>100&&dual[i+2]>200)++rightBlue;}
 CHECK(rightRed>7000&&rightBlue>7000);
 auto set=Fn<void(*)(std::string,int)>(p,"GUI_Set_Selection_Value");
 auto get=Fn<int(*)(std::string)>(p,"GUI_Selection_Value");
 set("Brush",284);CHECK(get("Brush")==284);set("Brush",0);CHECK(get("Brush")==0);
 // Deliberately oppose asset-name order to verify explicit layer ordering.
 Fn<void(*)(bool)>(p,"Set_GUI_Visible")(false);
 auto load=Fn<bool(*)(std::string,std::string,std::string,int,int,int,int)>(p,"Load_Animation");
 auto paste=Fn<void(*)(std::string,int,int,int)>(p,"Paste");
 auto order=Fn<void(*)(std::string,int)>(p,"Set_Draw_Order");
 CHECK(load("Z_Bottom","Still","Examples/Place_Holder_Tiles.png",64,64,0,1));
 CHECK(load("A_Top","Still","Examples/Place_Holder_Tiles.png",64,64,77,1));
 paste("Z_Bottom",200,150,0);paste("A_Top",200,150,0);
 order("Z_Bottom",0);order("A_Top",1);
 auto pixel=[&]() {render();unsigned char rgb[3];glFinish();glReadBuffer(GL_FRONT);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(232,118,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);CHECK(glGetError()==GL_NO_ERROR);return std::vector<unsigned char>(rgb,rgb+3);};
 auto top=pixel();CHECK(top[0]<100&&top[1]>100&&top[2]>200);
 order("A_Top",-1);auto bottom=pixel();CHECK(bottom[0]>200&&bottom[1]<100&&bottom[2]<100);
 // A transparent top tile reveals the red bottom layer.
 CHECK(load("A_Top","Still","Examples/Place_Holder_Tiles.png",64,64,10,1));
 order("A_Top",1);auto transparent=pixel();CHECK(transparent==bottom);
 CHECK(load("M_Third","Still","Examples/Place_Holder_Tiles.png",64,64,49,1));
 paste("M_Third",200,150,0);order("M_Third",2);
 auto blended=pixel();
 CHECK(blended[0]>150&&blended[0]<170&&blended[1]>110&&blended[1]<130&&blended[2]>55&&blended[2]<80);
 Fn<void(*)()>(p,"Close_All")();Fn<void(*)()>(p,"Destructor")();FreeLibrary(p);
 std::cout<<"GRAPHICS_GUI_IMAGE_PASSED\n";
}
