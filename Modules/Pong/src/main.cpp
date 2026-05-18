#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"

#include <iostream>
#include <string>

Plugin Module;
bool Module_Debug = true;
Named_List<Module_Data> *Globals = nullptr;

void (*Debug)(bool Enable);
void (*Reset)();
void (*Set_Field)(float Width, float Height);
void (*Update)();
void (*Left_Up)();
void (*Left_Down)();
void (*Left_Stop)();
void (*Right_Up)();
void (*Right_Down)();
void (*Right_Stop)();
float (*Left_X_Value)();
float (*Left_Y_Value)();
float (*Right_X_Value)();
float (*Right_Y_Value)();
float (*Paddle_W_Value)();
float (*Paddle_H_Value)();
float (*Ball_X_Value)();
float (*Ball_Y_Value)();
float (*Ball_Size_Value)();
int (*Left_Score_Value)();
int (*Right_Score_Value)();

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

void Set_Global(std::string Name, std::string Type, std::string Value)
{
    if(Globals == nullptr) return;

    std::string Full_Name = "Pong." + Name;
    Module_Data Data;
    Data.Module_Name = "Pong";
    Data.Name = Name;
    Data.Type = Type;
    Data.Value = Value;

    if(Globals->Find(Full_Name)) Globals->Current->Value = Data;
    else Globals->Add_Node(Full_Name, Data);
}

void Update_Globals()
{
    if(Left_X_Value != nullptr) Set_Global("Left.X", "float", std::to_string(Left_X_Value()));
    if(Left_Y_Value != nullptr) Set_Global("Left.Y", "float", std::to_string(Left_Y_Value()));
    if(Right_X_Value != nullptr) Set_Global("Right.X", "float", std::to_string(Right_X_Value()));
    if(Right_Y_Value != nullptr) Set_Global("Right.Y", "float", std::to_string(Right_Y_Value()));
    if(Paddle_W_Value != nullptr) Set_Global("Paddle.W", "float", std::to_string(Paddle_W_Value()));
    if(Paddle_H_Value != nullptr) Set_Global("Paddle.H", "float", std::to_string(Paddle_H_Value()));
    if(Ball_X_Value != nullptr) Set_Global("Ball.X", "float", std::to_string(Ball_X_Value()));
    if(Ball_Y_Value != nullptr) Set_Global("Ball.Y", "float", std::to_string(Ball_Y_Value()));
    if(Ball_Size_Value != nullptr) Set_Global("Ball.Size", "float", std::to_string(Ball_Size_Value()));
    if(Left_Score_Value != nullptr) Set_Global("Score.Left", "int", std::to_string(Left_Score_Value()));
    if(Right_Score_Value != nullptr) Set_Global("Score.Right", "int", std::to_string(Right_Score_Value()));
}

extern "C" void Init_Globals(std::string Name, Named_List<Module_Data> *Shared_Globals)
{
    Globals = Shared_Globals;
    Module.Load(Name);

    Assign("Debug", Debug);
    Assign("Reset", Reset);
    Assign("Set_Field", Set_Field);
    Assign("Update", Update);
    Assign("Left_Up", Left_Up);
    Assign("Left_Down", Left_Down);
    Assign("Left_Stop", Left_Stop);
    Assign("Right_Up", Right_Up);
    Assign("Right_Down", Right_Down);
    Assign("Right_Stop", Right_Stop);
    Assign("Left_X_Value", Left_X_Value);
    Assign("Left_Y_Value", Left_Y_Value);
    Assign("Right_X_Value", Right_X_Value);
    Assign("Right_Y_Value", Right_Y_Value);
    Assign("Paddle_W_Value", Paddle_W_Value);
    Assign("Paddle_H_Value", Paddle_H_Value);
    Assign("Ball_X_Value", Ball_X_Value);
    Assign("Ball_Y_Value", Ball_Y_Value);
    Assign("Ball_Size_Value", Ball_Size_Value);
    Assign("Left_Score_Value", Left_Score_Value);
    Assign("Right_Score_Value", Right_Score_Value);

    if(Reset != nullptr) Reset();
    Update_Globals();
}

extern "C" void Init(std::string Name)
{
    Init_Globals(Name, nullptr);
}

extern "C" void Shutdown()
{
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
    else if(Command == "Reset")
    {
        if(Reset != nullptr) Reset();
        Update_Globals();
    }
    else if(Command == "Set_Field")
    {
        float Width, Height;
        *Data >> Width;
        *Data >> Height;
        if(Set_Field != nullptr) Set_Field(Width, Height);
        Update_Globals();
    }
    else if(Command == "Update")
    {
        if(Update != nullptr) Update();
        Update_Globals();
    }
    else if(Command == "Left_Up")
    {
        if(Left_Up != nullptr) Left_Up();
    }
    else if(Command == "Left_Down")
    {
        if(Left_Down != nullptr) Left_Down();
    }
    else if(Command == "Left_Stop")
    {
        if(Left_Stop != nullptr) Left_Stop();
    }
    else if(Command == "Right_Up")
    {
        if(Right_Up != nullptr) Right_Up();
    }
    else if(Command == "Right_Down")
    {
        if(Right_Down != nullptr) Right_Down();
    }
    else if(Command == "Right_Stop")
    {
        if(Right_Stop != nullptr) Right_Stop();
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Pong Dictionary" << std::endl;
    }
}
