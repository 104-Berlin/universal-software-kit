#include "prefix_util.h"

using namespace Engine;

struct sockaddr_in;
struct sockaddr_in6;
struct sockaddr_un;

EBasicSocket::EBasicSocket(ESocketDomain domain, ESocketType type)
    : fDomain(domain), fType(type), fBoundAddress(nullptr), fConnectedToAddress(nullptr), fSocketId(0)
{
    try { fSocketId = CreateSocket(domain, type); }
    catch (...) { throw; }
}

EBasicSocket::~EBasicSocket()
{
    
}

bool EBasicSocket::IsConnectedTo() const
{
    return fConnectedToAddress != nullptr;
}

bool EBasicSocket::IsBound() const
{
    return fBoundAddress != nullptr;
}

void EBasicSocket::Bind(int port)
{
    if (IsBound()) {
        throw ESocketException(ESocketException::EType::Bind, "Allready Bound!");
    }
    if (fSocketId <= 0) {
        throw ESocketException(ESocketException::EType::Bind, "SocketID invalid!");
    }
    if (port < 0 || port > 65535) {
        throw ESocketException(ESocketException::EType::Bind, "Port invalid!");
    }
    fBoundAddress = CreateAddress(fDomain, port);

    if (bind(fSocketId, fBoundAddress, GetAddressSize(fDomain)))
    {
        throw ESocketException(ESocketException::EType::Bind, "Bind failed!");
    }
}

int EBasicSocket::CreateSocket(ESocketDomain domain, ESocketType type)
{
    int result = socket(GetSocketDomain(domain), GetSocketType(type), 0);
    if (result == -1) {
        throw ESocketException(ESocketException::EType::Socket, Socket::GetLastSocketError());
    }
    return result;
}

sockaddr* EBasicSocket::CreateAddress(ESocketDomain domain, int port)
{
    sockaddr* result = nullptr;
    switch (domain) {
        case ESocketDomain::IPv4:
            result = (sockaddr*) new sockaddr_in();
            ((sockaddr_in*)result)->sin_family = GetSocketDomain(domain);
            ((sockaddr_in*)result)->sin_port = htons(port);
            ((sockaddr_in*)result)->sin_addr.s_addr = INADDR_ANY;
            break;
        case ESocketDomain::IPv6:
            result = (sockaddr*) new sockaddr_in6();
            ((sockaddr_in6*)result)->sin6_family = GetSocketDomain(domain);
            ((sockaddr_in6*)result)->sin6_port = htons(port);
            ((sockaddr_in6*)result)->sin6_addr  = in6addr_any;
            break;
        case ESocketDomain::Unix:
            result = (sockaddr*) new sockaddr_un();
            ((sockaddr_un*)result)->sun_family = GetSocketDomain(domain);
            ((sockaddr_un*)result)->sun_path[0] = '\0';
            ((sockaddr_un*)result)->sun_len = 0;
            break;
    };
    return result;
}

size_t EBasicSocket::GetAddressSize(ESocketDomain domain)
{
    switch (domain)
    {
    case ESocketDomain::IPv4:
        return sizeof(sockaddr_in);
    case ESocketDomain::IPv6:
        return sizeof(sockaddr_in6);
    case ESocketDomain::Unix:
        return sizeof(sockaddr_un);
    }
}

void Socket::Close(int socketId) 
{
#ifdef EWIN
        closesocket(socketId);
#else
        ::close(socketId);
#endif
}

int Socket::Read(int socketId, u8* data, size_t data_size) 
{
    size_t bytesRead = 0;
    while (bytesRead < data_size) 
    {
        int n = 0;
    #ifdef EWIN
        n = recv(socketId, (char*) (data + bytesRead), data_size - bytesRead, 0);
    #else
        n = ::read(socketId, data + bytesRead, data_size - bytesRead);
    #endif
        if (n == -1)
        {
            PrintLastSocketError();
            return n;
        }
        else if (n == 0)
        {
            E_WARN("Could not read data anymore!");
            PrintLastSocketError();
            return bytesRead;
        }
        else
        {
            bytesRead += n;
        }
    }

    return bytesRead;
}

int Socket::Send(int socketId, const u8* data, size_t data_size) 
{
    size_t bytesSend = 0;

    while (bytesSend < data_size)
    {
        int n = -1;
    #ifdef EWIN
        n = ::send(socketId, (const char*) (data + bytesSend), data_size - bytesSend, 0);
    #else
        n = write(socketId, data + bytesSend, data_size - bytesSend);
    #endif
        if (n == -1)
        {
            PrintLastSocketError();
            return n;
        }
        bytesSend += n;
    }
    return bytesSend;
}

EString Socket::GetLastSocketError()
{

#ifdef EWIN
        char msgbuf [256];   // for a message up to 255 bytes.


        msgbuf [0] = '\0';    // Microsoft doesn't guarantee this on man page.

        int err = WSAGetLastError ();

        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
                    NULL,                // lpsource
                    err,                 // message id
                    MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),    // languageid
                    msgbuf,              // output buffer
                    sizeof (msgbuf),     // size of msgbuf, bytes
                    NULL);               // va_list of arguments

        return msgbuf;

#else
        return strerror(errno);
#endif
}

void Socket::PrintLastSocketError() 
{
    E_ERROR(EString("Socket Error: ") + GetLastSocketError());
}
