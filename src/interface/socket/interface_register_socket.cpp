#include "prefix_interface.h"

using namespace Engine;
#define LISTEN_BACKLOG 50



ERegisterSocket::Connection::~Connection() 
{
    if (Thread.joinable())
    {
        Thread.join();
    }
    if (Address)
    {
        delete Address;
        Address = nullptr;
    }
}

void ERegisterSocket::Connection::SendPacket(const ERegisterPacket& packet) 
{
    std::lock_guard<std::mutex> lock(SendMutex);

    _sock::send_packet(SocketId, packet);

    _sock::print_packet(EString("SENDING TO ") + inet_ntoa(Address->sin_addr), packet);
}


ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port), fSocketId(-1), fAddressInfo(nullptr), fLoadedRegister(new EDataBase())
{
    Init();
}

ERegisterSocket::~ERegisterSocket() 
{
    CleanUp();
}

bool ERegisterSocket::IsRunning() const
{
    return fIsRunning;
}


void ERegisterSocket::Init() 
{
    fIsRunning = false;
    if (fLoadedRegister)
    {
        delete fLoadedRegister;
    }
    fLoadedRegister = new EDataBase();

    fLoadedRegister->CatchAllEvents([this](ERef<EProperty> data){
        HandleRegisterEvent(data);
    });
 

    // TODO: For single machine interface use AF_LOCAL
    int socketDomain = AF_INET;

    // SETUP MEMORY SOCKET

    fSocketId = socket(AF_INET, SOCK_STREAM, 0);

    if (fSocketId == -1)
    {
        E_ERROR("Could not create socket!");
        return;
    }
    else
    {
        E_INFO("Server socket succesfully started!");
    }

    fAddressInfo = new sockaddr_in();
    memset(fAddressInfo, 0, sizeof(sockaddr_in));
    fAddressInfo->sin_family = AF_INET;
    fAddressInfo->sin_addr.s_addr = INADDR_ANY;
    fAddressInfo->sin_port = htons(fPort);

    if (bind(fSocketId, (const sockaddr*)fAddressInfo, sizeof(sockaddr_in)) == -1)
    {
        E_ERROR("Could not bind the socket " + std::to_string(fSocketId));
        _sock::print_last_socket_error();
        return;
    }

    fIsRunning = true;
    fAcceptThread = std::thread([this](){
        inter::SetCurrentThreadName("Socket_Accept_Connection");
        Run_AcceptConnections();
    });  
    fRegisterEventThread = std::thread([this](){
        inter::SetCurrentThreadName("Socket_Dispatching_Events");
        while (fIsRunning)
        {
            // Wait for event
            fLoadedRegister->WaitForEvent();
            fLoadedRegister->UpdateEvents();
        }
    });
}

void ERegisterSocket::CleanUp() 
{
    fIsRunning = false;

    if (fSocketId > -1)
    {
        _sock::close(fSocketId);
        fSocketId = -1;
    }
    


    if (fAcceptThread.joinable())
    {
        fAcceptThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(fConnectionMutex);
        for (Connection* entry : fConnections)
        {
            delete entry;
        }
        fConnections.clear();
    }

    fLoadedRegister->GetEventDispatcher().StopWaiting();
    if (fRegisterEventThread.joinable())
    {
        fRegisterEventThread.join();
    }

    if (fAddressInfo)
    {
        delete fAddressInfo;
    }

    if (fLoadedRegister)
    {
        delete fLoadedRegister;
        fLoadedRegister = nullptr;
    }

}

void ERegisterSocket::Run_AcceptConnections() 
{
    listen(fSocketId, LISTEN_BACKLOG);
    while (fIsRunning)
    {
        sockaddr_in* client_address = new sockaddr_in();
        socklen_t client_length = sizeof(sockaddr_in);

        int newSocketId = accept(fSocketId, (sockaddr*)client_address, &client_length);
        if (newSocketId != -1)
        {
            E_INFO(EString("RegisterSocket: Got new connection from ") + inet_ntoa(client_address->sin_addr) + " port: " + std::to_string(ntohs(client_address->sin_port)));
            HandleConnection(newSocketId, client_address);
        }
        else
        {
            _sock::print_last_socket_error();
        }
    }
}

void ERegisterSocket::Run_Connection(Connection* connection) 
{
    bool disconnect = false;
    while (fIsRunning)
    {
        ERegisterPacket packet;
        int n = _sock::read_packet(connection->SocketId, &packet);
        if (n == 0)
        {
            disconnect = true;
            break;
        }
        else if (n < 0)
        {
            _sock::print_last_socket_error();
            continue;
        }
        
        ConnectionGotPacket(connection, packet);
    }
    if (disconnect)
    {
        E_INFO(EString("User disconnect: ") + inet_ntoa(connection->Address->sin_addr));
    }
}

