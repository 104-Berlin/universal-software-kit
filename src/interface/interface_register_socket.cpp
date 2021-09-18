#include "prefix_interface.h"

#define LISTEN_BACKLOG 50

using namespace Engine;

ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port), fSocketId(-1)
{
    Init();
}

ERegisterSocket::~ERegisterSocket() 
{
    CleanUp();
}

void ERegisterSocket::Connect(ERegisterConnection* receiver) 
{
    
}

void ERegisterSocket::Init() 
{
    // TODO: For single machine interface use AF_LOCAL
    int socketDomain = AF_INET;

    fSocketId = socket(socketDomain, SOCK_STREAM, 0);

    if (fSocketId == -1)
    {
        E_ERROR("Could not create socket!");
        return;
    }

    fAddressInfo.sin_family = socketDomain;
    fAddressInfo.sin_addr.s_addr = INADDR_ANY;
    fAddressInfo.sin_port = htons(fPort);

    if (bind(fSocketId, (const sockaddr*)&fAddressInfo, sizeof(fAddressInfo)) == -1)
    {
        E_ERROR("Could not bind the socket " + std::to_string(fSocketId));
        return;
    }

    fIsRunning = true;
    fAcceptThread = std::thread([this](){
        Run_AcceptConnections();
    });    
}

void ERegisterSocket::CleanUp() 
{
    fIsRunning = false;


    

    if (fSocketId > -1)
    {
#ifdef EWIN
        closesocket(fSocketId);
#else
        close(fSocketId);
#endif

        fSocketId = -1;
    }


    if (fAcceptThread.joinable())
    {
        fAcceptThread.join();
    }

    for (auto& entry : fConnections)
    {
        if (entry.Thread->joinable())
        {
            entry.Thread->join();
        }
        delete entry.Thread;
    }
    fConnections.clear();
}

void ERegisterSocket::Run_AcceptConnections() 
{
    listen(fSocketId, LISTEN_BACKLOG);
    while (fIsRunning)
    {
        sockaddr_in client_address;
        socklen_t client_length = sizeof(client_address);

        int newSocketId = accept(fSocketId, (sockaddr*)&client_address, &client_length);
        if (newSocketId != -1)
        {
            E_INFO(EString("RegisterSocket: Got new connection from ") + inet_ntoa(client_address.sin_addr) + " port: " + std::to_string(ntohs(client_address.sin_port)));
            HandleConnection(newSocketId, client_address);
        }
    }
}

void ERegisterSocket::Run_Connection(int socketId, const sockaddr_in& address) 
{
    while (fIsRunning)
    {
        char buffer[255];
        memset(buffer, 0, 255);
        int n;
#ifdef EWIN
        n = recv(socketId, buffer, 255, 0);
#else
        n = read(socketId, buffer, 255);
#endif
        if (n == 0)
        {
            E_INFO(EString("User disconnect: ") + inet_ntoa(address.sin_addr));
            break;
        }
        // Handle Command and send back something

        // For now just print message
        // Make it null terminate for safety
        buffer[254] = 0;
        EString asString = buffer;

        E_INFO("Server got from user: " + asString);
    }
}

void ERegisterSocket::HandleConnection(int socketId, const sockaddr_in& address) 
{
    std::lock_guard<std::mutex> lock(fConnectionMutex);


    E_ASSERT(socketId != -1);
    if (socketId == -1) { return; }


    Connection newConnection;
    newConnection.SocketId = socketId;
    newConnection.Thread = new std::thread([this, socketId, address](){
        Run_Connection(socketId, address);
    });

    fConnections.push_back(newConnection);
}

ERegisterConnection::ERegisterConnection() 
{
}

ERegisterConnection::~ERegisterConnection() 
{
    
}

void ERegisterConnection::Send_CreateNewEntity() 
{
    int n;
    const char* buffer = "Hello World";

#ifdef EWIN
    n = send(fSocketId, buffer, strlen(buffer), 0);
#else
    n = write(fSocketId, buffer, strlen(buffer));
#endif


}

void ERegisterConnection::Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description) 
{
    
}

void ERegisterConnection::Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString) 
{
    
}

void ERegisterConnection::Init() 
{
    int socketDomain = AF_INET;
    fSocketId = socket(socketDomain, SOCK_STREAM, 0);
    if (fSocketId == -1)
    {
        E_ERROR("Could not create receiver socket!");
        return;
    }
}

void ERegisterConnection::CleanUp() 
{
    if (fSocketId > -1)
    {
#ifdef EWIN
        closesocket(fSocketId);
#else
        close(fSocketId);
#endif

        fSocketId = -1;
    }
}

void ERegisterConnection::Connect(const EString& connectTo, int connectToPort) 
{
    hostent* connect_to_server = gethostbyname(connectTo.c_str());
    if (connect_to_server == NULL)
    {
        E_ERROR("Could not find Server " + connectTo);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    bcopy((char*)connect_to_server->h_addr_list[0], (char*)&serverAddr.sin_addr, connect_to_server->h_length);
    serverAddr.sin_port = htons(connectToPort);

    if (connect(fSocketId, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        E_ERROR("Could not connect to server!");
        return;
    }
}
