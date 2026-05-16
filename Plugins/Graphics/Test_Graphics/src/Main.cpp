#include <iostream>
extern "C" void Debug(bool Enable) { (void)Enable; }

#include <string>

extern "C" void Constructor()
{
    std::cout << "Test_Graphics Constructor()" << std::endl;
}

extern "C" void Destructor()
{
    std::cout << "Test_Graphics Destructor()" << std::endl;
}

extern "C" void Set_Paths(std::string Resources)
{
    std::cout << "Set_Paths(" << Resources << ")" << std::endl;
}

extern "C" void Add_Window(std::string Name, std::string Title, int Width, int Height)
{
    std::cout << "Add_Window(" << Name << ", " << Title << ", " << Width << ", " << Height << ")" << std::endl;
}

extern "C" void Add_Full_Screen_Window(std::string Name, int Monitor)
{
    std::cout << "Add_Full_Screen_Window(" << Name << ", " << Monitor << ")" << std::endl;
}

extern "C" void Delete_Window(std::string Name)
{
    std::cout << "Delete_Window(" << Name << ")" << std::endl;
}

extern "C" void Close_All()
{
    std::cout << "Close_All()" << std::endl;
}

extern "C" void Show(std::string Name)
{
    std::cout << "Show(" << Name << ")" << std::endl;
}

extern "C" void Hide(std::string Name)
{
    std::cout << "Hide(" << Name << ")" << std::endl;
}

extern "C" void Set_Focus(std::string Name)
{
    std::cout << "Set_Focus(" << Name << ")" << std::endl;
}

extern "C" void Set_Monitor(std::string Name, int Monitor)
{
    std::cout << "Set_Monitor(" << Name << ", " << Monitor << ")" << std::endl;
}

extern "C" void Set_FPS(int Frames)
{
    std::cout << "Set_FPS(" << Frames << ")" << std::endl;
}

extern "C" void Set_View_Port(std::string Window, int X, int Y, int W, int H)
{
    std::cout << "Set_View_Port(" << Window << ", " << X << ", " << Y << ", " << W << ", " << H << ")" << std::endl;
}

extern "C" void Set_Perspective(std::string Window, float Fovy, float Near, float Far)
{
    std::cout << "Set_Perspective(" << Window << ", " << Fovy << ", " << Near << ", " << Far << ")" << std::endl;
}

extern "C" void Set_Ortho(std::string Window, float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    std::cout << "Set_Ortho(" << Window << ", " << Left << ", " << Right << ", " << Bottom << ", " << Top << ", " << Near << ", " << Far << ")" << std::endl;
}

extern "C" void Set_Camera_Location(std::string Window, float X, float Y, float Z)
{
    std::cout << "Set_Camera_Location(" << Window << ", " << X << ", " << Y << ", " << Z << ")" << std::endl;
}

extern "C" void Set_Camera_Orientation(std::string Window, float Yaw, float Pitch, float Roll)
{
    std::cout << "Set_Camera_Orientation(" << Window << ", " << Yaw << ", " << Pitch << ", " << Roll << ")" << std::endl;
}

extern "C" void Move_Camera(std::string Window, float X, float Y, float Z)
{
    std::cout << "Move_Camera(" << Window << ", " << X << ", " << Y << ", " << Z << ")" << std::endl;
}

extern "C" void Rotate_Camera(std::string Window, float Yaw, float Pitch, float Roll)
{
    std::cout << "Rotate_Camera(" << Window << ", " << Yaw << ", " << Pitch << ", " << Roll << ")" << std::endl;
}

extern "C" void Update_Camera(std::string Window)
{
    std::cout << "Update_Camera(" << Window << ")" << std::endl;
}

extern "C" void Load_Shader(std::string Name, std::string File)
{
    std::cout << "Load_Shader(" << Name << ", " << File << ")" << std::endl;
}

extern "C" void Load_Texture(std::string Name, std::string File)
{
    std::cout << "Load_Texture(" << Name << ", " << File << ")" << std::endl;
}

