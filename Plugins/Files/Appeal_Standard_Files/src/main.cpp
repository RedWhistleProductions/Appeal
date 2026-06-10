#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../../../Source/Named_List.h"

namespace fs = std::filesystem;

enum class File_Type
{
	Directory,
	Text,
	Appeal_Script,
	Image,
	Audio,
	Model,
	Archive,
	Executable,
	Unknown
};

struct File_Record
{
	std::string Name;
	std::string Path;
	std::string Extension;
	std::string Extension_Type;
	std::string Detected_Type;
	bool Is_Directory = false;
	bool Extension_Matches_Detected_Type = false;
};

struct File_Handle
{
	std::string Name;
	std::string File;
	std::string Mode;
	std::fstream Stream;
};

bool DEBUG = false;
std::string Resources_Path = "./Resources";
Named_List<File_Record> Last_Scan;
Named_List<File_Handle *> Handles;

extern "C" void Close(std::string Name);

std::string Normalize_Path(std::string Path)
{
	for(char &Character : Path)
	{
		if(Character == '\\')
		{
			Character = '/';
		}
	}
	return Path;
}

std::string Lower(std::string Text)
{
	std::transform(Text.begin(), Text.end(), Text.begin(), [](unsigned char Character)
	{
		return static_cast<char>(std::tolower(Character));
	});
	return Text;
}

std::string Type_To_String(File_Type Type)
{
	switch(Type)
	{
		case File_Type::Directory: return "Directory";
		case File_Type::Text: return "Text";
		case File_Type::Appeal_Script: return "Appeal_Script";
		case File_Type::Image: return "Image";
		case File_Type::Audio: return "Audio";
		case File_Type::Model: return "Model";
		case File_Type::Archive: return "Archive";
		case File_Type::Executable: return "Executable";
		default: return "Unknown";
	}
}

File_Type Type_From_Extension(const fs::path &Path)
{
	std::string Extension = Lower(Path.extension().string());
	if(Extension == ".appeal")
	{
		return File_Type::Appeal_Script;
	}
	if(Extension == ".txt" || Extension == ".ini" || Extension == ".cfg" || Extension == ".json" || Extension == ".html")
	{
		return File_Type::Text;
	}
	if(Extension == ".png" || Extension == ".jpg" || Extension == ".jpeg" || Extension == ".bmp" || Extension == ".gif")
	{
		return File_Type::Image;
	}
	if(Extension == ".mp3" || Extension == ".wav" || Extension == ".ogg")
	{
		return File_Type::Audio;
	}
	if(Extension == ".obj" || Extension == ".fbx" || Extension == ".dae" || Extension == ".gltf" || Extension == ".glb")
	{
		return File_Type::Model;
	}
	if(Extension == ".zip" || Extension == ".apkg" || Extension == ".7z")
	{
		return File_Type::Archive;
	}
	if(Extension == ".exe" || Extension == ".dll" || Extension == ".so")
	{
		return File_Type::Executable;
	}
	return File_Type::Unknown;
}

bool Starts_With(const std::vector<unsigned char> &Bytes, std::initializer_list<unsigned char> Pattern)
{
	if(Bytes.size() < Pattern.size())
	{
		return false;
	}

	size_t Index = 0;
	for(unsigned char Byte : Pattern)
	{
		if(Bytes[Index] != Byte)
		{
			return false;
		}
		Index++;
	}
	return true;
}

File_Type Detect_File_Type(const fs::path &Path)
{
	if(fs::is_directory(Path))
	{
		return File_Type::Directory;
	}

	std::ifstream Input(Path, std::ios::binary);
	if(!Input)
	{
		return File_Type::Unknown;
	}

	std::vector<unsigned char> Bytes(16, 0);
	Input.read(reinterpret_cast<char *>(Bytes.data()), static_cast<std::streamsize>(Bytes.size()));
	Bytes.resize(static_cast<size_t>(Input.gcount()));

	if(Starts_With(Bytes, {0x89, 'P', 'N', 'G'}))
	{
		return File_Type::Image;
	}
	if(Starts_With(Bytes, {0xff, 0xd8, 0xff}))
	{
		return File_Type::Image;
	}
	if(Starts_With(Bytes, {'G', 'I', 'F'}))
	{
		return File_Type::Image;
	}
	if(Starts_With(Bytes, {'R', 'I', 'F', 'F'}))
	{
		return File_Type::Audio;
	}
	if(Starts_With(Bytes, {'I', 'D', '3'}))
	{
		return File_Type::Audio;
	}
	if(Starts_With(Bytes, {'P', 'K'}))
	{
		return File_Type::Archive;
	}
	if(Starts_With(Bytes, {'M', 'Z'}) || Starts_With(Bytes, {0x7f, 'E', 'L', 'F'}))
	{
		return File_Type::Executable;
	}

	return Type_From_Extension(Path);
}

fs::path Resolve_Path(std::string Path)
{
	Path = Normalize_Path(Path);
	fs::path Direct(Path);
	if(Direct.is_absolute() || fs::exists(Direct))
	{
		return Direct;
	}

	fs::path Resource_Candidate = fs::path(Resources_Path) / Path;
	if(fs::exists(Resource_Candidate))
	{
		return Resource_Candidate;
	}

	return Resource_Candidate;
}

