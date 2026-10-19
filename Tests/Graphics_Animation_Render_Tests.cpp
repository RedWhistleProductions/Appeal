// Windows/OpenGL integration check against the built SDL plugin.
#include <windows.h>
#include <GL/gl.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#define REQUIRE(condition) do { if(!(condition)) { std::cerr << "Failed: " #condition << " at line " << __LINE__ << std::endl; std::exit(1); } } while(false)
template<class T> T Function(HMODULE Plugin, const char* Name) {
    auto Address = GetProcAddress(Plugin, Name);
    REQUIRE(Address);
    return reinterpret_cast<T>(Address);
}
int main(int argc, char** argv) {
    auto Plugin = LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/Appeal_SDL2_Graphics/Appeal_SDL2_Graphics.dll");
    REQUIRE(Plugin);
    auto Add = Function<void(*)(std::string,std::string,int,int)>(Plugin,"Add_Window");
    auto Load = Function<bool(*)(std::string,std::string,std::string,int,int,int,int)>(Plugin,"Load_Animation");
    auto Scale = Function<void(*)(std::string,float,float,float)>(Plugin,"Set_Scale");
    auto Paste = Function<void(*)(std::string,int,int,int)>(Plugin,"Paste");
    auto FPS = Function<bool(*)(std::string,std::string,float)>(Plugin,"Set_Animation_FPS");
    auto Select = Function<bool(*)(std::string,std::string)>(Plugin,"Set_Animation");
    auto Render = Function<void(*)()>(Plugin,"Render");
    auto Read = []() {
        std::vector<unsigned char> Pixels(640*240*3);
        glFinish();
        // This Windows SDL/OpenGL configuration preserves the rendered back
        // buffer after swap; its front buffer may be unavailable remotely.
        glReadBuffer(GL_BACK);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(0,0,640,240,GL_RGB,GL_UNSIGNED_BYTE,Pixels.data());
        REQUIRE(glGetError() == GL_NO_ERROR);
        return Pixels;
    };
    Add("Check", "Animation rendering check", 640, 240);
    Function<void(*)(bool)>(Plugin,"Set_GUI_Visible")(false);
    for(auto Name : {"Adam", "Mirror", "Eve"}) {
        const std::string File = std::string("Examples/Characters/") + (std::string(Name)=="Eve" ? "Eve" : "Adam") + "_Movement.png";
        REQUIRE(Load(Name,"Standing",File,64,64,0,8));
        REQUIRE(FPS(Name,"Standing",.01f));
        Scale(Name, std::string(Name)=="Adam" ? 3 : -3,3,1);
    }
    Paste("Adam",0,24,0); Paste("Mirror",416,24,0); Paste("Eve",640,24,0);
    Render();
    auto Pixels = Read();
    int Adam_Pixels = 0, Eve_Pixels = 0;
    for(int Y=24; Y<216; ++Y) for(int X=0; X<192; ++X) {
        int A = (Y*640+X)*3, B = (Y*640+415-X)*3, E = (Y*640+448+X)*3;
        for(int C=0; C<3; ++C) REQUIRE(Pixels[A+C] == Pixels[B+C]);
        if(Pixels[A]!=24 || Pixels[A+1]!=28 || Pixels[A+2]!=34) ++Adam_Pixels;
        if(Pixels[E]!=24 || Pixels[E+1]!=28 || Pixels[E+2]!=34) ++Eve_Pixels;
    }
    REQUIRE(Adam_Pixels > 1000 && Adam_Pixels < 30000);
    REQUIRE(Eve_Pixels > 1000 && Eve_Pixels < 30000);
    if(argc > 1) {
        std::ofstream Image(argv[1],std::ios::binary);
        Image << "P6\n640 240\n255\n";
        for(int Y=239;Y>=0;--Y) Image.write(reinterpret_cast<char*>(Pixels.data()+Y*640*3),640*3);
    }
    // A different atlas cell must produce different rendered pixels.
    REQUIRE(Load("Adam","Second","Examples/Characters/Adam_Movement.png",64,64,1,1));
    REQUIRE(Select("Adam","Second"));
    Render();
    REQUIRE(Read() != Pixels);
    Function<void(*)()>(Plugin,"Close_All")();
    Function<void(*)()>(Plugin,"Destructor")();
    FreeLibrary(Plugin);
    std::cout << "GRAPHICS_ANIMATION_RENDER_PASSED\n";
}
