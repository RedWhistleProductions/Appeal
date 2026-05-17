#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);

void (*Add_Window)(std::string Name, std::string Title, int Width, int Height);
void (*Add_Full_Screen_Window)(std::string Name, int Monitor);
void (*Delete_Window)(std::string Name);
void (*Close_All)();
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
void (*Set_Active_Asset)(std::string Name);
void (*Rotate_Active_Yaw_Input)(int Value);
void (*Move_Active_Forward_Input)(int Value);

void (*Draw_Text)(std::string Font, std::string Text, float X, float Y);
void (*GUI_Begin)(std::string Name);
void (*GUI_Text)(std::string Name, std::string Text);
void (*GUI_Button)(std::string Name, std::string Text);
bool (*GUI_Button_Clicked)(std::string Name);
void (*Set_GUI_Button_Function)(std::string Name, std::string Function);
void (*GUI_Slider_Int)(std::string Name, std::string Text, int Value, int Min, int Max);
int (*GUI_Slider_Int_Value)(std::string Name);
bool (*GUI_Slider_Int_Changed)(std::string Name);
void (*Set_GUI_Slider_Int_Function)(std::string Name, std::string Function);
void (*GUI_Checkbox)(std::string Name, std::string Text, int Value);
int (*GUI_Checkbox_Value)(std::string Name);
bool (*GUI_Checkbox_Changed)(std::string Name);
void (*Set_GUI_Checkbox_Function)(std::string Name, std::string Function);
void (*GUI_End)();

void (*Render)();
void (*Update)();
void (*Wait_Until_Close)(std::string Window);
bool (*Window_Open)(std::string Window);

Named_List<Module_Data> *Globals = nullptr;

void Set_Global(std::string Name, std::string Type, std::string Value)
{
    if(Globals == nullptr) return;

    std::string Full_Name = "Graphics." + Name;
    Module_Data Data;
    Data.Module_Name = "Graphics";
    Data.Name = Name;
    Data.Type = Type;
    Data.Value = Value;

    if(Globals->Find(Full_Name))
    {
        Globals->Current->Value = Data;
    }
    else
    {
        Globals->Add_Node(Full_Name, Data);
    }
}

void Update_Window_Global(std::string Name)
{
    if(Window_Open != nullptr)
    {
        Set_Global("Window." + Name + ".Open", "bool", Window_Open(Name) ? "1" : "0");
    }
}

void Update_GUI_Button_Global(std::string Name)
{
    if(GUI_Button_Clicked != nullptr)
    {
        Set_Global("GUI." + Name + ".Clicked", "bool", GUI_Button_Clicked(Name) ? "1" : "0");
    }
}

void Update_GUI_Slider_Global(std::string Name)
{
    if(GUI_Slider_Int_Value != nullptr)
    {
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Slider_Int_Value(Name)));
    }
    if(GUI_Slider_Int_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Slider_Int_Changed(Name) ? "1" : "0");
    }
}

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init_Globals(std::string Name, Named_List<Module_Data> *Shared_Globals)
{
    Globals = Shared_Globals;
    Module.Load(Name);

    Assign("Debug", Debug);
    Assign("Set_Paths", Set_Paths);

    Assign("Add_Window", Add_Window);
    Assign("Add_Full_Screen_Window", Add_Full_Screen_Window);
    Assign("Delete_Window", Delete_Window);
    Assign("Close_All", Close_All);
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
    Assign("Set_Active_Asset", Set_Active_Asset);
    Assign("Rotate_Active_Yaw_Input", Rotate_Active_Yaw_Input);
    Assign("Move_Active_Forward_Input", Move_Active_Forward_Input);

    Assign("Draw_Text", Draw_Text);
    Assign("GUI_Begin", GUI_Begin);
    Assign("GUI_Text", GUI_Text);
    Assign("GUI_Button", GUI_Button);
    Assign("GUI_Button_Clicked", GUI_Button_Clicked);
    Assign("Set_GUI_Button_Function", Set_GUI_Button_Function);
    Assign("GUI_Slider_Int", GUI_Slider_Int);
    Assign("GUI_Slider_Int_Value", GUI_Slider_Int_Value);
    Assign("GUI_Slider_Int_Changed", GUI_Slider_Int_Changed);
    Assign("Set_GUI_Slider_Int_Function", Set_GUI_Slider_Int_Function);
    Assign("GUI_Checkbox", GUI_Checkbox);
    Assign("GUI_Checkbox_Value", GUI_Checkbox_Value);
    Assign("GUI_Checkbox_Changed", GUI_Checkbox_Changed);
    Assign("Set_GUI_Checkbox_Function", Set_GUI_Checkbox_Function);
    Assign("GUI_End", GUI_End);

    Assign("Render", Render);
    Assign("Update", Update);
    Assign("Wait_Until_Close", Wait_Until_Close);
    Assign("Window_Open", Window_Open);

    Set_Global("Window.Main.Open", "bool", "0");
}