void Clear_Scan()
{
	while(Last_Scan.Nodes > 0)
	{
		Last_Scan.Remove();
	}
}

File_Handle *Find_Handle(std::string Name)
{
	if(Handles.Find(Name))
	{
		return Handles.Current->Value;
	}
	return nullptr;
}

void Close_Handle(File_Handle *Handle)
{
	if(Handle == nullptr)
	{
		return;
	}
	if(Handle->Stream.is_open())
	{
		Handle->Stream.close();
	}
	delete Handle;
}

extern "C" void Constructor()
{
}

extern "C" void Destructor()
{
	while(Handles.Nodes > 0)
	{
		Close_Handle(Handles.Current->Value);
		Handles.Remove();
	}
}

extern "C" void Debug(bool Enable)
{
	DEBUG = Enable;
}

extern "C" void Set_Paths(std::string Resources)
{
	Resources_Path = Normalize_Path(Resources);
	if(DEBUG)
	{
		std::cout << "Files resources path: " << Resources_Path << std::endl;
	}
}

extern "C" std::string Scan(std::string Name, std::string Path)
{
	(void)Name;
	Clear_Scan();

	fs::path Root = Resolve_Path(Path);
	if(!fs::exists(Root))
	{
		if(DEBUG)
		{
			std::cout << "Files scan path not found: " << Root.string() << std::endl;
		}
		return "";
	}

	std::stringstream Items;
	for(const auto &Entry : fs::directory_iterator(Root))
	{
		File_Record Record;
		Record.Name = Entry.path().filename().string();
		Record.Path = Normalize_Path(Entry.path().string());
		Record.Extension = Lower(Entry.path().extension().string());
		Record.Is_Directory = Entry.is_directory();
		Record.Extension_Type = Type_To_String(Record.Is_Directory ? File_Type::Directory : Type_From_Extension(Entry.path()));
		Record.Detected_Type = Type_To_String(Detect_File_Type(Entry.path()));
		Record.Extension_Matches_Detected_Type = Record.Extension_Type == Record.Detected_Type || Record.Extension_Type == "Unknown";

		Last_Scan.Add_Node(Record.Name, Record);
		Items << Record.Name;
		if(Record.Is_Directory)
		{
			Items << "/";
		}
		Items << "|";

		if(DEBUG)
		{
			std::cout << Record.Name << " [" << Record.Detected_Type << "]" << std::endl;
		}
	}

	return Items.str();
}

extern "C" bool Open(std::string Name, std::string File, std::string Mode)
{
	Close(Name);

	File_Handle *Handle = new File_Handle;
	Handle->Name = Name;
	Handle->File = Normalize_Path(Resolve_Path(File).string());
	Handle->Mode = Mode;

	std::string Lower_Mode = Lower(Mode);
	std::ios::openmode Open_Mode = std::ios::in;
	if(Lower_Mode == "write" || Lower_Mode == "output")
	{
		Open_Mode = std::ios::out | std::ios::trunc;
	}
	else if(Lower_Mode == "append")
	{
		Open_Mode = std::ios::out | std::ios::app;
	}
	else if(Lower_Mode == "readwrite" || Lower_Mode == "both")
	{
		Open_Mode = std::ios::in | std::ios::out;
	}

	if((Open_Mode & std::ios::out) != 0)
	{
		fs::path Parent = fs::path(Handle->File).parent_path();
		if(!Parent.empty())
		{
			fs::create_directories(Parent);
		}
	}

	Handle->Stream.open(Handle->File, Open_Mode);
	if(!Handle->Stream.is_open())
	{
		if(DEBUG)
		{
			std::cout << "Files open failed: " << Handle->File << std::endl;
		}
		delete Handle;
		return false;
	}

	Handles.Add_Node(Name, Handle);
	if(DEBUG)
	{
		std::cout << "Files opened: " << Name << " -> " << Handle->File << std::endl;
	}
	return true;
}

extern "C" void Close(std::string Name)
{
	if(Handles.Find(Name))
	{
		Close_Handle(Handles.Current->Value);
		Handles.Remove();
	}
}

extern "C" std::string Read(std::string Name)
{
	File_Handle *Handle = Find_Handle(Name);
	if(Handle == nullptr || !Handle->Stream.is_open())
	{
		return "";
	}

	std::string Value;
	Handle->Stream >> Value;
	if(DEBUG)
	{
		std::cout << "Files read: " << Name << " -> " << Value << std::endl;
	}
	return Value;
}

extern "C" bool Write(std::string Name, std::string Value)
{
	File_Handle *Handle = Find_Handle(Name);
	if(Handle == nullptr || !Handle->Stream.is_open())
	{
		return false;
	}

	Handle->Stream << Value << " ";
	Handle->Stream.flush();
	if(DEBUG)
	{
		std::cout << "Files write: " << Name << " <- " << Value << std::endl;
	}
	return true;
}

