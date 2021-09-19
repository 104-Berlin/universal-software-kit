#include "prefix_interface.h"

using namespace Engine;

#define LISTEN_BACKLOG 50


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

int ERegisterSocket::Receive(int socketId, u8* data, size_t data_size) 
{
    int n;
#ifdef EWIN
        n = recv(socketId, (char*) data, data_size, 0);
#else
        n = read(socketId, (void*) data, data_size);
#endif
    return n;
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
        ESocketEvent event;
        int n = Receive(socketId, (u8*)&event, sizeof(ESocketEvent));
        if (n == 0)
        {
            HandleDisconnect(socketId);
            break;
        }
        // Handle Command and send back something
        switch (event)
        {
        case ESocketEvent::CREATE_ENTITY:
        {
            E_ERROR("CREATE ENTITY");
            break;
        }
        case ESocketEvent::CREATE_COMPONENT:
        {
            size_t dataLen;
            n = Receive(socketId, (u8*)&dataLen, sizeof(size_t));
            if (n == 0) { HandleDisconnect(socketId); return; }
            

            u8* buffer = new u8[dataLen];
            memset(buffer, 0, dataLen);
            n = Receive(socketId, buffer, dataLen);
            if (n == 0) { HandleDisconnect(socketId); return; }

            EString asString = EString((const char*) buffer);
            E_INFO("GOT JSON: " + asString);

            break;
        }
        }
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

void ERegisterSocket::HandleDisconnect(int socketId) 
{
    std::lock_guard<std::mutex> lock(fConnectionMutex);

    EVector<Connection>::iterator it = std::find_if(fConnections.begin(), fConnections.end(), [socketId](const Connection& con) -> bool{
        return con.SocketId == socketId;
    });
    if (it != fConnections.end())
    {
        E_INFO(EString("User disconnect: ") + inet_ntoa(it->Address.sin_addr));
        fConnections.erase(it);
    }
    else
    {
        E_ERROR(EString("Could not disconnect user with SocketID: ") + std::to_string(socketId));
    }
}
