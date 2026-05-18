#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool DEBUG = true;
std::string Resources_Path = "./Resources";
std::string Package_Path = "./Resources/Package";
std::string Repo_File = "./Resources/Package/Repos.txt";
std::string Installed_Path = "./Resources/Package/Installed";
std::string Cache_Path = "./Resources/Package/Cache";
std::string Current_Format = "directory";
int Compression_Level = 6;
std::map<std::string, std::string> Repos;

std::string Normalize_Path(std::string Path)
{
    for(char &Character : Path)
    {
        if(Character == '\\') Character = '/';
    }
    return Path;
}

void Ensure_Directories()
{
    fs::create_directories(Package_Path);
    fs::create_directories(Installed_Path);
    fs::create_directories(Cache_Path);
}

void Load_Repos()
{
    Repos.clear();
    std::ifstream Input(Repo_File);
    std::string Name;
    std::string URL;
    while(Input >> Name >> URL)
    {
        Repos[Name] = URL;
    }
}

void Save_Repos()
{
    Ensure_Directories();
    std::ofstream Output(Repo_File, std::ios::trunc);
    for(const auto &Repo : Repos)
    {
        Output << Repo.first << " " << Repo.second << "\n";
    }
}

fs::path Resolve_Package_Path(std::string Name)
{
    fs::path Direct(Name);
    if(fs::exists(Direct))
    {
        return Direct;
    }

    Load_Repos();
    for(const auto &Repo : Repos)
    {
        fs::path Repo_Path(Repo.second);
        fs::path Candidate = Repo_Path / (Name + ".apkg");
        if(fs::exists(Candidate))
        {
            return Candidate;
        }

        Candidate = Repo_Path / Name;
        if(fs::exists(Candidate))
        {
            return Candidate;
        }
    }

    fs::path Cache_Candidate = fs::path(Cache_Path) / (Name + ".apkg");
    if(fs::exists(Cache_Candidate))
    {
        return Cache_Candidate;
    }

    fs::path Installed_Candidate = fs::path(Installed_Path) / Name;
    if(fs::exists(Installed_Candidate))
    {
        return Installed_Candidate;
    }

    return {};
}

void Copy_Tree(const fs::path &Source, const fs::path &Target)
{
    if(!fs::exists(Source))
    {
        return;
    }

    if(fs::is_regular_file(Source))
    {
        fs::create_directories(Target.parent_path());
        fs::copy_file(Source, Target, fs::copy_options::overwrite_existing);
        return;
    }

    fs::create_directories(Target);
    for(const auto &Entry : fs::recursive_directory_iterator(Source))
    {
        fs::path Relative = fs::relative(Entry.path(), Source);
        fs::path Destination = Target / Relative;
        if(Entry.is_directory())
        {
            fs::create_directories(Destination);
        }
        else if(Entry.is_regular_file())
        {
            fs::create_directories(Destination.parent_path());
            fs::copy_file(Entry.path(), Destination, fs::copy_options::overwrite_existing);
        }
    }
}

fs::path Package_Files_Path(const fs::path &Package)
{
    fs::path Files = Package / "Files";
    if(fs::exists(Files))
    {
        return Files;
    }
    return Package;
}

void Print_Manifest(const fs::path &Package)
{
    fs::path Manifest = Package / "Manifest.Appeal";
    if(!fs::exists(Manifest))
    {
        if(DEBUG) std::cout << "Package manifest not found: " << Manifest.string() << std::endl;
        return;
    }

    std::ifstream Input(Manifest);
    std::string Line;
    while(std::getline(Input, Line))
    {
        std::cout << Line << std::endl;
    }
}

extern "C" void Constructor()
{
    Ensure_Directories();
    Load_Repos();
}

extern "C" void Destructor()
{
}

extern "C" void Debug(bool Enable)
{
    DEBUG = Enable;
}

extern "C" void Set_Paths(std::string Resources)
{
    Resources_Path = Normalize_Path(Resources);
    Package_Path = Resources_Path + "/Package";
    Repo_File = Package_Path + "/Repos.txt";
    Installed_Path = Package_Path + "/Installed";
    Cache_Path = Package_Path + "/Cache";
    Ensure_Directories();
    Load_Repos();
}

extern "C" void Add_Repo(std::string Name, std::string URL)
{
    Ensure_Directories();
    Repos[Name] = Normalize_Path(URL);
    Save_Repos();
    if(DEBUG) std::cout << "Package repo added: " << Name << " -> " << URL << std::endl;
}

extern "C" void Remove_Repo(std::string Name)
{
    Load_Repos();
    Repos.erase(Name);
    Save_Repos();
    if(DEBUG) std::cout << "Package repo removed: " << Name << std::endl;
}

extern "C" void Update_Repos()
{
    Load_Repos();
    if(DEBUG) std::cout << "Package repos updated: " << Repos.size() << std::endl;
}