extern "C" std::string Read_All(std::string File)
{
	fs::path Path = Resolve_Path(File);
	std::ifstream Input(Path, std::ios::binary);
	if(!Input)
	{
		if(DEBUG)
		{
			std::cout << "Files read all failed: " << Path.string() << std::endl;
		}
		return "";
	}

	std::ostringstream Stream;
	Stream << Input.rdbuf();
	if(DEBUG)
	{
		std::cout << "Files read all: " << Path.string() << std::endl;
	}
	return Stream.str();
}

extern "C" bool Write_All(std::string File, std::string Value)
{
	fs::path Path = Resolve_Path(File);
	fs::path Parent = Path.parent_path();
	if(!Parent.empty())
	{
		fs::create_directories(Parent);
	}

	std::ofstream Output(Path, std::ios::binary | std::ios::trunc);
	if(!Output)
	{
		if(DEBUG)
		{
			std::cout << "Files write all failed: " << Path.string() << std::endl;
		}
		return false;
	}

	Output << Value;
	if(DEBUG)
	{
		std::cout << "Files write all: " << Path.string() << std::endl;
	}
	return true;
}

extern "C" std::string Join(std::string Left, std::string Right)
{
	Right = Normalize_Path(Right);
	if(!Right.empty() && Right.back() == '/')
	{
		Right.pop_back();
	}

	fs::path Result = fs::path(Normalize_Path(Left)) / Right;
	return Normalize_Path(Result.string());
}

extern "C" std::string Parent(std::string Path)
{
	fs::path Parent_Path = fs::path(Normalize_Path(Path)).parent_path();
	std::string Result = Normalize_Path(Parent_Path.string());
	if(Result.empty())
	{
		return Path;
	}
	return Result;
}

extern "C" std::string Relative(std::string Base, std::string Path)
{
	fs::path Base_Path = Resolve_Path(Base);
	fs::path Target_Path = Resolve_Path(Path);

	try
	{
		return Normalize_Path(fs::relative(Target_Path, Base_Path).string());
	}
	catch(const std::exception &)
	{
		return Normalize_Path(Path);
	}
}

extern "C" std::string Add_Extension(std::string Name, std::string Extension)
{
	if(Extension.empty())
	{
		return Name;
	}
	if(Extension.front() != '.')
	{
		Extension = "." + Extension;
	}

	fs::path Path(Name);
	if(Lower(Path.extension().string()) == Lower(Extension))
	{
		return Normalize_Path(Path.string());
	}

	return Normalize_Path((Path.string() + Extension));
}

extern "C" bool Is_Directory(std::string Path)
{
	fs::path Resolved = Resolve_Path(Path);
	return fs::exists(Resolved) && fs::is_directory(Resolved);
}

extern "C" bool Create_Directory(std::string Path)
{
	fs::path Resolved = Resolve_Path(Path);
	try
	{
		if(fs::exists(Resolved))
		{
			return fs::is_directory(Resolved);
		}
		return fs::create_directories(Resolved);
	}
	catch(const std::exception &Error)
	{
		if(DEBUG)
		{
			std::cout << "Files create directory failed: " << Resolved.string() << std::endl
			          << Error.what() << std::endl;
		}
		return false;
	}
}

extern "C" bool Create_Game_Project(std::string Name)
{
	Name = Normalize_Path(Name);
	if(Name.empty() || Name == "." || Name == ".." || Name.find('/') != std::string::npos)
	{
		if(DEBUG)
		{
			std::cout << "Files create game project rejected name: " << Name << std::endl;
		}
		return false;
	}

	try
	{
		fs::path Game_Folder = Resolve_Path("Appeal/Games/" + Name);
		fs::path Levels_Folder = Game_Folder / "Levels";
		fs::path Game_Script = Game_Folder / (Name + ".Appeal");

		if(!fs::exists(Game_Folder))
		{
			fs::create_directories(Game_Folder);
		}
		if(!fs::is_directory(Game_Folder))
		{
			return false;
		}

		fs::create_directories(Levels_Folder);

		if(!fs::exists(Game_Script))
		{
			std::ofstream Output(Game_Script, std::ios::binary | std::ios::trunc);
			if(!Output)
			{
				return false;
			}
			Output << "Return\n";
		}

		if(DEBUG)
		{
			std::cout << "Files create game project: " << Game_Folder.string() << std::endl;
		}
		return true;
	}
	catch(const std::exception &Error)
	{
		if(DEBUG)
		{
			std::cout << "Files create game project failed: " << Name << std::endl
			          << Error.what() << std::endl;
		}
		return false;
	}
}

extern "C" bool Append_Line(std::string File, std::string Value)
{
	fs::path Path = Resolve_Path(File);
	fs::path Parent_Path = Path.parent_path();
	if(!Parent_Path.empty())
	{
		fs::create_directories(Parent_Path);
	}

	std::ofstream Output(Path, std::ios::binary | std::ios::app);
	if(!Output)
	{
		if(DEBUG)
		{
			std::cout << "Files append line failed: " << Path.string() << std::endl;
		}
		return false;
	}

	Output << Value << "\n";
	if(DEBUG)
	{
		std::cout << "Files append line: " << Path.string() << " <- " << Value << std::endl;
	}
	return true;
}