extern "C" void Init(std::string Name)
{
    Init_Globals(Name, nullptr);
}

extern "C" void Set_Function_Runner(void (*Run_Function)(std::string), void (*Run_Analog_Function)(std::string, int))
{
    void (*Plugin_Set_Function_Runner)(void (*Run_Function)(std::string), void (*Run_Analog_Function)(std::string, int));
    Module.Assign("Set_Function_Runner", Plugin_Set_Function_Runner);
    if(Plugin_Set_Function_Runner != nullptr)
    {
        Plugin_Set_Function_Runner(Run_Function, Run_Analog_Function);
    }
}

extern "C" void Graphics_GUI_Begin(std::string Name) { GUI_Begin(Name); }
extern "C" void Graphics_GUI_Text(std::string Name, std::string Text) { GUI_Text(Name, Text); }
extern "C" void Graphics_GUI_Button(std::string Name, std::string Text) { GUI_Button(Name, Text); }
extern "C" void Graphics_GUI_End() { GUI_End(); }
extern "C" bool Graphics_GUI_Button_Clicked(std::string Name) { return GUI_Button_Clicked(Name); }
extern "C" void Graphics_Set_GUI_Button_Function(std::string Name, std::string Function) { Set_GUI_Button_Function(Name, Function); }
extern "C" void Graphics_GUI_Slider_Int(std::string Name, std::string Text, int Value, int Min, int Max) { GUI_Slider_Int(Name, Text, Value, Min, Max); }
extern "C" int Graphics_GUI_Slider_Int_Value(std::string Name) { return GUI_Slider_Int_Value(Name); }
extern "C" bool Graphics_GUI_Slider_Int_Changed(std::string Name) { return GUI_Slider_Int_Changed(Name); }
extern "C" void Graphics_Set_GUI_Slider_Int_Function(std::string Name, std::string Function) { Set_GUI_Slider_Int_Function(Name, Function); }
extern "C" void Graphics_GUI_Checkbox(std::string Name, std::string Text, int Value) { GUI_Checkbox(Name, Text, Value); }
extern "C" int Graphics_GUI_Checkbox_Value(std::string Name) { return GUI_Checkbox_Value(Name); }
extern "C" bool Graphics_GUI_Checkbox_Changed(std::string Name) { return GUI_Checkbox_Changed(Name); }
extern "C" void Graphics_Set_GUI_Checkbox_Function(std::string Name, std::string Function) { Set_GUI_Checkbox_Function(Name, Function); }
extern "C" void Graphics_Close_All() { Close_All(); }
extern "C" void Graphics_Render() { Render(); }
extern "C" void Graphics_Update() { Update(); }
extern "C" bool Graphics_Window_Open(std::string Window) { return Window_Open(Window); }

