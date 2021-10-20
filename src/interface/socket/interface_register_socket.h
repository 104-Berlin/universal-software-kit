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
            std::thread    Thread;
            int            SocketId;
            sockaddr_in*   Address = nullptr;

            Connection() = default;
            ~Connection();

            void SendPacket(const ERegisterPacket& packet);
        private:
            std::mutex      SendMutex;
        };
    private:
        ERegister*          fLoadedRegister; // The register to get and set data


        int                 fSocketId; // The running socket
        sockaddr_in*        fAddressInfo; // Info to the current socket

        int                 fPort; // Running port

        std::atomic<bool>   fIsRunning;

        std::thread         fAcceptThread; // Thread that accepts new connections
        std::thread         fRegisterEventThread; // Thread that updates register events

        EVector<Connection*>fConnections;
        std::mutex          fConnectionMutex;

        std::mutex          fSendEventDataMutex;
    public:
        ERegisterSocket(int port);
        ~ERegisterSocket();
    private:
        void Init();
        void CleanUp();

        void Run_AcceptConnections();
        void Run_Connection(Connection* connection);

        void HandleConnection(int socketId, sockaddr_in* address);

        void ConnectionGotPacket(Connection* connection, const ERegisterPacket& packet);


        void HandleRegisterEvent(EStructProperty* data);

        // All packet type functions
        EJson Pk_HandleCreateEntity(const ERegisterPacket& packet);
        EJson Pk_HandleCreateComponent(const ERegisterPacket& packet);
        EJson Pk_HandleAddResource(const ERegisterPacket& packet);
        EJson Pk_HandleSetValue(const ERegisterPacket& packet);
        EJson Pk_HandleLoadRegister(const ERegisterPacket& packet);
        EJson Pk_HandleAddArrayEntry(const ERegisterPacket& packet);
        EJson Pk_HandleGetValue(const ERegisterPacket& packet);
        EJson Pk_HandleGetAllValues(const ERegisterPacket& packet);
        EJson Pk_HandleGetResource(const ERegisterPacket& packet);
        EJson Pk_HandleGetLoadedResources(const ERegisterPacket& packet); // This just returns the descriptions of the resources. Not the actual data
        EJson Pk_HandleGetRegisterBuffer(const ERegisterPacket& packet);
        EJson Pk_HandleGetAllEntites(const ERegisterPacket& packet);
    };

}

