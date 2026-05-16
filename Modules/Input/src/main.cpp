#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;
bool Module_Debug = true;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
void (*Add_Control_Set)(std::string Name);
void (*Set_Control_Set)(std::string Name);
void (*Update)(std::string Control_Set);

void (*Bind_Key)(std::string Action, std::string Key);
void (*Bind_Mouse)(std::string Action, std::string Button);
void (*Bind_Button)(std::string Action, std::string Button);
void (*Bind_Axis)(std::string Action, std::string Axis);
void (*Unbind)(std::string Action);

bool (*Down)(std::string Action);
bool (*Pressed)(std::string Action);
bool (*Released)(std::string Action);
float (*Axis)(std::string Action);

int (*Mouse_X)();
int (*Mouse_Y)();
int (*Mouse_Delta_X)();
int (*Mouse_Delta_Y)();
int (*Mouse_Wheel)();
bool (*Mouse_Down)(std::string Button);
bool (*Mouse_Pressed)(std::string Button);
bool (*Mouse_Released)(std::string Button);

void (*Set_Key_Down)(std::string Key, std::string Function);
void (*Set_Key_Up)(std::string Key, std::string Function);
void (*Set_Mouse_Down)(std::string Button, std::string Function);
void (*Set_Mouse_Up)(std::string Button, std::string Function);
void (*Set_Controller_Button_Down)(std::string Button, std::string Function);
void (*Set_Controller_Button_Up)(std::string Button, std::string Function);
void (*Set_Controller_Axis)(std::string Axis, std::string Function);
void (*Open_Controller)(int Controller);
void (*Close_Controller)(int Controller);
bool (*Controller_Down)(int Controller, std::string Button);
bool (*Controller_Pressed)(int Controller, std::string Button);
bool (*Controller_Released)(int Controller, std::string Button);
int (*Controller_Axis)(int Controller, std::string Axis);
void (*Set_Controller_Button_Down_For)(int Controller, std::string Button, std::string Function);
void (*Set_Controller_Button_Up_For)(int Controller, std::string Button, std::string Function);
void (*Set_Controller_Axis_For)(int Controller, std::string Axis, std::string Function);
void (*Set_Controller_Axis_Range)(int Controller, std::string Axis, int Min, int Max, std::string Function);

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init(std::string Name)
{
    Module.Load(Name);

    Assign("Debug", Debug);
    Assign("Set_Paths", Set_Paths);
    Assign("Add_Control_Set", Add_Control_Set);
    Assign("Set_Control_Set", Set_Control_Set);
    Assign("Update", Update);

    Assign("Bind_Key", Bind_Key);
    Assign("Bind_Mouse", Bind_Mouse);
    Assign("Bind_Button", Bind_Button);
    Assign("Bind_Axis", Bind_Axis);
    Assign("Unbind", Unbind);

    Assign("Down", Down);
    Assign("Pressed", Pressed);
    Assign("Released", Released);
    Assign("Axis", Axis);

    Assign("Mouse_X", Mouse_X);
    Assign("Mouse_Y", Mouse_Y);
    Assign("Mouse_Delta_X", Mouse_Delta_X);
    Assign("Mouse_Delta_Y", Mouse_Delta_Y);
    Assign("Mouse_Wheel", Mouse_Wheel);
    Assign("Mouse_Down", Mouse_Down);
    Assign("Mouse_Pressed", Mouse_Pressed);
    Assign("Mouse_Released", Mouse_Released);

    Assign("Set_Key_Down", Set_Key_Down);
    Assign("Set_Key_Up", Set_Key_Up);
    Assign("Set_Mouse_Down", Set_Mouse_Down);
    Assign("Set_Mouse_Up", Set_Mouse_Up);
    Assign("Set_Controller_Button_Down", Set_Controller_Button_Down);
    Assign("Set_Controller_Button_Up", Set_Controller_Button_Up);
    Assign("Set_Controller_Axis", Set_Controller_Axis);
    Assign("Open_Controller", Open_Controller);
    Assign("Close_Controller", Close_Controller);
    Assign("Controller_Down", Controller_Down);
    Assign("Controller_Pressed", Controller_Pressed);
    Assign("Controller_Released", Controller_Released);
    Assign("Controller_Axis", Controller_Axis);
    Assign("Set_Controller_Button_Down_For", Set_Controller_Button_Down_For);
    Assign("Set_Controller_Button_Up_For", Set_Controller_Button_Up_For);
    Assign("Set_Controller_Axis_For", Set_Controller_Axis_For);
    Assign("Set_Controller_Axis_Range", Set_Controller_Axis_Range);
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
    else if(Command == "Add_Control_Set")
    {
        std::string Name;
        *Data >> Name;
        Add_Control_Set(Name);
    }
    else if(Command == "Set_Control_Set")
    {
        std::string Name;
        *Data >> Name;
        Set_Control_Set(Name);
    }
    else if(Command == "Update")
    {
        std::string Control_Set;
        *Data >> Control_Set;
        Update(Control_Set);
    }
    else if(Command == "Bind_Key")
    {
        std::string Action, Key;
        *Data >> Action;
        *Data >> Key;
        Bind_Key(Action, Key);
    }
    else if(Command == "Bind_Mouse")
    {
        std::string Action, Button;
        *Data >> Action;
        *Data >> Button;
        Bind_Mouse(Action, Button);
    }
    else if(Command == "Bind_Button")
    {
        std::string Action, Button;
        *Data >> Action;
        *Data >> Button;
        Bind_Button(Action, Button);
    }
    else if(Command == "Bind_Axis")
    {
        std::string Action, Axis_Name;
        *Data >> Action;
        *Data >> Axis_Name;
        Bind_Axis(Action, Axis_Name);
    }
    else if(Command == "Unbind")
    {
        std::string Action;
        *Data >> Action;
        Unbind(Action);
    }
    else if(Command == "Down")
    {
        std::string Action;
        *Data >> Action;
        if(Module_Debug) std::cout << Down(Action) << std::endl;
    }
    else if(Command == "Pressed")
    {
        std::string Action;
        *Data >> Action;
        if(Module_Debug) std::cout << Pressed(Action) << std::endl;
    }
    else if(Command == "Released")
    {
        std::string Action;
        *Data >> Action;
        if(Module_Debug) std::cout << Released(Action) << std::endl;
    }
    else if(Command == "Axis")
    {
        std::string Action;
        *Data >> Action;
        if(Module_Debug) std::cout << Axis(Action) << std::endl;
    }
    else if(Command == "Mouse_X")
    {
        if(Module_Debug) std::cout << Mouse_X() << std::endl;
    }
    else if(Command == "Mouse_Y")
    {
        if(Module_Debug) std::cout << Mouse_Y() << std::endl;
    }
    else if(Command == "Mouse_Delta_X")
    {
        if(Module_Debug) std::cout << Mouse_Delta_X() << std::endl;
    }
    else if(Command == "Mouse_Delta_Y")
    {
        if(Module_Debug) std::cout << Mouse_Delta_Y() << std::endl;
    }
    else if(Command == "Mouse_Wheel")
    {
        if(Module_Debug) std::cout << Mouse_Wheel() << std::endl;
    }
    else if(Command == "Mouse_Down")
    {
        std::string Button;
        *Data >> Button;
        if(Module_Debug) std::cout << Mouse_Down(Button) << std::endl;
    }
    else if(Command == "Mouse_Pressed")
    {
        std::string Button;
        *Data >> Button;
        if(Module_Debug) std::cout << Mouse_Pressed(Button) << std::endl;
    }
    else if(Command == "Mouse_Released")
    {
        std::string Button;
        *Data >> Button;
        if(Module_Debug) std::cout << Mouse_Released(Button) << std::endl;
    }
    else if(Command == "Set_Key_Down")
    {
        std::string Key, Function;
        *Data >> Key;
        *Data >> Function;
        Set_Key_Down(Key, Function);
    }
    else if(Command == "Set_Key_Up")
    {
        std::string Key, Function;
        *Data >> Key;
        *Data >> Function;
        Set_Key_Up(Key, Function);
    }
    else if(Command == "Set_Mouse_Down")
    {
        std::string Button, Function;
        *Data >> Button;
        *Data >> Function;
        Set_Mouse_Down(Button, Function);
    }
    else if(Command == "Set_Mouse_Up")
    {
        std::string Button, Function;
        *Data >> Button;
        *Data >> Function;
        Set_Mouse_Up(Button, Function);
    }
    else if(Command == "Set_Controller_Button_Down")
    {
        std::string Button, Function;
        *Data >> Button;
        *Data >> Function;
        Set_Controller_Button_Down(Button, Function);
    }
    else if(Command == "Set_Controller_Button_Up")
    {
        std::string Button, Function;
        *Data >> Button;
        *Data >> Function;
        Set_Controller_Button_Up(Button, Function);
    }
    else if(Command == "Set_Controller_Axis")
    {
        std::string Axis_Name, Function;
        *Data >> Axis_Name;
        *Data >> Function;
        Set_Controller_Axis(Axis_Name, Function);
    }
    else if(Command == "Open_Controller")
    {
        int Controller;
        *Data >> Controller;
        Open_Controller(Controller);
    }
    else if(Command == "Close_Controller")
    {
        int Controller;
        *Data >> Controller;
        Close_Controller(Controller);
    }
    else if(Command == "Controller_Down")
    {
        int Controller;
        std::string Button;
        *Data >> Controller;
        *Data >> Button;
        if(Module_Debug) std::cout << Controller_Down(Controller, Button) << std::endl;
    }
    else if(Command == "Controller_Pressed")
    {
        int Controller;
        std::string Button;
        *Data >> Controller;
        *Data >> Button;
        if(Module_Debug) std::cout << Controller_Pressed(Controller, Button) << std::endl;
    }
    else if(Command == "Controller_Released")
    {
        int Controller;
        std::string Button;
        *Data >> Controller;
        *Data >> Button;
        if(Module_Debug) std::cout << Controller_Released(Controller, Button) << std::endl;
    }
    else if(Command == "Controller_Axis")
    {
        int Controller;
        std::string Axis_Name;
        *Data >> Controller;
        *Data >> Axis_Name;
        if(Module_Debug) std::cout << Controller_Axis(Controller, Axis_Name) << std::endl;
    }
    else if(Command == "Set_Controller_Button_Down_For")
    {
        int Controller;
        std::string Button, Function;
        *Data >> Controller;
        *Data >> Button;
        *Data >> Function;
        Set_Controller_Button_Down_For(Controller, Button, Function);
    }
    else if(Command == "Set_Controller_Button_Up_For")
    {
        int Controller;
        std::string Button, Function;
        *Data >> Controller;
        *Data >> Button;
        *Data >> Function;
        Set_Controller_Button_Up_For(Controller, Button, Function);
    }
    else if(Command == "Set_Controller_Axis_For")
    {
        int Controller;
        std::string Axis_Name, Function;
        *Data >> Controller;
        *Data >> Axis_Name;
        *Data >> Function;
        Set_Controller_Axis_For(Controller, Axis_Name, Function);
    }
    else if(Command == "Set_Controller_Axis_Range")
    {
        int Controller, Min, Max;
        std::string Axis_Name, Function;
        *Data >> Controller;
        *Data >> Axis_Name;
        *Data >> Min;
        *Data >> Max;
        *Data >> Function;
        Set_Controller_Axis_Range(Controller, Axis_Name, Min, Max, Function);
    }
    else
    {
        if(Module_Debug) std::cout << "\tError: " << Command << " not found in Input Dictionary" << std::endl;
    }
}
