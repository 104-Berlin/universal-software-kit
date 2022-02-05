#include "prefix_interface.h"

using namespace Engine;


ERegisterConnection::ERegisterConnection() 
    : fSocketId(-1), fLastPacketId(0), fConnectionStatus(Status::Disconnected)
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
    SendToServer(packet);
}

void ERegisterConnection::Send_CreateNewComponent(EDataBase::Entity entity, ERef<EProperty> initValue) 
{
    E_ASSERT(initValue);
    if (!initValue) { return; }

    EJson createJson = ESerializer::WritePropertyToJs(initValue.get(), true);
    createJson["Entity"] = entity;


    ERegisterPacket packet;
    packet.PacketType = EPacketType::CREATE_COMPONENT;
    packet.ID = GetNewPacketID();
    packet.Body = createJson;
    
    SendToServer(packet);
}

void ERegisterConnection::Send_AddResource(EResourceBase* data) 
{
    ERegisterPacket packet;
    packet.PacketType = EPacketType::ADD_RESOURCE;
    packet.ID = GetNewPacketID();
    packet.Body = ESerializer::WritEResourceBaseToJson(*data, true);

    SendToServer(packet);
}

void ERegisterConnection::Send_LoadRegister(ESharedBuffer buffer) 
{
    if (buffer.IsNull()) { return; }
    ERegisterPacket packet;
    packet.PacketType = EPacketType::LOAD_REGISTER;
    packet.ID = GetNewPacketID();
    
    EJson body = EJson::object();
    body["Data"] = Base64::Encode(buffer.Data<u8>(), buffer.GetSizeInByte());
    packet.Body = body;

    SendToServer(packet);
}

void ERegisterConnection::Send_SetValue(EDataBase::Entity entity, const EString& valueIdent, const EString& valueString) 
{
    EJson requestJson = EJson::object();
    requestJson["Entity"] = entity;
    requestJson["ValueIdent"] = valueIdent;
    requestJson["Value"] = valueString;

    ERegisterPacket packet;
    packet.PacketType = EPacketType::SET_VALUE;
    packet.ID = GetNewPacketID();
    packet.Body = requestJson;

    SendToServer(packet);
}

void ERegisterConnection::Send_AddArrayEntry(EDataBase::Entity entity, const EString& valueIdent) 
{
    EJson requestJson = EJson::object();
    requestJson["Entity"] = entity;
    requestJson["ValueIdent"] = valueIdent;

    ERegisterPacket packet;
    packet.PacketType = EPacketType::ADD_ARRAY_ENTRY;
    packet.ID = GetNewPacketID();
    packet.Body = requestJson;

    SendToServer(packet);
}

EVector<EDataBase::Entity> ERegisterConnection::Send_GetAllEntites() 
{
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_ALL_ENTITES;
    packet.ID = GetNewPacketID();
    packet.Body = EJson::object();

    SendToServer(packet);

    EJson response = WaitForRequest(packet.ID);

    EVector<EDataBase::Entity> result;

    if (response.is_array())
    {
        for (const EJson& entityJson : response)
        {
            result.push_back(entityJson.get<EDataBase::Entity>());
        }
    }

    return result;
}

