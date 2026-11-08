#include "Appeal.h"
#include <GL/gl.h>
#include <iostream>
bool lit(){bool yes=false;for(GLenum b:{GL_FRONT,GL_BACK}){unsigned char p[3]{};
    glReadBuffer(b);glReadPixels(320,240,1,1,GL_RGB,GL_UNSIGNED_BYTE,p);if(p[1]>50)yes=true;}return yes;}
int main(int argc,char**argv){if(argc!=2)return 1;Appeal app;app.Set_Resource_Folder("./Resources");app.Run(argv[1]);
    if(!lit()){std::cerr<<"Model missing\n";return 1;}app.Run_Function("Move_Model");
    if(lit()){std::cerr<<"Model did not move out of view\n";return 1;}std::cout<<"GAME_VIEW3D_TESTS_PASSED\n";}
