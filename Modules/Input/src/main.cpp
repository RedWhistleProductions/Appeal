#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;

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

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init(std::string Name)
{
    Module.Load(Name);

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
        std::cout << Down(Action) << std::endl;
    }
    else if(Command == "Pressed")
    {
        std::string Action;
        *Data >> Action;
        std::cout << Pressed(Action) << std::endl;
    }
    else if(Command == "Released")
    {
        std::string Action;
        *Data >> Action;
        std::cout << Released(Action) << std::endl;
    }
    else if(Command == "Axis")
    {
        std::string Action;
        *Data >> Action;
        std::cout << Axis(Action) << std::endl;
    }
    else if(Command == "Mouse_X")
    {
        std::cout << Mouse_X() << std::endl;
    }
    else if(Command == "Mouse_Y")
    {
        std::cout << Mouse_Y() << std::endl;
    }
    else if(Command == "Mouse_Delta_X")
    {
        std::cout << Mouse_Delta_X() << std::endl;
    }
    else if(Command == "Mouse_Delta_Y")
    {
        std::cout << Mouse_Delta_Y() << std::endl;
    }
    else if(Command == "Mouse_Wheel")
    {
        std::cout << Mouse_Wheel() << std::endl;
    }
    else if(Command == "Mouse_Down")
    {
        std::string Button;
        *Data >> Button;
        std::cout << Mouse_Down(Button) << std::endl;
    }
    else if(Command == "Mouse_Pressed")
    {
        std::string Button;
        *Data >> Button;
        std::cout << Mouse_Pressed(Button) << std::endl;
    }
    else if(Command == "Mouse_Released")
    {
        std::string Button;
        *Data >> Button;
        std::cout << Mouse_Released(Button) << std::endl;
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
    else
    {
        std::cout << "\tError: " << Command << " not found in Input Dictionary" << std::endl;
    }
}
