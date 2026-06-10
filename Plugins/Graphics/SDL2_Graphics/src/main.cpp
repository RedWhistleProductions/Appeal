#ifdef __MINGW32__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
    #include "SDL2/SDL_opengl.h"
    #include "SDL2/SDL_syswm.h"
    #include <windows.h>
#endif

#ifdef __linux__
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_opengl.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

#ifdef APPEAL_USE_ASSIMP
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>
    #include <assimp/scene.h>
#endif

#include <cmath>
#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const float Pi = 3.1415926535f;

float Degrees_To_Radians(float Degrees)
{
    return Degrees * Pi / 180.0f;
}

struct Vec3
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
};

struct Vec2
{
    float X = 0.0f;
    float Y = 0.0f;
};

struct Window_State
{
    SDL_Window *Window = nullptr;
    SDL_GLContext GL_Context = nullptr;
    int Width = 0;
    int Height = 0;
    SDL_Color Clear_Color = {24, 28, 34, 255};
    int View_X = 0;
    int View_Y = 0;
    int View_W = 0;
    int View_H = 0;
    bool Custom_Viewport = false;
    bool Perspective = true;
    float Fovy = 45.0f;
    float Near = 0.1f;
    float Far = 100.0f;
    float Ortho_Left = 0.0f;
    float Ortho_Right = 640.0f;
    float Ortho_Bottom = 360.0f;
    float Ortho_Top = 0.0f;
    Vec3 Camera_Location = {0.0f, 0.0f, -6.0f};
    Vec3 Camera_Orientation = {0.0f, 0.0f, 0.0f};
#ifdef __MINGW32__
    HICON Big_Icon = nullptr;
    HICON Small_Icon = nullptr;
#endif
};

struct Mesh_Triangle
{
    int A = 0;
    int B = 0;
    int C = 0;
    int Material = 0;
};

struct Mesh_Material
{
    std::string Texture_File;
    GLuint Texture_ID = 0;
    SDL_Color Diffuse = {120, 220, 170, 255};
};

struct Asset_State
{
    std::string File;
    std::string Texture;
    std::string Shader;
    std::string Mesh;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float Yaw = 0.0f;
    float Pitch = 0.0f;
    float Roll = 0.0f;
    float Scale_X = 1.0f;
    float Scale_Y = 1.0f;
    float Scale_Z = 1.0f;
};

struct Mesh_State
{
    std::string File;
    std::vector<Vec3> Vertices;
    std::vector<Vec3> Normals;
    std::vector<Vec2> UVs;
    std::vector<std::pair<int, int>> Edges;
    std::vector<Mesh_Triangle> Triangles;
    std::vector<Mesh_Material> Materials;
};

struct Texture_State
{
    std::string File;
    GLuint Texture = 0;
    int Width = 0;
    int Height = 0;
};

struct Font_State
{
    std::string File;
    int Size = 0;
    TTF_Font *Font = nullptr;
};

struct Text_Command
{
    std::string Font;
    std::string Text;
    float X = 0.0f;
    float Y = 0.0f;
    SDL_Color Color = {230, 236, 245, 255};
};

struct Rect_Command
{
    std::string Name;
    float X = 0.0f;
    float Y = 0.0f;
    float W = 0.0f;
    float H = 0.0f;
    SDL_Color Color = {255, 255, 255, 255};
};

struct GUI_Command
{
    std::string Kind;
    std::string Name;
    std::string Text;
    std::string Value_Text;
    std::string Extra_Text;
    std::string Tooltip_Text;
    int Min = 0;
    int Max = 0;
    int Value = 0;
    float Float_Min = 0.0f;
    float Float_Max = 0.0f;
    float Float_Value = 0.0f;
    float Float_Step = 0.0f;
};

struct GUI_File_Picker_State
{
    std::string Root;
    std::string Start;
    std::string Current;
    std::string Selected;
    std::string Selected_Path;
    std::string Selected_Relative;
    bool Selected_Is_Directory = false;
    bool Changed = false;
    bool Initialized = false;
    int Selected_Index = -1;
};

std::string Resources = "./Resources";
std::string Graphics_Path = "./Resources/Graphics/";
std::string Icon_Path = "./Resources/Graphics/Icons/";
std::string Texture_Path = "./Resources/Graphics/Textures/";
std::string Font_Path = "./Resources/Graphics/Fonts/";
std::string Shader_Path = "./Resources/Graphics/Shaders/";
std::string Mesh_Path = "./Resources/Graphics/Meshes/";
std::string Material_Path = "./Resources/Graphics/Materials/";
std::string GUI_Path = "./Resources/Graphics/GUI/";

std::map<std::string, Window_State> Windows;
std::map<std::string, Asset_State> Assets;
std::map<std::string, Texture_State> Textures;
std::map<std::string, Font_State> Fonts;
std::map<std::string, std::string> Shaders;
std::map<std::string, Mesh_State> Meshes;
std::vector<Text_Command> Text_Commands;
std::map<std::string, Rect_Command> Rect_Commands;
std::vector<GUI_Command> GUI_Commands;
std::map<std::string, bool> GUI_Clicked;
std::map<std::string, int> GUI_Slider_Values;
std::map<std::string, bool> GUI_Slider_Changed;
std::map<std::string, float> GUI_Float_Values;
std::map<std::string, bool> GUI_Float_Changed_Values;
std::map<std::string, std::string> GUI_Text_Values;
std::map<std::string, bool> GUI_Text_Changed_Values;
std::map<std::string, int> GUI_Int_Values;
std::map<std::string, bool> GUI_Int_Changed_Values;
std::map<std::string, int> GUI_Selection_Values;
std::map<std::string, std::string> GUI_Selection_Text_Values;
std::map<std::string, bool> GUI_Selection_Changed_Values;
std::map<std::string, bool> GUI_Checkbox_Values;
std::map<std::string, bool> GUI_Checkbox_Changed_Values;
std::map<std::string, GUI_File_Picker_State> GUI_File_Pickers;
std::map<std::string, std::string> GUI_Button_Functions;
std::map<std::string, std::string> GUI_Slider_Functions;
std::map<std::string, std::string> GUI_Checkbox_Functions;
std::map<std::string, std::string> GUI_Functions;
void (*Run_Function_Callback)(std::string Function) = nullptr;
void (*Run_Analog_Function_Callback)(std::string Function, int Value) = nullptr;
std::string Active_Asset;

std::string Focused_Window;
int Target_FPS = 60;
bool SDL_Ready = false;
bool IMG_Ready = false;
bool TTF_Ready = false;
bool ImGui_Ready = false;
bool GUI_Visible = false;
bool GUI_Docking_Enabled = true;
bool DEBUG = false;

extern "C" void Delete_Window(std::string Name);

void Run_GUI_Function(const std::string &Name, int Value);
void Run_GUI_Digital_Function(const std::string &Name);

std::string Directory_Name(std::string File)
{
    size_t Slash = File.find_last_of("/\\");
    if(Slash == std::string::npos)
    {
        return "";
    }
    return File.substr(0, Slash + 1);
}

std::string Normalize_Path(std::string Path)
{
    for(char &Character : Path)
    {
        if(Character == '\\') Character = '/';
    }
    return Path;
}

GLuint Create_GL_Texture_From_Surface(SDL_Surface *Loaded, int *Width = nullptr, int *Height = nullptr)
{
    SDL_Surface *Surface = SDL_ConvertSurfaceFormat(Loaded, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(Loaded);
    if(Surface == nullptr)
    {
        if(DEBUG) std::cout << "Texture format conversion failed: " << SDL_GetError() << std::endl;
        return 0;
    }

    GLuint Texture = 0;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Surface->w, Surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, Surface->pixels);

    if(Width != nullptr) *Width = Surface->w;
    if(Height != nullptr) *Height = Surface->h;
    SDL_FreeSurface(Surface);
    return Texture;
}

GLuint Load_GL_Texture_File(const std::string &File, int *Width = nullptr, int *Height = nullptr)
{
    SDL_Surface *Loaded = IMG_Load(File.c_str());
    if(Loaded == nullptr)
    {
        Loaded = SDL_LoadBMP(File.c_str());
    }

    if(Loaded == nullptr)
    {
        if(DEBUG) std::cout << "Texture did not load: " << File << std::endl
                  << "SDL_image Error: " << IMG_GetError() << std::endl
                  << "SDL Error: " << SDL_GetError() << std::endl;
        return 0;
    }

    return Create_GL_Texture_From_Surface(Loaded, Width, Height);
}

#ifdef APPEAL_USE_ASSIMP
GLuint Load_Assimp_Embedded_Texture(const aiScene *Scene, const aiString &Texture_Path)
{
    std::string Path = Texture_Path.C_Str();
    if(Path.size() < 2 || Path[0] != '*')
    {
        return 0;
    }

    int Texture_Index = -1;
    try
    {
        Texture_Index = std::stoi(Path.substr(1));
    }
    catch(...)
    {
        return 0;
    }

    if(Texture_Index < 0 || static_cast<unsigned int>(Texture_Index) >= Scene->mNumTextures)
    {
        return 0;
    }

    const aiTexture *Texture = Scene->mTextures[Texture_Index];
    if(Texture == nullptr)
    {
        return 0;
    }

    if(Texture->mHeight == 0)
    {
        SDL_RWops *RW = SDL_RWFromConstMem(Texture->pcData, Texture->mWidth);
        if(RW == nullptr)
        {
            return 0;
        }
        SDL_Surface *Surface = IMG_Load_RW(RW, 1);
        if(Surface == nullptr)
        {
            if(DEBUG) std::cout << "Embedded texture did not load: " << Path << std::endl
                      << "SDL_image Error: " << IMG_GetError() << std::endl;
            return 0;
        }
        return Create_GL_Texture_From_Surface(Surface);
    }

    SDL_Surface *Surface = SDL_CreateRGBSurfaceWithFormatFrom(
        Texture->pcData,
        Texture->mWidth,
        Texture->mHeight,
        32,
        Texture->mWidth * 4,
        SDL_PIXELFORMAT_ABGR8888
    );
    if(Surface == nullptr)
    {
        return 0;
    }

    SDL_Surface *Copy = SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(Surface);
    if(Copy == nullptr)
    {
        return 0;
    }
    return Create_GL_Texture_From_Surface(Copy);
}
#endif

void Set_Viewport_Projection(const Window_State &Window)
{
    int View_W = Window.View_W > 0 ? Window.View_W : Window.Width;
    int View_H = Window.View_H > 0 ? Window.View_H : Window.Height;
    int View_X = Window.View_X;
    int View_Y = Window.View_Y;
    if(View_H <= 0) View_H = 1;

    glViewport(View_X, View_Y, View_W, View_H);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(Window.Perspective)
    {
        double Aspect = static_cast<double>(View_W) / static_cast<double>(View_H);
        double Top = std::tan(Degrees_To_Radians(Window.Fovy) * 0.5) * Window.Near;
        double Bottom = -Top;
        double Right = Top * Aspect;
        double Left = -Right;
        glFrustum(Left, Right, Bottom, Top, Window.Near, Window.Far);
    }
    else
    {
        glOrtho(Window.Ortho_Left, Window.Ortho_Right, Window.Ortho_Bottom, Window.Ortho_Top, Window.Near, Window.Far);
    }
}

void Set_Model_View(const Window_State &Window)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(-Window.Camera_Orientation.Z, 0.0f, 0.0f, 1.0f);
    glRotatef(-Window.Camera_Orientation.Y, 1.0f, 0.0f, 0.0f);
    glRotatef(-Window.Camera_Orientation.X, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, -1.0f);
    glTranslatef(-Window.Camera_Location.X, -Window.Camera_Location.Y, -Window.Camera_Location.Z);
}

