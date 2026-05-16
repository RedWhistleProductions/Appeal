#include <iostream>
extern "C" void Debug(bool Enable) { (void)Enable; }

#include <string>

extern "C" void Constructor() { std::cout << "Test_Package Constructor()" << std::endl; }
extern "C" void Destructor() { std::cout << "Test_Package Destructor()" << std::endl; }
extern "C" void Set_Paths(std::string Resources) { std::cout << "Set_Paths(" << Resources << ")" << std::endl; }
extern "C" void Add_Repo(std::string Name, std::string URL) { std::cout << "Add_Repo(" << Name << ", " << URL << ")" << std::endl; }
extern "C" void Remove_Repo(std::string Name) { std::cout << "Remove_Repo(" << Name << ")" << std::endl; }
extern "C" void Update_Repos() { std::cout << "Update_Repos()" << std::endl; }
extern "C" void Search(std::string Query) { std::cout << "Search(" << Query << ")" << std::endl; }
extern "C" void Install(std::string Name) { std::cout << "Install(" << Name << ")" << std::endl; }
extern "C" void Remove(std::string Name) { std::cout << "Remove(" << Name << ")" << std::endl; }
extern "C" void Upgrade() { std::cout << "Upgrade()" << std::endl; }
extern "C" void List() { std::cout << "List()" << std::endl; }
extern "C" void Info(std::string Name) { std::cout << "Info(" << Name << ")" << std::endl; }
extern "C" void Verify(std::string Name) { std::cout << "Verify(" << Name << ")" << std::endl; }
extern "C" void Load_Module(std::string Name) { std::cout << "Load_Module(" << Name << ")" << std::endl; }
extern "C" void Unload_Module(std::string Name) { std::cout << "Unload_Module(" << Name << ")" << std::endl; }
extern "C" void Load_Plugin(std::string Module, std::string Plugin) { std::cout << "Load_Plugin(" << Module << ", " << Plugin << ")" << std::endl; }
extern "C" void Unload_Plugin(std::string Module) { std::cout << "Unload_Plugin(" << Module << ")" << std::endl; }

