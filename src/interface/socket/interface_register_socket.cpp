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
}


ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port), fSocketId(-1), fAddressInfo(nullptr), fLoadedRegister(new ERegister())
{
    Init();
}

ERegisterSocket::~ERegisterSocket() 
{
    CleanUp();
}

void ERegisterSocket::Init() 
{
    if (fLoadedRegister)
    {
        delete fLoadedRegister;
    }
    fLoadedRegister = new ERegister();

    fLoadedRegister->CatchAllEvents([this](EStructProperty* data){
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
        Run_AcceptConnections();
    });  
    fRegisterEventThread = std::thread([this](){
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
    while (fIsRunning)
    {
        ERegisterPacket packet;
        int n = _sock::read_packet(connection->SocketId, &packet);
        if (n == 0)
        {
            HandleDisconnect(connection->SocketId);
            break;
        }
        else if (n < 0)
        {
            _sock::print_last_socket_error();
            continue;
        }
        
        E_INFO("Register received packet: " + std::to_string(packet.ID));
        if (packet.ID == 0) { continue; }

        EJson responseJson = EJson::object();

        // Handle Command and send back something
        switch (packet.PacketType)
        {
        case EPacketType::CREATE_ENTITY:
        {
            ERegister::Entity entity = fLoadedRegister->CreateEntity();
            E_ERROR("CREATE ENTITY " + std::to_string(entity));
            break;
        }
        case EPacketType::CREATE_COMPONENT:
        {
            const EJson& descriptionJson = packet.Body["ValueDescription"];
            EValueDescription dsc;
            if (!EDeserializer::ReadStorageDescriptionFromJson(descriptionJson, &dsc))
            {
                E_ERROR("Could not read storage description from parsed json!");
                break;
            }
            
            if (packet.Body["Entity"].is_number_integer())
            {
                ERegister::Entity entity = (ERegister::Entity) packet.Body["Entity"].get<int>();
                EStructProperty* property = fLoadedRegister->AddComponent(entity, dsc);
                if (property)
                {
                    E_INFO("Add Component " + dsc.GetId());
                    inter::PrintProperty(property);
                }
            }
            break;
        }
        case EPacketType::SET_VALUE:
        {
            if (packet.Body["Entity"].is_number_integer() && packet.Body["ValueIdent"].is_string() && packet.Body["Value"].is_string())
            {
                ERegister::Entity entity = packet.Body["Entity"].get<int>();
                EString valueIdent = packet.Body["ValueIdent"].get<EString>();
                EString value = packet.Body["Value"].get<EString>();

                EProperty* foundProperty = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);
                if (!foundProperty)
                {
                    E_ERROR("Could not set value. Value not found!");
                    return;
                }
                if (EDeserializer::ReadPropertyFromJson(EJson::parse(value), foundProperty))
                {
                    E_INFO("Value " + valueIdent + " setted on entity " + std::to_string(entity) + "!");
                }
            }
            break;
        }
        case EPacketType::GET_VALUE:
        {
            if (packet.Body["Entity"].is_number_integer() && packet.Body["ValueIdent"].is_string())
            {
                ERegister::Entity entity = packet.Body["Entity"].get<int>();
                EString valueIdent = packet.Body["ValueIdent"].get<EString>();
                EProperty* foundValue = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);

                if (foundValue)
                {
                    responseJson = ESerializer::WritePropertyToJs(foundValue, true /* With Description*/);
                    responseJson["PropertyName"] = foundValue->GetPropertyName();
                }
            }
            break;
        }
        case EPacketType::GET_ALL_VALUES:
        {
            if (packet.Body["Entity"].is_number_integer())
            {
                ERegister::Entity entity = packet.Body["Entity"].get<int>();
                EJson propertyArrayJson = EJson::array();
                for (EProperty* property : fLoadedRegister->GetAllComponents(entity))
                {
                    propertyArrayJson.push_back(ESerializer::WritePropertyToJs(property, true/*With description*/));
                }
                responseJson = propertyArrayJson;
            }
            break;
        }
        }

        ERegisterPacket responsePacket;
        responsePacket.ID = packet.ID;
        responsePacket.PacketType = packet.PacketType;
        responsePacket.Body = responseJson;

        connection->SendPacket(responsePacket);
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
        Run_Connection(newConnection);
    });

    fConnections.push_back(newConnection);
    
}

void ERegisterSocket::HandleDisconnect(int socketId) 
{
    std::lock_guard<std::mutex> lock(fConnectionMutex);

    EVector<Connection*>::iterator it = std::find_if(fConnections.begin(), fConnections.end(), [socketId](Connection* con) -> bool{
        return con->SocketId == socketId;
    });
    if (it != fConnections.end())
    {
        E_INFO(EString("User disconnect: ") + inet_ntoa((*it)->Address->sin_addr));
        fConnections.erase(it);
        delete *it;
    }
    else
    {
        E_ERROR(EString("Could not disconnect user with SocketID: ") + std::to_string(socketId));
    }
}

void ERegisterSocket::HandleRegisterEvent(EStructProperty* data) 
{
    ERegisterPacket packet;
    packet.PacketType = EPacketType::REGISTER_EVENT;
    packet.ID = 1;
    
    packet.Body["ValueDescription"] = ESerializer::WriteStorageDescriptionToJson(data->GetDescription());
    packet.Body["Value"] = ESerializer::WritePropertyToJs(data);

    for (Connection* con : fConnections)
    {
        con->SendPacket(packet);
    }
}
