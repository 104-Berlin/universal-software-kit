#include "prefix_interface.h"

using namespace Engine;

void _sock::close(int socketId) 
{
#ifdef EWIN
        closesocket(socketId);
#else
        close(socketId);
#endif
}

int _sock::read(int socketId, u8* data, size_t data_size) 
{
    int n = -1;
#ifdef EWIN
    n = recv(socketId, (char*) data, data_size, 0);
#else
    n = read(socketId, data, data_size);
#endif
    return n;
}

int _sock::send(int socketId, const u8* data, size_t data_size) 
{
    int n = -1;
#ifdef EWIN
    n = ::send(socketId, (const char*) data, data_size, 0);
#else
    n = write(socketId, data, data_size);
#endif
    return n;
}