extern "C" void Load_Font(std::string Name, std::string File, int Size)
{
    std::cout << "Load_Font(" << Name << ", " << File << ", " << Size << ")" << std::endl;
}

extern "C" void Load_Mesh(std::string Name, std::string File)
{
    std::cout << "Load_Mesh(" << Name << ", " << File << ")" << std::endl;
}

extern "C" void Load_Asset(std::string Name, std::string File)
{
    std::cout << "Load_Asset(" << Name << ", " << File << ")" << std::endl;
}

extern "C" void Set_Texture(std::string Name, std::string Texture)
{
    std::cout << "Set_Texture(" << Name << ", " << Texture << ")" << std::endl;
}

extern "C" void Set_Shader(std::string Name, std::string Shader)
{
    std::cout << "Set_Shader(" << Name << ", " << Shader << ")" << std::endl;
}

extern "C" void Set_Location(std::string Name, float X, float Y, float Z)
{
    std::cout << "Set_Location(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl;
}

extern "C" void Set_Orientation(std::string Name, float Yaw, float Pitch, float Roll)
{
    std::cout << "Set_Orientation(" << Name << ", " << Yaw << ", " << Pitch << ", " << Roll << ")" << std::endl;
}

extern "C" void Set_Scale(std::string Name, float X, float Y, float Z)
{
    std::cout << "Set_Scale(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl;
}

extern "C" void Move_Asset(std::string Name, float X, float Y, float Z)
{
    std::cout << "Move_Asset(" << Name << ", " << X << ", " << Y << ", " << Z << ")" << std::endl;
}

extern "C" void Rotate_Asset(std::string Name, float Yaw, float Pitch, float Roll)
{
    std::cout << "Rotate_Asset(" << Name << ", " << Yaw << ", " << Pitch << ", " << Roll << ")" << std::endl;
}

extern "C" void Draw_Text(std::string Font, std::string Text, float X, float Y)
{
    std::cout << "Draw_Text(" << Font << ", " << Text << ", " << X << ", " << Y << ")" << std::endl;
}

extern "C" void GUI_Begin(std::string Name)
{
    std::cout << "GUI_Begin(" << Name << ")" << std::endl;
}

extern "C" void GUI_Text(std::string Name, std::string Text)
{
    std::cout << "GUI_Text(" << Name << ", " << Text << ")" << std::endl;
}

extern "C" void GUI_Button(std::string Name, std::string Text)
{
    std::cout << "GUI_Button(" << Name << ", " << Text << ")" << std::endl;
}

extern "C" bool GUI_Button_Clicked(std::string Name)
{
    std::cout << "GUI_Button_Clicked(" << Name << ")" << std::endl;
    return false;
}

extern "C" void GUI_Slider_Int(std::string Name, std::string Text, int Value, int Min, int Max)
{
    std::cout << "GUI_Slider_Int(" << Name << ", " << Text << ", " << Value << ", " << Min << ", " << Max << ")" << std::endl;
}

extern "C" int GUI_Slider_Int_Value(std::string Name)
{
    std::cout << "GUI_Slider_Int_Value(" << Name << ")" << std::endl;
    return 0;
}

extern "C" bool GUI_Slider_Int_Changed(std::string Name)
{
    std::cout << "GUI_Slider_Int_Changed(" << Name << ")" << std::endl;
    return false;
}

extern "C" void GUI_End()
{
    std::cout << "GUI_End()" << std::endl;
}

extern "C" void Render()
{
    std::cout << "Render()" << std::endl;
}

extern "C" void Update()
{
    std::cout << "Update()" << std::endl;
}

extern "C" void Wait_Until_Close(std::string Window)
{
    std::cout << "Wait_Until_Close(" << Window << ")" << std::endl;
}

extern "C" bool Window_Open(std::string Window)
{
    std::cout << "Window_Open(" << Window << ")" << std::endl;
    return false;
}

