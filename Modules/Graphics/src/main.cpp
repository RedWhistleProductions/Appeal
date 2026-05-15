#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;

void (*Set_Paths)(std::string Resources);

void (*Add_Window)(std::string Name, std::string Title, int Width, int Height);
void (*Add_Full_Screen_Window)(std::string Name, int Monitor);
void (*Delete_Window)(std::string Name);
void (*Show)(std::string Name);
void (*Hide)(std::string Name);
void (*Set_Focus)(std::string Name);
void (*Set_Monitor)(std::string Name, int Monitor);
void (*Set_FPS)(int Frames);

void (*Set_View_Port)(std::string Window, int X, int Y, int W, int H);
void (*Set_Perspective)(std::string Window, float Fovy, float Near, float Far);
void (*Set_Ortho)(std::string Window, float Left, float Right, float Bottom, float Top, float Near, float Far);

void (*Set_Camera_Location)(std::string Window, float X, float Y, float Z);
void (*Set_Camera_Orientation)(std::string Window, float Yaw, float Pitch, float Roll);
void (*Move_Camera)(std::string Window, float X, float Y, float Z);
void (*Rotate_Camera)(std::string Window, float Yaw, float Pitch, float Roll);
void (*Update_Camera)(std::string Window);

void (*Load_Shader)(std::string Name, std::string File);
void (*Load_Texture)(std::string Name, std::string File);
void (*Load_Font)(std::string Name, std::string File, int Size);
void (*Load_Mesh)(std::string Name, std::string File);
void (*Load_Asset)(std::string Name, std::string File);

void (*Set_Texture)(std::string Name, std::string Texture);
void (*Set_Shader)(std::string Name, std::string Shader);
void (*Set_Location)(std::string Name, float X, float Y, float Z);
void (*Set_Orientation)(std::string Name, float Yaw, float Pitch, float Roll);
void (*Set_Scale)(std::string Name, float X, float Y, float Z);
void (*Move_Asset)(std::string Name, float X, float Y, float Z);
void (*Rotate_Asset)(std::string Name, float Yaw, float Pitch, float Roll);

void (*Draw_Text)(std::string Font, std::string Text, float X, float Y);
void (*GUI_Begin)(std::string Name);
void (*GUI_Text)(std::string Name, std::string Text);
void (*GUI_Button)(std::string Name, std::string Text);
void (*GUI_End)();

void (*Render)();
void (*Update)();

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init(std::string Name)
{
    Module.Load(Name);

    Assign("Set_Paths", Set_Paths);

    Assign("Add_Window", Add_Window);
    Assign("Add_Full_Screen_Window", Add_Full_Screen_Window);
    Assign("Delete_Window", Delete_Window);
    Assign("Show", Show);
    Assign("Hide", Hide);
    Assign("Set_Focus", Set_Focus);
    Assign("Set_Monitor", Set_Monitor);
    Assign("Set_FPS", Set_FPS);

    Assign("Set_View_Port", Set_View_Port);
    Assign("Set_Perspective", Set_Perspective);
    Assign("Set_Ortho", Set_Ortho);

    Assign("Set_Camera_Location", Set_Camera_Location);
    Assign("Set_Camera_Orientation", Set_Camera_Orientation);
    Assign("Move_Camera", Move_Camera);
    Assign("Rotate_Camera", Rotate_Camera);
    Assign("Update_Camera", Update_Camera);

    Assign("Load_Shader", Load_Shader);
    Assign("Load_Texture", Load_Texture);
    Assign("Load_Font", Load_Font);
    Assign("Load_Mesh", Load_Mesh);
    Assign("Load_Asset", Load_Asset);

    Assign("Set_Texture", Set_Texture);
    Assign("Set_Shader", Set_Shader);
    Assign("Set_Location", Set_Location);
    Assign("Set_Orientation", Set_Orientation);
    Assign("Set_Scale", Set_Scale);
    Assign("Move_Asset", Move_Asset);
    Assign("Rotate_Asset", Rotate_Asset);

    Assign("Draw_Text", Draw_Text);
    Assign("GUI_Begin", GUI_Begin);
    Assign("GUI_Text", GUI_Text);
    Assign("GUI_Button", GUI_Button);
    Assign("GUI_End", GUI_End);

    Assign("Render", Render);
    Assign("Update", Update);
}

