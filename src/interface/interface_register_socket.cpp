#include "prefix_interface.h"

using namespace Engine;

ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port), fSocketId(-1)
{
    
}

ERegisterSocket::~ERegisterSocket() 
{
    if (fSocketId > -1)
    {
#ifdef EWIN
        closesocket(fSocketId);
#else
        close(fSocketId);
#endif
    }
}

void ERegisterSocket::Connect(ERegisterReceiver* receiver) 
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

    
}

ERegisterReceiver::ERegisterReceiver(ERegisterSocket* connectTo) 
{
    
}

ERegisterReceiver::~ERegisterReceiver() 
{
    
}

void ERegisterReceiver::Send_CreateNewEntity() 
{
    
}

void ERegisterReceiver::Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description) 
{
    
}

void ERegisterReceiver::Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString) 
{
    
}
