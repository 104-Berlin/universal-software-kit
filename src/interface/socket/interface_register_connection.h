#pragma once

namespace Engine {
    
    class E_INTER_API ERegisterConnection
    {
    private:   
        ERegisterPacket::PackId fLastPacketId;
        int                 fSocketId;

        std::condition_variable fConnected;

        std::atomic<bool>   fListening;
        std::thread         fListenThread;
        std::thread         fEventThread;
        std::mutex          fNewIdMutex;


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
        void Run_EventLoop();

        EJson WaitForRequest(ERegisterPacket::PackId id);
        bool IsWaitingForRequest(ERegisterPacket::PackId id);
        void GotPacket(const ERegisterPacket& packet);

        ERegisterPacket::PackId GetNewPacketID();
    };
}