extern "C" void Search(std::string Query)
{
    Load_Repos();
    for(const auto &Repo : Repos)
    {
        fs::path Repo_Path(Repo.second);
        if(!fs::exists(Repo_Path))
        {
            continue;
        }

        for(const auto &Entry : fs::directory_iterator(Repo_Path))
        {
            std::string Name = Entry.path().filename().string();
            if(Name.find(Query) != std::string::npos)
            {
                std::cout << Name << std::endl;
            }
        }
    }
}

extern "C" void Install(std::string Name)
{
    Ensure_Directories();
    fs::path Package = Resolve_Package_Path(Name);
    if(Package.empty() || !fs::exists(Package))
    {
        if(DEBUG) std::cout << "Package not found: " << Name << std::endl;
        return;
    }

    fs::path Target = fs::path(Installed_Path) / Name;
    if(Target.extension() == ".apkg")
    {
        Target.replace_extension("");
    }

    fs::remove_all(Target);
    Copy_Tree(Package_Files_Path(Package), Target);
    if(DEBUG) std::cout << "Package installed: " << Name << std::endl;
}

extern "C" void Remove(std::string Name)
{
    fs::path Target = fs::path(Installed_Path) / Name;
    fs::remove_all(Target);
    if(DEBUG) std::cout << "Package removed: " << Name << std::endl;
}

extern "C" void Upgrade()
{
    if(DEBUG) std::cout << "Package upgrade checked. Version comparison is not implemented yet." << std::endl;
}

extern "C" void List()
{
    Ensure_Directories();
    for(const auto &Entry : fs::directory_iterator(Installed_Path))
    {
        std::cout << Entry.path().filename().string() << std::endl;
    }
}

extern "C" void Info(std::string Name)
{
    fs::path Package = Resolve_Package_Path(Name);
    if(Package.empty() || !fs::exists(Package))
    {
        if(DEBUG) std::cout << "Package not found: " << Name << std::endl;
        return;
    }
    Print_Manifest(Package);
}

extern "C" void Verify(std::string Name)
{
    fs::path Package = Resolve_Package_Path(Name);
    bool Valid = !Package.empty() && fs::exists(Package);
    if(Valid && fs::is_directory(Package))
    {
        Valid = fs::exists(Package / "Manifest.Appeal") || fs::exists(Package / "Files");
    }

    std::cout << (Valid ? "Package verified: " : "Package verify failed: ") << Name << std::endl;
}

extern "C" void Set_Format(std::string Format)
{
    Current_Format = Format;
    if(DEBUG) std::cout << "Package format set: " << Current_Format << std::endl;
}

extern "C" void Set_Level(int Level)
{
    Compression_Level = std::max(0, std::min(22, Level));
    if(DEBUG) std::cout << "Package compression level set: " << Compression_Level << std::endl;
}

extern "C" void Create(std::string Name, std::string Source, std::string Output)
{
    Ensure_Directories();
    fs::path Source_Path(Source);
    fs::path Output_Path(Output);
    if(Output_Path.extension() != ".apkg")
    {
        Output_Path /= (Name + ".apkg");
    }

    fs::remove_all(Output_Path);
    fs::create_directories(Output_Path / "Files");

    std::ofstream Manifest(Output_Path / "Manifest.Appeal", std::ios::trunc);
    Manifest << "Package " << Name << "\n";
    Manifest << "Format " << Current_Format << "\n";
    Manifest << "Compression_Level " << Compression_Level << "\n";
    Manifest << "Source " << Source << "\n";
    Manifest.close();

    Copy_Tree(Source_Path, Output_Path / "Files");
    if(DEBUG) std::cout << "Package created: " << Output_Path.string() << std::endl;
}

extern "C" void Extract(std::string Package_Name, std::string Target)
{
    fs::path Package = Resolve_Package_Path(Package_Name);
    if(Package.empty() || !fs::exists(Package))
    {
        if(DEBUG) std::cout << "Package not found: " << Package_Name << std::endl;
        return;
    }

    fs::path Target_Path(Target);
    fs::remove_all(Target_Path);
    Copy_Tree(Package_Files_Path(Package), Target_Path);
    if(DEBUG) std::cout << "Package extracted: " << Package_Name << " -> " << Target << std::endl;
}

extern "C" void Load_Module(std::string Name)
{
    if(DEBUG) std::cout << "Package Load_Module is reserved for runtime integration: " << Name << std::endl;
}

extern "C" void Unload_Module(std::string Name)
{
    if(DEBUG) std::cout << "Package Unload_Module is reserved for runtime integration: " << Name << std::endl;
}

extern "C" void Load_Plugin(std::string Module, std::string Plugin)
{
    if(DEBUG) std::cout << "Package Load_Plugin is reserved for runtime integration: " << Module << " " << Plugin << std::endl;
}

extern "C" void Unload_Plugin(std::string Module)
{
    if(DEBUG) std::cout << "Package Unload_Plugin is reserved for runtime integration: " << Module << std::endl;
}
