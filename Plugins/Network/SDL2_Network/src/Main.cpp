#ifdef __MINGW32__
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#endif

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Listener_State
{
    TCPsocket Socket = nullptr;
    int Port = 0;
};

struct Session_State
{
    TCPsocket Socket = nullptr;
    std::string Buffer;
    std::string Last_Data;
    bool Has_Data = false;
    bool Connected = false;
};

bool DEBUG = true;
bool Network_Ready = false;
std::string Resources = "./Resources";
std::map<std::string, Listener_State> Listeners;
std::map<std::string, Session_State> Sessions;
SDLNet_SocketSet Socket_Set = nullptr;

void Log(std::string Message)
{
    if(DEBUG)
    {
        std::cout << Message << std::endl;
    }
}

bool Ensure_Network()
{
    if(Network_Ready)
    {
        return true;
    }

    if(SDLNet_Init() < 0)
    {
        Log(std::string("SDL_net could not initialize: ") + SDLNet_GetError());
        return false;
    }

    Network_Ready = true;
    return true;
}

void Close_Session(std::string Session)
{
    auto It = Sessions.find(Session);
    if(It == Sessions.end())
    {
        return;
    }

    if(It->second.Socket != nullptr)
    {
        SDLNet_TCP_Close(It->second.Socket);
    }
    Sessions.erase(It);
}

void Rebuild_Socket_Set()
{
    if(Socket_Set != nullptr)
    {
        SDLNet_FreeSocketSet(Socket_Set);
        Socket_Set = nullptr;
    }

    int Count = 0;
    for(auto &Item : Listeners)
    {
        if(Item.second.Socket != nullptr) Count++;
    }
    for(auto &Item : Sessions)
    {
        if(Item.second.Socket != nullptr) Count++;
    }

    if(Count < 1)
    {
        return;
    }

    Socket_Set = SDLNet_AllocSocketSet(Count);
    if(Socket_Set == nullptr)
    {
        Log(std::string("SDL_net socket set failed: ") + SDLNet_GetError());
        return;
    }

    for(auto &Item : Listeners)
    {
        if(Item.second.Socket != nullptr)
        {
            SDLNet_TCP_AddSocket(Socket_Set, Item.second.Socket);
        }
    }
    for(auto &Item : Sessions)
    {
        if(Item.second.Socket != nullptr)
        {
            SDLNet_TCP_AddSocket(Socket_Set, Item.second.Socket);
        }
    }
}

std::string Trim_Line(std::string Line)
{
    while(!Line.empty() && (Line.back() == '\n' || Line.back() == '\r'))
    {
        Line.pop_back();
    }
    return Line;
}

void Poll_Session(std::string Name, Session_State &Session)
{
    if(Session.Socket == nullptr || Socket_Set == nullptr || !SDLNet_SocketReady(Session.Socket))
    {
        return;
    }

    char Data[4096];
    int Received = SDLNet_TCP_Recv(Session.Socket, Data, sizeof(Data) - 1);
    if(Received <= 0)
    {
        Log("Network disconnected session: " + Name);
        Close_Session(Name);
        Rebuild_Socket_Set();
        return;
    }

    Data[Received] = '\0';
    Session.Last_Data.assign(Data, Received);
    Session.Buffer.append(Data, Received);
    Session.Has_Data = true;
}

extern "C" void Constructor()
{
    Ensure_Network();
}

extern "C" void Destructor()
{
    for(auto &Item : Sessions)
    {
        if(Item.second.Socket != nullptr)
        {
            SDLNet_TCP_Close(Item.second.Socket);
        }
    }
    Sessions.clear();

    for(auto &Item : Listeners)
    {
        if(Item.second.Socket != nullptr)
        {
            SDLNet_TCP_Close(Item.second.Socket);
        }
    }
    Listeners.clear();

    if(Socket_Set != nullptr)
    {
        SDLNet_FreeSocketSet(Socket_Set);
        Socket_Set = nullptr;
    }

    if(Network_Ready)
    {
        SDLNet_Quit();
        Network_Ready = false;
    }
}

extern "C" void Debug(bool Enable)
{
    DEBUG = Enable;
}

extern "C" void Set_Paths(std::string Path)
{
    Resources = Path;
}

extern "C" void Listen(std::string Listener, int Port)
{
    if(!Ensure_Network()) return;

    IPaddress Address;
    if(SDLNet_ResolveHost(&Address, nullptr, static_cast<Uint16>(Port)) < 0)
    {
        Log(std::string("Network listen resolve failed: ") + SDLNet_GetError());
        return;
    }

    TCPsocket Socket = SDLNet_TCP_Open(&Address);
    if(Socket == nullptr)
    {
        Log(std::string("Network listen failed: ") + SDLNet_GetError());
        return;
    }

    if(Listeners[Listener].Socket != nullptr)
    {
        SDLNet_TCP_Close(Listeners[Listener].Socket);
    }
    Listeners[Listener] = {Socket, Port};
    Rebuild_Socket_Set();
    Log("Network listening " + Listener + " on port " + std::to_string(Port));
}

