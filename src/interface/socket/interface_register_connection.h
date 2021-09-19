#pragma once

namespace Engine {
    
    class E_INTER_API ERegisterConnection
    {
    private:   
        int                 fSocketId;

        std::thread         fListenThread;
    public:
        ERegisterConnection();
        ~ERegisterConnection();

        void Send_CreateNewEntity();
        void Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description);
        void Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);

        ERef<EProperty> Send_GetValue(ERegister::Entity entity, const EString& valueIdent);

        void Connect(const EString& connectTo, int connectToPort);

        void Init();
        void CleanUp();
    private:
        void Get(EJson& outValue);
        int Get(u8* buffer, size_t buffer_size);

        void Send(const u8* buffer, size_t buffer_size);

        void Send(ESocketEvent eventType);
        void Send(const EJson& value);
    };
}