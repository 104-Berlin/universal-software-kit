#include "prefix_util.h"

#define LISTEN_BACKLOG 50

int Engine::GetSocketDomain(Engine::ESocketDomain domain)
{
    switch (domain) {
        case ESocketDomain::IPv4:
        case ESocketDomain::IPv6:
            return AF_INET;
        case ESocketDomain::Unix:
            return AF_UNIX;
    }
    throw ESocketException(ESocketException::EType::Socket, "Invalid socket domain");
    return 0;
}

int Engine::GetSocketType(Engine::ESocketType type)
{
    switch (type) {
        case ESocketType::TCP:
            return SOCK_STREAM;
        case ESocketType::UDP:
            return SOCK_DGRAM;
    }
    throw ESocketException(ESocketException::EType::Socket, "Invalid socket type");
    return 0;
}

using namespace Engine;

struct sockaddr_in;
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

    listen(fSocketId, LISTEN_BACKLOG);
}

int EBasicSocket::Accept()
{
    if (fSocketId <= 0) {
        throw ESocketException(ESocketException::EType::Accept, "SocketID invalid!");
    }
    if (!IsBound()) {
        throw ESocketException(ESocketException::EType::Accept, "Not bound!");
    }
    sockaddr* client_add = CreateAddress(fDomain);
    int client_len = GetAddressSize(fDomain);

    int newSocketId = accept(fSocketId, client_add, &client_len);
    if (newSocketId == -1)
    {
        throw ESocketException(ESocketException::EType::Accept, "Accept failed!" + Socket::GetLastSocketError());
    }
    return newSocketId;
}


int EBasicSocket::CreateSocket(ESocketDomain domain, ESocketType type)
{
    int result = socket(GetSocketDomain(domain), GetSocketType(type), 0);
    if (result == -1) {
        throw ESocketException(ESocketException::EType::Socket, Socket::GetLastSocketError());
    }
    return result;
}

sockaddr* EBasicSocket::CreateAddress(ESocketDomain domain)
{
    sockaddr* result = nullptr;
    switch (domain) {
        case ESocketDomain::IPv4:
        case ESocketDomain::IPv6:
            result = (sockaddr*) new sockaddr_in();
            break;
        case ESocketDomain::Unix:
            break;
    }
    if (!result)
    {
        throw ESocketException(ESocketException::EType::Socket, "Invalid socket domain");
    }
    return result;
}

sockaddr* EBasicSocket::CreateAddress(ESocketDomain domain, int port)
{
    sockaddr* result = nullptr;
    switch (domain) {
        case ESocketDomain::IPv4:
        case ESocketDomain::IPv6:
            result = (sockaddr*) new sockaddr_in();
            ((sockaddr_in*)result)->sin_family = GetSocketDomain(domain);
            ((sockaddr_in*)result)->sin_port = htons(port);
            ((sockaddr_in*)result)->sin_addr.s_addr = INADDR_ANY;
            break;
        case ESocketDomain::Unix:
            throw ESocketException(ESocketException::EType::Socket, "Cant create socket for unix domain!");
            /*result = (sockaddr*) new sockaddr_un();
            ((sockaddr_un*)result)->sun_family = GetSocketDomain(domain);
            ((sockaddr_un*)result)->sun_path[0] = '\0';
            ((sockaddr_un*)result)->sun_len = 0;*/
            break;
    };
    return result;
}

int EBasicSocket::GetAddressSize(ESocketDomain domain)
{
    switch (domain)
    {
    case ESocketDomain::IPv4:
    case ESocketDomain::IPv6:
        return sizeof(sockaddr_in);
    case ESocketDomain::Unix:
        throw ESocketException(ESocketException::EType::Socket, "Cant get address size for unix domain! Unsupported!");
    }
    return 0;
}

char* EBasicSocket::GetAddressString(sockaddr* address, ESocketDomain domain)
{
    char* result = nullptr;
    switch (domain)
    {
    case ESocketDomain::IPv4:
    case ESocketDomain::IPv6:
        result = inet_ntoa(((sockaddr_in*)address)->sin_addr);
        break;
    case ESocketDomain::Unix:
        throw ESocketException(ESocketException::EType::Socket, "Cant get address string for unix domain! Unsupported!");
    }
    return result;
}

u16 EBasicSocket::GetAddressPort(sockaddr* address, ESocketDomain domain)
{
    u16 result = 0;
    switch (domain)
    {
    case ESocketDomain::IPv4:
    case ESocketDomain::IPv6:
        result = ntohs(((sockaddr_in*)address)->sin_port);
        break;
    case ESocketDomain::Unix:
        throw ESocketException(ESocketException::EType::Socket, "Cant get address port for unix domain! Unsupported!");
    }
    return result;
}

EVector<EString> EBasicSocket::GetHTTPRequest(int socketId)
{
    EVector<EString> result;
    result.push_back(GetHTTPSplitString(socketId));
    result.push_back(GetHTTPSplitString(socketId));
    return result;
}

EString EBasicSocket::GetHTTPSplitString(int socketId)
{
    bool foundEnd = false;
    EVector<char> request;
    char currentChar = 0;
    while (Socket::Read(socketId, (u8*)&currentChar, sizeof(char)) >= 0)
    {
        if (currentChar == '\r') { foundEnd = true; continue;}
        if (currentChar == '\n' && foundEnd) { break; }
        request.push_back(currentChar);
    }
    request.push_back('\0');
    return request.data();
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
