#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"

Plugin Module;
bool Module_Debug = false;
bool Plugin_Loaded = false;

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
void (*Set_Window_Icon)(std::string Name, std::string File);

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
void (*Free_Texture)(std::string Name);
void (*Free_Asset)(std::string Name);

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
void (*Draw_Rect)(std::string Name, float X, float Y, float W, float H, int R, int G, int B, int A);
void (*GUI_Begin)(std::string Name);
void (*GUI_Begin_Fullscreen)(std::string Name);
void (*GUI_End_Window)();
void (*GUI_Text)(std::string Name, std::string Text);
void (*GUI_Label_Text)(std::string Name, std::string Text);
void (*GUI_Bullet_Text)(std::string Name, std::string Text);
void (*GUI_Separator)();
void (*GUI_Separator_Text)(std::string Text);
void (*GUI_Same_Line)();
void (*GUI_New_Line)();
void (*GUI_Spacing)();
void (*GUI_Indent)(int Pixels);
void (*GUI_Unindent)(int Pixels);
void (*GUI_Group_Begin)(std::string Name);
void (*GUI_Group_End)();
void (*GUI_Button)(std::string Name, std::string Text);
void (*GUI_New_Game_Project_Button)(std::string Name, std::string Text, std::string Name_Input, std::string Picker);
void (*GUI_New_Level_Project_Button)(std::string Name, std::string Text, std::string Game_Input, std::string Level_Input, std::string Picker);
void (*GUI_Small_Button)(std::string Name, std::string Text);
bool (*GUI_Button_Clicked)(std::string Name);
void (*Set_GUI_Button_Function)(std::string Name, std::string Function);
void (*GUI_Slider_Int)(std::string Name, std::string Text, int Value, int Min, int Max);
int (*GUI_Slider_Int_Value)(std::string Name);
bool (*GUI_Slider_Int_Changed)(std::string Name);
void (*GUI_Slider_Float)(std::string Name, std::string Text, float Value, float Min, float Max);
float (*GUI_Float_Value)(std::string Name);
bool (*GUI_Float_Changed)(std::string Name);
void (*GUI_Drag_Int)(std::string Name, std::string Text, int Value, int Min, int Max, int Speed);
void (*GUI_Drag_Float)(std::string Name, std::string Text, float Value, float Min, float Max, float Speed);
void (*GUI_Input_Text)(std::string Name, std::string Text, std::string Value);
void (*GUI_Input_Text_Multiline)(std::string Name, std::string Text, std::string Value, int Width, int Height);
void (*GUI_Set_Text_Value)(std::string Name, std::string Value);
std::string (*GUI_Text_Value)(std::string Name);
bool (*GUI_Text_Changed)(std::string Name);
void (*GUI_Input_Int)(std::string Name, std::string Text, int Value);
void (*GUI_Input_Float)(std::string Name, std::string Text, float Value);
int (*GUI_Int_Value)(std::string Name);
bool (*GUI_Int_Changed)(std::string Name);
void (*Set_GUI_Slider_Int_Function)(std::string Name, std::string Function);
void (*GUI_Checkbox)(std::string Name, std::string Text, int Value);
int (*GUI_Checkbox_Value)(std::string Name);
bool (*GUI_Checkbox_Changed)(std::string Name);
void (*Set_GUI_Checkbox_Function)(std::string Name, std::string Function);
void (*GUI_Radio_Button)(std::string Name, std::string Text, int Option);
void (*GUI_Combo)(std::string Name, std::string Text, std::string Items, int Value);
void (*GUI_List_Box)(std::string Name, std::string Text, std::string Items, int Value);
void (*GUI_File_Picker)(std::string Name, std::string Text, std::string Root, std::string Start);
void (*GUI_File_Picker_Select)(std::string Name, std::string Path);
void (*GUI_File_Picker_Set_Start)(std::string Name, std::string Start);
void (*GUI_Selectable)(std::string Name, std::string Text, int Selected);
int (*GUI_Selection_Value)(std::string Name);
std::string (*GUI_Selection_Text)(std::string Name);
bool (*GUI_Selection_Changed)(std::string Name);
std::string (*GUI_File_Picker_Current)(std::string Name);
std::string (*GUI_File_Picker_Selected)(std::string Name);
std::string (*GUI_File_Picker_Selected_Path)(std::string Name);
std::string (*GUI_File_Picker_Selected_Relative)(std::string Name);
bool (*GUI_File_Picker_Selected_Is_Directory)(std::string Name);
bool (*GUI_File_Picker_Changed)(std::string Name);
void (*GUI_Progress_Bar)(std::string Name, float Fraction, std::string Text);
void (*GUI_Tooltip)(std::string Text);
void (*GUI_Tree_Node)(std::string Name);
void (*GUI_Tree_End)();
void (*GUI_Tab_Bar_Begin)(std::string Name);
void (*GUI_Tab_Bar_End)();
void (*GUI_Tab_Item_Begin)(std::string Name);
void (*GUI_Tab_Item_End)();
void (*GUI_Menu_Bar_Begin)();
void (*GUI_Menu_Bar_End)();
void (*GUI_Menu_Begin)(std::string Name);
void (*GUI_Menu_End)();
void (*GUI_Menu_Item)(std::string Name, std::string Text);
void (*Set_GUI_Function)(std::string Name, std::string Function);
void (*Set_GUI_Visible)(bool Visible);
void (*Set_GUI_Docking)(bool Enable);
void (*Toggle_GUI)();
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

