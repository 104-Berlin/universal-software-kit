#pragma once

namespace Engine {
    
    class E_INTER_API ERegisterConnection
    {
    public:
        enum class Status 
        {
            Disconnected,
            Connecting,
            Connected
        };
    private:   
        ERegisterPacket::PackId fLastPacketId;
        int                     fSocketId;
        Status                  fConnectionStatus;
        EString                 fConnectToAddress;

        std::condition_variable fConnected;

        std::atomic<bool>   fListening;
        std::thread         fListenThread;
        std::thread         fConnectingThread;
        std::mutex          fNewIdMutex;
        std::mutex          fSendMutex;


        struct Request
        {
            std::condition_variable GotResult;
            EJson                   Json;

            Request()
            {
                Json = EJson::object();
            }
        };
        EUnorderedMap<ERegisterPacket::PackId, Request> fRequests;

        EEventDispatcher                                fEventDispatcher;
    public:
        ERegisterConnection();
        ~ERegisterConnection();

        void Send_CreateNewEntity();
        void Send_CreateNewComponent(EDataBase::Entity entity, ERef<EProperty> initValue);
        void Send_AddResource(EResource* data);
        void Send_LoadRegister(ESharedBuffer buffer);

        void Send_SetValue(EDataBase::Entity entity, const EString& valueIdent, const EString& valueString);
        void Send_AddArrayEntry(EDataBase::Entity entity, const EString& valueIdent);

        EVector<EDataBase::Entity> Send_GetAllEntites();
        ERef<EProperty> Send_GetValue(EDataBase::Entity entity, const EString& valueIdent);
        EVector<ERef<EProperty>> Send_GetAllValues(EDataBase::Entity entity);
        ERef<EResource> Send_GetResourceData(EResource::t_ID resourceId);
        EVector<ERef<EResource>> Send_GetAllResources(const EString& resourceType = "");
        ESharedBuffer Send_GetRegisterBuffer();

        void Connect(const EString& connectTo, int connectToPort);

        Status GetConnectionStatus() const;
        const EString& GetConnectedToAddress() const;

        void Init();
        void CleanUp();

        EEventDispatcher& GetEventDispatcher();
        const EEventDispatcher& GetEventDispatcher() const;
    private:
        void SendToServer(const ERegisterPacket& packet);

        void Run_ListenLoop();

        EJson WaitForRequest(ERegisterPacket::PackId id);
        bool IsWaitingForRequest(ERegisterPacket::PackId id);
        void GotPacket(const ERegisterPacket& packet);

        ERegisterPacket::PackId GetNewPacketID();
    };
}
