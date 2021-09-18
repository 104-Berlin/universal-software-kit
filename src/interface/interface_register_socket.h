#pragma once

namespace Engine {

    class E_INTER_API ERegisterSocket
    {
    private:
        ERegister       fLoadedRegister; // The register to get and set data
        sockaddr_in     fAddressInfo; // Info to the current socket
        
        int             fPort; // Running port
        int             fSocketId; // The running socket
    public:
        ERegisterSocket(int port);
        ~ERegisterSocket();

    private:
        void Init();
    };

}