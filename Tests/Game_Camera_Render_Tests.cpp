#include "Appeal.h"
#include <GL/gl.h>
#include <iostream>
int main(int argc,char**argv){
    if(argc!=2)return 1;Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    bool found=false;for(GLenum b:{GL_FRONT,GL_BACK}){unsigned char p[3]{};glReadBuffer(b);glReadPixels(400,300,1,1,GL_RGB,GL_UNSIGNED_BYTE,p);
        if(p[0]==102&&p[1]==204&&p[2]==51)found=true;}
    if(!found){std::cerr<<"Camera rectangle missing\n";return 1;}std::cout<<"GAME_CAMERA_RENDER_TESTS_PASSED\n";
}