extern "C" void Stop_Listening(std::string Listener)
{
    auto It = Listeners.find(Listener);
    if(It == Listeners.end())
    {
        return;
    }

    if(It->second.Socket != nullptr)
    {
        SDLNet_TCP_Close(It->second.Socket);
    }
    Listeners.erase(It);
    Rebuild_Socket_Set();
}

extern "C" void Accept(std::string Listener, std::string Session)
{
    if(!Ensure_Network()) return;

    auto It = Listeners.find(Listener);
    if(It == Listeners.end() || It->second.Socket == nullptr)
    {
        Log("Network listener not found: " + Listener);
        return;
    }

    TCPsocket Socket = SDLNet_TCP_Accept(It->second.Socket);
    if(Socket == nullptr)
    {
        return;
    }

    Close_Session(Session);
    Sessions[Session].Socket = Socket;
    Sessions[Session].Connected = true;
    Rebuild_Socket_Set();
    Log("Network accepted session: " + Session);
}

extern "C" void Connect(std::string Session, std::string Host, int Port)
{
    if(!Ensure_Network()) return;

    IPaddress Address;
    if(SDLNet_ResolveHost(&Address, Host.c_str(), static_cast<Uint16>(Port)) < 0)
    {
        Log(std::string("Network connect resolve failed: ") + SDLNet_GetError());
        return;
    }

    TCPsocket Socket = SDLNet_TCP_Open(&Address);
    if(Socket == nullptr)
    {
        Log(std::string("Network connect failed: ") + SDLNet_GetError());
        return;
    }

    Close_Session(Session);
    Sessions[Session].Socket = Socket;
    Sessions[Session].Connected = true;
    Rebuild_Socket_Set();
    Log("Network connected session: " + Session);
}

extern "C" void Disconnect(std::string Session)
{
    Close_Session(Session);
    Rebuild_Socket_Set();
}

extern "C" void Send(std::string Session, std::string Message)
{
    auto It = Sessions.find(Session);
    if(It == Sessions.end() || It->second.Socket == nullptr)
    {
        Log("Network session not found: " + Session);
        return;
    }

    std::string Out = Message;
    if(Out.find('\n') == std::string::npos)
    {
        Out += "\n";
    }

    int Sent = SDLNet_TCP_Send(It->second.Socket, Out.c_str(), static_cast<int>(Out.size()));
    if(Sent < static_cast<int>(Out.size()))
    {
        Log(std::string("Network send failed: ") + SDLNet_GetError());
    }
}

extern "C" void Receive(std::string Session)
{
    auto It = Sessions.find(Session);
    if(It == Sessions.end())
    {
        Log("Network session not found: " + Session);
        return;
    }

    Poll_Session(Session, It->second);
    if(It->second.Has_Data)
    {
        Log(It->second.Last_Data);
        It->second.Has_Data = false;
    }
}

extern "C" void Poll()
{
    if(!Ensure_Network() || Socket_Set == nullptr)
    {
        return;
    }

    int Ready = SDLNet_CheckSockets(Socket_Set, 0);
    if(Ready <= 0)
    {
        return;
    }

    std::vector<std::string> Session_Names;
    for(auto &Item : Sessions)
    {
        Session_Names.push_back(Item.first);
    }

    for(const std::string &Name : Session_Names)
    {
        auto It = Sessions.find(Name);
        if(It != Sessions.end())
        {
            Poll_Session(Name, It->second);
        }
    }
}

extern "C" void Has_Data(std::string Session)
{
    auto It = Sessions.find(Session);
    bool Value = It != Sessions.end() && It->second.Has_Data;
    Log(Value ? "1" : "0");
}

extern "C" void Has_Command(std::string Session)
{
    auto It = Sessions.find(Session);
    bool Value = It != Sessions.end() && It->second.Buffer.find('\n') != std::string::npos;
    Log(Value ? "1" : "0");
}

extern "C" void Read_Command(std::string Session)
{
    auto It = Sessions.find(Session);
    if(It == Sessions.end())
    {
        Log("");
        return;
    }

    size_t End = It->second.Buffer.find('\n');
    if(End == std::string::npos)
    {
        Log("");
        return;
    }

    std::string Command = Trim_Line(It->second.Buffer.substr(0, End + 1));
    It->second.Buffer.erase(0, End + 1);
    It->second.Has_Data = !It->second.Buffer.empty();
    Log(Command);
}

extern "C" void Run(std::string Session)
{
    Read_Command(Session);
}

extern "C" void Download(std::string Session, std::string Remote, std::string Local)
{
    std::string Request = "DOWNLOAD " + Remote + " " + Local;
    Send(Session, Request);
}