void Update_GUI_Float_Global(std::string Name)
{
    if(GUI_Float_Value != nullptr)
    {
        Set_Global("GUI." + Name + ".Value", "float", std::to_string(GUI_Float_Value(Name)));
    }
    if(GUI_Float_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Float_Changed(Name) ? "1" : "0");
    }
}

void Update_GUI_Int_Global(std::string Name)
{
    if(GUI_Int_Value != nullptr)
    {
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Int_Value(Name)));
    }
    if(GUI_Int_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Int_Changed(Name) ? "1" : "0");
    }
}

void Update_GUI_Text_Global(std::string Name)
{
    if(GUI_Text_Value != nullptr)
    {
        Set_Global("GUI." + Name + ".Value", "string", GUI_Text_Value(Name));
    }
    if(GUI_Text_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Text_Changed(Name) ? "1" : "0");
    }
}

void Update_GUI_Selection_Global(std::string Name)
{
    if(GUI_Selection_Value != nullptr)
    {
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Selection_Value(Name)));
    }
    if(GUI_Selection_Text != nullptr)
    {
        Set_Global("GUI." + Name + ".Text", "string", GUI_Selection_Text(Name));
    }
    if(GUI_Selection_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Selection_Changed(Name) ? "1" : "0");
    }
}

void Update_GUI_File_Picker_Global(std::string Name)
{
    if(GUI_File_Picker_Current != nullptr)
    {
        Set_Global("GUI." + Name + ".Current", "string", GUI_File_Picker_Current(Name));
    }
    if(GUI_File_Picker_Selected != nullptr)
    {
        Set_Global("GUI." + Name + ".Selected", "string", GUI_File_Picker_Selected(Name));
    }
    if(GUI_File_Picker_Selected_Path != nullptr)
    {
        Set_Global("GUI." + Name + ".Selected_Path", "string", GUI_File_Picker_Selected_Path(Name));
    }
    if(GUI_File_Picker_Selected_Relative != nullptr)
    {
        Set_Global("GUI." + Name + ".Selected_Relative", "string", GUI_File_Picker_Selected_Relative(Name));
    }
    if(GUI_File_Picker_Selected_Is_Directory != nullptr)
    {
        Set_Global("GUI." + Name + ".Selected_Is_Directory", "bool", GUI_File_Picker_Selected_Is_Directory(Name) ? "1" : "0");
    }
    if(GUI_File_Picker_Changed != nullptr)
    {
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_File_Picker_Changed(Name) ? "1" : "0");
    }
}

