#pragma once

namespace Engine {
    
    class E_INTER_API ERegisterConnection
    {
    private:   
        ERegisterPacket::PackId fLastPacketId;
        int                     fSocketId;
        bool                    fIsConnected;

        std::condition_variable fConnected;

        std::atomic<bool>   fListening;
        std::thread         fListenThread;
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
        void Send_CreateNewComponent(ERegister::Entity entity, EStructProperty* initValue);
        void Send_AddResource(EResourceData* data);
        void Send_LoadRegister(ESharedBuffer buffer);

        void Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);
        void Send_AddArrayEntry(ERegister::Entity entity, const EString& valueIdent);

        EVector<ERegister::Entity> Send_GetAllEntites();
        ERef<EProperty> Send_GetValue(ERegister::Entity entity, const EString& valueIdent);
        EVector<ERef<EProperty>> Send_GetAllValues(ERegister::Entity entity);
        ERef<EResourceData> Send_GetResourceData(EResourceData::t_ID resourceId);
        EVector<ERef<EResourceData>> Send_GetAllResources(const EString& resourceType = "");
        ESharedBuffer Send_GetRegisterBuffer();

        void Connect(const EString& connectTo, int connectToPort);

        bool IsConnected() const;

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
