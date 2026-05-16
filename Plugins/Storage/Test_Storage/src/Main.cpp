#include <iostream>
extern "C" void Debug(bool Enable) { (void)Enable; }

#include <string>

extern "C" void Constructor() { std::cout << "Test_Storage Constructor()" << std::endl; }
extern "C" void Destructor() { std::cout << "Test_Storage Destructor()" << std::endl; }
extern "C" void Set_Paths(std::string Resources) { std::cout << "Set_Paths(" << Resources << ")" << std::endl; }
extern "C" void Open(std::string Name) { std::cout << "Open(" << Name << ")" << std::endl; }
extern "C" void Close(std::string Name) { std::cout << "Close(" << Name << ")" << std::endl; }
extern "C" void Read(std::string File) { std::cout << "Read(" << File << ")" << std::endl; }
extern "C" void Write(std::string Name, std::string File) { std::cout << "Write(" << Name << ", " << File << ")" << std::endl; }
extern "C" void Exists(std::string File) { std::cout << "Exists(" << File << ")" << std::endl; }
extern "C" void Delete(std::string File) { std::cout << "Delete(" << File << ")" << std::endl; }
extern "C" void Set(std::string Key, std::string Value) { std::cout << "Set(" << Key << ", " << Value << ")" << std::endl; }
extern "C" void Get(std::string Key) { std::cout << "Get(" << Key << ")" << std::endl; }
extern "C" void Save(std::string Name) { std::cout << "Save(" << Name << ")" << std::endl; }
extern "C" void Load(std::string Name) { std::cout << "Load(" << Name << ")" << std::endl; }