void ERegisterSocket::HandleConnection(int socketId, sockaddr_in* address) 
{
    std::lock_guard<std::mutex> lock(fConnectionMutex);


    E_ASSERT(socketId != -1);
    if (socketId == -1) { return; }

    Connection* newConnection = new Connection();
    newConnection->Address = address;
    newConnection->SocketId = socketId;
    newConnection->Thread = std::thread([this, newConnection](){
        inter::SetCurrentThreadName(EString("Handle_Connection") + inet_ntoa(newConnection->Address->sin_addr));
        Run_Connection(newConnection);
    });

    fConnections.push_back(newConnection);
    
}

void ERegisterSocket::ConnectionGotPacket(Connection* connection, const ERegisterPacket& packet) 
{
    if (packet.ID == 0) { return; }

    EJson responseJson = EJson::object();

    // Handle Command and send back something
    switch (packet.PacketType)
    {
    case EPacketType::CREATE_ENTITY:
    {
        responseJson = Pk_HandleCreateEntity(packet);
        break;
    }
    case EPacketType::CREATE_COMPONENT:
    {
        responseJson = Pk_HandleCreateComponent(packet);
        break;
    }
    case EPacketType::LOAD_REGISTER:
    {
        responseJson = Pk_HandleLoadRegister(packet);
        break;
    }
    case EPacketType::ADD_RESOURCE:
    {
        responseJson = Pk_HandleAddResource(packet);
        break;
    }
    case EPacketType::ADD_ARRAY_ENTRY:
    {
        responseJson = Pk_HandleAddArrayEntry(packet);
        break;
    }
    case EPacketType::SET_VALUE:
    {
        responseJson = Pk_HandleSetValue(packet);
        break;
    }
    case EPacketType::GET_VALUE:
    {
        responseJson = Pk_HandleGetValue(packet);
        break;
    }
    case EPacketType::GET_ALL_VALUES:
    {
        responseJson = Pk_HandleGetAllValues(packet);
        break;
    }
    case EPacketType::GET_RESOURCE:
    {
        responseJson = Pk_HandleGetResource(packet);
        break;
    }
    case EPacketType::GET_LOADED_RESOURCES:
    {
        responseJson = Pk_HandleGetLoadedResources(packet);
        break;
    }
    case EPacketType::GET_REGISTER_BUFFER:
    {
        responseJson = Pk_HandleGetRegisterBuffer(packet);
        break;
    }
    case EPacketType::GET_ALL_ENTITES:
    {
        responseJson = Pk_HandleGetAllEntites(packet);
        break;
    }
    case EPacketType::REGISTER_EVENT: break;
    }

    ERegisterPacket responsePacket;
    responsePacket.ID = packet.ID;
    responsePacket.PacketType = packet.PacketType;
    responsePacket.Body = responseJson;

    connection->SendPacket(responsePacket);

    _sock::print_packet(EString("REGISTER") + inet_ntoa(connection->Address->sin_addr), packet);
}

void ERegisterSocket::HandleRegisterEvent(ERef<EProperty> data) 
{
    ERegisterPacket packet;
    packet.PacketType = EPacketType::REGISTER_EVENT;
    packet.ID = 0;
    
    packet.Body = ESerializer::WritePropertyToJs(data.get(), true);

    for (Connection* con : fConnections)
    {
        con->SendPacket(packet);
    }
}

EJson ERegisterSocket::Pk_HandleCreateEntity(const ERegisterPacket& packet) 
{
    EDataBase::Entity entity = fLoadedRegister->CreateEntity();
    E_ERROR("CREATE ENTITY " + std::to_string(entity));
    return EJson::object();
}

EJson ERegisterSocket::Pk_HandleCreateComponent(const ERegisterPacket& packet) 
{
    ERef<EProperty> initValue;
    if (EDeserializer::ReadPropertyFromJson_WithDescription(packet.Body, &initValue))
    {
        if (packet.Body["Entity"].is_number_integer())
        {
            EDataBase::Entity entity = (EDataBase::Entity) packet.Body["Entity"].get<int>();
            EWeakRef<EProperty> property = fLoadedRegister->AddComponent(entity, initValue->GetDescription());
            if (property.lock())
            {
                E_INFO("Add Component " + initValue->GetDescription().GetId());
                property.lock()->Copy(initValue.get());
                inter::PrintProperty(property.lock().get());
            }
        }
    }
    return EJson::object();
}

EJson ERegisterSocket::Pk_HandleAddResource(const ERegisterPacket& packet) 
{
    EResource* data;
    if (EDeserializer::ReadResourceFromJson(packet.Body, &data, true))
    {
        if (!fLoadedRegister->GetResourceManager().AddResource(data))
        {
            delete data;
        }
    }
    else
    {
        delete data;
    }

    return EJson::object();
}

EJson ERegisterSocket::Pk_HandleSetValue(const ERegisterPacket& packet) 
{
    if (packet.Body["Entity"].is_number_integer() && packet.Body["ValueIdent"].is_string() && packet.Body["Value"].is_string())
    {
        EDataBase::Entity entity = packet.Body["Entity"].get<int>();
        EString valueIdent = packet.Body["ValueIdent"].get<EString>();
        EString value = packet.Body["Value"].get<EString>();

        EWeakRef<EProperty> foundProperty = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);
        if (foundProperty.expired() || !foundProperty.lock())
        {
            E_ERROR("Could not set value. Value not found!");
            return EJson::object();
        }
        if (EDeserializer::ReadPropertyFromJson(EJson::parse(value), foundProperty.lock().get()))
        {
            E_INFO("Value " + valueIdent + " setted on entity " + std::to_string(entity) + "!");
        }
    }
    return EJson::object();
}