extern "C" void Interpreter(Data_Source *Data)
{
    std::string Command;
    *Data >> Command;

    if(Command == "Init")
    {
        std::string Name;
        *Data >> Name;
        Init(Name);
    }
    else if(Command == "Set_Paths")
    {
        std::string Resources;
        *Data >> Resources;
        Set_Paths(Resources);
    }
    else if(Command == "Add_Window")
    {
        std::string Name, Title;
        int Width, Height;
        *Data >> Name;
        *Data >> Title;
        *Data >> Width;
        *Data >> Height;
        Add_Window(Name, Title, Width, Height);
    }
    else if(Command == "Add_Full_Screen_Window")
    {
        std::string Name;
        int Monitor;
        *Data >> Name;
        *Data >> Monitor;
        Add_Full_Screen_Window(Name, Monitor);
    }
    else if(Command == "Delete_Window")
    {
        std::string Name;
        *Data >> Name;
        Delete_Window(Name);
    }
    else if(Command == "Show")
    {
        std::string Name;
        *Data >> Name;
        Show(Name);
    }
    else if(Command == "Hide")
    {
        std::string Name;
        *Data >> Name;
        Hide(Name);
    }
    else if(Command == "Set_Focus")
    {
        std::string Name;
        *Data >> Name;
        Set_Focus(Name);
    }
    else if(Command == "Set_Monitor")
    {
        std::string Name;
        int Monitor;
        *Data >> Name;
        *Data >> Monitor;
        Set_Monitor(Name, Monitor);
    }
    else if(Command == "Set_FPS")
    {
        int Frames;
        *Data >> Frames;
        Set_FPS(Frames);
    }
    else if(Command == "Set_View_Port")
    {
        std::string Window;
        int X, Y, W, H;
        *Data >> Window;
        *Data >> X;
        *Data >> Y;
        *Data >> W;
        *Data >> H;
        Set_View_Port(Window, X, Y, W, H);
    }
    else if(Command == "Set_Perspective")
    {
        std::string Window;
        float Fovy, Near, Far;
        *Data >> Window;
        *Data >> Fovy;
        *Data >> Near;
        *Data >> Far;
        Set_Perspective(Window, Fovy, Near, Far);
    }
    else if(Command == "Set_Ortho")
    {
        std::string Window;
        float Left, Right, Bottom, Top, Near, Far;
        *Data >> Window;
        *Data >> Left;
        *Data >> Right;
        *Data >> Bottom;
        *Data >> Top;
        *Data >> Near;
        *Data >> Far;
        Set_Ortho(Window, Left, Right, Bottom, Top, Near, Far);
    }
    else if(Command == "Set_Camera_Location")
    {
        std::string Window;
        float X, Y, Z;
        *Data >> Window;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Camera_Location(Window, X, Y, Z);
    }
    else if(Command == "Set_Camera_Orientation")
    {
        std::string Window;
        float Yaw, Pitch, Roll;
        *Data >> Window;
        *Data >> Yaw;
        *Data >> Pitch;
        *Data >> Roll;
        Set_Camera_Orientation(Window, Yaw, Pitch, Roll);
    }
    else if(Command == "Move_Camera")
    {
        std::string Window;
        float X, Y, Z;
        *Data >> Window;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Move_Camera(Window, X, Y, Z);
    }
    else if(Command == "Rotate_Camera")
    {
        std::string Window;
        float Yaw, Pitch, Roll;
        *Data >> Window;
        *Data >> Yaw;
        *Data >> Pitch;
        *Data >> Roll;
        Rotate_Camera(Window, Yaw, Pitch, Roll);
    }
    else if(Command == "Update_Camera")
    {
        std::string Window;
        *Data >> Window;
        Update_Camera(Window);
    }
    else if(Command == "Load_Shader")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Load_Shader(Name, File);
    }
    else if(Command == "Load_Texture")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Load_Texture(Name, File);
    }
    else if(Command == "Load_Font")
    {
        std::string Name, File;
        int Size;
        *Data >> Name;
        *Data >> File;
        *Data >> Size;
        Load_Font(Name, File, Size);
    }
    else if(Command == "Load_Mesh")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Load_Mesh(Name, File);
    }
    else if(Command == "Load_Asset")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Load_Asset(Name, File);
    }
    else if(Command == "Set_Texture")
    {
        std::string Name, Texture;
        *Data >> Name;
        *Data >> Texture;
        Set_Texture(Name, Texture);
    }
    else if(Command == "Set_Shader")
    {
        std::string Name, Shader;
        *Data >> Name;
        *Data >> Shader;
        Set_Shader(Name, Shader);
    }
    else if(Command == "Set_Location")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Location(Name, X, Y, Z);
    }
    else if(Command == "Set_Orientation")
    {
        std::string Name;
        float Yaw, Pitch, Roll;
        *Data >> Name;
        *Data >> Yaw;
        *Data >> Pitch;
        *Data >> Roll;
        Set_Orientation(Name, Yaw, Pitch, Roll);
    }
    else if(Command == "Set_Scale")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Set_Scale(Name, X, Y, Z);
    }
    else if(Command == "Move_Asset")
    {
        std::string Name;
        float X, Y, Z;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> Z;
        Move_Asset(Name, X, Y, Z);
    }
    else if(Command == "Rotate_Asset")
    {
        std::string Name;
        float Yaw, Pitch, Roll;
        *Data >> Name;
        *Data >> Yaw;
        *Data >> Pitch;
        *Data >> Roll;
        Rotate_Asset(Name, Yaw, Pitch, Roll);
    }
    else if(Command == "Draw_Text")
    {
        std::string Font, Text;
        float X, Y;
        *Data >> Font;
        *Data >> Text;
        *Data >> X;
        *Data >> Y;
        Draw_Text(Font, Text, X, Y);
    }
    else if(Command == "GUI_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Begin(Name);
    }
    else if(Command == "GUI_Text")
    {
        std::string Name, Text;
        *Data >> Name;
        *Data >> Text;
        GUI_Text(Name, Text);
    }
    else if(Command == "GUI_Button")
    {
        std::string Name, Text;
        *Data >> Name;
        *Data >> Text;
        GUI_Button(Name, Text);
    }
    else if(Command == "GUI_End")
    {
        GUI_End();
    }
    else if(Command == "Render")
    {
        Render();
    }
    else if(Command == "Update")
    {
        Update();
    }
    else
    {
        std::cout << "\tError: " << Command << " not found in Graphics Dictionary" << std::endl;
    }
}