extern "C" void Shutdown()
{
    if(Close_All != nullptr)
    {
        Close_All();
    }
    Module.Unload();
    Globals = nullptr;
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
    else if(Command == "Debug")
    {
        int Enable;
        *Data >> Enable;
        Module_Debug = Enable != 0;
        if(Debug != nullptr) Debug(Module_Debug);
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
        Update_Window_Global(Name);
    }
    else if(Command == "Add_Full_Screen_Window")
    {
        std::string Name;
        int Monitor;
        *Data >> Name;
        *Data >> Monitor;
        Add_Full_Screen_Window(Name, Monitor);
        Update_Window_Global(Name);
    }
    else if(Command == "Delete_Window")
    {
        std::string Name;
        *Data >> Name;
        Delete_Window(Name);
        Update_Window_Global(Name);
    }
    else if(Command == "Close_All")
    {
        Close_All();
        Set_Global("Window.Main.Open", "bool", "0");
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
    else if(Command == "Set_Active_Asset")
    {
        std::string Name;
        *Data >> Name;
        Set_Active_Asset(Name);
    }
    else if(Command == "Rotate_Active_Yaw_Input")
    {
        int Value;
        *Data >> Value;
        Rotate_Active_Yaw_Input(Value);
    }
    else if(Command == "Move_Active_Forward_Input")
    {
        int Value;
        *Data >> Value;
        Move_Active_Forward_Input(Value);
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
    else if(Command == "GUI_Button_Clicked")
    {
        std::string Name;
        *Data >> Name;
        bool Clicked = GUI_Button_Clicked(Name);
        Set_Global("GUI." + Name + ".Clicked", "bool", Clicked ? "1" : "0");
    }
    else if(Command == "Set_GUI_Button_Function")
    {
        std::string Name, Function;
        *Data >> Name;
        *Data >> Function;
        Set_GUI_Button_Function(Name, Function);
    }
    else if(Command == "GUI_Slider_Int")
    {
        std::string Name, Text;
        int Value, Min, Max;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Min;
        *Data >> Max;
        GUI_Slider_Int(Name, Text, Value, Min, Max);
        Update_GUI_Slider_Global(Name);
    }
    else if(Command == "GUI_Slider_Int_Value")
    {
        std::string Name;
        *Data >> Name;
        int Value = GUI_Slider_Int_Value(Name);
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(Value));
    }
    else if(Command == "GUI_Slider_Int_Changed")
    {
        std::string Name;
        *Data >> Name;
        bool Changed = GUI_Slider_Int_Changed(Name);
        Set_Global("GUI." + Name + ".Changed", "bool", Changed ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Slider_Int_Value(Name)));
    }
    else if(Command == "Set_GUI_Slider_Int_Function")
    {
        std::string Name, Function;
        *Data >> Name;
        *Data >> Function;
        Set_GUI_Slider_Int_Function(Name, Function);
    }
    else if(Command == "GUI_Checkbox")
    {
        std::string Name, Text;
        int Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        GUI_Checkbox(Name, Text, Value);
        Set_Global("GUI." + Name + ".Value", "bool", GUI_Checkbox_Value(Name) ? "1" : "0");
    }
    else if(Command == "GUI_Checkbox_Value")
    {
        std::string Name;
        *Data >> Name;
        int Value = GUI_Checkbox_Value(Name);
        Set_Global("GUI." + Name + ".Value", "bool", Value ? "1" : "0");
    }
    else if(Command == "GUI_Checkbox_Changed")
    {
        std::string Name;
        *Data >> Name;
        bool Changed = GUI_Checkbox_Changed(Name);
        Set_Global("GUI." + Name + ".Changed", "bool", Changed ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "bool", GUI_Checkbox_Value(Name) ? "1" : "0");
    }
    else if(Command == "Set_GUI_Checkbox_Function")
    {
        std::string Name, Function;
        *Data >> Name;
        *Data >> Function;
        Set_GUI_Checkbox_Function(Name, Function);
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
        Set_Global("Updated", "bool", "1");
    }
    else if(Command == "Wait_Until_Close")
    {
        std::string Window;
        *Data >> Window;
        Wait_Until_Close(Window);
    }
    else if(Command == "Window_Open")
    {
        std::string Window;
        *Data >> Window;
        Update_Window_Global(Window);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Graphics Dictionary" << std::endl;
    }
}
