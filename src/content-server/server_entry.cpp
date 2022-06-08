#include "prefix_http_server.h"

using namespace Engine;

int main()
{
    #ifdef EWIN
        WSADATA wsaData;

        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0)
        {
            E_ERROR("Could not start windows socket api!");
            E_ERROR("PROGRAMM NOT RUNNING CORRECTLY!");
            return -1;
        }
    #endif
    try
    {
        EBasicSocket socket(ESocketDomain::IPv4, ESocketType::TCP);
        socket.Bind(8080);
    }
    catch (ESocketException& e)
    {
        std::cerr << e.what() << std::endl;
    }

    #ifdef EWIN
        WSACleanup();
    #endif
    return 0;
}