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
    int n = -1;
#ifdef EWIN
    n = recv(socketId, (char*) data, data_size, 0);
#else
    n = ::read(socketId, data, data_size);
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

void _sock::send_packet(int socketId, const ERegisterPacket& packet) 
{
    send(socketId, (u8*) &packet.PacketType, sizeof(EPacketType));
    send(socketId, (u8*) &packet.ID, sizeof(ERegisterPacket::PackId));

    EString bodyString = packet.Body.dump();
    const char* data = bodyString.c_str();
    size_t dataLen = strlen(data);
    send(socketId, (u8*)&dataLen, sizeof(size_t));
    send(socketId, (u8*)data, dataLen);
}

void _sock::read_packet(int socketId, ERegisterPacket* outPacket) 
{
    EPacketType type;
    ERegisterPacket::PackId ID;
    read(socketId, (u8*) &type, sizeof(EPacketType));
    read(socketId, (u8*) &ID, sizeof(ERegisterPacket::PackId));

    size_t dataLen = 0;
    read(socketId, (u8*)&dataLen, sizeof(size_t));
    u8* bodyData = new u8[dataLen];
    memset(bodyData, 0, dataLen);
    read(socketId, bodyData, dataLen);

    EString bodyAsString = EString((char*)bodyData);
    outPacket->PacketType = type;
    outPacket->ID = ID;
    outPacket->Body = EJson::parse(bodyAsString);
}