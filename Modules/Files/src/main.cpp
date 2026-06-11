#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"

Plugin Module;
bool Module_Debug = false;
bool Plugin_Loaded = false;
Named_List<Module_Data> *Globals = nullptr;

void (*Debug)(bool Enable);
void (*Set_Paths)(std::string Resources);
std::string (*Scan)(std::string Name, std::string Path);
bool (*Open)(std::string Name, std::string File, std::string Mode);
void (*Close)(std::string Name);
std::string (*Read)(std::string Name);
bool (*Write)(std::string Name, std::string Value);
std::string (*Read_All)(std::string File);
bool (*Write_All)(std::string File, std::string Value);
std::string (*Join)(std::string Left, std::string Right);
std::string (*Parent)(std::string Path);
std::string (*Relative)(std::string Base, std::string Path);
std::string (*Add_Extension)(std::string Name, std::string Extension);
bool (*Is_Directory)(std::string Path);
bool (*Create_Directory)(std::string Path);
bool (*Create_Game_Project)(std::string Name);
bool (*Append_Line)(std::string File, std::string Value);

void Set_Global(std::string Name, std::string Type, std::string Value)
{
	if(Globals == nullptr)
	{
		return;
	}

	std::string Full_Name = "Files." + Name;
	Module_Data Data;
	Data.Module_Name = "Files";
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
	Assign("Scan", Scan);
	Assign("Open", Open);
	Assign("Close", Close);
	Assign("Read", Read);
	Assign("Write", Write);
	Assign("Read_All", Read_All);
	Assign("Write_All", Write_All);
	Assign("Join", Join);
	Assign("Parent", Parent);
	Assign("Relative", Relative);
	Assign("Add_Extension", Add_Extension);
	Assign("Is_Directory", Is_Directory);
	Assign("Create_Directory", Create_Directory);
	Assign("Create_Game_Project", Create_Game_Project);
	Assign("Append_Line", Append_Line);

	Set_Global("Last_Read", "string", "");
	Set_Global("Last_Scan", "string", "");
	Set_Global("Last_Error", "string", "");
}

extern "C" void Init(std::string Name)
{
	Init_Globals(Name, nullptr);
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
		if(Value == "1" || Value == "On" || Value == "on" || Value == "True" || Value == "true")
		{
			Module_Debug = true;
		}
		else if(Value == "0" || Value == "Off" || Value == "off" || Value == "False" || Value == "false")
		{
			Module_Debug = false;
		}
		if(Debug != nullptr)
		{
			Debug(Module_Debug);
		}
	}
	else if(Command == "Set_Paths")
	{
		std::string Resources;
		*Data >> Resources;
		Set_Paths(Resources);
	}
	else if(Command == "Scan")
	{
		std::string Name;
		std::string Path;
		*Data >> Name;
		*Data >> Path;
		if(Scan != nullptr)
		{
			std::string Items = Scan(Name, Path);
			Set_Global("Last_Scan", "string", Name);
			Set_Global(Name + ".Items", "string", Items);
		}
	}
	else if(Command == "Open")
	{
		std::string Name;
		std::string File;
		std::string Mode;
		*Data >> Name;
		*Data >> File;
		*Data >> Mode;
		if(Open != nullptr)
		{
			Set_Global("File." + Name + ".Open", "bool", Open(Name, File, Mode) ? "1" : "0");
		}
	}
	else if(Command == "Close")
	{
		std::string Name;
		*Data >> Name;
		Close(Name);
	}
	else if(Command == "Read" || Command == ">>")
	{
		std::string Name;
		*Data >> Name;
		if(Read != nullptr)
		{
			std::string Value = Read(Name);
			Set_Global("Last_Read", "string", Value);
			Set_Global("File." + Name + ".Last_Read", "string", Value);
		}
	}
	else if(Command == "Write" || Command == "<<")
	{
		std::string Name;
		std::string Value;
		*Data >> Name;
		*Data >> Value;
		if(Write != nullptr)
		{
			Set_Global("File." + Name + ".Last_Write", "bool", Write(Name, Value) ? "1" : "0");
		}
	}
	else if(Command == "Read_All")
	{
		std::string File;
		*Data >> File;
		if(Read_All != nullptr)
		{
			std::string Value = Read_All(File);
			Set_Global("Last_Read", "string", Value);
			Set_Global("File.Last_Read_All", "string", Value);
		}
	}
	else if(Command == "Write_All")
	{
		std::string File;
		std::string Value;
		*Data >> File;
		*Data >> Value;
		if(Write_All != nullptr)
		{
			Set_Global("File.Last_Write_All", "bool", Write_All(File, Value) ? "1" : "0");
		}
	}
	else if(Command == "Join")
	{
		std::string Left;
		std::string Right;
		*Data >> Left;
		*Data >> Right;
		if(Join != nullptr)
		{
			Set_Global("Last_Path", "string", Join(Left, Right));
		}
	}
	else if(Command == "Parent")
	{
		std::string Path;
		*Data >> Path;
		if(Parent != nullptr)
		{
			Set_Global("Last_Path", "string", Parent(Path));
		}
	}
	else if(Command == "Relative")
	{
		std::string Base;
		std::string Path;
		*Data >> Base;
		*Data >> Path;
		if(Relative != nullptr)
		{
			Set_Global("Last_Path", "string", Relative(Base, Path));
		}
	}
	else if(Command == "Add_Extension")
	{
		std::string Name;
		std::string Extension;
		*Data >> Name;
		*Data >> Extension;
		if(Add_Extension != nullptr)
		{
			Set_Global("Last_Path", "string", Add_Extension(Name, Extension));
		}
	}
	else if(Command == "Is_Directory")
	{
		std::string Path;
		*Data >> Path;
		if(Is_Directory != nullptr)
		{
			Set_Global("Last_Is_Directory", "bool", Is_Directory(Path) ? "1" : "0");
		}
	}
	else if(Command == "Create_Directory")
	{
		std::string Path;
		*Data >> Path;
		if(Create_Directory != nullptr)
		{
			Set_Global("Last_Create_Directory", "bool", Create_Directory(Path) ? "1" : "0");
		}
	}
	else if(Command == "Create_Game_Project")
	{
		std::string Name;
		*Data >> Name;
		if(Create_Game_Project != nullptr)
		{
			bool Created = Create_Game_Project(Name);
			Set_Global("Last_Create_Game_Project", "bool", Created ? "1" : "0");
			Set_Global("Last_Path", "string", "Appeal/Games/" + Name);
		}
	}
	else if(Command == "Append_Line")
	{
		std::string File;
		std::string Value;
		*Data >> File;
		*Data >> Value;
		if(Append_Line != nullptr)
		{
			Set_Global("File.Last_Append_Line", "bool", Append_Line(File, Value) ? "1" : "0");
		}
	}
	else
	{
		if(Module_Debug)
		{
			std::cout << "\tError: " << Command << " not found in Files Dictionary" << std::endl;
		}
	}
}
