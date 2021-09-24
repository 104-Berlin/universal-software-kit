#include "prefix_interface.h"

using namespace Engine;

void _sock::close(int socketId) 
{
#ifdef EWIN
        closesocket(socketId);
#else
        ::close(socketId);
#endif
}

int _sock::read(int socketId, u8* data, size_t data_size) 
{
    int n = 0;
#ifdef EWIN
    n = recv(socketId, (char*) data, data_size, 0);
#else
    n = ::read(socketId, data, data_size);
#endif
    if (n == -1)
    {
        print_last_socket_error();
    }
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
    if (n == -1)
    {
        print_last_socket_error();
    }
    return n;
}

void _sock::send_packet(int socketId, const ERegisterPacket& packet) 
{
    send(socketId, (u8*) &packet.PacketType, sizeof(EPacketType));
    send(socketId, (u8*) &packet.ID, sizeof(ERegisterPacket::PackId));

    EString bodyString = packet.Body.dump();
    const char* data = bodyString.c_str();
    size_t dataLen = strlen(data) + 1;
    send(socketId, (u8*)&dataLen, sizeof(size_t));
    send(socketId, (u8*)data, dataLen);
}

int _sock::read_packet(int socketId, ERegisterPacket* outPacket) 
{
    int n = 0;

    EPacketType type;
    ERegisterPacket::PackId ID;
    n = read(socketId, (u8*) &type, sizeof(EPacketType));
    if (n <= 0) { return n; }
    read(socketId, (u8*) &ID, sizeof(ERegisterPacket::PackId));
    if (n <= 0) { return n; }

    size_t dataLen = 0;
    read(socketId, (u8*)&dataLen, sizeof(size_t));
    if (n <= 0) { return n; }
    u8* bodyData = new u8[dataLen];
    memset(bodyData, 0, dataLen);
    read(socketId, bodyData, dataLen);
    if (n <= 0) { delete[] bodyData; return n; }

    EString bodyAsString = EString((char*)bodyData);
    delete[] bodyData;

    outPacket->PacketType = type;
    outPacket->ID = ID;
    outPacket->Body = EJson::parse(bodyAsString,nullptr, false);

    return n;
}

void _sock::print_last_socket_error() 
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

        E_ERROR(EString("Socket Error: ") + msgbuf);

#else
        E_ERROR(EString("Socket Error: ") + strerror(errno));
#endif
}

void _sock::print_packet(const EString& name, const ERegisterPacket& packet) 
{
    E_INFO("------" + name + " PACKET -------");           
    E_INFO("| ID   =  " + std::to_string(packet.ID) + "  |");           
    E_INFO("|-------------------|");
    E_INFO(EString("| TYPE = | ") + GetPacketTypeString(packet.PacketType) + " |");
    E_INFO("---------------------");
    E_INFO("|       Body        |");
    E_INFO(packet.Body.dump());
    E_INFO("---------------------");
}