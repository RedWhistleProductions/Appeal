#include <iostream>
#include <string>

extern "C" void Constructor()
{
    std::cout << "Test_Network Constructor()" << std::endl;
}

extern "C" void Destructor()
{
    std::cout << "Test_Network Destructor()" << std::endl;
}

extern "C" void Set_Paths(std::string Resources)
{
    std::cout << "Set_Paths(" << Resources << ")" << std::endl;
}

extern "C" void Listen(std::string Listener, int Port)
{
    std::cout << "Listen(" << Listener << ", " << Port << ")" << std::endl;
}

extern "C" void Stop_Listening(std::string Listener)
{
    std::cout << "Stop_Listening(" << Listener << ")" << std::endl;
}

extern "C" void Accept(std::string Listener, std::string Session)
{
    std::cout << "Accept(" << Listener << ", " << Session << ")" << std::endl;
}

extern "C" void Connect(std::string Session, std::string Host, int Port)
{
    std::cout << "Connect(" << Session << ", " << Host << ", " << Port << ")" << std::endl;
}

extern "C" void Disconnect(std::string Session)
{
    std::cout << "Disconnect(" << Session << ")" << std::endl;
}

extern "C" void Send(std::string Session, std::string Message)
{
    std::cout << "Send(" << Session << ", " << Message << ")" << std::endl;
}

extern "C" void Receive(std::string Session)
{
    std::cout << "Receive(" << Session << ")" << std::endl;
}

extern "C" void Poll()
{
    std::cout << "Poll()" << std::endl;
}

extern "C" void Has_Data(std::string Session)
{
    std::cout << "Has_Data(" << Session << ")" << std::endl;
}

extern "C" void Has_Command(std::string Session)
{
    std::cout << "Has_Command(" << Session << ")" << std::endl;
}

extern "C" void Read_Command(std::string Session)
{
    std::cout << "Read_Command(" << Session << ")" << std::endl;
}

extern "C" void Run(std::string Session)
{
    std::cout << "Run(" << Session << ")" << std::endl;
}

extern "C" void Download(std::string Session, std::string Remote, std::string Local)
{
    std::cout << "Download(" << Session << ", " << Remote << ", " << Local << ")" << std::endl;
}
