#pragma once


struct sockaddr;

namespace Engine {

    enum class ESocketDomain {
        IPv4,
        IPv6, // For now IPv6 is not supported. It will use IPv4 instead.
        Unix
    };

    enum class ESocketType {
        TCP,
        UDP
    };

    static int GetSocketDomain(ESocketDomain domain);
    static int GetSocketType(ESocketType type);

    class EBasicSocket {
    private:
        int fSocketId;

        ESocketDomain fDomain;
        ESocketType   fType;

        sockaddr* fBoundAddress;
        sockaddr* fConnectedToAddress;
    public:
        EBasicSocket(ESocketDomain domain = ESocketDomain::IPv4, ESocketType type = ESocketType::TCP);
        ~EBasicSocket();

        /**
         * @brief Check if the socket is connected to a server
         * 
         * @return true 
         * @return false 
         */
        bool IsConnectedTo() const;

        /**
         * @brief Check if the socket is bound to a port
         * 
         * @return 
         */
        bool IsBound() const;


        /**
         * @brief Bind the socket to a port
         * 
         * @param port The port to bind to
         */
        void Bind(int port = 1024);

        /**
         * @brief Waits for new connection and accepts it
         * 
         * @return SocketId
         */
        int Accept();

        /**
         * @brief Connect the socket to a server
         * 
         * @param address The address to connect to. <ip-address>:<port>
         */
        void Connect(const EString& address);


    private:
        int CreateSocket(ESocketDomain domain, ESocketType type);
        sockaddr* CreateAddress(ESocketDomain domain);
        sockaddr* CreateAddress(ESocketDomain domain, int port);
        int GetAddressSize(ESocketDomain domain);
        char* GetAddressString(sockaddr* address, ESocketDomain domain);
        u16 GetAddressPort(sockaddr* address, ESocketDomain domain);

        EVector<EString> GetHTTPRequest(int socketId);
        EString GetHTTPSplitString(int socketId);
    };

    namespace Socket {
        void Close(int socketId);
        int Read(int socketId, u8* data, size_t data_size);
        int Send(int socketId, const u8* data, size_t data_size);

        EString GetLastSocketError();
        void PrintLastSocketError();

    }

}