#pragma once

struct sockaddr_in;

namespace Engine {
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
            sockaddr_in*    Address;

            Connection() = default;
        };
    private:
        ERegister*          fLoadedRegister; // The register to get and set data


        int                 fSocketId; // The running socket
        sockaddr_in*        fAddressInfo; // Info to the current socket

        int                 fPort; // Running port

        std::atomic<bool>   fIsRunning;

        std::thread         fAcceptThread; // Thread that accepts new connections
        std::thread         fRegisterEventThread; // Thread that updates register events

        EVector<Connection> fConnections;
        std::mutex          fConnectionMutex;

        std::mutex          fSendEventDataMutex;
    public:
        ERegisterSocket(int port);
        ~ERegisterSocket();
    private:
        void Init();
        void CleanUp();

        void Run_AcceptConnections();
        void Run_Connection(int socketId, sockaddr_in* address);

        void HandleConnection(int socketId, sockaddr_in* address);
        void HandleDisconnect(int socketId);


        void HandleRegisterEvent(EStructProperty* data);
    };

}

