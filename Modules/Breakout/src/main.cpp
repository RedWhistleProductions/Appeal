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
void (*Left)();
void (*Right)();
void (*Stop)();
float (*Paddle_X_Value)();
float (*Paddle_Y_Value)();
float (*Paddle_W_Value)();
float (*Paddle_H_Value)();
float (*Ball_X_Value)();
float (*Ball_Y_Value)();
float (*Ball_Size_Value)();
int (*Score_Value)();
int (*Lives_Value)();
std::string (*Event_Value)();
void (*Clear_Event)();
int (*Brick_Alive)(int Index);
float (*Brick_X_Value)(int Index);
float (*Brick_Y_Value)(int Index);
float (*Brick_W_Value)();
float (*Brick_H_Value)();

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

void Set_Global(std::string Name, std::string Type, std::string Value)
{
    if(Globals == nullptr) return;

    std::string Full_Name = "Breakout." + Name;
    Module_Data Data;
    Data.Module_Name = "Breakout";
    Data.Name = Name;
    Data.Type = Type;
    Data.Value = Value;

    if(Globals->Find(Full_Name)) Globals->Current->Value = Data;
    else Globals->Add_Node(Full_Name, Data);
}

void Update_Globals()
{
    if(Paddle_X_Value != nullptr) Set_Global("Paddle.X", "float", std::to_string(Paddle_X_Value()));
    if(Paddle_Y_Value != nullptr) Set_Global("Paddle.Y", "float", std::to_string(Paddle_Y_Value()));
    if(Paddle_W_Value != nullptr) Set_Global("Paddle.W", "float", std::to_string(Paddle_W_Value()));
    if(Paddle_H_Value != nullptr) Set_Global("Paddle.H", "float", std::to_string(Paddle_H_Value()));
    if(Ball_X_Value != nullptr) Set_Global("Ball.X", "float", std::to_string(Ball_X_Value()));
    if(Ball_Y_Value != nullptr) Set_Global("Ball.Y", "float", std::to_string(Ball_Y_Value()));
    if(Ball_Size_Value != nullptr) Set_Global("Ball.Size", "float", std::to_string(Ball_Size_Value()));
    if(Score_Value != nullptr) Set_Global("Score", "int", std::to_string(Score_Value()));
    if(Lives_Value != nullptr) Set_Global("Lives", "int", std::to_string(Lives_Value()));
    if(Event_Value != nullptr) Set_Global("Event", "string", Event_Value());
    if(Brick_W_Value != nullptr) Set_Global("Brick.W", "float", std::to_string(Brick_W_Value()));
    if(Brick_H_Value != nullptr) Set_Global("Brick.H", "float", std::to_string(Brick_H_Value()));

    if(Brick_Alive != nullptr && Brick_X_Value != nullptr && Brick_Y_Value != nullptr)
    {
        for(int i = 0; i < 50; ++i)
        {
            Set_Global("Brick." + std::to_string(i) + ".Alive", "int", std::to_string(Brick_Alive(i)));
            Set_Global("Brick." + std::to_string(i) + ".X", "float", std::to_string(Brick_X_Value(i)));
            Set_Global("Brick." + std::to_string(i) + ".Y", "float", std::to_string(Brick_Y_Value(i)));
        }
    }
}

extern "C" void Init_Globals(std::string Name, Named_List<Module_Data> *Shared_Globals)
{
    Globals = Shared_Globals;
    Module.Load(Name);

    Assign("Debug", Debug);
    Assign("Reset", Reset);
    Assign("Set_Field", Set_Field);
    Assign("Update", Update);
    Assign("Left", Left);
    Assign("Right", Right);
    Assign("Stop", Stop);
    Assign("Paddle_X_Value", Paddle_X_Value);
    Assign("Paddle_Y_Value", Paddle_Y_Value);
    Assign("Paddle_W_Value", Paddle_W_Value);
    Assign("Paddle_H_Value", Paddle_H_Value);
    Assign("Ball_X_Value", Ball_X_Value);
    Assign("Ball_Y_Value", Ball_Y_Value);
    Assign("Ball_Size_Value", Ball_Size_Value);
    Assign("Score_Value", Score_Value);
    Assign("Lives_Value", Lives_Value);
    Assign("Event_Value", Event_Value);
    Assign("Clear_Event", Clear_Event);
    Assign("Brick_Alive", Brick_Alive);
    Assign("Brick_X_Value", Brick_X_Value);
    Assign("Brick_Y_Value", Brick_Y_Value);
    Assign("Brick_W_Value", Brick_W_Value);
    Assign("Brick_H_Value", Brick_H_Value);

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
    else if(Command == "Left")
    {
        if(Left != nullptr) Left();
    }
    else if(Command == "Right")
    {
        if(Right != nullptr) Right();
    }
    else if(Command == "Stop")
    {
        if(Stop != nullptr) Stop();
    }
    else if(Command == "Clear_Event")
    {
        if(Clear_Event != nullptr) Clear_Event();
        Update_Globals();
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Breakout Dictionary" << std::endl;
    }
}
