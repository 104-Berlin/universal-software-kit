#pragma once

namespace Engine {

    class ERegisterConnection;


    class E_INTER_API ERegisterSocket
    {
        /**
         * Accepted commands: 
         *  connect
         *  
         * 
         * 
         */

        struct Connection
        {
            std::thread*    Thread;
            int             SocketId;
            sockaddr_in     Address;

            Connection() = default;
        };
    private:
        ERegister           fLoadedRegister; // The register to get and set data
        sockaddr_in         fAddressInfo; // Info to the current socket
        
        int                 fPort; // Running port
        int                 fSocketId; // The running socket

        std::atomic<bool>   fIsRunning;

        std::thread         fAcceptThread; // Thread that accepts new connections

        EVector<Connection> fConnections;
        std::mutex          fConnectionMutex;
    public:
        ERegisterSocket(int port);
        ~ERegisterSocket();

        void Connect(ERegisterConnection* receiver);
    private:
        void Init();
        void CleanUp();

        void Run_AcceptConnections();
        void Run_Connection(int socketId, const sockaddr_in& address);

        void HandleConnection(int socketId, const sockaddr_in& address);
    };

    class E_INTER_API ERegisterConnection
    {
    private:   
        int                 fSocketId;


    public:
        ERegisterConnection();
        ~ERegisterConnection();

        void Send_CreateNewEntity();
        void Send_CreateNewComponent(ERegister::Entity entity, const EValueDescription& description);
        void Send_SetValue(ERegister::Entity entity, const EString& valueIdent, const EString& valueString);


        void Connect(const EString& connectTo, int connectToPort);

        void Init();
        void CleanUp();
    private:
    };

}