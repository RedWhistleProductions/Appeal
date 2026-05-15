#include "Plugin.h"
#include "Data_Source.h"

Plugin Module;

void (*Set_Paths)(std::string Resources);
void (*Listen)(std::string Listener, int Port);
void (*Stop_Listening)(std::string Listener);
void (*Accept)(std::string Listener, std::string Session);
void (*Connect)(std::string Session, std::string Host, int Port);
void (*Disconnect)(std::string Session);
void (*Send)(std::string Session, std::string Message);
void (*Receive)(std::string Session);
void (*Poll)();
void (*Has_Data)(std::string Session);
void (*Has_Command)(std::string Session);
void (*Read_Command)(std::string Session);
void (*Run)(std::string Session);
void (*Download)(std::string Session, std::string Remote, std::string Local);

template<typename T>
void Assign(std::string Name, T &Function)
{
    Module.Assign(Name, Function);
}

extern "C" void Init(std::string Name)
{
    Module.Load(Name);
    Assign("Set_Paths", Set_Paths);
    Assign("Listen", Listen);
    Assign("Stop_Listening", Stop_Listening);
    Assign("Accept", Accept);
    Assign("Connect", Connect);
    Assign("Disconnect", Disconnect);
    Assign("Send", Send);
    Assign("Receive", Receive);
    Assign("Poll", Poll);
    Assign("Has_Data", Has_Data);
    Assign("Has_Command", Has_Command);
    Assign("Read_Command", Read_Command);
    Assign("Run", Run);
    Assign("Download", Download);
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
    else if(Command == "Connect")
    {
        std::string Session, Host;
        int Port;
        *Data >> Session;
        *Data >> Host;
        *Data >> Port;
        Connect(Session, Host, Port);
    }
    else if(Command == "Listen")
    {
        std::string Listener;
        int Port;
        *Data >> Listener;
        *Data >> Port;
        Listen(Listener, Port);
    }
    else if(Command == "Stop_Listening")
    {
        std::string Listener;
        *Data >> Listener;
        Stop_Listening(Listener);
    }
    else if(Command == "Accept")
    {
        std::string Listener, Session;
        *Data >> Listener;
        *Data >> Session;
        Accept(Listener, Session);
    }
    else if(Command == "Disconnect")
    {
        std::string Session;
        *Data >> Session;
        Disconnect(Session);
    }
    else if(Command == "Send")
    {
        std::string Session, Message;
        *Data >> Session;
        *Data >> Message;
        Send(Session, Message);
    }
    else if(Command == "Receive")
    {
        std::string Session;
        *Data >> Session;
        Receive(Session);
    }
    else if(Command == "Poll")
    {
        Poll();
    }
    else if(Command == "Has_Data")
    {
        std::string Session;
        *Data >> Session;
        Has_Data(Session);
    }
    else if(Command == "Has_Command")
    {
        std::string Session;
        *Data >> Session;
        Has_Command(Session);
    }
    else if(Command == "Read_Command")
    {
        std::string Session;
        *Data >> Session;
        Read_Command(Session);
    }
    else if(Command == "Run")
    {
        std::string Session;
        *Data >> Session;
        Run(Session);
    }
    else if(Command == "Download")
    {
        std::string Session, Remote, Local;
        *Data >> Session;
        *Data >> Remote;
        *Data >> Local;
        Download(Session, Remote, Local);
    }
    else
    {
        std::cout << "\tError: " << Command << " not found in Network Dictionary" << std::endl;
    }
}
