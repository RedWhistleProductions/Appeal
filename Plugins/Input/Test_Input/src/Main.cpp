#include <iostream>
#include <string>

extern "C" void Constructor()
{
    std::cout << "Test_Input Constructor()" << std::endl;
}

extern "C" void Destructor()
{
    std::cout << "Test_Input Destructor()" << std::endl;
}

extern "C" void Set_Paths(std::string Resources)
{
    std::cout << "Set_Paths(" << Resources << ")" << std::endl;
}

extern "C" void Add_Control_Set(std::string Name)
{
    std::cout << "Add_Control_Set(" << Name << ")" << std::endl;
}

extern "C" void Set_Control_Set(std::string Name)
{
    std::cout << "Set_Control_Set(" << Name << ")" << std::endl;
}

extern "C" void Update(std::string Control_Set)
{
    std::cout << "Update(" << Control_Set << ")" << std::endl;
}

extern "C" void Bind_Key(std::string Action, std::string Key)
{
    std::cout << "Bind_Key(" << Action << ", " << Key << ")" << std::endl;
}

extern "C" void Bind_Mouse(std::string Action, std::string Button)
{
    std::cout << "Bind_Mouse(" << Action << ", " << Button << ")" << std::endl;
}

extern "C" void Bind_Button(std::string Action, std::string Button)
{
    std::cout << "Bind_Button(" << Action << ", " << Button << ")" << std::endl;
}

extern "C" void Bind_Axis(std::string Action, std::string Axis)
{
    std::cout << "Bind_Axis(" << Action << ", " << Axis << ")" << std::endl;
}

extern "C" void Unbind(std::string Action)
{
    std::cout << "Unbind(" << Action << ")" << std::endl;
}

extern "C" bool Down(std::string Action)
{
    std::cout << "Down(" << Action << ")" << std::endl;
    return false;
}

extern "C" bool Pressed(std::string Action)
{
    std::cout << "Pressed(" << Action << ")" << std::endl;
    return false;
}

extern "C" bool Released(std::string Action)
{
    std::cout << "Released(" << Action << ")" << std::endl;
    return false;
}

extern "C" float Axis(std::string Action)
{
    std::cout << "Axis(" << Action << ")" << std::endl;
    return 0.0f;
}

extern "C" int Mouse_X()
{
    std::cout << "Mouse_X()" << std::endl;
    return 0;
}

extern "C" int Mouse_Y()
{
    std::cout << "Mouse_Y()" << std::endl;
    return 0;
}

extern "C" int Mouse_Delta_X()
{
    std::cout << "Mouse_Delta_X()" << std::endl;
    return 0;
}

extern "C" int Mouse_Delta_Y()
{
    std::cout << "Mouse_Delta_Y()" << std::endl;
    return 0;
}

extern "C" int Mouse_Wheel()
{
    std::cout << "Mouse_Wheel()" << std::endl;
    return 0;
}

extern "C" bool Mouse_Down(std::string Button)
{
    std::cout << "Mouse_Down(" << Button << ")" << std::endl;
    return false;
}

extern "C" bool Mouse_Pressed(std::string Button)
{
    std::cout << "Mouse_Pressed(" << Button << ")" << std::endl;
    return false;
}

extern "C" bool Mouse_Released(std::string Button)
{
    std::cout << "Mouse_Released(" << Button << ")" << std::endl;
    return false;
}

extern "C" void Set_Key_Down(std::string Key, std::string Function)
{
    std::cout << "Set_Key_Down(" << Key << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Key_Up(std::string Key, std::string Function)
{
    std::cout << "Set_Key_Up(" << Key << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Mouse_Down(std::string Button, std::string Function)
{
    std::cout << "Set_Mouse_Down(" << Button << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Mouse_Up(std::string Button, std::string Function)
{
    std::cout << "Set_Mouse_Up(" << Button << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Controller_Button_Down(std::string Button, std::string Function)
{
    std::cout << "Set_Controller_Button_Down(" << Button << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Controller_Button_Up(std::string Button, std::string Function)
{
    std::cout << "Set_Controller_Button_Up(" << Button << ", " << Function << ")" << std::endl;
}

extern "C" void Set_Controller_Axis(std::string Axis, std::string Function)
{
    std::cout << "Set_Controller_Axis(" << Axis << ", " << Function << ")" << std::endl;
}