void Set_2D_Projection(const Window_State &Window)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, Window.Width, Window.Height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool Ensure_SDL()
{
    if(SDL_Ready)
    {
        return true;
    }

    if(SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        if(DEBUG) std::cout << "SDL video could not initialize: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Ready = true;

    int Image_Flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int Image_Result = IMG_Init(Image_Flags);
    if((Image_Result & Image_Flags) != Image_Flags)
    {
        if(DEBUG) std::cout << "SDL_image partial initialization: " << IMG_GetError() << std::endl;
    }
    IMG_Ready = Image_Result != 0;

    if(TTF_Init() == 0)
    {
        TTF_Ready = true;
    }
    else
    {
        if(DEBUG) std::cout << "SDL_ttf could not initialize: " << TTF_GetError() << std::endl;
    }

    return true;
}

Window_State *Find_Window(std::string Name)
{
    auto It = Windows.find(Name);
    if(It == Windows.end())
    {
        if(DEBUG) std::cout << "Graphics window not found: " << Name << std::endl;
        return nullptr;
    }

    return &It->second;
}

bool Window_Exists(std::string Name)
{
    return Windows.find(Name) != Windows.end();
}

extern "C" bool Window_Open(std::string Name)
{
    return Window_Exists(Name);
}

void Destroy_Window(Window_State &State)
{
#ifdef __MINGW32__
    if(State.Big_Icon != nullptr)
    {
        DestroyIcon(State.Big_Icon);
        State.Big_Icon = nullptr;
    }
    if(State.Small_Icon != nullptr)
    {
        DestroyIcon(State.Small_Icon);
        State.Small_Icon = nullptr;
    }
#endif

    if(State.GL_Context != nullptr)
    {
        SDL_GL_DeleteContext(State.GL_Context);
        State.GL_Context = nullptr;
    }

    if(State.Window != nullptr)
    {
        SDL_DestroyWindow(State.Window);
        State.Window = nullptr;
    }
}

Window_State *Render_Window()
{
    if(Windows.empty())
    {
        return nullptr;
    }

    if(Focused_Window != "")
    {
        Window_State *State = Find_Window(Focused_Window);
        if(State != nullptr) return State;
    }

    return &Windows.begin()->second;
}

Vec3 Rotate_Point(Vec3 Point, float Yaw, float Pitch, float Roll)
{
    float Yaw_Radians = Degrees_To_Radians(Yaw);
    float Pitch_Radians = Degrees_To_Radians(Pitch);
    float Roll_Radians = Degrees_To_Radians(Roll);

    float Cos_Yaw = std::cos(Yaw_Radians);
    float Sin_Yaw = std::sin(Yaw_Radians);
    float X1 = Point.X * Cos_Yaw + Point.Z * Sin_Yaw;
    float Z1 = -Point.X * Sin_Yaw + Point.Z * Cos_Yaw;
    Point.X = X1;
    Point.Z = Z1;

    float Cos_Pitch = std::cos(Pitch_Radians);
    float Sin_Pitch = std::sin(Pitch_Radians);
    float Y2 = Point.Y * Cos_Pitch - Point.Z * Sin_Pitch;
    float Z2 = Point.Y * Sin_Pitch + Point.Z * Cos_Pitch;
    Point.Y = Y2;
    Point.Z = Z2;

    float Cos_Roll = std::cos(Roll_Radians);
    float Sin_Roll = std::sin(Roll_Radians);
    float X3 = Point.X * Cos_Roll - Point.Y * Sin_Roll;
    float Y3 = Point.X * Sin_Roll + Point.Y * Cos_Roll;
    Point.X = X3;
    Point.Y = Y3;

    return Point;
}

bool Project_Point(const Window_State &Window, Vec3 Point, SDL_Point &Output)
{
    Point.X -= Window.Camera_Location.X;
    Point.Y -= Window.Camera_Location.Y;
    Point.Z -= Window.Camera_Location.Z;
    Point = Rotate_Point(Point, -Window.Camera_Orientation.X, -Window.Camera_Orientation.Y, -Window.Camera_Orientation.Z);

    int View_W = Window.View_W > 0 ? Window.View_W : Window.Width;
    int View_H = Window.View_H > 0 ? Window.View_H : Window.Height;
    int View_X = Window.View_X;
    int View_Y = Window.View_Y;

    if(Window.Perspective)
    {
        if(Point.Z <= Window.Near || Point.Z >= Window.Far)
        {
            return false;
        }

        float Focal_Length = (View_H * 0.5f) / std::tan(Degrees_To_Radians(Window.Fovy) * 0.5f);
        Output.x = View_X + static_cast<int>((View_W * 0.5f) + (Point.X * Focal_Length / Point.Z));
        Output.y = View_Y + static_cast<int>((View_H * 0.5f) - (Point.Y * Focal_Length / Point.Z));
        return true;
    }

    float Ortho_W = Window.Ortho_Right - Window.Ortho_Left;
    float Ortho_H = Window.Ortho_Bottom - Window.Ortho_Top;
    if(Ortho_W == 0.0f || Ortho_H == 0.0f) return false;

    Output.x = View_X + static_cast<int>(((Point.X - Window.Ortho_Left) / Ortho_W) * View_W);
    Output.y = View_Y + static_cast<int>(((Point.Y - Window.Ortho_Top) / Ortho_H) * View_H);
    return true;
}

Mesh_State Make_Cube_Mesh(std::string File)
{
    Mesh_State Mesh;
    Mesh.File = File;
    Mesh.Vertices = {
        {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}
    };
    Mesh.Normals = {
        {-0.577f, -0.577f, -0.577f}, {0.577f, -0.577f, -0.577f}, {0.577f, 0.577f, -0.577f}, {-0.577f, 0.577f, -0.577f},
        {-0.577f, -0.577f, 0.577f}, {0.577f, -0.577f, 0.577f}, {0.577f, 0.577f, 0.577f}, {-0.577f, 0.577f, 0.577f}
    };
    Mesh.UVs = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };
    Mesh.Edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    Mesh.Materials.push_back({"", 0, {120, 220, 170, 255}});
    Mesh.Triangles = {
        {0, 1, 2, 0}, {0, 2, 3, 0},
        {4, 6, 5, 0}, {4, 7, 6, 0},
        {0, 4, 5, 0}, {0, 5, 1, 0},
        {1, 5, 6, 0}, {1, 6, 2, 0},
        {2, 6, 7, 0}, {2, 7, 3, 0},
        {3, 7, 4, 0}, {3, 4, 0, 0}
    };
    return Mesh;
}

int Parse_Obj_Vertex_Index(const std::string &Token, int Vertex_Count)
{
    if(Token.empty())
    {
        return -1;
    }

    size_t Slash = Token.find('/');
    std::string Index_Text = Slash == std::string::npos ? Token : Token.substr(0, Slash);
    if(Index_Text.empty())
    {
        return -1;
    }

    int Index = 0;
    try
    {
        Index = std::stoi(Index_Text);
    }
    catch(...)
    {
        return -1;
    }

    if(Index > 0)
    {
        return Index - 1;
    }

    if(Index < 0)
    {
        return Vertex_Count + Index;
    }

    return -1;
}

Mesh_State Load_Mesh_File(std::string File)
{
    Mesh_State Mesh;
    Mesh.File = File;

#ifdef APPEAL_USE_ASSIMP
    Assimp::Importer Importer;
    const aiScene *Scene = Importer.ReadFile(
        File,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_GenNormals |
        aiProcess_SortByPType
    );

    if(Scene != nullptr && Scene->HasMeshes())
    {
        std::string Base_Directory = Directory_Name(File);
        if(Scene->HasMaterials())
        {
            for(unsigned int Material_Index = 0; Material_Index < Scene->mNumMaterials; ++Material_Index)
            {
                aiMaterial *Imported_Material = Scene->mMaterials[Material_Index];
                Mesh_Material Material;

                aiColor4D Diffuse;
                if(AI_SUCCESS == aiGetMaterialColor(Imported_Material, AI_MATKEY_COLOR_DIFFUSE, &Diffuse))
                {
                    Material.Diffuse = {
                        static_cast<Uint8>(std::max(0.0f, std::min(1.0f, Diffuse.r)) * 255.0f),
                        static_cast<Uint8>(std::max(0.0f, std::min(1.0f, Diffuse.g)) * 255.0f),
                        static_cast<Uint8>(std::max(0.0f, std::min(1.0f, Diffuse.b)) * 255.0f),
                        static_cast<Uint8>(std::max(0.0f, std::min(1.0f, Diffuse.a)) * 255.0f)
                    };
                }

                aiString Texture_Path;
                if(Imported_Material->GetTexture(aiTextureType_DIFFUSE, 0, &Texture_Path) == AI_SUCCESS ||
                   Imported_Material->GetTexture(aiTextureType_BASE_COLOR, 0, &Texture_Path) == AI_SUCCESS)
                {
                    std::string Texture_Name = Texture_Path.C_Str();
                    if(Texture_Name.size() > 0 && Texture_Name[0] == '*')
                    {
                        Material.Texture_File = Texture_Name;
                        Material.Texture_ID = Load_Assimp_Embedded_Texture(Scene, Texture_Path);
                    }
                    else
                    {
                        Material.Texture_File = Normalize_Path(Base_Directory + Texture_Name);
                        Material.Texture_ID = Load_GL_Texture_File(Material.Texture_File);
                    }
                }

                Mesh.Materials.push_back(Material);
            }
        }

        if(Mesh.Materials.empty())
        {
            Mesh.Materials.push_back({"", 0, {120, 220, 170, 255}});
        }

        for(unsigned int Mesh_Index = 0; Mesh_Index < Scene->mNumMeshes; ++Mesh_Index)
        {
            const aiMesh *Imported_Mesh = Scene->mMeshes[Mesh_Index];
            int Vertex_Offset = static_cast<int>(Mesh.Vertices.size());
            int Material_Index = Imported_Mesh->mMaterialIndex < Mesh.Materials.size() ? static_cast<int>(Imported_Mesh->mMaterialIndex) : 0;

            for(unsigned int Vertex_Index = 0; Vertex_Index < Imported_Mesh->mNumVertices; ++Vertex_Index)
            {
                const aiVector3D &Vertex = Imported_Mesh->mVertices[Vertex_Index];
                Mesh.Vertices.push_back({Vertex.x, Vertex.y, Vertex.z});

                if(Imported_Mesh->HasNormals())
                {
                    const aiVector3D &Normal = Imported_Mesh->mNormals[Vertex_Index];
                    Mesh.Normals.push_back({Normal.x, Normal.y, Normal.z});
                }
                else
                {
                    Mesh.Normals.push_back({0.0f, 1.0f, 0.0f});
                }

                if(Imported_Mesh->HasTextureCoords(0))
                {
                    const aiVector3D &UV = Imported_Mesh->mTextureCoords[0][Vertex_Index];
                    Mesh.UVs.push_back({UV.x, 1.0f - UV.y});
                }
                else
                {
                    Mesh.UVs.push_back({0.0f, 0.0f});
                }
            }

            for(unsigned int Face_Index = 0; Face_Index < Imported_Mesh->mNumFaces; ++Face_Index)
            {
                const aiFace &Face = Imported_Mesh->mFaces[Face_Index];
                if(Face.mNumIndices < 2) continue;

                for(unsigned int Index = 0; Index < Face.mNumIndices; ++Index)
                {
                    int A = Vertex_Offset + static_cast<int>(Face.mIndices[Index]);
                    int B = Vertex_Offset + static_cast<int>(Face.mIndices[(Index + 1) % Face.mNumIndices]);
                    Mesh.Edges.push_back({A, B});
                }

                if(Face.mNumIndices == 3)
                {
                    Mesh.Triangles.push_back({
                        Vertex_Offset + static_cast<int>(Face.mIndices[0]),
                        Vertex_Offset + static_cast<int>(Face.mIndices[1]),
                        Vertex_Offset + static_cast<int>(Face.mIndices[2]),
                        Material_Index
                    });
                }
            }
        }

        if(!Mesh.Vertices.empty() && (!Mesh.Edges.empty() || !Mesh.Triangles.empty()))
        {
            if(DEBUG)
            {
                std::cout << "Assimp loaded mesh: " << File
                          << " Vertices: " << Mesh.Vertices.size()
                          << " Edges: " << Mesh.Edges.size()
                          << " Triangles: " << Mesh.Triangles.size()
                          << std::endl;
            }
            return Mesh;
        }
    }
    else
    {
        if(DEBUG) std::cout << "Assimp did not load mesh: " << File << std::endl
                  << "Assimp Error: " << Importer.GetErrorString() << std::endl;
    }
#endif

    std::ifstream Input(File.c_str());
    if(!Input.is_open())
    {
        return Make_Cube_Mesh(File);
    }

    std::string Line;
    while(std::getline(Input, Line))
    {
        if(Line == "" || Line[0] == '#') continue;

        std::istringstream Stream(Line);
        std::string Type;
        Stream >> Type;

        if(Type == "v" || Type == "V")
        {
            Vec3 Vertex;
            Stream >> Vertex.X >> Vertex.Y >> Vertex.Z;
            Mesh.Vertices.push_back(Vertex);
            Mesh.Normals.push_back({0.0f, 1.0f, 0.0f});
            Mesh.UVs.push_back({0.0f, 0.0f});
        }
        else if(Type == "vn" || Type == "VN")
        {
            Vec3 Normal;
            Stream >> Normal.X >> Normal.Y >> Normal.Z;
            if(!Mesh.Normals.empty())
            {
                Mesh.Normals.back() = Normal;
            }
        }
        else if(Type == "vt" || Type == "VT")
        {
            Vec2 UV;
            Stream >> UV.X >> UV.Y;
            if(!Mesh.UVs.empty())
            {
                Mesh.UVs.back() = {UV.X, 1.0f - UV.Y};
            }
        }
        else if(Type == "e" || Type == "E")
        {
            int A, B;
            Stream >> A >> B;
            Mesh.Edges.push_back({A, B});
        }
        else if(Type == "f" || Type == "F")
        {
            std::vector<int> Face;
            std::string Token;
            while(Stream >> Token)
            {
                int Index = Parse_Obj_Vertex_Index(Token, static_cast<int>(Mesh.Vertices.size()));
                if(Index >= 0 && Index < static_cast<int>(Mesh.Vertices.size()))
                {
                    Face.push_back(Index);
                }
            }

            if(Face.size() >= 2)
            {
                for(size_t Index = 0; Index < Face.size(); ++Index)
                {
                    int A = Face[Index];
                    int B = Face[(Index + 1) % Face.size()];
                    Mesh.Edges.push_back({A, B});
                }
            }

            if(Face.size() >= 3)
            {
                if(Mesh.Materials.empty())
                {
                    Mesh.Materials.push_back({"", 0, {120, 220, 170, 255}});
                }
                for(size_t Index = 1; Index + 1 < Face.size(); ++Index)
                {
                    Mesh.Triangles.push_back({Face[0], Face[Index], Face[Index + 1], 0});
                }
            }
        }
    }

    if(Mesh.Vertices.empty() || (Mesh.Edges.empty() && Mesh.Triangles.empty()))
    {
        return Make_Cube_Mesh(File);
    }

    return Mesh;
}

