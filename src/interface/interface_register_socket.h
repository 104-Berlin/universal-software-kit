#pragma once

namespace Engine {

    class ERegisterReceiver;


    class E_INTER_API ERegisterSocket
    {
        /**
         * Accepted commands: 
         *  connect
         *  
         * 
         * 
         */
    private:
        ERegister       fLoadedRegister; // The register to get and set data
        sockaddr_in     fAddressInfo; // Info to the current socket
        
        int             fPort; // Running port
        int             fSocketId; // The running socket
    public:
        ERegisterSocket(int port);
        ~ERegisterSocket();

        void Connect(ERegisterReceiver* receiver);
    private:
        void Init();
    };

    class E_INTER_API ERegisterReceiver
    {
    private:   
        ERegisterSocket* fConnectedTo;
    public:
        ERegisterReceiver(ERegisterSocket* connectTo);
        ~ERegisterReceiver();

        void Send_CreateNewEntity();
        void Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description);
        void Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);


    };

}