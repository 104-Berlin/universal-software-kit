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
    EJson requestJson = EJson::object();
    requestJson["Entity"] = entity;
    requestJson["ValueIdent"] = valueIdent;
    requestJson["Value"] = valueString;

    Send(ESocketEvent::SET_VALUE);
    Send(requestJson);
}

ERef<EProperty> ERegisterConnection::Send_GetValue(ERegister::Entity entity, const EString& valueIdent) 
{
    Send(ESocketEvent::GET_VALUE);
    EJson request = EJson::object();
    request["Entity"] = entity;
    request["ValueIdent"] = valueIdent;
    Send(request);

    EJson result = EJson::object();
    Get(result);

    EValueDescription propertyDescription;
    if (!result["ValueDescription"].is_null() && result["PropertyName"].is_string() && !result["Value"].is_null())
    {
        EDeserializer::ReadStorageDescriptionFromJson(result["ValueDescription"], &propertyDescription);

        ERef<EProperty> resProperty = ERef<EProperty>(EProperty::CreateFromDescription(result["PropertyName"].get<EString>(), propertyDescription));

        if (EDeserializer::ReadPropertyFromJson(result["Value"], resProperty.get()))
        {
            return resProperty;
        }
        return nullptr;
    }
    return nullptr;
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
        _sock::close(fSocketId);

        fSocketId = -1;
    }
}

void ERegisterConnection::Get(EJson& outValue) 
{
    size_t bufLen = 0;
    Get((u8*)&bufLen, sizeof(size_t));
    u8* buffer = new u8[bufLen];
    Get(buffer, bufLen);
    EString bufAsString = EString((const char*)buffer);
    delete[] buffer;
    outValue = EJson::parse(bufAsString);
}

int ERegisterConnection::Get(u8* buffer, size_t buffer_size) 
{
    return _sock::read(fSocketId, buffer, buffer_size);
}

void ERegisterConnection::Send(ESocketEvent eventType) 
{
    Send((u8*)&eventType, sizeof(ESocketEvent));
}

void ERegisterConnection::Send(const u8* buffer, size_t buffer_size) 
{
    _sock::send(fSocketId, buffer, buffer_size);
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