void Set_GUI_Control_Function(std::string Name, std::string Function)
{
    if(Set_GUI_Function != nullptr && Function != "" && Function != "None")
    {
        Set_GUI_Function(Name, Function);
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
    Plugin_Loaded = true;

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
    Assign("Set_Window_Icon", Set_Window_Icon);

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
    Assign("Free_Texture", Free_Texture);
    Assign("Free_Asset", Free_Asset);

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
    Assign("Draw_Rect", Draw_Rect);
    Assign("GUI_Begin", GUI_Begin);
    Assign("GUI_Begin_Fullscreen", GUI_Begin_Fullscreen);
    Assign("GUI_End_Window", GUI_End_Window);
    Assign("GUI_Text", GUI_Text);
    Assign("GUI_Label_Text", GUI_Label_Text);
    Assign("GUI_Bullet_Text", GUI_Bullet_Text);
    Assign("GUI_Separator", GUI_Separator);
    Assign("GUI_Separator_Text", GUI_Separator_Text);
    Assign("GUI_Same_Line", GUI_Same_Line);
    Assign("GUI_New_Line", GUI_New_Line);
    Assign("GUI_Spacing", GUI_Spacing);
    Assign("GUI_Indent", GUI_Indent);
    Assign("GUI_Unindent", GUI_Unindent);
    Assign("GUI_Group_Begin", GUI_Group_Begin);
    Assign("GUI_Group_End", GUI_Group_End);
    Assign("GUI_Button", GUI_Button);
    Assign("GUI_New_Game_Project_Button", GUI_New_Game_Project_Button);
    Assign("GUI_New_Level_Project_Button", GUI_New_Level_Project_Button);
    Assign("GUI_Small_Button", GUI_Small_Button);
    Assign("GUI_Button_Clicked", GUI_Button_Clicked);
    Assign("Set_GUI_Button_Function", Set_GUI_Button_Function);
    Assign("GUI_Slider_Int", GUI_Slider_Int);
    Assign("GUI_Slider_Int_Value", GUI_Slider_Int_Value);
    Assign("GUI_Slider_Int_Changed", GUI_Slider_Int_Changed);
    Assign("GUI_Slider_Float", GUI_Slider_Float);
    Assign("GUI_Float_Value", GUI_Float_Value);
    Assign("GUI_Float_Changed", GUI_Float_Changed);
    Assign("GUI_Drag_Int", GUI_Drag_Int);
    Assign("GUI_Drag_Float", GUI_Drag_Float);
    Assign("GUI_Input_Text", GUI_Input_Text);
    Assign("GUI_Input_Text_Multiline", GUI_Input_Text_Multiline);
    Assign("GUI_Set_Text_Value", GUI_Set_Text_Value);
    Assign("GUI_Text_Value", GUI_Text_Value);
    Assign("GUI_Text_Changed", GUI_Text_Changed);
    Assign("GUI_Input_Int", GUI_Input_Int);
    Assign("GUI_Input_Float", GUI_Input_Float);
    Assign("GUI_Int_Value", GUI_Int_Value);
    Assign("GUI_Int_Changed", GUI_Int_Changed);
    Assign("Set_GUI_Slider_Int_Function", Set_GUI_Slider_Int_Function);
    Assign("GUI_Checkbox", GUI_Checkbox);
    Assign("GUI_Checkbox_Value", GUI_Checkbox_Value);
    Assign("GUI_Checkbox_Changed", GUI_Checkbox_Changed);
    Assign("Set_GUI_Checkbox_Function", Set_GUI_Checkbox_Function);
    Assign("GUI_Radio_Button", GUI_Radio_Button);
    Assign("GUI_Combo", GUI_Combo);
    Assign("GUI_List_Box", GUI_List_Box);
    Assign("GUI_File_Picker", GUI_File_Picker);
    Assign("GUI_File_Picker_Select", GUI_File_Picker_Select);
    Assign("GUI_File_Picker_Set_Start", GUI_File_Picker_Set_Start);
    Assign("GUI_Selectable", GUI_Selectable);
    Assign("GUI_Selection_Value", GUI_Selection_Value);
    Assign("GUI_Selection_Text", GUI_Selection_Text);
    Assign("GUI_Selection_Changed", GUI_Selection_Changed);
    Assign("GUI_File_Picker_Current", GUI_File_Picker_Current);
    Assign("GUI_File_Picker_Selected", GUI_File_Picker_Selected);
    Assign("GUI_File_Picker_Selected_Path", GUI_File_Picker_Selected_Path);
    Assign("GUI_File_Picker_Selected_Relative", GUI_File_Picker_Selected_Relative);
    Assign("GUI_File_Picker_Selected_Is_Directory", GUI_File_Picker_Selected_Is_Directory);
    Assign("GUI_File_Picker_Changed", GUI_File_Picker_Changed);
    Assign("GUI_Progress_Bar", GUI_Progress_Bar);
    Assign("GUI_Tooltip", GUI_Tooltip);
    Assign("GUI_Tree_Node", GUI_Tree_Node);
    Assign("GUI_Tree_End", GUI_Tree_End);
    Assign("GUI_Tab_Bar_Begin", GUI_Tab_Bar_Begin);
    Assign("GUI_Tab_Bar_End", GUI_Tab_Bar_End);
    Assign("GUI_Tab_Item_Begin", GUI_Tab_Item_Begin);
    Assign("GUI_Tab_Item_End", GUI_Tab_Item_End);
    Assign("GUI_Menu_Bar_Begin", GUI_Menu_Bar_Begin);
    Assign("GUI_Menu_Bar_End", GUI_Menu_Bar_End);
    Assign("GUI_Menu_Begin", GUI_Menu_Begin);
    Assign("GUI_Menu_End", GUI_Menu_End);
    Assign("GUI_Menu_Item", GUI_Menu_Item);
    Assign("Set_GUI_Function", Set_GUI_Function);
    Assign("Set_GUI_Visible", Set_GUI_Visible);
    Assign("Set_GUI_Docking", Set_GUI_Docking);
    Assign("Toggle_GUI", Toggle_GUI);
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
    if(!Plugin_Loaded)
    {
        Globals = nullptr;
        return;
    }

    Plugin_Loaded = false;

    if(Close_All != nullptr)
    {
        Close_All();
    }
    Module.Unload();
    Globals = nullptr;

    Debug = nullptr;
    Set_Paths = nullptr;
    Add_Window = nullptr;
    Add_Full_Screen_Window = nullptr;
    Delete_Window = nullptr;
    Close_All = nullptr;
    Show = nullptr;
    Hide = nullptr;
    Set_Focus = nullptr;
    Set_Monitor = nullptr;
    Set_FPS = nullptr;
    Set_Window_Icon = nullptr;
    Set_View_Port = nullptr;
    Set_Perspective = nullptr;
    Set_Ortho = nullptr;
    Set_Camera_Location = nullptr;
    Set_Camera_Orientation = nullptr;
    Move_Camera = nullptr;
    Rotate_Camera = nullptr;
    Update_Camera = nullptr;
    Load_Shader = nullptr;
    Load_Texture = nullptr;
    Load_Font = nullptr;
    Load_Mesh = nullptr;
    Load_Asset = nullptr;
    Free_Texture = nullptr;
    Free_Asset = nullptr;
    Set_Texture = nullptr;
    Set_Shader = nullptr;
    Set_Location = nullptr;
    Set_Orientation = nullptr;
    Set_Scale = nullptr;
    Move_Asset = nullptr;
    Rotate_Asset = nullptr;
    Set_Active_Asset = nullptr;
    Rotate_Active_Yaw_Input = nullptr;
    Move_Active_Forward_Input = nullptr;
    Draw_Text = nullptr;
    Draw_Rect = nullptr;
    GUI_Begin = nullptr;
    GUI_Begin_Fullscreen = nullptr;
    GUI_End_Window = nullptr;
    GUI_Text = nullptr;
    GUI_Label_Text = nullptr;
    GUI_Bullet_Text = nullptr;
    GUI_Separator = nullptr;
    GUI_Separator_Text = nullptr;
    GUI_Same_Line = nullptr;
    GUI_New_Line = nullptr;
    GUI_Spacing = nullptr;
    GUI_Indent = nullptr;
    GUI_Unindent = nullptr;
    GUI_Group_Begin = nullptr;
    GUI_Group_End = nullptr;
    GUI_Button = nullptr;
    GUI_New_Game_Project_Button = nullptr;
    GUI_New_Level_Project_Button = nullptr;
    GUI_Small_Button = nullptr;
    GUI_Button_Clicked = nullptr;
    Set_GUI_Button_Function = nullptr;
    GUI_Slider_Int = nullptr;
    GUI_Slider_Int_Value = nullptr;
    GUI_Slider_Int_Changed = nullptr;
    GUI_Slider_Float = nullptr;
    GUI_Float_Value = nullptr;
    GUI_Float_Changed = nullptr;
    GUI_Drag_Int = nullptr;
    GUI_Drag_Float = nullptr;
    GUI_Input_Text = nullptr;
    GUI_Input_Text_Multiline = nullptr;
    GUI_Set_Text_Value = nullptr;
    GUI_Text_Value = nullptr;
    GUI_Text_Changed = nullptr;
    GUI_Input_Int = nullptr;
    GUI_Input_Float = nullptr;
    GUI_Int_Value = nullptr;
    GUI_Int_Changed = nullptr;
    Set_GUI_Slider_Int_Function = nullptr;
    GUI_Checkbox = nullptr;
    GUI_Checkbox_Value = nullptr;
    GUI_Checkbox_Changed = nullptr;
    Set_GUI_Checkbox_Function = nullptr;
    GUI_Radio_Button = nullptr;
    GUI_Combo = nullptr;
    GUI_List_Box = nullptr;
    GUI_File_Picker = nullptr;
    GUI_File_Picker_Select = nullptr;
    GUI_File_Picker_Set_Start = nullptr;
    GUI_Selectable = nullptr;
    GUI_Selection_Value = nullptr;
    GUI_Selection_Text = nullptr;
    GUI_Selection_Changed = nullptr;
    GUI_File_Picker_Current = nullptr;
    GUI_File_Picker_Selected = nullptr;
    GUI_File_Picker_Selected_Path = nullptr;
    GUI_File_Picker_Selected_Relative = nullptr;
    GUI_File_Picker_Selected_Is_Directory = nullptr;
    GUI_File_Picker_Changed = nullptr;
    GUI_Progress_Bar = nullptr;
    GUI_Tooltip = nullptr;
    GUI_Tree_Node = nullptr;
    GUI_Tree_End = nullptr;
    GUI_Tab_Bar_Begin = nullptr;
    GUI_Tab_Bar_End = nullptr;
    GUI_Tab_Item_Begin = nullptr;
    GUI_Tab_Item_End = nullptr;
    GUI_Menu_Bar_Begin = nullptr;
    GUI_Menu_Bar_End = nullptr;
    GUI_Menu_Begin = nullptr;
    GUI_Menu_End = nullptr;
    GUI_Menu_Item = nullptr;
    Set_GUI_Function = nullptr;
    Set_GUI_Visible = nullptr;
    Set_GUI_Docking = nullptr;
    Toggle_GUI = nullptr;
    GUI_End = nullptr;
    Render = nullptr;
    Update = nullptr;
    Wait_Until_Close = nullptr;
    Window_Open = nullptr;
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
        std::string Value;
        *Data >> Value;
        if(Value == "1" || Value == "On" || Value == "True")
        {
            Module_Debug = true;
        }
        else if(Value == "0" || Value == "Off" || Value == "False")
        {
            Module_Debug = false;
        }
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
    else if(Command == "Set_Window_Icon")
    {
        std::string Name, File;
        *Data >> Name;
        *Data >> File;
        Set_Window_Icon(Name, File);
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
    else if(Command == "Free_Texture")
    {
        std::string Name;
        *Data >> Name;
        Free_Texture(Name);
    }
    else if(Command == "Free_Asset")
    {
        std::string Name;
        *Data >> Name;
        Free_Asset(Name);
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
    else if(Command == "Draw_Rect")
    {
        std::string Name;
        float X, Y, W, H;
        int R, G, B, A;
        *Data >> Name;
        *Data >> X;
        *Data >> Y;
        *Data >> W;
        *Data >> H;
        *Data >> R;
        *Data >> G;
        *Data >> B;
        *Data >> A;
        Draw_Rect(Name, X, Y, W, H, R, G, B, A);
    }
    else if(Command == "GUI_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Begin(Name);
    }
    else if(Command == "GUI_Begin_Fullscreen")
    {
        std::string Name;
        *Data >> Name;
        if(GUI_Begin_Fullscreen != nullptr)
        {
            GUI_Begin_Fullscreen(Name);
        }
    }
    else if(Command == "GUI_End_Window")
    {
        GUI_End_Window();
    }
    else if(Command == "GUI_Text")
    {
        std::string Name, Text;
        *Data >> Name;
        *Data >> Text;
        GUI_Text(Name, Text);
    }
    else if(Command == "GUI_Label_Text")
    {
        std::string Name, Text;
        *Data >> Name;
        *Data >> Text;
        GUI_Label_Text(Name, Text);
    }
    else if(Command == "GUI_Bullet_Text")
    {
        std::string Name, Text;
        *Data >> Name;
        *Data >> Text;
        GUI_Bullet_Text(Name, Text);
    }
    else if(Command == "GUI_Separator")
    {
        GUI_Separator();
    }
    else if(Command == "GUI_Separator_Text")
    {
        std::string Text;
        *Data >> Text;
        GUI_Separator_Text(Text);
    }
    else if(Command == "GUI_Same_Line")
    {
        GUI_Same_Line();
    }
    else if(Command == "GUI_New_Line")
    {
        GUI_New_Line();
    }
    else if(Command == "GUI_Spacing")
    {
        GUI_Spacing();
    }
    else if(Command == "GUI_Indent")
    {
        int Pixels;
        *Data >> Pixels;
        GUI_Indent(Pixels);
    }
    else if(Command == "GUI_Unindent")
    {
        int Pixels;
        *Data >> Pixels;
        GUI_Unindent(Pixels);
    }
    else if(Command == "GUI_Group_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Group_Begin(Name);
    }
    else if(Command == "GUI_Group_End")
    {
        GUI_Group_End();
    }
    else if(Command == "GUI_Button")
    {
        std::string Name, Text, Function;
        *Data >> Name;
        *Data >> Text;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Button(Name, Text);
    }
    else if(Command == "GUI_New_Game_Project_Button")
    {
        std::string Name, Text, Name_Input, Picker;
        *Data >> Name;
        *Data >> Text;
        *Data >> Name_Input;
        *Data >> Picker;
        if(GUI_New_Game_Project_Button != nullptr)
        {
            GUI_New_Game_Project_Button(Name, Text, Name_Input, Picker);
        }
    }
    else if(Command == "GUI_New_Level_Project_Button")
    {
        std::string Name, Text, Game_Input, Level_Input, Picker;
        *Data >> Name;
        *Data >> Text;
        *Data >> Game_Input;
        *Data >> Level_Input;
        *Data >> Picker;
        if(GUI_New_Level_Project_Button != nullptr)
        {
            GUI_New_Level_Project_Button(Name, Text, Game_Input, Level_Input, Picker);
        }
    }
    else if(Command == "GUI_Small_Button")
    {
        std::string Name, Text, Function;
        *Data >> Name;
        *Data >> Text;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Small_Button(Name, Text);
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
        std::string Name, Text, Function;
        int Value, Min, Max;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Min;
        *Data >> Max;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Slider_Int(Name, Text, Value, Min, Max);
        Update_GUI_Slider_Global(Name);
    }
    else if(Command == "GUI_Slider_Float")
    {
        std::string Name, Text, Function;
        float Value, Min, Max;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Min;
        *Data >> Max;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Slider_Float(Name, Text, Value, Min, Max);
        Update_GUI_Float_Global(Name);
    }
    else if(Command == "GUI_Float_Value")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Value", "float", std::to_string(GUI_Float_Value(Name)));
    }
    else if(Command == "GUI_Float_Changed")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Float_Changed(Name) ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "float", std::to_string(GUI_Float_Value(Name)));
    }
    else if(Command == "GUI_Drag_Int")
    {
        std::string Name, Text, Function;
        int Value, Min, Max, Speed;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Min;
        *Data >> Max;
        *Data >> Speed;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Drag_Int(Name, Text, Value, Min, Max, Speed);
        Update_GUI_Int_Global(Name);
    }
    else if(Command == "GUI_Drag_Float")
    {
        std::string Name, Text, Function;
        float Value, Min, Max, Speed;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Min;
        *Data >> Max;
        *Data >> Speed;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Drag_Float(Name, Text, Value, Min, Max, Speed);
        Update_GUI_Float_Global(Name);
    }
    else if(Command == "GUI_Input_Text")
    {
        std::string Name, Text, Value, Function;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Input_Text(Name, Text, Value);
        Update_GUI_Text_Global(Name);
    }
    else if(Command == "GUI_Input_Text_Multiline")
    {
        std::string Name, Text, Value, Function;
        int Width, Height;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Width;
        *Data >> Height;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        if(GUI_Input_Text_Multiline != nullptr)
        {
            GUI_Input_Text_Multiline(Name, Text, Value, Width, Height);
        }
        Update_GUI_Text_Global(Name);
    }
    else if(Command == "GUI_Set_Text_Value")
    {
        std::string Name, Value;
        *Data >> Name;
        *Data >> Value;
        if(GUI_Set_Text_Value != nullptr)
        {
            GUI_Set_Text_Value(Name, Value);
        }
        Update_GUI_Text_Global(Name);
    }
    else if(Command == "GUI_Text_Value")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Value", "string", GUI_Text_Value(Name));
    }
    else if(Command == "GUI_Text_Changed")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Text_Changed(Name) ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "string", GUI_Text_Value(Name));
    }
    else if(Command == "GUI_Input_Int")
    {
        std::string Name, Text, Function;
        int Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Input_Int(Name, Text, Value);
        Update_GUI_Int_Global(Name);
    }
    else if(Command == "GUI_Input_Float")
    {
        std::string Name, Text, Function;
        float Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Input_Float(Name, Text, Value);
        Update_GUI_Float_Global(Name);
    }
    else if(Command == "GUI_Int_Value")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Int_Value(Name)));
    }
    else if(Command == "GUI_Int_Changed")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Int_Changed(Name) ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Int_Value(Name)));
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
        std::string Name, Text, Function;
        int Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
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
    else if(Command == "GUI_Radio_Button")
    {
        std::string Name, Text, Function;
        int Option;
        *Data >> Name;
        *Data >> Text;
        *Data >> Option;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Radio_Button(Name, Text, Option);
        Update_GUI_Selection_Global(Name);
    }
    else if(Command == "GUI_Combo")
    {
        std::string Name, Text, Items, Function;
        int Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Items;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Combo(Name, Text, Items, Value);
        Update_GUI_Selection_Global(Name);
    }
    else if(Command == "GUI_List_Box")
    {
        std::string Name, Text, Items, Function;
        int Value;
        *Data >> Name;
        *Data >> Text;
        *Data >> Items;
        *Data >> Value;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_List_Box(Name, Text, Items, Value);
        Update_GUI_Selection_Global(Name);
    }
    else if(Command == "GUI_File_Picker")
    {
        std::string Name, Text, Root, Start, Function;
        *Data >> Name;
        *Data >> Text;
        *Data >> Root;
        *Data >> Start;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        if(GUI_File_Picker != nullptr)
        {
            GUI_File_Picker(Name, Text, Root, Start);
        }
        Update_GUI_File_Picker_Global(Name);
    }
    else if(Command == "GUI_File_Picker_Select")
    {
        std::string Name, Path;
        *Data >> Name;
        *Data >> Path;
        if(GUI_File_Picker_Select != nullptr)
        {
            GUI_File_Picker_Select(Name, Path);
        }
        Update_GUI_File_Picker_Global(Name);
    }
    else if(Command == "GUI_File_Picker_Set_Start")
    {
        std::string Name, Start;
        *Data >> Name;
        *Data >> Start;
        if(GUI_File_Picker_Set_Start != nullptr)
        {
            GUI_File_Picker_Set_Start(Name, Start);
        }
        Update_GUI_File_Picker_Global(Name);
    }
    else if(Command == "GUI_Selectable")
    {
        std::string Name, Text, Function;
        int Selected;
        *Data >> Name;
        *Data >> Text;
        *Data >> Selected;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Selectable(Name, Text, Selected);
        Set_Global("GUI." + Name + ".Value", "bool", GUI_Checkbox_Value(Name) ? "1" : "0");
    }
    else if(Command == "GUI_Selection_Value")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Selection_Value(Name)));
        if(GUI_Selection_Text != nullptr)
        {
            Set_Global("GUI." + Name + ".Text", "string", GUI_Selection_Text(Name));
        }
    }
    else if(Command == "GUI_Selection_Text")
    {
        std::string Name;
        *Data >> Name;
        if(GUI_Selection_Text != nullptr)
        {
            Set_Global("GUI." + Name + ".Text", "string", GUI_Selection_Text(Name));
        }
    }
    else if(Command == "GUI_Selection_Changed")
    {
        std::string Name;
        *Data >> Name;
        Set_Global("GUI." + Name + ".Changed", "bool", GUI_Selection_Changed(Name) ? "1" : "0");
        Set_Global("GUI." + Name + ".Value", "int", std::to_string(GUI_Selection_Value(Name)));
        if(GUI_Selection_Text != nullptr)
        {
            Set_Global("GUI." + Name + ".Text", "string", GUI_Selection_Text(Name));
        }
    }
    else if(Command == "GUI_File_Picker_Value")
    {
        std::string Name;
        *Data >> Name;
        Update_GUI_File_Picker_Global(Name);
    }
    else if(Command == "GUI_Progress_Bar")
    {
        std::string Name, Text;
        float Fraction;
        *Data >> Name;
        *Data >> Fraction;
        *Data >> Text;
        GUI_Progress_Bar(Name, Fraction, Text);
    }
    else if(Command == "GUI_Tooltip")
    {
        std::string Text;
        *Data >> Text;
        GUI_Tooltip(Text);
    }
    else if(Command == "GUI_Tree_Node")
    {
        std::string Name;
        *Data >> Name;
        GUI_Tree_Node(Name);
    }
    else if(Command == "GUI_Tree_End")
    {
        GUI_Tree_End();
    }
    else if(Command == "GUI_Tab_Bar_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Tab_Bar_Begin(Name);
    }
    else if(Command == "GUI_Tab_Bar_End")
    {
        GUI_Tab_Bar_End();
    }
    else if(Command == "GUI_Tab_Item_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Tab_Item_Begin(Name);
    }
    else if(Command == "GUI_Tab_Item_End")
    {
        GUI_Tab_Item_End();
    }
    else if(Command == "GUI_Menu_Bar_Begin")
    {
        GUI_Menu_Bar_Begin();
    }
    else if(Command == "GUI_Menu_Bar_End")
    {
        GUI_Menu_Bar_End();
    }
    else if(Command == "GUI_Menu_Begin")
    {
        std::string Name;
        *Data >> Name;
        GUI_Menu_Begin(Name);
    }
    else if(Command == "GUI_Menu_End")
    {
        GUI_Menu_End();
    }
    else if(Command == "GUI_Menu_Item")
    {
        std::string Name, Text, Function;
        *Data >> Name;
        *Data >> Text;
        *Data >> Function;
        Set_GUI_Control_Function(Name, Function);
        GUI_Menu_Item(Name, Text);
    }
    else if(Command == "Set_GUI_Function")
    {
        std::string Name, Function;
        *Data >> Name;
        *Data >> Function;
        Set_GUI_Function(Name, Function);
    }
    else if(Command == "Set_GUI_Visible")
    {
        std::string Value;
        *Data >> Value;

        if(Set_GUI_Visible != nullptr)
        {
            if(Value == "1" || Value == "On" || Value == "True")
            {
                Set_GUI_Visible(true);
            }
            else if(Value == "0" || Value == "Off" || Value == "False")
            {
                Set_GUI_Visible(false);
            }
        }
    }
    else if(Command == "Set_GUI_Docking")
    {
        std::string Value;
        *Data >> Value;

        if(Set_GUI_Docking != nullptr)
        {
            if(Value == "1" || Value == "On" || Value == "True" || Value == "true")
            {
                Set_GUI_Docking(true);
            }
            else if(Value == "0" || Value == "Off" || Value == "False" || Value == "false")
            {
                Set_GUI_Docking(false);
            }
        }
    }
    else if(Command == "Toggle_GUI")
    {
        if(Toggle_GUI != nullptr)
        {
            Toggle_GUI();
        }
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
