#include "prefix_interface.h"

using namespace Engine;


ERegisterConnection::ERegisterConnection() 
{
}

ERegisterConnection::~ERegisterConnection() 
{
    
}

void ERegisterConnection::Send_CreateNewEntity() 
{
    Send(ESocketEvent::CREATE_ENTITY);
}

void ERegisterConnection::Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description) 
{
    E_ASSERT(description.Valid());
    if (!description.Valid()) { return; }

    EJson createJson = EJson::object();
    createJson["ValueDescription"] = ESerializer::WriteStorageDescriptionToJson(description);
    createJson["Entity"] = entity;

    Send(ESocketEvent::CREATE_COMPONENT);
    Send(createJson);
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

void ERegisterConnection::Send(ESocketEvent eventType) 
{
    Send((u8*)&eventType, sizeof(ESocketEvent));
}

void ERegisterConnection::Send(const u8* buffer, size_t buffer_size) 
{
    int n;
#ifdef EWIN
    n = send(fSocketId, (const char*) buffer, buffer_size, 0);
#else
    n = write(fSocketId, buffer, buffer_size);
#endif
}

void ERegisterConnection::Send(const EJson& value) 
{
    EString valueDump = value.dump();
    const char* buffer = valueDump.c_str();
    size_t len = strlen(buffer) + 1;

    Send((u8*)&len, sizeof(size_t));
    Send((u8*)buffer, len);
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
