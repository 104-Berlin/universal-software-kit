#pragma once

namespace Engine {
    
    class E_INTER_API ERegisterConnection
    {
    private:   
        int                 fSocketId;

        std::atomic<bool>   fListening;
        std::thread         fListenThread;


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
        void Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description);
        void Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);

        ERef<EProperty> Send_GetValue(ERegister::Entity entity, const EString& valueIdent);
        EVector<ERef<EProperty>> Send_GetAllValues(ERegister::Entity entity);

        void Connect(const EString& connectTo, int connectToPort);

        void Init();
        void CleanUp();

        EEventDispatcher& GetEventDispatcher();
        const EEventDispatcher& GetEventDispatcher() const;
    private:
        void Run_ListenLoop();

        EJson WaitForRequest(ERegisterPacket::PackId id);
        bool IsWaitingForRequest(ERegisterPacket::PackId id);
        void GotPacket(const ERegisterPacket& packet);

        ERegisterPacket::PackId GetNewPacketID() const;
    };
}