EJson ERegisterSocket::Pk_HandleAddArrayEntry(const ERegisterPacket& packet) 
{
    if (packet.Body["Entity"].is_number_integer() && packet.Body["ValueIdent"].is_string())
    {
        EDataBase::Entity entity = packet.Body["Entity"].get<int>();
        EString valueIdent = packet.Body["ValueIdent"].get<EString>();
        EWeakRef<EProperty> foundProperty = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);
        if (foundProperty.expired() || !foundProperty.lock() || foundProperty.lock()->GetDescription().GetType() != EValueType::ARRAY)
        {
            E_ERROR("Could not add array entry. Value not found or is not array!");
            return EJson::object();
        }
        ERef<EArrayProperty> asArray = std::static_pointer_cast<EArrayProperty>(foundProperty.lock());
        asArray->AddElement();
    }
    return EJson::object();
}

EJson ERegisterSocket::Pk_HandleGetValue(const ERegisterPacket& packet) 
{
    EJson result = EJson::object();
    if (packet.Body["Entity"].is_number_integer() && packet.Body["ValueIdent"].is_string())
    {
        EDataBase::Entity entity = packet.Body["Entity"].get<int>();
        EString valueIdent = packet.Body["ValueIdent"].get<EString>();
        EWeakRef<EProperty> foundValue = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);

        if (!foundValue.expired() && foundValue.lock())
        {
            result = ESerializer::WritePropertyToJs(foundValue.lock().get(), true /* With Description*/);
            result["PropertyName"] = foundValue.lock()->GetPropertyName();
        }
    }
    return result;
}

EJson ERegisterSocket::Pk_HandleLoadRegister(const ERegisterPacket& packet) 
{
    if (packet.Body["Data"].is_string())
    {
        EString base64 = packet.Body["Data"].get<EString>();
        u8* bufferData;
        size_t bufferLen;
        if (Base64::Decode(base64, &bufferData, &bufferLen))
        {
            ESharedBuffer buffer;
            buffer.InitWith<u8>(bufferData, bufferLen);
            EDeserializer::ReadSceneFromFileBuffer(buffer, fLoadedRegister);
            delete[] bufferData;
        }
    }
    return EJson::object();
}


EJson ERegisterSocket::Pk_HandleGetAllValues(const ERegisterPacket& packet) 
{
    EJson result = EJson::object();
    if (packet.Body["Entity"].is_number_integer())
    {
        EDataBase::Entity entity = packet.Body["Entity"].get<int>();
        EJson propertyArrayJson = EJson::array();
        EVector<ERef<EProperty>> properties = fLoadedRegister->GetAllComponents(entity);
        for (ERef<EProperty> property : properties)
        {
            propertyArrayJson.push_back(ESerializer::WritePropertyToJs(property.get(), true/*With description*/));
        }
        result = propertyArrayJson;
    }
    return result;
}

EJson ERegisterSocket::Pk_HandleGetResource(const ERegisterPacket& packet) 
{
    EJson result = EJson::object();

    if (packet.Body["ID"].is_number_integer())
    {
        EResource::t_ID id = packet.Body["ID"].get<EResource::t_ID>();
        EResource* foundData = fLoadedRegister->GetResourceManager().GetResource(id);
        if (foundData)
        {
            result = ESerializer::WritEResourceBaseToJson(foundData, true);
        }
    }

    return result;
}

EJson ERegisterSocket::Pk_HandleGetLoadedResources(const ERegisterPacket& packet) 
{
    EString resourceString;
    if (packet.Body.size() && packet.Body["ResourceType"].is_string())
    {
        resourceString = packet.Body["ResourceType"].get<EString>();
    }
    EJson result = EJson::array();

    EVector<EResource*> resources;
    if (resourceString.empty()) { resources = fLoadedRegister->GetResourceManager().GetAllResource(); }
    else { resources = fLoadedRegister->GetResourceManager().GetAllResource(resourceString); }

    for (EResource* data : resources)
    {
        result.push_back(ESerializer::WritEResourceBaseToJson(data, false));
    }

    return result;
}


EJson ERegisterSocket::Pk_HandleGetRegisterBuffer(const ERegisterPacket& packet)
{
    EJson result = EJson::object();

    ESharedBuffer buffer = ESerializer::WriteFullSceneBuffer(fLoadedRegister);
    if (!buffer.IsNull())
    {
        result["Data"] = Base64::Encode(buffer.Data<u8>(), buffer.GetSizeInByte());
    }

    return result;
}

EJson ERegisterSocket::Pk_HandleGetAllEntites(const ERegisterPacket& packet)
{
    EJson result = EJson::array();

    for (EDataBase::Entity entity : fLoadedRegister->GetAllEntities())
    {
        result.push_back(entity);
    }

    return result;
}