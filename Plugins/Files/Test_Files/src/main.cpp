#include <iostream>
#include <string>

extern "C" void Debug(bool Enable)
{
	(void)Enable;
}

extern "C" void Constructor()
{
	std::cout << "Test_Files Constructor()" << std::endl;
}

extern "C" void Destructor()
{
	std::cout << "Test_Files Destructor()" << std::endl;
}

extern "C" void Set_Paths(std::string Resources)
{
	std::cout << "Set_Paths(" << Resources << ")" << std::endl;
}

extern "C" std::string Scan(std::string Name, std::string Path)
{
	std::cout << "Scan(" << Name << ", " << Path << ")" << std::endl;
	return "Test_File.txt|Test_Folder/";
}

extern "C" bool Open(std::string Name, std::string File, std::string Mode)
{
	std::cout << "Open(" << Name << ", " << File << ", " << Mode << ")" << std::endl;
	return true;
}

extern "C" void Close(std::string Name)
{
	std::cout << "Close(" << Name << ")" << std::endl;
}

extern "C" std::string Read(std::string Name)
{
	std::cout << "Read(" << Name << ")" << std::endl;
	return "Test_Read_Value";
}

extern "C" bool Write(std::string Name, std::string Value)
{
	std::cout << "Write(" << Name << ", " << Value << ")" << std::endl;
	return true;
}

extern "C" std::string Read_All(std::string File)
{
	std::cout << "Read_All(" << File << ")" << std::endl;
	return "Test whole file text.";
}

extern "C" bool Write_All(std::string File, std::string Value)
{
	std::cout << "Write_All(" << File << ", " << Value << ")" << std::endl;
	return true;
}

extern "C" std::string Join(std::string Left, std::string Right)
{
	std::cout << "Join(" << Left << ", " << Right << ")" << std::endl;
	return Left + "/" + Right;
}

extern "C" std::string Parent(std::string Path)
{
	std::cout << "Parent(" << Path << ")" << std::endl;
	return Path;
}

extern "C" std::string Relative(std::string Base, std::string Path)
{
	std::cout << "Relative(" << Base << ", " << Path << ")" << std::endl;
	return Path;
}

extern "C" std::string Add_Extension(std::string Name, std::string Extension)
{
	std::cout << "Add_Extension(" << Name << ", " << Extension << ")" << std::endl;
	return Name + Extension;
}

extern "C" bool Is_Directory(std::string Path)
{
	std::cout << "Is_Directory(" << Path << ")" << std::endl;
	return false;
}

extern "C" bool Create_Directory(std::string Path)
{
	std::cout << "Create_Directory(" << Path << ")" << std::endl;
	return true;
}

extern "C" bool Create_Game_Project(std::string Name)
{
	std::cout << "Create_Game_Project(" << Name << ")" << std::endl;
	return true;
}

extern "C" bool Append_Line(std::string File, std::string Value)
{
	std::cout << "Append_Line(" << File << ", " << Value << ")" << std::endl;
	return true;
}
