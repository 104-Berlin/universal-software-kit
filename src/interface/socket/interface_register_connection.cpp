#include "prefix_interface.h"

using namespace Engine;


ERegisterConnection::ERegisterConnection() 
    : fSocketId(-1)
{
    fListening = false;
}

ERegisterConnection::~ERegisterConnection() 
{
    
}

void ERegisterConnection::Send_CreateNewEntity() 
{
    ERegisterPacket packet;
    packet.ID = GetNewPacketID();
    packet.PacketType = EPacketType::CREATE_ENTITY;
    _sock::send_packet(fSocketId, packet);
}

void ERegisterConnection::Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description) 
{
    E_ASSERT(description.Valid());
    if (!description.Valid()) { return; }

    EJson createJson = EJson::object();
    createJson["ValueDescription"] = ESerializer::WriteStorageDescriptionToJson(description);
    createJson["Entity"] = entity;


    ERegisterPacket packet;
    packet.PacketType = EPacketType::CREATE_COMPONENT;
    packet.ID = GetNewPacketID();
    packet.Body = createJson;
    
    _sock::send_packet(fSocketId, packet);
}

void ERegisterConnection::Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString) 
{
    EJson requestJson = EJson::object();
    requestJson["Entity"] = entity;
    requestJson["ValueIdent"] = valueIdent;
    requestJson["Value"] = valueString;

    ERegisterPacket packet;
    packet.PacketType = EPacketType::SET_VALUE;
    packet.ID = GetNewPacketID();
    packet.Body = requestJson;

    _sock::send_packet(fSocketId, packet);
}

ERef<EProperty> ERegisterConnection::Send_GetValue(ERegister::Entity entity, const EString& valueIdent) 
{
    EJson request = EJson::object();
    request["Entity"] = entity;
    request["ValueIdent"] = valueIdent;
    
    
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_VALUE;
    packet.ID = GetNewPacketID();
    packet.Body = request;

    _sock::send_packet(fSocketId, packet);

    EJson result = WaitForRequest(packet.ID);

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

EVector<ERef<EProperty>> ERegisterConnection::Send_GetAllValues(ERegister::Entity entity) 
{
    EJson request = EJson::object();
    request["Entity"] = entity;
    
    
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_ALL_VALUES;
    packet.ID = GetNewPacketID();
    packet.Body = request;

    _sock::send_packet(fSocketId, packet);

    EJson response = WaitForRequest(packet.ID);

    EVector<ERef<EProperty>> result;
    for (EJson& valueJson : response)
    {
        EProperty* property;
        if (EDeserializer::ReadPropertyFromJson_WithDescription(valueJson, &property))
        {
            result.push_back(ERef<EProperty>(property));
        }
    }
    return result;
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

    fListening = true;
    fListenThread = std::thread([this](){
        Run_ListenLoop();
    });
}

void ERegisterConnection::CleanUp() 
{
    fListening = false;

    if (fSocketId > -1)
    {
        _sock::close(fSocketId);

        fSocketId = -1;
    }

    if (fListenThread.joinable())
    {
        fListenThread.join();
    }
}

EEventDispatcher& ERegisterConnection::GetEventDispatcher() 
{
    return fEventDispatcher;
}

const EEventDispatcher& ERegisterConnection::GetEventDispatcher() const
{
    return fEventDispatcher;
}

void ERegisterConnection::Run_ListenLoop() 
{
    while (fListening)
    {
        ERegisterPacket packet;
        int n = _sock::read_packet(fSocketId, &packet);
        if (n <= 0)
        {
            if (n == 0)
            {
                E_WARN("Connection to Server lost!");
                break;
            }
            _sock::print_last_socket_error();
            continue;
        }
            
        GotPacket(packet);
    }
}

EJson ERegisterConnection::WaitForRequest(ERegisterPacket::PackId id) 
{
    std::mutex waitMutex;
    std::unique_lock<std::mutex> lock(waitMutex);
    fRequests[id].GotResult.wait(lock);
    
    EJson result = fRequests[id].Json;
    fRequests.erase(id);
    return result;
}

bool ERegisterConnection::IsWaitingForRequest(ERegisterPacket::PackId id) 
{
    return fRequests.find(id) != fRequests.end();
}

void ERegisterConnection::GotPacket(const ERegisterPacket& packet) 
{
    if (IsWaitingForRequest(packet.ID))
    {
        fRequests[packet.ID].Json = packet.Body;

        fRequests[packet.ID].GotResult.notify_all();
    }
    if (packet.PacketType == EPacketType::REGISTER_EVENT)
    {
        EValueDescription propertyDescription;
        if (!packet.Body["ValueDescription"].is_null() && !packet.Body["Value"].is_null())
        {
            EDeserializer::ReadStorageDescriptionFromJson(packet.Body["ValueDescription"], &propertyDescription);

            ERef<EProperty> resProperty = ERef<EProperty>(EProperty::CreateFromDescription(propertyDescription.GetId(), propertyDescription));

            if (EDeserializer::ReadPropertyFromJson(packet.Body["Value"], resProperty.get()))
            {
                fEventDispatcher.Post_P(propertyDescription, resProperty.get());
            }
        }
    }
}

ERegisterPacket::PackId ERegisterConnection::GetNewPacketID() const
{
    static ERegisterPacket::PackId result = 0;
    while (result == 0 || fRequests.find(result) != fRequests.end())
    {
        result = result + 7 * 3;
    }
    return result;
}

void ERegisterConnection::Connect(const EString& connectTo, int connectToPort) 
{
    E_INFO("Conecting to server " + connectTo);
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
    E_INFO("Connected");
}
