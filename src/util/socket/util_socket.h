#pragma once


struct sockaddr;

namespace Engine {

    enum class ESocketDomain {
        IPv4,
        IPv6,
        Unix
    };

    enum class ESocketType {
        TCP,
        UDP
    };

    static constexpr int GetSocketDomain(ESocketDomain domain) {
        switch (domain) {
            case ESocketDomain::IPv4:
                return AF_INET;
            case ESocketDomain::IPv6:
                return AF_INET6;
            case ESocketDomain::Unix:
                return AF_UNIX;
        }
    };

    static constexpr int GetSocketType(ESocketType type) {
        switch (type) {
            case ESocketType::TCP:
                return SOCK_STREAM;
            case ESocketType::UDP:
                return SOCK_DGRAM;
        }
    };

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
         * @brief Connect the socket to a server
         * 
         * @param address The address to connect to. <ip-address>:<port>
         */
        void Connect(const EString& address);

    private:
        int CreateSocket(ESocketDomain domain, ESocketType type);
        sockaddr* CreateAddress(ESocketDomain domain, int port);
        size_t GetAddressSize(ESocketDomain domain);
    };

    namespace Socket {
        void Close(int socketId);
        int Read(int socketId, u8* data, size_t data_size);
        int Send(int socketId, const u8* data, size_t data_size);

        EString GetLastSocketError();
        void PrintLastSocketError();

    }

}