void Destroy_Texture(Texture_State &State)
{
    if(State.Texture != 0)
    {
        glDeleteTextures(1, &State.Texture);
        State.Texture = 0;
    }
}

void Destroy_Font(Font_State &State)
{
    if(State.Font != nullptr)
    {
        TTF_CloseFont(State.Font);
        State.Font = nullptr;
    }
}

bool Make_Current_GL_Context()
{
    for(auto &Item : Windows)
    {
        Window_State &State = Item.second;
        if(State.Window != nullptr && State.GL_Context != nullptr)
        {
            SDL_GL_MakeCurrent(State.Window, State.GL_Context);
            return true;
        }
    }

    return false;
}

void Destroy_GL_Resources()
{
    if(!Make_Current_GL_Context())
    {
        return;
    }

    for(auto &Item : Textures)
    {
        Destroy_Texture(Item.second);
    }
    Textures.clear();

    for(auto &Mesh_Item : Meshes)
    {
        for(Mesh_Material &Material : Mesh_Item.second.Materials)
        {
            if(Material.Texture_ID != 0)
            {
                glDeleteTextures(1, &Material.Texture_ID);
                Material.Texture_ID = 0;
            }
        }
    }
    Meshes.clear();
}

bool Ensure_ImGui(Window_State &Window)
{
    if(ImGui_Ready)
    {
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &IO = ImGui::GetIO();
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    if(!ImGui_ImplSDL2_InitForOpenGL(Window.Window, Window.GL_Context))
    {
        if(DEBUG) std::cout << "Dear ImGui SDL2 backend could not initialize" << std::endl;
        return false;
    }

    if(!ImGui_ImplOpenGL2_Init())
    {
        if(DEBUG) std::cout << "Dear ImGui OpenGL backend could not initialize" << std::endl;
        ImGui_ImplSDL2_Shutdown();
        return false;
    }

    ImGui_Ready = true;
    return true;
}

void Begin_ImGui_Frame(Window_State &Window)
{
    if(!Ensure_ImGui(Window)) return;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    if(GUI_Visible && GUI_Docking_Enabled)
    {
        ImGui::DockSpaceOverViewport();
    }
}

void End_ImGui_Frame(Window_State &Window)
{
    if(!ImGui_Ready) return;

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void Draw_Text_Command(Window_State &Window, const Text_Command &Command)
{
    auto It = Fonts.find(Command.Font);
    if(It == Fonts.end() || It->second.Font == nullptr)
    {
        if(Fonts.empty())
        {
            if(DEBUG) std::cout << "Font not loaded: " << Command.Font << std::endl;
            return;
        }
        It = Fonts.begin();
        if(It->second.Font == nullptr)
        {
            if(DEBUG) std::cout << "Font not loaded: " << Command.Font << std::endl;
            return;
        }
    }

    SDL_Surface *Surface = TTF_RenderUTF8_Blended(It->second.Font, Command.Text.c_str(), Command.Color);
    if(Surface == nullptr)
    {
        if(DEBUG) std::cout << "Text surface could not be created: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface *Converted = SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(Surface);
    if(Converted == nullptr)
    {
        if(DEBUG) std::cout << "Text texture conversion failed: " << SDL_GetError() << std::endl;
        return;
    }

    GLuint Texture = 0;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Converted->w, Converted->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, Converted->pixels);

    float X = Command.X;
    float Y = Command.Y;
    float W = static_cast<float>(Converted->w);
    float H = static_cast<float>(Converted->h);
    SDL_FreeSurface(Converted);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(X, Y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(X + W, Y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(X + W, Y + H);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(X, Y + H);
    glEnd();
    glDeleteTextures(1, &Texture);
}

void Ensure_GUI_Window(bool &In_Window)
{
    if(!In_Window)
    {
        ImGui::Begin("Appeal");
        In_Window = true;
    }
}

std::vector<std::string> Split_GUI_Items(const std::string &Items)
{
    std::vector<std::string> Result;
    std::stringstream Stream(Items);
    std::string Item;
    while(std::getline(Stream, Item, '|'))
    {
        if(!Item.empty())
        {
            Result.push_back(Item);
        }
    }
    return Result;
}

std::string Normalize_GUI_Path(std::string Path)
{
    std::replace(Path.begin(), Path.end(), '\\', '/');
    while(Path.find("//") != std::string::npos)
    {
        Path.replace(Path.find("//"), 2, "/");
    }
    if(Path.size() > 2 && Path.substr(0, 2) == "./")
    {
        Path = Path.substr(2);
    }
    if(Path.size() > 1 && Path.back() == '/')
    {
        Path.pop_back();
    }
    return Path;
}

fs::path Canonical_Or_Absolute(fs::path Path)
{
    try
    {
        return fs::weakly_canonical(Path);
    }
    catch(const std::exception &)
    {
        return fs::absolute(Path);
    }
}

fs::path Resolve_Resource_Path(const std::string &Path)
{
    fs::path Candidate(Path);
    if(Candidate.is_absolute())
    {
        return Canonical_Or_Absolute(Candidate);
    }

    if(Path == "Resources")
    {
        return Canonical_Or_Absolute(fs::path(Resources));
    }

    fs::path Resource_Root = Canonical_Or_Absolute(fs::path(Resources));
    fs::path Resource_Name = Resource_Root.filename();
    if(!Resource_Name.empty() && Path.rfind(Resource_Name.string() + "/", 0) == 0)
    {
        return Canonical_Or_Absolute(Resource_Root.parent_path() / Candidate);
    }

    return Canonical_Or_Absolute(Resource_Root / Candidate);
}

bool Path_Is_Inside(const fs::path &Root, const fs::path &Path)
{
    fs::path Root_Path = Canonical_Or_Absolute(Root);
    fs::path Target_Path = Canonical_Or_Absolute(Path);
    auto Root_It = Root_Path.begin();
    auto Target_It = Target_Path.begin();
    for(; Root_It != Root_Path.end(); ++Root_It, ++Target_It)
    {
        if(Target_It == Target_Path.end() || *Root_It != *Target_It)
        {
            return false;
        }
    }
    return true;
}

std::string Relative_Path_Text(const fs::path &Base, const fs::path &Path)
{
    try
    {
        return Normalize_GUI_Path(fs::relative(Canonical_Or_Absolute(Path), Canonical_Or_Absolute(Base)).generic_string());
    }
    catch(const std::exception &)
    {
        return Normalize_GUI_Path(Path.generic_string());
    }
}

fs::path Picker_Root_Path(const GUI_File_Picker_State &State)
{
    return Resolve_Resource_Path(State.Root.empty() ? "Resources" : State.Root);
}

fs::path Picker_Start_Path(const GUI_File_Picker_State &State)
{
    fs::path Root_Path = Picker_Root_Path(State);
    if(State.Start.empty() || State.Start == ".")
    {
        return Root_Path;
    }

    fs::path Start = Resolve_Resource_Path(State.Start);
    if(Path_Is_Inside(Root_Path, Start))
    {
        return Start;
    }
    return Root_Path;
}

fs::path Picker_Current_Path(const GUI_File_Picker_State &State)
{
    if(State.Current.empty() || State.Current == ".")
    {
        return Picker_Root_Path(State);
    }
    return Resolve_Resource_Path(State.Current);
}

void Set_File_Picker_Current(GUI_File_Picker_State &State, const fs::path &Path)
{
    fs::path Root_Path = Picker_Root_Path(State);
    fs::path Candidate = Canonical_Or_Absolute(Path);
    if(!fs::exists(Candidate) || !fs::is_directory(Candidate) || !Path_Is_Inside(Root_Path, Candidate))
    {
        Candidate = Root_Path;
    }

    State.Current = Relative_Path_Text(Root_Path.parent_path(), Candidate);
    if(State.Current == ".")
    {
        State.Current = Normalize_GUI_Path(State.Root.empty() ? "Resources" : State.Root);
    }
    State.Selected.clear();
    State.Selected_Path.clear();
    State.Selected_Relative.clear();
    State.Selected_Is_Directory = false;
    State.Selected_Index = -1;
    State.Changed = true;
}

void Ensure_File_Picker_State(GUI_File_Picker_State &State, const std::string &Root, const std::string &Start)
{
    if(!State.Initialized || State.Root != Root || State.Start != Start)
    {
        State.Root = Root.empty() ? "Resources" : Root;
        State.Start = Start.empty() ? "." : Start;
        State.Initialized = true;
        State.Selected.clear();
        State.Selected_Path.clear();
        State.Selected_Relative.clear();
        State.Selected_Is_Directory = false;
        Set_File_Picker_Current(State, Picker_Start_Path(State));
    }
}

void Select_File_Picker_Item(GUI_File_Picker_State &State, const fs::path &Item_Path, const std::string &Display_Name, int Index)
{
    fs::path Root_Path = Picker_Root_Path(State);
    fs::path Start_Path = Picker_Start_Path(State);
    State.Selected = Display_Name;
    State.Selected_Index = Index;
    State.Selected_Is_Directory = fs::is_directory(Item_Path);
    State.Selected_Path = Relative_Path_Text(Root_Path.parent_path(), Item_Path);
    State.Selected_Relative = Relative_Path_Text(Start_Path, Item_Path);
    State.Changed = true;
}

void Select_File_Picker_Path(const std::string &Name, const std::string &Path)
{
    GUI_File_Picker_State &State = GUI_File_Pickers[Name];
    fs::path Root_Path = Picker_Root_Path(State);
    fs::path Item_Path = Resolve_Resource_Path(Path);
    if(!Path_Is_Inside(Root_Path, Item_Path) || !fs::exists(Item_Path))
    {
        return;
    }

    fs::path Parent_Path = Item_Path.parent_path();
    Set_File_Picker_Current(State, Parent_Path);
    State.Selected = fs::is_directory(Item_Path) ? Item_Path.filename().generic_string() + "/" : Item_Path.filename().generic_string();
    State.Selected_Is_Directory = fs::is_directory(Item_Path);
    State.Selected_Path = Relative_Path_Text(Root_Path.parent_path(), Item_Path);
    State.Selected_Relative = Relative_Path_Text(Picker_Start_Path(State), Item_Path);
    State.Selected_Index = -1;
    State.Changed = true;
}

bool Create_Game_Project_From_GUI(const std::string &Project_Name, const std::string &Picker_Name)
{
    std::string Name = Normalize_GUI_Path(Project_Name);
    if(Name.empty() || Name == "." || Name == ".." || Name.find('/') != std::string::npos)
    {
        return false;
    }

    try
    {
        fs::path Game_Folder = Resolve_Resource_Path("Appeal/Games/" + Name);
        fs::path Levels_Folder = Game_Folder / "Levels";
        fs::path Game_Script = Game_Folder / (Name + ".Appeal");

        if(fs::exists(Game_Folder) && !fs::is_directory(Game_Folder))
        {
            return false;
        }

        fs::create_directories(Levels_Folder);

        if(!fs::exists(Game_Script))
        {
            std::ofstream Output(Game_Script, std::ios::binary | std::ios::trunc);
            if(!Output)
            {
                return false;
            }
            Output << "Return\n";
        }

        GUI_Text_Values["Level_Game_Name"] = Name;
        GUI_Text_Changed_Values["Level_Game_Name"] = true;
        Select_File_Picker_Path(Picker_Name, "Appeal/Games/" + Name);
        Run_GUI_Digital_Function(Picker_Name);
        return true;
    }
    catch(const std::exception &)
    {
        return false;
    }
}

bool Create_Level_Project_From_GUI(const std::string &Game_Name, const std::string &Level_Name, const std::string &Picker_Name)
{
    std::string Game = Normalize_GUI_Path(Game_Name);
    std::string Level = Normalize_GUI_Path(Level_Name);
    if(Game.empty())
    {
        Game = Normalize_GUI_Path(GUI_File_Pickers["Game_File_Picker"].Selected_Relative);
    }
    if(Game.size() > 7 && Game.substr(Game.size() - 7) == ".Appeal")
    {
        Game = Game.substr(0, Game.size() - 7);
    }
    if(Game.empty() || Game == "." || Game == ".." || Game.find('/') != std::string::npos)
    {
        return false;
    }
    if(Level.empty() || Level == "." || Level == ".." || Level.find('/') != std::string::npos)
    {
        return false;
    }

    try
    {
        fs::path Level_Folder = Resolve_Resource_Path("Appeal/Games/" + Game + "/Levels/" + Level);
        fs::path Level_Script = Level_Folder / (Level + ".Appeal");

        if(fs::exists(Level_Folder) && !fs::is_directory(Level_Folder))
        {
            return false;
        }

        fs::create_directories(Level_Folder);

        if(!fs::exists(Level_Script))
        {
            std::ofstream Output(Level_Script, std::ios::binary | std::ios::trunc);
            if(!Output)
            {
                return false;
            }
            Output << "Return\n";
        }

        GUI_File_Picker_State &State = GUI_File_Pickers[Picker_Name];
        Ensure_File_Picker_State(State, "Resources", "Appeal/Games/" + Game + "/Levels");
        Select_File_Picker_Path(Picker_Name, "Appeal/Games/" + Game + "/Levels/" + Level);
        return true;
    }
    catch(const std::exception &)
    {
        return false;
    }
}

void Draw_File_Picker(const GUI_Command &Command)
{
    GUI_File_Picker_State &State = GUI_File_Pickers[Command.Name];
    Ensure_File_Picker_State(State, Command.Value_Text, Command.Extra_Text);

    fs::path Root_Path = Picker_Root_Path(State);
    fs::path Start_Path = Picker_Start_Path(State);
    fs::path Current_Path = Picker_Current_Path(State);
    if(!Path_Is_Inside(Root_Path, Current_Path))
    {
        Set_File_Picker_Current(State, Root_Path);
        Current_Path = Picker_Current_Path(State);
    }

    ImGui::TextUnformatted(Command.Text.c_str());
    ImGui::Text("Current: %s", State.Current.c_str());

    if(ImGui::SmallButton("Root"))
    {
        Set_File_Picker_Current(State, Root_Path);
        Current_Path = Picker_Current_Path(State);
        Run_GUI_Digital_Function(Command.Name);
    }
    ImGui::SameLine();
    if(ImGui::SmallButton("Start"))
    {
        Set_File_Picker_Current(State, Start_Path);
        Current_Path = Picker_Current_Path(State);
        Run_GUI_Digital_Function(Command.Name);
    }
    ImGui::SameLine();
    bool Can_Go_Back = Canonical_Or_Absolute(Current_Path) != Canonical_Or_Absolute(Root_Path);
    ImGui::BeginDisabled(!Can_Go_Back);
    if(ImGui::SmallButton("Back"))
    {
        Set_File_Picker_Current(State, Current_Path.parent_path());
        Current_Path = Picker_Current_Path(State);
        Run_GUI_Digital_Function(Command.Name);
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Text("Selected: %s", State.Selected.empty() ? "(none)" : State.Selected.c_str());

    struct Entry
    {
        std::string Display;
        fs::path Path;
        bool Directory = false;
    };

    std::vector<Entry> Entries;
    try
    {
        for(const fs::directory_entry &Item : fs::directory_iterator(Current_Path))
        {
            Entry New_Entry;
            New_Entry.Directory = Item.is_directory();
            New_Entry.Display = Item.path().filename().generic_string();
            if(New_Entry.Directory)
            {
                New_Entry.Display += "/";
            }
            New_Entry.Path = Item.path();
            Entries.push_back(New_Entry);
        }
    }
    catch(const std::exception &)
    {
        ImGui::TextUnformatted("Unable to read this folder.");
    }

    std::sort(Entries.begin(), Entries.end(), [](const Entry &Left, const Entry &Right)
    {
        if(Left.Directory != Right.Directory)
        {
            return Left.Directory && !Right.Directory;
        }
        return Left.Display < Right.Display;
    });

    ImVec2 List_Size(-1.0f, std::max(180.0f, ImGui::GetContentRegionAvail().y - 40.0f));
    if(ImGui::BeginListBox(("##" + Command.Name + "_Files").c_str(), List_Size))
    {
        if(Can_Go_Back)
        {
            if(ImGui::Selectable("..", false))
            {
                Set_File_Picker_Current(State, Current_Path.parent_path());
                Run_GUI_Digital_Function(Command.Name);
            }
        }

        for(size_t Index = 0; Index < Entries.size(); ++Index)
        {
            const Entry &Item = Entries[Index];
            bool Selected = State.Selected_Index == static_cast<int>(Index);
            if(ImGui::Selectable(Item.Display.c_str(), Selected))
            {
                Select_File_Picker_Item(State, Item.Path, Item.Display, static_cast<int>(Index));
                Run_GUI_Digital_Function(Command.Name);
            }
            if(Selected)
            {
                ImGui::SetItemDefaultFocus();
            }
            if(Item.Directory && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                Set_File_Picker_Current(State, Item.Path);
                Run_GUI_Digital_Function(Command.Name);
            }
        }
        ImGui::EndListBox();
    }
}

void Run_GUI_Function(const std::string &Name, int Value)
{
    if(Run_Analog_Function_Callback != nullptr && GUI_Functions[Name] != "")
    {
        Run_Analog_Function_Callback(GUI_Functions[Name], Value);
    }
}

void Run_GUI_Digital_Function(const std::string &Name)
{
    if(Run_Function_Callback != nullptr && GUI_Functions[Name] != "")
    {
        Run_Function_Callback(GUI_Functions[Name]);
    }
}

void Draw_GUI(Window_State &Window)
{
    if(!Ensure_ImGui(Window)) return;

    bool In_Window = false;
    std::vector<bool> Tree_Stack;
    std::vector<bool> Tab_Bar_Stack;
    std::vector<bool> Tab_Item_Stack;
    std::vector<bool> Menu_Bar_Stack;
    std::vector<bool> Menu_Stack;
    auto Context_Open = [&]()
    {
        for(bool Open : Tab_Item_Stack)
        {
            if(!Open)
            {
                return false;
            }
        }
        return true;
    };

    for(const GUI_Command &Command : GUI_Commands)
    {
        if(Command.Kind == "Begin")
        {
            if(In_Window)
            {
                ImGui::End();
            }
            ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(420.0f, 240.0f), ImGuiCond_Once);
            ImGui::Begin(Command.Name.c_str());
            In_Window = true;
        }
        else if(Command.Kind == "Begin_Fullscreen")
        {
            if(In_Window)
            {
                ImGui::End();
            }
            ImGuiViewport *Viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(Viewport->WorkPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(Viewport->WorkSize, ImGuiCond_Always);
            ImGui::SetNextWindowViewport(Viewport->ID);
            ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse
                                   | ImGuiWindowFlags_NoMove
                                   | ImGuiWindowFlags_NoResize
                                   | ImGuiWindowFlags_NoBringToFrontOnFocus;
            ImGui::Begin(Command.Name.c_str(), nullptr, Flags);
            In_Window = true;
        }
        else if(Command.Kind == "End_Window")
        {
            if(In_Window)
            {
                ImGui::End();
                In_Window = false;
            }
        }
        else if(Command.Kind == "Tab_Bar_Begin")
        {
            Ensure_GUI_Window(In_Window);
            Tab_Bar_Stack.push_back(ImGui::BeginTabBar(Command.Name.c_str()));
        }
        else if(Command.Kind == "Tab_Bar_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Tab_Bar_Stack.empty())
            {
                if(Tab_Bar_Stack.back())
                {
                    ImGui::EndTabBar();
                }
                Tab_Bar_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Tab_Item_Begin")
        {
            Ensure_GUI_Window(In_Window);
            bool Parent_Open = Tab_Bar_Stack.empty() ? true : Tab_Bar_Stack.back();
            Tab_Item_Stack.push_back(Parent_Open && ImGui::BeginTabItem(Command.Text.c_str()));
        }
        else if(Command.Kind == "Tab_Item_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Tab_Item_Stack.empty())
            {
                if(Tab_Item_Stack.back())
                {
                    ImGui::EndTabItem();
                }
                Tab_Item_Stack.pop_back();
            }
        }
        else if(!Context_Open())
        {
            continue;
        }
        else if(Command.Kind == "Text")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::TextUnformatted(Command.Text.c_str());
        }
        else if(Command.Kind == "Bullet_Text")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::BulletText("%s", Command.Text.c_str());
        }
        else if(Command.Kind == "Label_Text")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::LabelText(Command.Name.c_str(), "%s", Command.Text.c_str());
        }
        else if(Command.Kind == "Separator")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::Separator();
        }
        else if(Command.Kind == "Separator_Text")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::SeparatorText(Command.Text.c_str());
        }
        else if(Command.Kind == "Same_Line")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::SameLine();
        }
        else if(Command.Kind == "New_Line")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::NewLine();
        }
        else if(Command.Kind == "Spacing")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::Spacing();
        }
        else if(Command.Kind == "Indent")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::Indent(static_cast<float>(Command.Value));
        }
        else if(Command.Kind == "Unindent")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::Unindent(static_cast<float>(Command.Value));
        }
        else if(Command.Kind == "Group_Begin")
        {
            Ensure_GUI_Window(In_Window);
            if(Command.Text != "")
            {
                ImGui::SeparatorText(Command.Text.c_str());
            }
            ImGui::BeginGroup();
        }
        else if(Command.Kind == "Group_End")
        {
            Ensure_GUI_Window(In_Window);
            ImGui::EndGroup();
        }
        else if(Command.Kind == "Button")
        {
            Ensure_GUI_Window(In_Window);
            if(ImGui::Button(Command.Text.c_str(), ImVec2(180.0f, 0.0f)))
            {
                GUI_Clicked[Command.Name] = true;
                if(Run_Function_Callback != nullptr && GUI_Button_Functions[Command.Name] != "")
                {
                    Run_Function_Callback(GUI_Button_Functions[Command.Name]);
                }
                Run_GUI_Digital_Function(Command.Name);
            }
        }
        else if(Command.Kind == "New_Game_Project_Button")
        {
            Ensure_GUI_Window(In_Window);
            if(ImGui::Button(Command.Text.c_str(), ImVec2(180.0f, 0.0f)))
            {
                GUI_Clicked[Command.Name] = true;
                Create_Game_Project_From_GUI(GUI_Text_Values[Command.Value_Text], Command.Extra_Text);
            }
        }
        else if(Command.Kind == "New_Level_Project_Button")
        {
            Ensure_GUI_Window(In_Window);
            if(ImGui::Button(Command.Text.c_str(), ImVec2(180.0f, 0.0f)))
            {
                GUI_Clicked[Command.Name] = true;
                Create_Level_Project_From_GUI(GUI_Text_Values[Command.Value_Text], GUI_Text_Values[Command.Extra_Text], Command.Tooltip_Text);
            }
        }
        else if(Command.Kind == "Small_Button")
        {
            Ensure_GUI_Window(In_Window);
            if(ImGui::SmallButton(Command.Text.c_str()))
            {
                GUI_Clicked[Command.Name] = true;
                Run_GUI_Digital_Function(Command.Name);
            }
        }
        else if(Command.Kind == "Slider_Int")
        {
            Ensure_GUI_Window(In_Window);
            int Value = GUI_Slider_Values[Command.Name];
            if(ImGui::SliderInt(Command.Text.c_str(), &Value, Command.Min, Command.Max))
            {
                GUI_Slider_Values[Command.Name] = Value;
                GUI_Slider_Changed[Command.Name] = true;
                if(Run_Analog_Function_Callback != nullptr && GUI_Slider_Functions[Command.Name] != "")
                {
                    Run_Analog_Function_Callback(GUI_Slider_Functions[Command.Name], Value);
                }
                Run_GUI_Function(Command.Name, Value);
            }
        }
        else if(Command.Kind == "Slider_Float")
        {
            Ensure_GUI_Window(In_Window);
            float Value = GUI_Float_Values[Command.Name];
            if(ImGui::SliderFloat(Command.Text.c_str(), &Value, Command.Float_Min, Command.Float_Max))
            {
                GUI_Float_Values[Command.Name] = Value;
                GUI_Float_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, static_cast<int>(Value * 1000.0f));
            }
        }
        else if(Command.Kind == "Drag_Int")
        {
            Ensure_GUI_Window(In_Window);
            int Value = GUI_Int_Values[Command.Name];
            if(ImGui::DragInt(Command.Text.c_str(), &Value, static_cast<float>(Command.Value), Command.Min, Command.Max))
            {
                GUI_Int_Values[Command.Name] = Value;
                GUI_Int_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, Value);
            }
        }
        else if(Command.Kind == "Drag_Float")
        {
            Ensure_GUI_Window(In_Window);
            float Value = GUI_Float_Values[Command.Name];
            if(ImGui::DragFloat(Command.Text.c_str(), &Value, Command.Float_Step, Command.Float_Min, Command.Float_Max))
            {
                GUI_Float_Values[Command.Name] = Value;
                GUI_Float_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, static_cast<int>(Value * 1000.0f));
            }
        }
        else if(Command.Kind == "Input_Text")
        {
            Ensure_GUI_Window(In_Window);
            char Buffer[256] = {};
            std::string Existing = GUI_Text_Values[Command.Name];
            std::snprintf(Buffer, sizeof(Buffer), "%s", Existing.c_str());
            if(ImGui::InputText(Command.Text.c_str(), Buffer, sizeof(Buffer)))
            {
                GUI_Text_Values[Command.Name] = Buffer;
                GUI_Text_Changed_Values[Command.Name] = true;
                Run_GUI_Digital_Function(Command.Name);
            }
        }
        else if(Command.Kind == "Input_Text_Multiline")
        {
            Ensure_GUI_Window(In_Window);
            std::string Existing = GUI_Text_Values[Command.Name];
            size_t Buffer_Size = std::max<size_t>(65536, Existing.size() + 4096);
            std::vector<char> Buffer(Buffer_Size, '\0');
            std::snprintf(Buffer.data(), Buffer.size(), "%s", Existing.c_str());
            ImVec2 Size(static_cast<float>(Command.Min), static_cast<float>(Command.Max));
            if(ImGui::InputTextMultiline(Command.Text.c_str(), Buffer.data(), Buffer.size(), Size))
            {
                GUI_Text_Values[Command.Name] = Buffer.data();
                GUI_Text_Changed_Values[Command.Name] = true;
                Run_GUI_Digital_Function(Command.Name);
            }
        }
        else if(Command.Kind == "Input_Int")
        {
            Ensure_GUI_Window(In_Window);
            int Value = GUI_Int_Values[Command.Name];
            if(ImGui::InputInt(Command.Text.c_str(), &Value))
            {
                GUI_Int_Values[Command.Name] = Value;
                GUI_Int_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, Value);
            }
        }
        else if(Command.Kind == "Input_Float")
        {
            Ensure_GUI_Window(In_Window);
            float Value = GUI_Float_Values[Command.Name];
            if(ImGui::InputFloat(Command.Text.c_str(), &Value))
            {
                GUI_Float_Values[Command.Name] = Value;
                GUI_Float_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, static_cast<int>(Value * 1000.0f));
            }
        }
        else if(Command.Kind == "Checkbox")
        {
            Ensure_GUI_Window(In_Window);
            bool Value = GUI_Checkbox_Values[Command.Name];
            if(ImGui::Checkbox(Command.Text.c_str(), &Value))
            {
                GUI_Checkbox_Values[Command.Name] = Value;
                GUI_Checkbox_Changed_Values[Command.Name] = true;
                if(Run_Analog_Function_Callback != nullptr && GUI_Checkbox_Functions[Command.Name] != "")
                {
                    Run_Analog_Function_Callback(GUI_Checkbox_Functions[Command.Name], Value ? 1 : 0);
                }
                Run_GUI_Function(Command.Name, Value ? 1 : 0);
            }
        }
        else if(Command.Kind == "Radio_Button")
        {
            Ensure_GUI_Window(In_Window);
            int Value = GUI_Selection_Values[Command.Name];
            if(ImGui::RadioButton(Command.Text.c_str(), &Value, Command.Value))
            {
                GUI_Selection_Values[Command.Name] = Value;
                GUI_Selection_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, Value);
            }
        }
        else if(Command.Kind == "Combo")
        {
            Ensure_GUI_Window(In_Window);
            std::vector<std::string> Items = Split_GUI_Items(Command.Value_Text);
            int Value = GUI_Selection_Values[Command.Name];
            if(Value >= 0 && static_cast<size_t>(Value) < Items.size())
            {
                GUI_Selection_Text_Values[Command.Name] = Items[Value];
            }
            const char *Preview = (Value >= 0 && static_cast<size_t>(Value) < Items.size()) ? Items[Value].c_str() : "";
            if(ImGui::BeginCombo(Command.Text.c_str(), Preview))
            {
                for(size_t Index = 0; Index < Items.size(); ++Index)
                {
                    bool Selected = Value == static_cast<int>(Index);
                    if(ImGui::Selectable(Items[Index].c_str(), Selected))
                    {
                        Value = static_cast<int>(Index);
                        GUI_Selection_Values[Command.Name] = Value;
                        GUI_Selection_Text_Values[Command.Name] = Items[Index];
                        GUI_Selection_Changed_Values[Command.Name] = true;
                        Run_GUI_Function(Command.Name, Value);
                    }
                    if(Selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        else if(Command.Kind == "List_Box")
        {
            Ensure_GUI_Window(In_Window);
            std::vector<std::string> Items = Split_GUI_Items(Command.Value_Text);
            int Value = GUI_Selection_Values[Command.Name];
            if(Value >= 0 && static_cast<size_t>(Value) < Items.size())
            {
                GUI_Selection_Text_Values[Command.Name] = Items[Value];
            }
            if(ImGui::BeginListBox(Command.Text.c_str()))
            {
                for(size_t Index = 0; Index < Items.size(); ++Index)
                {
                    bool Selected = Value == static_cast<int>(Index);
                    if(ImGui::Selectable(Items[Index].c_str(), Selected))
                    {
                        Value = static_cast<int>(Index);
                        GUI_Selection_Values[Command.Name] = Value;
                        GUI_Selection_Text_Values[Command.Name] = Items[Index];
                        GUI_Selection_Changed_Values[Command.Name] = true;
                        Run_GUI_Function(Command.Name, Value);
                    }
                }
                ImGui::EndListBox();
            }
        }
        else if(Command.Kind == "File_Picker")
        {
            Ensure_GUI_Window(In_Window);
            Draw_File_Picker(Command);
        }
        else if(Command.Kind == "Selectable")
        {
            Ensure_GUI_Window(In_Window);
            bool Selected = GUI_Checkbox_Values[Command.Name];
            if(ImGui::Selectable(Command.Text.c_str(), Selected))
            {
                GUI_Checkbox_Values[Command.Name] = !Selected;
                GUI_Checkbox_Changed_Values[Command.Name] = true;
                Run_GUI_Function(Command.Name, GUI_Checkbox_Values[Command.Name] ? 1 : 0);
            }
        }
        else if(Command.Kind == "Progress_Bar")
        {
            Ensure_GUI_Window(In_Window);
            float Fraction = std::clamp(Command.Float_Value, 0.0f, 1.0f);
            ImGui::ProgressBar(Fraction, ImVec2(-1.0f, 0.0f), Command.Text.empty() ? nullptr : Command.Text.c_str());
        }
        else if(Command.Kind == "Tooltip")
        {
            Ensure_GUI_Window(In_Window);
            if(ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", Command.Text.c_str());
            }
        }
        else if(Command.Kind == "Tree_Node")
        {
            Ensure_GUI_Window(In_Window);
            bool Open = ImGui::TreeNode(Command.Text.c_str());
            Tree_Stack.push_back(Open);
        }
        else if(Command.Kind == "Tree_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Tree_Stack.empty())
            {
                if(Tree_Stack.back())
                {
                    ImGui::TreePop();
                }
                Tree_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Tab_Bar_Begin")
        {
            Ensure_GUI_Window(In_Window);
            Tab_Bar_Stack.push_back(ImGui::BeginTabBar(Command.Name.c_str()));
        }
        else if(Command.Kind == "Tab_Bar_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Tab_Bar_Stack.empty())
            {
                if(Tab_Bar_Stack.back())
                {
                    ImGui::EndTabBar();
                }
                Tab_Bar_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Tab_Item_Begin")
        {
            Ensure_GUI_Window(In_Window);
            bool Parent_Open = Tab_Bar_Stack.empty() ? true : Tab_Bar_Stack.back();
            Tab_Item_Stack.push_back(Parent_Open && ImGui::BeginTabItem(Command.Text.c_str()));
        }
        else if(Command.Kind == "Tab_Item_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Tab_Item_Stack.empty())
            {
                if(Tab_Item_Stack.back())
                {
                    ImGui::EndTabItem();
                }
                Tab_Item_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Menu_Bar_Begin")
        {
            Ensure_GUI_Window(In_Window);
            Menu_Bar_Stack.push_back(ImGui::BeginMenuBar());
        }
        else if(Command.Kind == "Menu_Bar_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Menu_Bar_Stack.empty())
            {
                if(Menu_Bar_Stack.back())
                {
                    ImGui::EndMenuBar();
                }
                Menu_Bar_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Menu_Begin")
        {
            Ensure_GUI_Window(In_Window);
            bool Parent_Open = Menu_Bar_Stack.empty() ? true : Menu_Bar_Stack.back();
            Menu_Stack.push_back(Parent_Open && ImGui::BeginMenu(Command.Text.c_str()));
        }
        else if(Command.Kind == "Menu_End")
        {
            Ensure_GUI_Window(In_Window);
            if(!Menu_Stack.empty())
            {
                if(Menu_Stack.back())
                {
                    ImGui::EndMenu();
                }
                Menu_Stack.pop_back();
            }
        }
        else if(Command.Kind == "Menu_Item")
        {
            Ensure_GUI_Window(In_Window);
            bool Parent_Open = Menu_Stack.empty() ? true : Menu_Stack.back();
            if(Parent_Open && ImGui::MenuItem(Command.Text.c_str()))
            {
                GUI_Clicked[Command.Name] = true;
                Run_GUI_Digital_Function(Command.Name);
            }
        }
    }

    while(!Tab_Item_Stack.empty())
    {
        if(Tab_Item_Stack.back()) ImGui::EndTabItem();
        Tab_Item_Stack.pop_back();
    }
    while(!Tab_Bar_Stack.empty())
    {
        if(Tab_Bar_Stack.back()) ImGui::EndTabBar();
        Tab_Bar_Stack.pop_back();
    }
    while(!Menu_Stack.empty())
    {
        if(Menu_Stack.back()) ImGui::EndMenu();
        Menu_Stack.pop_back();
    }
    while(!Menu_Bar_Stack.empty())
    {
        if(Menu_Bar_Stack.back()) ImGui::EndMenuBar();
        Menu_Bar_Stack.pop_back();
    }
    while(!Tree_Stack.empty())
    {
        if(Tree_Stack.back()) ImGui::TreePop();
        Tree_Stack.pop_back();
    }

    if(In_Window)
    {
        ImGui::End();
    }
}

void Draw_Mesh_Asset(Window_State &Window, const Asset_State &Asset)
{
    auto Mesh_It = Meshes.find(Asset.Mesh);
    if(Mesh_It == Meshes.end())
    {
        return;
    }

    SDL_Color Line_Color = {245, 196, 66, 255};
    if(Asset.Shader != "" && Shaders.find(Asset.Shader) != Shaders.end())
    {
        Line_Color = {120, 220, 170, 255};
    }

    const Mesh_State &Mesh = Mesh_It->second;

    bool Wire_Only = Asset.Shader == "Wire";
    glPushMatrix();
    glTranslatef(Asset.X, Asset.Y, Asset.Z);
    glRotatef(Asset.Yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(Asset.Pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(Asset.Roll, 0.0f, 0.0f, 1.0f);
    glScalef(Asset.Scale_X, Asset.Scale_Y, Asset.Scale_Z);

    if(!Wire_Only && !Mesh.Triangles.empty())
    {
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        int Bound_Material = -1;
        for(const Mesh_Triangle &Triangle : Mesh.Triangles)
        {
            int A = Triangle.A;
            int B = Triangle.B;
            int C = Triangle.C;
            if(A < 0 || B < 0 || C < 0) continue;
            if(static_cast<size_t>(A) >= Mesh.Vertices.size()) continue;
            if(static_cast<size_t>(B) >= Mesh.Vertices.size()) continue;
            if(static_cast<size_t>(C) >= Mesh.Vertices.size()) continue;

            int Material_Index = Triangle.Material;
            if(Material_Index < 0 || static_cast<size_t>(Material_Index) >= Mesh.Materials.size())
            {
                Material_Index = 0;
            }

            if(Bound_Material != Material_Index)
            {
                Bound_Material = Material_Index;
                const Mesh_Material &Material = Mesh.Materials[Material_Index];
                if(Material.Texture_ID != 0)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, Material.Texture_ID);
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                    glColor4ub(255, 255, 255, 255);
                }
                else
                {
                    glDisable(GL_TEXTURE_2D);
                    SDL_Color Diffuse = Material.Diffuse;
                    if(Diffuse.r < 8 && Diffuse.g < 8 && Diffuse.b < 8)
                    {
                        Diffuse = {120, 220, 170, 255};
                    }
                    glColor4ub(Diffuse.r, Diffuse.g, Diffuse.b, 255);
                }
            }

            glBegin(GL_TRIANGLES);
                int Indices[3] = {A, B, C};
                for(int Index : Indices)
                {
                    if(static_cast<size_t>(Index) < Mesh.Normals.size())
                    {
                        const Vec3 &Normal = Mesh.Normals[Index];
                        glNormal3f(Normal.X, Normal.Y, Normal.Z);
                    }
                    if(static_cast<size_t>(Index) < Mesh.UVs.size())
                    {
                        const Vec2 &UV = Mesh.UVs[Index];
                        glTexCoord2f(UV.X, UV.Y);
                    }
                    const Vec3 &Vertex = Mesh.Vertices[Index];
                    glVertex3f(Vertex.X, Vertex.Y, Vertex.Z);
                }
            glEnd();
        }
    }
    else
    {
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor4ub(Line_Color.r, Line_Color.g, Line_Color.b, Line_Color.a);
        glBegin(GL_LINES);
        for(const auto &Edge : Mesh.Edges)
        {
            if(Edge.first < 0 || Edge.second < 0) continue;
            if(static_cast<size_t>(Edge.first) >= Mesh.Vertices.size() || static_cast<size_t>(Edge.second) >= Mesh.Vertices.size()) continue;
            const Vec3 &A = Mesh.Vertices[Edge.first];
            const Vec3 &B = Mesh.Vertices[Edge.second];
            glVertex3f(A.X, A.Y, A.Z);
            glVertex3f(B.X, B.Y, B.Z);
        }
        glEnd();
    }

    glPopMatrix();
}

extern "C" void Constructor()
{
    Ensure_SDL();
}

extern "C" void Destructor()
{
    Make_Current_GL_Context();

    if(ImGui_Ready)
    {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ImGui_Ready = false;
    }

    Destroy_GL_Resources();

    for(auto &Item : Fonts)
    {
        Destroy_Font(Item.second);
    }
    Fonts.clear();

    Text_Commands.clear();
    GUI_Commands.clear();

    for(auto &Item : Windows)
    {
        Destroy_Window(Item.second);
    }

    Windows.clear();

    if(SDL_Ready)
    {
        if(TTF_Ready)
        {
            TTF_Quit();
            TTF_Ready = false;
        }
        if(IMG_Ready)
        {
            IMG_Quit();
            IMG_Ready = false;
        }
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Ready = false;
    }
}

extern "C" void Set_Paths(std::string Path_of_Resources)
{
    Resources = Path_of_Resources;
    Graphics_Path = Resources + "/Graphics/";
    Icon_Path = Graphics_Path + "Icons/";
    Texture_Path = Graphics_Path + "Textures/";
    Font_Path = Graphics_Path + "Fonts/";
    Shader_Path = Graphics_Path + "Shaders/";
    Mesh_Path = Graphics_Path + "Meshes/";
    Material_Path = Graphics_Path + "Materials/";
    GUI_Path = Graphics_Path + "GUI/";
}

extern "C" void Add_Window(std::string Name, std::string Title, int Width, int Height)
{
    if(!Ensure_SDL()) return;

    Delete_Window(Name);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    Window_State State;
    State.Width = Width;
    State.Height = Height;
    State.View_W = Width;
    State.View_H = Height;
    State.Window = SDL_CreateWindow(
        Title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Width,
        Height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );

    if(State.Window == nullptr)
    {
        if(DEBUG) std::cout << "SDL window could not be created: " << SDL_GetError() << std::endl;
        return;
    }

    State.GL_Context = SDL_GL_CreateContext(State.Window);
    if(State.GL_Context == nullptr)
    {
        if(DEBUG) std::cout << "SDL OpenGL context could not be created: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(State.Window);
        return;
    }

    SDL_GL_MakeCurrent(State.Window, State.GL_Context);
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Windows[Name] = State;
    Focused_Window = Name;

    if(!ImGui_Ready)
    {
        Ensure_ImGui(Windows[Name]);
    }
}

extern "C" void Add_Full_Screen_Window(std::string Name, int Monitor)
{
    SDL_DisplayMode Mode;
    if(SDL_GetCurrentDisplayMode(Monitor, &Mode) != 0)
    {
        if(DEBUG) std::cout << "SDL display mode unavailable: " << SDL_GetError() << std::endl;
        Add_Window(Name, "Appeal", 1280, 720);
        return;
    }

    Add_Window(Name, "Appeal", Mode.w, Mode.h);
    Window_State *State = Find_Window(Name);
    if(State != nullptr)
    {
        SDL_SetWindowFullscreen(State->Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        int Drawable_W = 0;
        int Drawable_H = 0;
        SDL_GL_GetDrawableSize(State->Window, &Drawable_W, &Drawable_H);
        if(Drawable_W > 0 && Drawable_H > 0)
        {
            State->Width = Drawable_W;
            State->Height = Drawable_H;
            State->View_X = 0;
            State->View_Y = 0;
            State->View_W = Drawable_W;
            State->View_H = Drawable_H;
            State->Custom_Viewport = false;
        }
    }
}

extern "C" void Delete_Window(std::string Name)
{
    auto It = Windows.find(Name);
    if(It == Windows.end()) return;

    if(Windows.size() == 1 && ImGui_Ready)
    {
        if(It->second.Window != nullptr && It->second.GL_Context != nullptr)
        {
            SDL_GL_MakeCurrent(It->second.Window, It->second.GL_Context);
        }
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ImGui_Ready = false;
    }

    if(Windows.size() == 1)
    {
        if(It->second.Window != nullptr && It->second.GL_Context != nullptr)
        {
            SDL_GL_MakeCurrent(It->second.Window, It->second.GL_Context);
        }
        Destroy_GL_Resources();
    }

    Destroy_Window(It->second);
    Windows.erase(It);

    if(Focused_Window == Name)
    {
        Focused_Window = Windows.empty() ? "" : Windows.begin()->first;
    }
}

extern "C" void Close_All()
{
    std::vector<std::string> Names;
    for(const auto &Item : Windows)
    {
        Names.push_back(Item.first);
    }

    for(const std::string &Name : Names)
    {
        Delete_Window(Name);
    }
}

extern "C" void Show(std::string Name)
{
    Window_State *State = Find_Window(Name);
    if(State != nullptr) SDL_ShowWindow(State->Window);
}

extern "C" void Hide(std::string Name)
{
    Window_State *State = Find_Window(Name);
    if(State != nullptr) SDL_HideWindow(State->Window);
}

extern "C" void Set_Focus(std::string Name)
{
    Window_State *State = Find_Window(Name);
    if(State != nullptr)
    {
        Focused_Window = Name;
        SDL_RaiseWindow(State->Window);
    }
}

extern "C" void Set_Monitor(std::string Name, int Monitor)
{
    Window_State *State = Find_Window(Name);
    if(State == nullptr) return;

    SDL_Rect Bounds;
    if(SDL_GetDisplayBounds(Monitor, &Bounds) == 0)
    {
        SDL_SetWindowPosition(State->Window, Bounds.x + 64, Bounds.y + 64);
    }
}

extern "C" void Set_FPS(int Frames)
{
    if(Frames > 0) Target_FPS = Frames;
}

extern "C" void Set_Window_Icon(std::string Name, std::string File)
{
    if(!Ensure_SDL()) return;

    Window_State *State = Find_Window(Name);
    if(State == nullptr || State->Window == nullptr)
    {
        if(DEBUG) std::cout << "Window not found for icon: " << Name << std::endl;
        return;
    }

    std::string Full_File = Icon_Path + File;

#ifdef __MINGW32__
    SDL_SysWMinfo Window_Info;
    SDL_VERSION(&Window_Info.version);
    if(SDL_GetWindowWMInfo(State->Window, &Window_Info) == SDL_TRUE)
    {
        HICON Big_Icon = static_cast<HICON>(LoadImageA(nullptr, Full_File.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
        HICON Small_Icon = static_cast<HICON>(LoadImageA(nullptr, Full_File.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
        if(Big_Icon != nullptr || Small_Icon != nullptr)
        {
            if(State->Big_Icon != nullptr) DestroyIcon(State->Big_Icon);
            if(State->Small_Icon != nullptr) DestroyIcon(State->Small_Icon);

            State->Big_Icon = Big_Icon;
            State->Small_Icon = Small_Icon;

            HWND Window_Handle = Window_Info.info.win.window;
            if(State->Big_Icon != nullptr)
            {
                SendMessageA(Window_Handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(State->Big_Icon));
            }
            if(State->Small_Icon != nullptr)
            {
                SendMessageA(Window_Handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(State->Small_Icon));
            }
            return;
        }
    }
#endif

    SDL_Surface *Icon = IMG_Load(Full_File.c_str());
    if(Icon == nullptr)
    {
        Icon = SDL_LoadBMP(Full_File.c_str());
    }

    if(Icon == nullptr)
    {
        if(DEBUG) std::cout << "Window icon did not load: " << Full_File << std::endl
                  << "SDL_image Error: " << IMG_GetError() << std::endl
                  << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetWindowIcon(State->Window, Icon);
    SDL_FreeSurface(Icon);
}

extern "C" void Set_View_Port(std::string Window, int X, int Y, int W, int H)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->View_X = X;
    State->View_Y = Y;
    State->View_W = W;
    State->View_H = H;
    State->Custom_Viewport = true;
}

extern "C" void Set_Perspective(std::string Window, float Fovy, float Near, float Far)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Perspective = true;
    State->Fovy = Fovy;
    State->Near = Near;
    State->Far = Far;
}

extern "C" void Set_Ortho(std::string Window, float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Perspective = false;
    State->Ortho_Left = Left;
    State->Ortho_Right = Right;
    State->Ortho_Bottom = Bottom;
    State->Ortho_Top = Top;
    State->Near = Near;
    State->Far = Far;
}

extern "C" void Set_Camera_Location(std::string Window, float X, float Y, float Z)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Camera_Location = {X, Y, Z};
}

extern "C" void Set_Camera_Orientation(std::string Window, float Yaw, float Pitch, float Roll)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Camera_Orientation = {Yaw, Pitch, Roll};
}

extern "C" void Move_Camera(std::string Window, float X, float Y, float Z)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Camera_Location.X += X;
    State->Camera_Location.Y += Y;
    State->Camera_Location.Z += Z;
}

extern "C" void Rotate_Camera(std::string Window, float Yaw, float Pitch, float Roll)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    State->Camera_Orientation.X += Yaw;
    State->Camera_Orientation.Y += Pitch;
    State->Camera_Orientation.Z += Roll;
}

extern "C" void Update_Camera(std::string Window)
{
    Find_Window(Window);
}

extern "C" void Load_Shader(std::string Name, std::string File)
{
    Shaders[Name] = Shader_Path + File;
}

extern "C" void Load_Texture(std::string Name, std::string File)
{
    if(!Ensure_SDL()) return;

    Window_State *Window = Render_Window();
    if(Window == nullptr)
    {
        if(DEBUG) std::cout << "Load_Texture needs a window before loading: " << Name << std::endl;
        return;
    }

    Texture_State &State = Textures[Name];
    Destroy_Texture(State);
    State.File = Texture_Path + File;

    SDL_GL_MakeCurrent(Window->Window, Window->GL_Context);
    State.Texture = Load_GL_Texture_File(State.File, &State.Width, &State.Height);
    if(State.Texture == 0)
    {
        return;
    }
}

extern "C" void Load_Font(std::string Name, std::string File, int Size)
{
    if(!Ensure_SDL()) return;

    Font_State &State = Fonts[Name];
    Destroy_Font(State);
    State.File = Font_Path + File;
    State.Size = Size;
    State.Font = TTF_OpenFont(State.File.c_str(), Size);

    if(State.Font == nullptr)
    {
        if(DEBUG) std::cout << "Font did not load: " << State.File << std::endl
                  << "TTF Error: " << TTF_GetError() << std::endl;
    }
}

extern "C" void Load_Mesh(std::string Name, std::string File)
{
    Meshes[Name] = Load_Mesh_File(Mesh_Path + File);
}

extern "C" void Load_Asset(std::string Name, std::string File)
{
    Assets[Name].File = File;
    if(Meshes.find(Name) != Meshes.end())
    {
        Assets[Name].Mesh = Name;
    }
    else if(Meshes.find(File) != Meshes.end())
    {
        Assets[Name].Mesh = File;
    }
    else
    {
        Meshes[Name] = Load_Mesh_File(Mesh_Path + File);
        Assets[Name].Mesh = Name;
    }
}

extern "C" void Free_Texture(std::string Name)
{
    auto It = Textures.find(Name);
    if(It == Textures.end())
    {
        return;
    }

    if(Make_Current_GL_Context())
    {
        Destroy_Texture(It->second);
    }
    Textures.erase(It);
}

extern "C" void Free_Asset(std::string Name)
{
    Assets.erase(Name);
    if(Active_Asset == Name)
    {
        Active_Asset = "";
    }
}

extern "C" void Set_Texture(std::string Name, std::string Texture)
{
    Assets[Name].Texture = Texture;
}

extern "C" void Set_Shader(std::string Name, std::string Shader)
{
    Assets[Name].Shader = Shader;
}

extern "C" void Set_Location(std::string Name, float X, float Y, float Z)
{
    Assets[Name].X = X;
    Assets[Name].Y = Y;
    Assets[Name].Z = Z;
}

extern "C" void Set_Orientation(std::string Name, float Yaw, float Pitch, float Roll)
{
    Assets[Name].Yaw = Yaw;
    Assets[Name].Pitch = Pitch;
    Assets[Name].Roll = Roll;
}

extern "C" void Set_Scale(std::string Name, float X, float Y, float Z)
{
    Assets[Name].Scale_X = X;
    Assets[Name].Scale_Y = Y;
    Assets[Name].Scale_Z = Z;
}

extern "C" void Move_Asset(std::string Name, float X, float Y, float Z)
{
    Assets[Name].X += X;
    Assets[Name].Y += Y;
    Assets[Name].Z += Z;
}

extern "C" void Rotate_Asset(std::string Name, float Yaw, float Pitch, float Roll)
{
    Assets[Name].Yaw += Yaw;
    Assets[Name].Pitch += Pitch;
    Assets[Name].Roll += Roll;
}

extern "C" void Set_Active_Asset(std::string Name)
{
    Active_Asset = Name;
}

extern "C" void Rotate_Active_Yaw_Input(int Value)
{
    if(Active_Asset == "" || Assets.find(Active_Asset) == Assets.end()) return;
    float Amount = static_cast<float>(Value) / 32768.0f;
    Assets[Active_Asset].Yaw += Amount * 3.0f;
}

extern "C" void Move_Active_Forward_Input(int Value)
{
    if(Active_Asset == "" || Assets.find(Active_Asset) == Assets.end()) return;
    float Amount = static_cast<float>(Value) / 32768.0f;
    Assets[Active_Asset].Z += -Amount * 0.08f;
}

extern "C" void Draw_Text(std::string Font, std::string Text, float X, float Y)
{
    Text_Commands.push_back({Font, Text, X, Y, {230, 236, 245, 255}});
}

extern "C" void Draw_Rect(std::string Name, float X, float Y, float W, float H, int R, int G, int B, int A)
{
    Rect_Commands[Name] = {Name, X, Y, W, H, {
        static_cast<Uint8>(std::clamp(R, 0, 255)),
        static_cast<Uint8>(std::clamp(G, 0, 255)),
        static_cast<Uint8>(std::clamp(B, 0, 255)),
        static_cast<Uint8>(std::clamp(A, 0, 255))
    }};
}

extern "C" void GUI_Begin(std::string Name)
{
    GUI_Commands.clear();
    GUI_Commands.push_back({"Begin", Name, Name});
}

extern "C" void GUI_Begin_Fullscreen(std::string Name)
{
    GUI_Commands.clear();
    GUI_Commands.push_back({"Begin_Fullscreen", Name, Name});
}

extern "C" void GUI_End_Window()
{
    GUI_Commands.push_back({"End_Window"});
}

extern "C" void GUI_Text(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Text", Name, Text});
}

extern "C" void GUI_Label_Text(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Label_Text", Name, Text});
}

extern "C" void GUI_Bullet_Text(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Bullet_Text", Name, Text});
}

extern "C" void GUI_Separator()
{
    GUI_Commands.push_back({"Separator"});
}

extern "C" void GUI_Separator_Text(std::string Text)
{
    GUI_Commands.push_back({"Separator_Text", "", Text});
}

extern "C" void GUI_Same_Line()
{
    GUI_Commands.push_back({"Same_Line"});
}

extern "C" void GUI_New_Line()
{
    GUI_Commands.push_back({"New_Line"});
}

extern "C" void GUI_Spacing()
{
    GUI_Commands.push_back({"Spacing"});
}

extern "C" void GUI_Indent(int Pixels)
{
    GUI_Command Command;
    Command.Kind = "Indent";
    Command.Value = Pixels;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Unindent(int Pixels)
{
    GUI_Command Command;
    Command.Kind = "Unindent";
    Command.Value = Pixels;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Group_Begin(std::string Name)
{
    GUI_Commands.push_back({"Group_Begin", Name, Name});
}

extern "C" void GUI_Group_End()
{
    GUI_Commands.push_back({"Group_End"});
}

extern "C" void GUI_Button(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Button", Name, Text});
}

extern "C" void GUI_New_Game_Project_Button(std::string Name, std::string Text, std::string Name_Input, std::string Picker)
{
    GUI_Command Command;
    Command.Kind = "New_Game_Project_Button";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value_Text = Name_Input;
    Command.Extra_Text = Picker;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_New_Level_Project_Button(std::string Name, std::string Text, std::string Game_Input, std::string Level_Input, std::string Picker)
{
    GUI_Command Command;
    Command.Kind = "New_Level_Project_Button";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value_Text = Game_Input;
    Command.Extra_Text = Level_Input;
    Command.Tooltip_Text = Picker;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Small_Button(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Small_Button", Name, Text});
}

extern "C" void Set_GUI_Button_Function(std::string Name, std::string Function)
{
    GUI_Button_Functions[Name] = Function;
}

extern "C" void GUI_Slider_Int(std::string Name, std::string Text, int Value, int Min, int Max)
{
    if(GUI_Slider_Values.find(Name) == GUI_Slider_Values.end())
    {
        GUI_Slider_Values[Name] = Value;
    }
    GUI_Command Command;
    Command.Kind = "Slider_Int";
    Command.Name = Name;
    Command.Text = Text;
    Command.Min = Min;
    Command.Max = Max;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Slider_Float(std::string Name, std::string Text, float Value, float Min, float Max)
{
    if(GUI_Float_Values.find(Name) == GUI_Float_Values.end())
    {
        GUI_Float_Values[Name] = std::clamp(Value, Min, Max);
    }

    GUI_Command Command;
    Command.Kind = "Slider_Float";
    Command.Name = Name;
    Command.Text = Text;
    Command.Float_Min = Min;
    Command.Float_Max = Max;
    GUI_Commands.push_back(Command);
}

extern "C" float GUI_Float_Value(std::string Name)
{
    return GUI_Float_Values[Name];
}

extern "C" bool GUI_Float_Changed(std::string Name)
{
    bool Changed = GUI_Float_Changed_Values[Name];
    GUI_Float_Changed_Values[Name] = false;
    return Changed;
}

extern "C" void GUI_Drag_Int(std::string Name, std::string Text, int Value, int Min, int Max, int Speed)
{
    if(GUI_Int_Values.find(Name) == GUI_Int_Values.end())
    {
        GUI_Int_Values[Name] = std::clamp(Value, Min, Max);
    }

    GUI_Command Command;
    Command.Kind = "Drag_Int";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value = Speed;
    Command.Min = Min;
    Command.Max = Max;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Drag_Float(std::string Name, std::string Text, float Value, float Min, float Max, float Speed)
{
    if(GUI_Float_Values.find(Name) == GUI_Float_Values.end())
    {
        GUI_Float_Values[Name] = std::clamp(Value, Min, Max);
    }

    GUI_Command Command;
    Command.Kind = "Drag_Float";
    Command.Name = Name;
    Command.Text = Text;
    Command.Float_Min = Min;
    Command.Float_Max = Max;
    Command.Float_Step = Speed;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Input_Text(std::string Name, std::string Text, std::string Value)
{
    if(GUI_Text_Values.find(Name) == GUI_Text_Values.end())
    {
        GUI_Text_Values[Name] = Value;
    }
    GUI_Commands.push_back({"Input_Text", Name, Text});
}

extern "C" void GUI_Input_Text_Multiline(std::string Name, std::string Text, std::string Value, int Width, int Height)
{
    if(GUI_Text_Values.find(Name) == GUI_Text_Values.end())
    {
        GUI_Text_Values[Name] = Value;
    }

    GUI_Command Command;
    Command.Kind = "Input_Text_Multiline";
    Command.Name = Name;
    Command.Text = Text;
    Command.Min = Width;
    Command.Max = Height;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Set_Text_Value(std::string Name, std::string Value)
{
    GUI_Text_Values[Name] = Value;
    GUI_Text_Changed_Values[Name] = true;
}

extern "C" std::string GUI_Text_Value(std::string Name)
{
    return GUI_Text_Values[Name];
}

extern "C" bool GUI_Text_Changed(std::string Name)
{
    bool Changed = GUI_Text_Changed_Values[Name];
    GUI_Text_Changed_Values[Name] = false;
    return Changed;
}

extern "C" void GUI_Input_Int(std::string Name, std::string Text, int Value)
{
    if(GUI_Int_Values.find(Name) == GUI_Int_Values.end())
    {
        GUI_Int_Values[Name] = Value;
    }
    GUI_Commands.push_back({"Input_Int", Name, Text});
}

extern "C" void GUI_Input_Float(std::string Name, std::string Text, float Value)
{
    if(GUI_Float_Values.find(Name) == GUI_Float_Values.end())
    {
        GUI_Float_Values[Name] = Value;
    }
    GUI_Commands.push_back({"Input_Float", Name, Text});
}

extern "C" int GUI_Int_Value(std::string Name)
{
    return GUI_Int_Values[Name];
}

extern "C" bool GUI_Int_Changed(std::string Name)
{
    bool Changed = GUI_Int_Changed_Values[Name];
    GUI_Int_Changed_Values[Name] = false;
    return Changed;
}

extern "C" void Set_GUI_Slider_Int_Function(std::string Name, std::string Function)
{
    GUI_Slider_Functions[Name] = Function;
}

extern "C" void GUI_Checkbox(std::string Name, std::string Text, int Value)
{
    GUI_Checkbox_Values[Name] = Value != 0;
    GUI_Commands.push_back({"Checkbox", Name, Text});
}

extern "C" int GUI_Checkbox_Value(std::string Name)
{
    return GUI_Checkbox_Values[Name] ? 1 : 0;
}

extern "C" bool GUI_Checkbox_Changed(std::string Name)
{
    bool Changed = GUI_Checkbox_Changed_Values[Name];
    GUI_Checkbox_Changed_Values[Name] = false;
    return Changed;
}

extern "C" void Set_GUI_Checkbox_Function(std::string Name, std::string Function)
{
    GUI_Checkbox_Functions[Name] = Function;
}

extern "C" void GUI_Radio_Button(std::string Name, std::string Text, int Option)
{
    GUI_Command Command;
    Command.Kind = "Radio_Button";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value = Option;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Combo(std::string Name, std::string Text, std::string Items, int Value)
{
    if(GUI_Selection_Values.find(Name) == GUI_Selection_Values.end())
    {
        GUI_Selection_Values[Name] = Value;
    }
    GUI_Command Command;
    Command.Kind = "Combo";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value_Text = Items;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_List_Box(std::string Name, std::string Text, std::string Items, int Value)
{
    if(GUI_Selection_Values.find(Name) == GUI_Selection_Values.end())
    {
        GUI_Selection_Values[Name] = Value;
    }
    GUI_Command Command;
    Command.Kind = "List_Box";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value_Text = Items;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_File_Picker(std::string Name, std::string Text, std::string Root, std::string Start)
{
    GUI_File_Picker_State &State = GUI_File_Pickers[Name];
    Ensure_File_Picker_State(State, Root, Start);

    GUI_Command Command;
    Command.Kind = "File_Picker";
    Command.Name = Name;
    Command.Text = Text;
    Command.Value_Text = Root;
    Command.Extra_Text = Start;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_File_Picker_Select(std::string Name, std::string Path)
{
    Select_File_Picker_Path(Name, Path);
}

extern "C" void GUI_File_Picker_Set_Start(std::string Name, std::string Start)
{
    GUI_File_Picker_State &State = GUI_File_Pickers[Name];
    std::string Root = State.Initialized ? State.Root : "Resources";
    Ensure_File_Picker_State(State, Root, Start);
}

extern "C" void GUI_Selectable(std::string Name, std::string Text, int Selected)
{
    if(GUI_Checkbox_Values.find(Name) == GUI_Checkbox_Values.end())
    {
        GUI_Checkbox_Values[Name] = Selected != 0;
    }
    GUI_Commands.push_back({"Selectable", Name, Text});
}

extern "C" int GUI_Selection_Value(std::string Name)
{
    return GUI_Selection_Values[Name];
}

extern "C" std::string GUI_Selection_Text(std::string Name)
{
    return GUI_Selection_Text_Values[Name];
}

extern "C" bool GUI_Selection_Changed(std::string Name)
{
    bool Changed = GUI_Selection_Changed_Values[Name];
    GUI_Selection_Changed_Values[Name] = false;
    return Changed;
}

extern "C" std::string GUI_File_Picker_Current(std::string Name)
{
    return GUI_File_Pickers[Name].Current;
}

extern "C" std::string GUI_File_Picker_Selected(std::string Name)
{
    return GUI_File_Pickers[Name].Selected;
}

extern "C" std::string GUI_File_Picker_Selected_Path(std::string Name)
{
    return GUI_File_Pickers[Name].Selected_Path;
}

extern "C" std::string GUI_File_Picker_Selected_Relative(std::string Name)
{
    return GUI_File_Pickers[Name].Selected_Relative;
}

extern "C" bool GUI_File_Picker_Selected_Is_Directory(std::string Name)
{
    return GUI_File_Pickers[Name].Selected_Is_Directory;
}

extern "C" bool GUI_File_Picker_Changed(std::string Name)
{
    bool Changed = GUI_File_Pickers[Name].Changed;
    GUI_File_Pickers[Name].Changed = false;
    return Changed;
}

extern "C" void GUI_Progress_Bar(std::string Name, float Fraction, std::string Text)
{
    GUI_Command Command;
    Command.Kind = "Progress_Bar";
    Command.Name = Name;
    Command.Text = Text;
    Command.Float_Value = Fraction;
    GUI_Commands.push_back(Command);
}

extern "C" void GUI_Tooltip(std::string Text)
{
    GUI_Commands.push_back({"Tooltip", "", Text});
}

extern "C" void GUI_Tree_Node(std::string Name)
{
    GUI_Commands.push_back({"Tree_Node", Name, Name});
}

extern "C" void GUI_Tree_End()
{
    GUI_Commands.push_back({"Tree_End"});
}

extern "C" void GUI_Tab_Bar_Begin(std::string Name)
{
    GUI_Commands.push_back({"Tab_Bar_Begin", Name, Name});
}

extern "C" void GUI_Tab_Bar_End()
{
    GUI_Commands.push_back({"Tab_Bar_End"});
}

extern "C" void GUI_Tab_Item_Begin(std::string Name)
{
    GUI_Commands.push_back({"Tab_Item_Begin", Name, Name});
}

extern "C" void GUI_Tab_Item_End()
{
    GUI_Commands.push_back({"Tab_Item_End"});
}

extern "C" void GUI_Menu_Bar_Begin()
{
    GUI_Commands.push_back({"Menu_Bar_Begin"});
}

extern "C" void GUI_Menu_Bar_End()
{
    GUI_Commands.push_back({"Menu_Bar_End"});
}

extern "C" void GUI_Menu_Begin(std::string Name)
{
    GUI_Commands.push_back({"Menu_Begin", Name, Name});
}

extern "C" void GUI_Menu_End()
{
    GUI_Commands.push_back({"Menu_End"});
}

extern "C" void GUI_Menu_Item(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Menu_Item", Name, Text});
}

extern "C" void Set_GUI_Function(std::string Name, std::string Function)
{
    GUI_Functions[Name] = Function;
}

extern "C" void Set_GUI_Visible(bool Visible)
{
    GUI_Visible = Visible;
}

extern "C" void Set_GUI_Docking(bool Enable)
{
    GUI_Docking_Enabled = Enable;
}

extern "C" void Toggle_GUI()
{
    GUI_Visible = !GUI_Visible;
}

extern "C" int GUI_Slider_Int_Value(std::string Name)
{
    return GUI_Slider_Values[Name];
}

extern "C" bool GUI_Slider_Int_Changed(std::string Name)
{
    bool Changed = GUI_Slider_Changed[Name];
    GUI_Slider_Changed[Name] = false;
    return Changed;
}

extern "C" bool GUI_Button_Clicked(std::string Name)
{
    bool Clicked = GUI_Clicked[Name];
    GUI_Clicked[Name] = false;
    return Clicked;
}

extern "C" void GUI_End()
{
}

extern "C" void Set_Function_Runner(void (*Run_Function)(std::string), void (*Run_Analog_Function)(std::string, int))
{
    Run_Function_Callback = Run_Function;
    Run_Analog_Function_Callback = Run_Analog_Function;
}

extern "C" void Debug(bool Enable)
{
    DEBUG = Enable;
}

extern "C" void Render()
{
    for(auto &Item : Windows)
    {
        Window_State &State = Item.second;
        SDL_GL_MakeCurrent(State.Window, State.GL_Context);
        int Current_W = 0;
        int Current_H = 0;
        SDL_GL_GetDrawableSize(State.Window, &Current_W, &Current_H);
        if(Current_W <= 0 || Current_H <= 0)
        {
            SDL_GetWindowSize(State.Window, &Current_W, &Current_H);
        }
        if(Current_W > 0 && Current_H > 0)
        {
            State.Width = Current_W;
            State.Height = Current_H;
            if(!State.Custom_Viewport)
            {
                State.View_X = 0;
                State.View_Y = 0;
                State.View_W = Current_W;
                State.View_H = Current_H;
            }
        }

        glClearColor(State.Clear_Color.r / 255.0f, State.Clear_Color.g / 255.0f, State.Clear_Color.b / 255.0f, State.Clear_Color.a / 255.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        Set_Viewport_Projection(State);
        Set_Model_View(State);

        GLfloat Ambient[] = {0.28f, 0.28f, 0.30f, 1.0f};
        GLfloat Diffuse[] = {0.86f, 0.86f, 0.78f, 1.0f};
        GLfloat Position[] = {2.5f, 4.0f, 4.0f, 0.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, Position);
        glEnable(GL_LIGHT0);
        Begin_ImGui_Frame(State);

        for(auto &Asset_Item : Assets)
        {
            Asset_State &Asset = Asset_Item.second;
            auto Texture_It = Textures.find(Asset.Texture);
            if(Texture_It != Textures.end() && Texture_It->second.Texture != 0)
            {
                Set_2D_Projection(State);
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, Texture_It->second.Texture);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                float Width = Texture_It->second.Width * Asset.Scale_X;
                float Height = Texture_It->second.Height * Asset.Scale_Y;
                float X = Asset.X;
                float Y = Asset.Y;
                glPushMatrix();
                glTranslatef(X + Width * 0.5f, Y + Height * 0.5f, 0.0f);
                glRotatef(Asset.Roll, 0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f); glVertex2f(-Width * 0.5f, -Height * 0.5f);
                    glTexCoord2f(1.0f, 0.0f); glVertex2f(Width * 0.5f, -Height * 0.5f);
                    glTexCoord2f(1.0f, 1.0f); glVertex2f(Width * 0.5f, Height * 0.5f);
                    glTexCoord2f(0.0f, 1.0f); glVertex2f(-Width * 0.5f, Height * 0.5f);
                glEnd();
                glPopMatrix();
                glEnable(GL_DEPTH_TEST);
                Set_Viewport_Projection(State);
                Set_Model_View(State);
            }
            Draw_Mesh_Asset(State, Asset);
        }

        Set_2D_Projection(State);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        for(const auto &Rect_Item : Rect_Commands)
        {
            const Rect_Command &Command = Rect_Item.second;
            glColor4f(Command.Color.r / 255.0f, Command.Color.g / 255.0f, Command.Color.b / 255.0f, Command.Color.a / 255.0f);
            glBegin(GL_QUADS);
                glVertex2f(Command.X, Command.Y);
                glVertex2f(Command.X + Command.W, Command.Y);
                glVertex2f(Command.X + Command.W, Command.Y + Command.H);
                glVertex2f(Command.X, Command.Y + Command.H);
            glEnd();
        }

        for(const Text_Command &Command : Text_Commands)
        {
            Draw_Text_Command(State, Command);
        }

        if(GUI_Visible)
        {
            Draw_GUI(State);
        }
        End_ImGui_Frame(State);
        SDL_GL_SwapWindow(State.Window);
    }

    Text_Commands.clear();
    Rect_Commands.clear();
}

extern "C" void Update()
{
    SDL_Event Event;
    std::vector<std::string> Windows_To_Delete;
    bool Close_All = false;

    while(SDL_PollEvent(&Event))
    {
        if(ImGui_Ready)
        {
            ImGui_ImplSDL2_ProcessEvent(&Event);
        }

        if(Event.type == SDL_QUIT)
        {
            Close_All = true;
        }
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE)
        {
            Close_All = true;
        }
        else if(Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            Uint32 Window_ID = Event.window.windowID;
            for(auto &Item : Windows)
            {
                if(Item.second.Window != nullptr && SDL_GetWindowID(Item.second.Window) == Window_ID)
                {
                    Windows_To_Delete.push_back(Item.first);
                }
            }
        }
    }

    if(Close_All)
    {
        for(auto &Item : Windows)
        {
            Windows_To_Delete.push_back(Item.first);
        }
    }

    for(const std::string &Name : Windows_To_Delete)
    {
        Delete_Window(Name);
    }

    if(Target_FPS > 0)
    {
        SDL_Delay(1000 / Target_FPS);
    }
}

extern "C" void Wait_Until_Close(std::string Name)
{
    while(Window_Exists(Name))
    {
        Render();
        Update();
    }
}
