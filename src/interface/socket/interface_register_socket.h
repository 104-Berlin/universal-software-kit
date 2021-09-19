#pragma once

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
        int Receive(int socketId, u8* data, size_t data_size);
        void Receive(int socketId, EJson& outJson);


        void Send(int socketId, const u8* data, size_t data_size);
        void Send(int socketId, const EJson& request);

        void Init();
        void CleanUp();

        void Run_AcceptConnections();
        void Run_Connection(int socketId, const sockaddr_in& address);

        void HandleConnection(int socketId, const sockaddr_in& address);
        void HandleDisconnect(int socketId);
    };

}

