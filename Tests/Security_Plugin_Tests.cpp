#define APPEAL_SECURITY_BRIDGE
#include "../Resources/Modules/Appeal_Module_Security/src/Security_Plugin_API.h"
#include "../Resources/Modules/Appeal_Module_Security/src/Data_Source.h"
#include "../Resources/Modules/Appeal_Module_Security/src/Module_Data.h"
#include "../Resources/Modules/Appeal_Module_Security/src/Named_List.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#define CHECK(x) do{if(!(x))throw std::runtime_error("Check failed at line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace fs=std::filesystem;
struct Library {
#ifdef _WIN32
    HMODULE Handle=nullptr;
#else
    void *Handle=nullptr;
#endif
    explicit Library(const fs::path &path){
#ifdef _WIN32
        Handle=LoadLibraryW(path.c_str());
#else
        Handle=dlopen(path.c_str(),RTLD_NOW|RTLD_LOCAL);
#endif
        CHECK(Handle);
    }
    ~Library(){
#ifdef _WIN32
        if(Handle)FreeLibrary(Handle);
#else
        if(Handle)dlclose(Handle);
#endif
    }
    template<class T>T Get(const char *name){
#ifdef _WIN32
        auto address=GetProcAddress(Handle,name);T result=nullptr;static_assert(sizeof(result)==sizeof(address));std::memcpy(&result,&address,sizeof(result));
#else
        auto result=reinterpret_cast<T>(dlsym(Handle,name));
#endif
        CHECK(result);return result;
    }
};
std::string Read(const fs::path &path){std::ifstream file(path,std::ios::binary);CHECK(file.good());return {std::istreambuf_iterator<char>(file),{}};}
void Write(const fs::path &path,const std::string &data){std::ofstream file(path,std::ios::binary);file.write(data.data(),data.size());CHECK(file.good());}
const std::string Test_Key(43,'A'); // Deterministic fixture only: never use as a real key.
const std::string Password="only a cross-platform test password";
const std::string Text=std::string("Binary\0text\n",12)+"\xe2\x98\x83";
std::string Payload(){std::string data(2*65536+33,'\0');for(size_t i=0;i<data.size();++i)data[i]=char(i%251);return data;}
struct Tokens:Data_Source {
    std::vector<std::string> Items;size_t Index=0;std::string Value;
    Tokens(std::initializer_list<std::string> items):Items(items){}
    std::string Get_Data() override{CHECK(Index<Items.size());return Items[Index++];}
    void Return_Value(std::string value) override{Value=std::move(value);}
    bool End_Of_Source() const override{return Index==Items.size();}
};
int main(int argc,char **argv)try {
    CHECK(argc>=4);Library plugin(fs::u8path(argv[1]));const auto folder=fs::absolute(fs::u8path(argv[3]));fs::create_directories(folder);
    plugin.Get<void(*)()>("Constructor")();
    auto keygen=plugin.Get<std::string(*)()>("Generate_Key");
    auto encrypt=plugin.Get<std::string(*)(std::string,std::string)>("Encrypt");auto decrypt=plugin.Get<std::string(*)(std::string,std::string)>("Decrypt");
    auto encrypt_pw=plugin.Get<std::string(*)(std::string,std::string)>("Encrypt_With_Password");auto decrypt_pw=plugin.Get<std::string(*)(std::string,std::string)>("Decrypt_With_Password");
    using File=bool(*)(std::string,std::string,std::string);
    auto encrypt_file=plugin.Get<File>("Encrypt_File"),decrypt_file=plugin.Get<File>("Decrypt_File"),encrypt_file_pw=plugin.Get<File>("Encrypt_File_With_Password"),decrypt_file_pw=plugin.Get<File>("Decrypt_File_With_Password");
    auto error=plugin.Get<std::string(*)()>("Last_Error");
    if(argc==5 && std::string(argv[4])=="produce"){
        Write(folder/"Text_Key.txt",encrypt(Text,Test_Key));Write(folder/"Text_Password.txt",encrypt_pw(Text,Password));Write(folder/"Source.bin",Payload());
        CHECK(encrypt_file((folder/"Source.bin").u8string(),(folder/"Key.enc").u8string(),Test_Key));
        CHECK(encrypt_file_pw((folder/"Source.bin").u8string(),(folder/"Password.enc").u8string(),Password));
        std::cout<<"SECURITY_FIXTURES_CREATED\n";return 0;
    }
    if(argc==5 && std::string(argv[4])=="consume"){
        CHECK(decrypt(Read(folder/"Text_Key.txt"),Test_Key)==Text);CHECK(decrypt_pw(Read(folder/"Text_Password.txt"),Password)==Text);
        CHECK(decrypt_file((folder/"Key.enc").u8string(),(folder/"Key.restored").u8string(),Test_Key));
        CHECK(decrypt_file_pw((folder/"Password.enc").u8string(),(folder/"Password.restored").u8string(),Password));
        CHECK(Read(folder/"Key.restored")==Payload());CHECK(Read(folder/"Password.restored")==Payload());
        std::cout<<"SECURITY_INTEROP_PASSED\n";return 0;
    }
    const auto key=keygen(),wrong=keygen();CHECK(key.size()==43 && key!=wrong);
    auto cipher=encrypt(Text,key);CHECK(!cipher.empty() && cipher!=encrypt(Text,key));CHECK(decrypt(cipher,key)==Text && error().empty());
    CHECK(decrypt(cipher,wrong).empty() && !error().empty());CHECK(decrypt(cipher,"bad key").empty() && !error().empty());
    auto damaged=cipher;damaged[damaged.size()/2]=damaged[damaged.size()/2]=='A'?'B':'A';CHECK(decrypt(damaged,key).empty() && !error().empty());
    CHECK(decrypt("APSEC1:!!",key).empty() && !error().empty());CHECK(decrypt(cipher.substr(0,40),key).empty() && !error().empty());
    CHECK(decrypt(encrypt("",key),key).empty() && error().empty());
    const auto protected_text=encrypt_pw(Text,Password);CHECK(protected_text!=encrypt_pw(Text,Password));CHECK(decrypt_pw(protected_text,Password)==Text);
    CHECK(decrypt_pw(protected_text,"wrong password").empty() && !error().empty());
    CHECK(decrypt_pw(cipher,Password).empty() && !error().empty());CHECK(encrypt_pw(Text,"").empty() && !error().empty());
    Write(folder/"Source.bin",Payload());const auto source=(folder/"Source.bin").u8string();
    const auto sealed=(folder/"Sealed.enc").u8string();CHECK(encrypt_file(source,sealed,key));CHECK(Read(folder/"Sealed.enc")!=Payload());
    CHECK(decrypt_file(sealed,(folder/"Restored.bin").u8string(),key));CHECK(Read(folder/"Restored.bin")==Payload());
    Write(folder/"Keep.bin","KEEP");CHECK(!decrypt_file(sealed,(folder/"Keep.bin").u8string(),key));CHECK(Read(folder/"Keep.bin")=="KEEP");
    CHECK(!encrypt_file(source,source,key));CHECK(Read(folder/"Source.bin")==Payload());
    const auto packet=Read(folder/"Sealed.enc");std::vector<std::string> corrupt;
    for(size_t offset:{size_t(0),size_t(8),size_t(25),size_t(49),size_t(53),packet.size()-1}){auto copy=packet;copy[offset]^=1;corrupt.push_back(std::move(copy));}
    corrupt.push_back(packet.substr(0,packet.size()-1));corrupt.push_back(packet+"trailing data");
    constexpr size_t record=65536+17+4;auto reordered=packet;reordered.replace(49,record,packet.substr(49+record,record));reordered.replace(49+record,record,packet.substr(49,record));corrupt.push_back(reordered);
    for(size_t i=0;i<corrupt.size();++i){
        const auto damaged_path=folder/("Bad_"+std::to_string(i)+".enc"),output=folder/("Bad_"+std::to_string(i)+".out");
        Write(damaged_path,corrupt[i]);CHECK(!decrypt_file(damaged_path.u8string(),output.u8string(),key));CHECK(!fs::exists(output));CHECK(!error().empty());
    }
    CHECK(!decrypt_file(sealed,(folder/"Wrong_Key.out").u8string(),wrong));CHECK(!fs::exists(folder/"Wrong_Key.out"));
    for(size_t size:{size_t(0),size_t(65536),size_t(65537)}){
        auto name="Size_"+std::to_string(size);Write(folder/name,std::string(size,'x'));
        CHECK(encrypt_file((folder/name).u8string(),(folder/(name+".enc")).u8string(),key));
        CHECK(decrypt_file((folder/(name+".enc")).u8string(),(folder/(name+".out")).u8string(),key));CHECK(Read(folder/(name+".out"))==std::string(size,'x'));
    }
    CHECK(encrypt_file_pw(source,(folder/"Password.enc").u8string(),Password));
    CHECK(!decrypt_file_pw((folder/"Password.enc").u8string(),(folder/"Wrong_Password.out").u8string(),"wrong"));CHECK(!fs::exists(folder/"Wrong_Password.out"));
    CHECK(decrypt_file_pw((folder/"Password.enc").u8string(),(folder/"Password.out").u8string(),Password));CHECK(Read(folder/"Password.out")==Payload());
    for(const auto &entry:fs::directory_iterator(folder))CHECK(entry.path().filename().u8string().rfind(".appeal-security-",0)!=0);
    // Exercise the actual module bridge and its status globals, not only backend calls.
    Library module(fs::u8path(argv[2]));Named_List<Module_Data> globals;
    auto init=module.Get<void(*)(std::string,Named_List<Module_Data>*)>("Init_Globals");auto interpret=module.Get<void(*)(Data_Source*)>("Interpreter");auto shutdown=module.Get<void(*)()>("Shutdown");
    init(fs::absolute(fs::u8path(argv[1])).u8string(),&globals);
    auto call=[&](std::initializer_list<std::string> args){Tokens tokens(args);interpret(&tokens);return tokens.Value;};
    const auto envelope=call({"Encrypt",Text,key});CHECK(call({"Decrypt",envelope,key})==Text);
    CHECK(globals.Find("Security.Success") && globals.Current->Value.Value=="1");
    CHECK(call({"Decrypt",envelope,wrong}).empty());CHECK(globals.Find("Security.Success") && globals.Current->Value.Value=="0");
    CHECK(call({"Decrypt",call({"Encrypt","",key}),key}).empty());CHECK(globals.Find("Security.Success") && globals.Current->Value.Value=="1");
    CHECK(!globals.Find("Security.Key") && !globals.Find("Security.Plaintext"));shutdown();shutdown();
    for(int i=0;i<10;++i){init(fs::absolute(fs::u8path(argv[1])).u8string(),&globals);CHECK(call({"Generate_Key"}).size()==43);shutdown();}
    plugin.Get<void(*)()>("Destructor")();
    std::cout<<"SECURITY_NATIVE_PASSED\n";
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
