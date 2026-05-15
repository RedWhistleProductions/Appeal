#ifdef __MINGW32__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
#endif

#ifdef __linux__
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#ifdef APPEAL_USE_ASSIMP
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>
    #include <assimp/scene.h>
#endif

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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

struct Window_State
{
    SDL_Window *Window = nullptr;
    SDL_Renderer *Renderer = nullptr;
    int Width = 0;
    int Height = 0;
    SDL_Color Clear_Color = {24, 28, 34, 255};
    int View_X = 0;
    int View_Y = 0;
    int View_W = 0;
    int View_H = 0;
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
    std::vector<std::pair<int, int>> Edges;
};

struct Texture_State
{
    std::string File;
    SDL_Texture *Texture = nullptr;
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

struct GUI_Command
{
    std::string Kind;
    std::string Name;
    std::string Text;
};

std::string Resources = "./Resources";
std::string Graphics_Path = "./Resources/Graphics/";
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
std::vector<GUI_Command> GUI_Commands;

std::string Focused_Window;
int Target_FPS = 60;
bool SDL_Ready = false;
bool IMG_Ready = false;
bool TTF_Ready = false;
bool ImGui_Ready = false;

extern "C" void Delete_Window(std::string Name);

bool Ensure_SDL()
{
    if(SDL_Ready)
    {
        return true;
    }

    if(SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        std::cout << "SDL video could not initialize: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Ready = true;

    int Image_Flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int Image_Result = IMG_Init(Image_Flags);
    if((Image_Result & Image_Flags) != Image_Flags)
    {
        std::cout << "SDL_image partial initialization: " << IMG_GetError() << std::endl;
    }
    IMG_Ready = Image_Result != 0;

    if(TTF_Init() == 0)
    {
        TTF_Ready = true;
    }
    else
    {
        std::cout << "SDL_ttf could not initialize: " << TTF_GetError() << std::endl;
    }

    return true;
}

Window_State *Find_Window(std::string Name)
{
    auto It = Windows.find(Name);
    if(It == Windows.end())
    {
        std::cout << "Graphics window not found: " << Name << std::endl;
        return nullptr;
    }

    return &It->second;
}

void Destroy_Window(Window_State &State)
{
    if(State.Renderer != nullptr)
    {
        SDL_DestroyRenderer(State.Renderer);
        State.Renderer = nullptr;
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
    Mesh.Edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    return Mesh;
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
        aiProcess_GenNormals |
        aiProcess_SortByPType
    );

    if(Scene != nullptr && Scene->HasMeshes())
    {
        for(unsigned int Mesh_Index = 0; Mesh_Index < Scene->mNumMeshes; ++Mesh_Index)
        {
            const aiMesh *Imported_Mesh = Scene->mMeshes[Mesh_Index];
            int Vertex_Offset = static_cast<int>(Mesh.Vertices.size());

            for(unsigned int Vertex_Index = 0; Vertex_Index < Imported_Mesh->mNumVertices; ++Vertex_Index)
            {
                const aiVector3D &Vertex = Imported_Mesh->mVertices[Vertex_Index];
                Mesh.Vertices.push_back({Vertex.x, Vertex.y, Vertex.z});
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
            }
        }

        if(!Mesh.Vertices.empty() && !Mesh.Edges.empty())
        {
            return Mesh;
        }
    }
    else
    {
        std::cout << "Assimp did not load mesh: " << File << std::endl
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
        }
        else if(Type == "e" || Type == "E")
        {
            int A, B;
            Stream >> A >> B;
            Mesh.Edges.push_back({A, B});
        }
        else if(Type == "f" || Type == "F")
        {
            int A, B, C;
            Stream >> A >> B >> C;
            Mesh.Edges.push_back({A, B});
            Mesh.Edges.push_back({B, C});
            Mesh.Edges.push_back({C, A});
        }
    }

    if(Mesh.Vertices.empty() || Mesh.Edges.empty())
    {
        return Make_Cube_Mesh(File);
    }

    return Mesh;
}

void Destroy_Texture(Texture_State &State)
{
    if(State.Texture != nullptr)
    {
        SDL_DestroyTexture(State.Texture);
        State.Texture = nullptr;
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
    ImGui::StyleColorsDark();

    if(!ImGui_ImplSDL2_InitForSDLRenderer(Window.Window, Window.Renderer))
    {
        std::cout << "Dear ImGui SDL2 backend could not initialize" << std::endl;
        return false;
    }

    if(!ImGui_ImplSDLRenderer2_Init(Window.Renderer))
    {
        std::cout << "Dear ImGui SDL_Renderer backend could not initialize" << std::endl;
        ImGui_ImplSDL2_Shutdown();
        return false;
    }

    ImGui_Ready = true;
    return true;
}

void Begin_ImGui_Frame(Window_State &Window)
{
    if(!Ensure_ImGui(Window)) return;

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void End_ImGui_Frame(Window_State &Window)
{
    if(!ImGui_Ready) return;

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Window.Renderer);
}

void Draw_Text_Command(Window_State &Window, const Text_Command &Command)
{
    auto It = Fonts.find(Command.Font);
    if(It == Fonts.end() || It->second.Font == nullptr)
    {
        if(Fonts.empty())
        {
            std::cout << "Font not loaded: " << Command.Font << std::endl;
            return;
        }
        It = Fonts.begin();
        if(It->second.Font == nullptr)
        {
            std::cout << "Font not loaded: " << Command.Font << std::endl;
            return;
        }
    }

    SDL_Surface *Surface = TTF_RenderUTF8_Blended(It->second.Font, Command.Text.c_str(), Command.Color);
    if(Surface == nullptr)
    {
        std::cout << "Text surface could not be created: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *Texture = SDL_CreateTextureFromSurface(Window.Renderer, Surface);
    if(Texture == nullptr)
    {
        std::cout << "Text texture could not be created: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(Surface);
        return;
    }

    SDL_Rect Destination = {static_cast<int>(Command.X), static_cast<int>(Command.Y), Surface->w, Surface->h};
    SDL_FreeSurface(Surface);
    SDL_RenderCopy(Window.Renderer, Texture, nullptr, &Destination);
    SDL_DestroyTexture(Texture);
}

void Draw_GUI(Window_State &Window)
{
    if(!Ensure_ImGui(Window)) return;

    bool In_Window = false;
    for(const GUI_Command &Command : GUI_Commands)
    {
        if(Command.Kind == "Begin")
        {
            if(In_Window)
            {
                ImGui::End();
            }
            ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(260.0f, 130.0f), ImGuiCond_Once);
            ImGui::Begin(Command.Name.c_str());
            In_Window = true;
        }
        else if(Command.Kind == "Text")
        {
            if(!In_Window)
            {
                ImGui::Begin("Appeal");
                In_Window = true;
            }
            ImGui::TextUnformatted(Command.Text.c_str());
        }
        else if(Command.Kind == "Button")
        {
            if(!In_Window)
            {
                ImGui::Begin("Appeal");
                In_Window = true;
            }
            ImGui::Button(Command.Text.c_str(), ImVec2(180.0f, 0.0f));
        }
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

    SDL_SetRenderDrawColor(Window.Renderer, Line_Color.r, Line_Color.g, Line_Color.b, Line_Color.a);

    const Mesh_State &Mesh = Mesh_It->second;
    std::vector<SDL_Point> Points(Mesh.Vertices.size());
    std::vector<bool> Visible(Mesh.Vertices.size(), false);

    for(size_t i = 0; i < Mesh.Vertices.size(); ++i)
    {
        Vec3 Point = Mesh.Vertices[i];
        Point.X *= Asset.Scale_X;
        Point.Y *= Asset.Scale_Y;
        Point.Z *= Asset.Scale_Z;
        Point = Rotate_Point(Point, Asset.Yaw, Asset.Pitch, Asset.Roll);
        Point.X += Asset.X;
        Point.Y += Asset.Y;
        Point.Z += Asset.Z;
        Visible[i] = Project_Point(Window, Point, Points[i]);
    }

    for(const auto &Edge : Mesh.Edges)
    {
        if(Edge.first < 0 || Edge.second < 0) continue;
        if(static_cast<size_t>(Edge.first) >= Points.size() || static_cast<size_t>(Edge.second) >= Points.size()) continue;
        if(!Visible[Edge.first] || !Visible[Edge.second]) continue;
        SDL_RenderDrawLine(Window.Renderer, Points[Edge.first].x, Points[Edge.first].y, Points[Edge.second].x, Points[Edge.second].y);
    }
}

extern "C" void Constructor()
{
    Ensure_SDL();
}

extern "C" void Destructor()
{
    if(ImGui_Ready)
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ImGui_Ready = false;
    }

    for(auto &Item : Windows)
    {
        Destroy_Window(Item.second);
    }

    Windows.clear();

    for(auto &Item : Textures)
    {
        Destroy_Texture(Item.second);
    }
    Textures.clear();

    for(auto &Item : Fonts)
    {
        Destroy_Font(Item.second);
    }
    Fonts.clear();

    Text_Commands.clear();
    GUI_Commands.clear();

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
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if(State.Window == nullptr)
    {
        std::cout << "SDL window could not be created: " << SDL_GetError() << std::endl;
        return;
    }

    State.Renderer = SDL_CreateRenderer(State.Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(State.Renderer == nullptr)
    {
        std::cout << "SDL renderer could not be created: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(State.Window);
        return;
    }

        SDL_SetRenderDrawBlendMode(State.Renderer, SDL_BLENDMODE_BLEND);
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
        std::cout << "SDL display mode unavailable: " << SDL_GetError() << std::endl;
        Add_Window(Name, "Appeal", 1280, 720);
        return;
    }

    Add_Window(Name, "Appeal", Mode.w, Mode.h);
    Window_State *State = Find_Window(Name);
    if(State != nullptr)
    {
        SDL_SetWindowFullscreen(State->Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

extern "C" void Delete_Window(std::string Name)
{
    auto It = Windows.find(Name);
    if(It == Windows.end()) return;

    if(Windows.size() == 1 && ImGui_Ready)
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ImGui_Ready = false;
    }

    Destroy_Window(It->second);
    Windows.erase(It);

    if(Focused_Window == Name)
    {
        Focused_Window = Windows.empty() ? "" : Windows.begin()->first;
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

extern "C" void Set_View_Port(std::string Window, int X, int Y, int W, int H)
{
    Window_State *State = Find_Window(Window);
    if(State == nullptr) return;

    SDL_Rect Viewport = {X, Y, W, H};
    SDL_RenderSetViewport(State->Renderer, &Viewport);
    State->View_X = X;
    State->View_Y = Y;
    State->View_W = W;
    State->View_H = H;
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
        std::cout << "Load_Texture needs a window before loading: " << Name << std::endl;
        return;
    }

    Texture_State &State = Textures[Name];
    Destroy_Texture(State);
    State.File = Texture_Path + File;

    State.Texture = IMG_LoadTexture(Window->Renderer, State.File.c_str());
    if(State.Texture == nullptr)
    {
        SDL_Surface *Surface = SDL_LoadBMP(State.File.c_str());
        if(Surface != nullptr)
        {
            State.Texture = SDL_CreateTextureFromSurface(Window->Renderer, Surface);
            State.Width = Surface->w;
            State.Height = Surface->h;
            SDL_FreeSurface(Surface);
        }
    }

    if(State.Texture == nullptr)
    {
        std::cout << "Texture did not load: " << State.File << std::endl
                  << "SDL_image Error: " << IMG_GetError() << std::endl
                  << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_QueryTexture(State.Texture, nullptr, nullptr, &State.Width, &State.Height);
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
        std::cout << "Font did not load: " << State.File << std::endl
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

extern "C" void Draw_Text(std::string Font, std::string Text, float X, float Y)
{
    Text_Commands.push_back({Font, Text, X, Y, {230, 236, 245, 255}});
}

extern "C" void GUI_Begin(std::string Name)
{
    GUI_Commands.clear();
    GUI_Commands.push_back({"Begin", Name, Name});
}

extern "C" void GUI_Text(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Text", Name, Text});
}

extern "C" void GUI_Button(std::string Name, std::string Text)
{
    GUI_Commands.push_back({"Button", Name, Text});
}

extern "C" void GUI_End()
{
}

extern "C" void Render()
{
    int Offset = 0;
    for(auto &Item : Windows)
    {
        Window_State &State = Item.second;
        SDL_SetRenderDrawColor(State.Renderer, State.Clear_Color.r, State.Clear_Color.g, State.Clear_Color.b, State.Clear_Color.a);
        SDL_RenderClear(State.Renderer);
        Begin_ImGui_Frame(State);

        SDL_SetRenderDrawColor(State.Renderer, 72, 156, 214, 255);
        SDL_Rect Test_Rect = {48 + Offset, 48 + Offset, 180, 120};
        SDL_RenderFillRect(State.Renderer, &Test_Rect);

        SDL_SetRenderDrawColor(State.Renderer, 245, 196, 66, 255);
        SDL_Rect Accent_Rect = {88 + Offset, 88 + Offset, 180, 120};
        SDL_RenderDrawRect(State.Renderer, &Accent_Rect);

        for(auto &Asset_Item : Assets)
        {
            Asset_State &Asset = Asset_Item.second;
            auto Texture_It = Textures.find(Asset.Texture);
            if(Texture_It == Textures.end() || Texture_It->second.Texture == nullptr)
            {
                Draw_Mesh_Asset(State, Asset);
                continue;
            }

            int Width = static_cast<int>(Texture_It->second.Width * Asset.Scale_X);
            int Height = static_cast<int>(Texture_It->second.Height * Asset.Scale_Y);
            SDL_Rect Destination = {static_cast<int>(Asset.X), static_cast<int>(Asset.Y), Width, Height};
            SDL_Point Center = {Width / 2, Height / 2};
            SDL_RenderCopyEx(State.Renderer, Texture_It->second.Texture, nullptr, &Destination, Asset.Roll, &Center, SDL_FLIP_NONE);
            Draw_Mesh_Asset(State, Asset);
        }

        for(const Text_Command &Command : Text_Commands)
        {
            Draw_Text_Command(State, Command);
        }

        Draw_GUI(State);
        End_ImGui_Frame(State);
        SDL_RenderPresent(State.Renderer);
        Offset += 24;
    }
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