ERef<EProperty> ERegisterConnection::Send_GetValue(EDataBase::Entity entity, const EString& valueIdent) 
{
    EJson request = EJson::object();
    request["Entity"] = entity;
    request["ValueIdent"] = valueIdent;
    
    
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_VALUE;
    packet.ID = GetNewPacketID();
    packet.Body = request;

    SendToServer(packet);

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

EVector<ERef<EProperty>> ERegisterConnection::Send_GetAllValues(EDataBase::Entity entity) 
{
    EJson request = EJson::object();
    request["Entity"] = entity;
    
    
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_ALL_VALUES;
    packet.ID = GetNewPacketID();
    packet.Body = request;

    SendToServer(packet);

    EJson response = WaitForRequest(packet.ID);

    EVector<ERef<EProperty>> result;
    for (EJson& valueJson : response)
    {
        if (!valueJson.is_object()) { continue; }
        ERef<EProperty> property;
        if (EDeserializer::ReadPropertyFromJson_WithDescription(valueJson, &property))
        {
            result.push_back(property);
        }
    }
    return result;
}

ERef<EResourceBase> ERegisterConnection::Send_GetResourceData(EResourceBase::t_ID resourceId) 
{
    EJson req = EJson::object();
    req["ID"] = resourceId;

    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_RESOURCE;
    packet.ID = GetNewPacketID();
    packet.Body = req;

    SendToServer(packet);

    EJson response = WaitForRequest(packet.ID);

    ERef<EResourceBase> resourceData = EMakeRef<EResourceBase>();
    if (!EDeserializer::ReadResourceFromJson(response, resourceData.get(), true))
    {
        return nullptr;
    }
    return resourceData;
}

EVector<ERef<EResourceBase>> ERegisterConnection::Send_GetAllResources(const EString& resourceType) 
{
    EVector<ERef<EResourceBase>> result;

    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_LOADED_RESOURCES;
    packet.ID = GetNewPacketID();

    EJson body = EJson::object();
    if (!resourceType.empty())
    {
        body["ResourceType"] = resourceType;
    }
    packet.Body = body;

    SendToServer(packet);

    EJson res = WaitForRequest(packet.ID);

    if (res.is_array())
    {
        for (EJson& arrayEntry : res)
        {
            ERef<EResourceBase> newData = EMakeRef<EResourceBase>();
            if (EDeserializer::ReadResourceFromJson(arrayEntry, newData.get(), false))
            {
                result.push_back(newData);
            }
        }
    }

    return result;
}

ESharedBuffer ERegisterConnection::Send_GetRegisterBuffer() 
{
    ERegisterPacket packet;
    packet.PacketType = EPacketType::GET_REGISTER_BUFFER;
    packet.ID = GetNewPacketID();

    SendToServer(packet);
    EJson res = WaitForRequest(packet.ID);

    ESharedBuffer result;
    if (res["Data"].is_string())
    {
        u8* data;
        size_t dataLen;
        EString base64 = res["Data"].get<EString>();
        if (Base64::Decode(base64, &data, &dataLen))
        {
            result.InitWith<u8>(data, dataLen);
            delete[] data;
        }
    }
    return result;
}

void ERegisterConnection::Init() 
{
    fConnectionStatus = Status::Disconnected;
    
    int socketDomain = AF_INET;
    fSocketId = socket(socketDomain, SOCK_STREAM, 0);
    if (fSocketId == -1)
    {
        E_ERROR("Could not create receiver socket!");
        return;
    }

    fListening = true;
    fListenThread = std::thread([this](){
        inter::SetCurrentThreadName("Connection_Listen");
        Run_ListenLoop();
    });
}

void ERegisterConnection::CleanUp() 
{
    fListening = false;

    fConnected.notify_all();

    if (fSocketId > -1)
    {
        _sock::close(fSocketId);

        fSocketId = -1;
    }

    for (auto& entry : fRequests)
    {
        entry.second.GotResult.notify_all();
    }

    if (fConnectingThread.joinable())
    {
        fConnectingThread.join();
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

void ERegisterConnection::SendToServer(const ERegisterPacket& packet) 
{
    std::lock_guard<std::mutex> lk(fSendMutex);

    _sock::send_packet(fSocketId, packet);
}

void ERegisterConnection::Run_ListenLoop() 
{
    {
        std::mutex waitForConnect;
        std::unique_lock<std::mutex> lk(waitForConnect);
        fConnected.wait(lk);
    }
    while (fListening)
    {
        ERegisterPacket packet;
        int n = _sock::read_packet(fSocketId, &packet);
        if (n <= 0)
        {
            if (n == 0)
            {
                E_WARN("Connection to Server lost!");
                fConnectionStatus = Status::Disconnected;
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
    _sock::print_packet("CONNECTION", packet);
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
                fEventDispatcher.Enqueue_P(propertyDescription, resProperty);
            }
        }
    }
}

ERegisterPacket::PackId ERegisterConnection::GetNewPacketID()
{
    std::lock_guard<std::mutex> lk(fNewIdMutex);
    while (fRequests.find(++fLastPacketId) != fRequests.end());
    return fLastPacketId;
}

void ERegisterConnection::Connect(const EString& connectTo, int connectToPort) 
{
    fConnectionStatus = Status::Connecting;
    fConnectToAddress = "";
    if (fConnectingThread.joinable())
    {
        fConnectingThread.join();
    }
    fConnectingThread = std::thread([this, connectTo, connectToPort](){
        E_INFO("Conecting to server " + connectTo);
        hostent* connect_to_server = gethostbyname(connectTo.c_str());
        if (connect_to_server == NULL)
        {
            E_ERROR("Could not find Server " + connectTo);
            fConnectionStatus = Status::Disconnected;
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        bcopy((char*)connect_to_server->h_addr_list[0], (char*)&serverAddr.sin_addr, connect_to_server->h_length);
        serverAddr.sin_port = htons(connectToPort);

        if (connect(fSocketId, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            E_ERROR("Could not connect to server!");
            fConnectionStatus = Status::Disconnected;
            _sock::print_last_socket_error();
            return;
        }
        fConnectToAddress = connectTo;
        fConnected.notify_all();
        fConnectionStatus = Status::Connected;
        E_INFO("Connected");
    });
}

ERegisterConnection::Status ERegisterConnection::GetConnectionStatus() const
{
    return fConnectionStatus;
}

const EString& ERegisterConnection::GetConnectedToAddress() const
{
    return fConnectToAddress;
}
