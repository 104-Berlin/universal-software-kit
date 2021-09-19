#include "prefix_interface.h"

using namespace Engine;

#define LISTEN_BACKLOG 50


ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port), fSocketId(-1), fAddressInfo(nullptr), fLoadedRegister(new ERegister())
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
    return _sock::read(socketId, data, data_size);
}

void ERegisterSocket::Receive(int socketId, EJson& outJson) 
{
    int n = 0;
    size_t dataLen;
    n = Receive(socketId, (u8*)&dataLen, sizeof(size_t));
    if (n == 0) { HandleDisconnect(socketId); return; }
    

    u8* buffer = new u8[dataLen];
    memset(buffer, 0, dataLen);
    n = Receive(socketId, buffer, dataLen);
    if (n == 0) { HandleDisconnect(socketId); return; }

    EString asString = EString((const char*) buffer);
    delete[] buffer;

    outJson = EJson::parse(asString);
}

void ERegisterSocket::Send(int socketId, const u8* data, size_t data_size) 
{
    std::lock_guard<std::mutex> lock(fSendEventDataMutex);
    
    int n = _sock::send(socketId, data, data_size);
}

void ERegisterSocket::Send(int socketId, const EJson& request) 
{
    EString requestAsString = request.dump();
    const char* buf = requestAsString.c_str();
    size_t bufLen = strlen(buf) + 1;
    Send(socketId, (u8*) &bufLen, sizeof(size_t));
    Send(socketId, (u8*) buf, bufLen);
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

    fSocketId = socket(socketDomain, SOCK_STREAM, 0);

    if (fSocketId == -1)
    {
        E_ERROR("Could not create socket!");
        return;
    }

    fAddressInfo = new sockaddr_in();
    fAddressInfo->sin_family = socketDomain;
    fAddressInfo->sin_addr.s_addr = INADDR_ANY;
    fAddressInfo->sin_port = htons(fPort);

    if (bind(fSocketId, (const sockaddr*)&fAddressInfo, sizeof(fAddressInfo)) == -1)
    {
        E_ERROR("Could not bind the socket " + std::to_string(fSocketId));
        return;
    }

    fIsRunning = true;
    fAcceptThread = std::thread([this](){
        Run_AcceptConnections();
    });  
    fRegisterEventThread = std::thread([this](){
        while (fIsRunning)
        {
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
    if (fRegisterEventThread.joinable())
    {
        fRegisterEventThread.join();
    }

    for (auto& entry : fConnections)
    {
        if (entry.Thread->joinable())
        {
            entry.Thread->join();
        }
        delete entry.Thread;
        delete entry.Address;
    }
    fConnections.clear();

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
        socklen_t client_length = sizeof(client_address);

        int newSocketId = accept(fSocketId, (sockaddr*)client_address, &client_length);
        if (newSocketId != -1)
        {
            E_INFO(EString("RegisterSocket: Got new connection from ") + inet_ntoa(client_address->sin_addr) + " port: " + std::to_string(ntohs(client_address->sin_port)));
            HandleConnection(newSocketId, client_address);
        }
    }
}

void ERegisterSocket::Run_Connection(int socketId, sockaddr_in* address) 
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
        // I think we have to wait here for the event thread to finish sending the data



        // Handle Command and send back something
        switch (event)
        {
        case ESocketEvent::CREATE_ENTITY:
        {
            ERegister::Entity entity = fLoadedRegister->CreateEntity();
            E_ERROR("CREATE ENTITY " + std::to_string(entity));
            break;
        }
        case ESocketEvent::CREATE_COMPONENT:
        {
            EJson inputJson = EJson::object();
            Receive(socketId, inputJson);

            const EJson& descriptionJson = inputJson["ValueDescription"];
            EValueDescription dsc;
            if (!EDeserializer::ReadStorageDescriptionFromJson(descriptionJson, &dsc))
            {
                E_ERROR("Could not read storage description from parsed json!");
                break;
            }
            
            if (inputJson["Entity"].is_number_integer())
            {
                ERegister::Entity entity = (ERegister::Entity) inputJson["Entity"].get<int>();
                EStructProperty* property = fLoadedRegister->AddComponent(entity, dsc);
                if (property)
                {
                    E_INFO("Add Component " + dsc.GetId());
                    inter::PrintProperty(property);
                }
            }
            break;
        }
        case ESocketEvent::SET_VALUE:
        {
            EJson requestJson = EJson::object();
            Receive(socketId, requestJson);

            if (requestJson["Entity"].is_number_integer() && requestJson["ValueIdent"].is_string() && requestJson["Value"].is_string())
            {
                ERegister::Entity entity = requestJson["Entity"].get<int>();
                EString valueIdent = requestJson["ValueIdent"].get<EString>();
                EString value = requestJson["Value"].get<EString>();

                EProperty* foundProperty = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);
                if (!foundProperty)
                {
                    E_ERROR("Could not set value. Value not found!");
                    return;
                }
                EDeserializer::ReadPropertyFromJson(EJson::parse(value), foundProperty);
                E_INFO("Value " + valueIdent + " setted on entity " + std::to_string(entity) + "!");
            }
            break;
        }
        case ESocketEvent::GET_VALUE:
        {
            EJson requestJson = EJson::object();
            Receive(socketId, requestJson);
            if (requestJson["Entity"].is_number_integer() && requestJson["ValueIdent"].is_string())
            {
                ERegister::Entity entity = requestJson["Entity"].get<int>();
                EString valueIdent = requestJson["ValueIdent"].get<EString>();
                EProperty* foundValue = fLoadedRegister->GetValueByIdentifier(entity, valueIdent);
                EJson resultJson = EJson::object();
                if (foundValue)
                {
                    resultJson["ValueDescription"] = ESerializer::WriteStorageDescriptionToJson(foundValue->GetDescription());
                    resultJson["PropertyName"] = foundValue->GetPropertyName();
                    resultJson["Value"] = ESerializer::WritePropertyToJs(foundValue);
                }
                Send(socketId, resultJson);
            }
            break;
        }
        }
    }
}

void ERegisterSocket::HandleConnection(int socketId, sockaddr_in* address) 
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
        E_INFO(EString("User disconnect: ") + inet_ntoa(it->Address->sin_addr));
        fConnections.erase(it);
    }
    else
    {
        E_ERROR(EString("Could not disconnect user with SocketID: ") + std::to_string(socketId));
    }
}

void ERegisterSocket::HandleRegisterEvent(EStructProperty* data) 
{

}
