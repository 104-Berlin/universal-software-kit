#include "prefix_interface.h"

using namespace Engine;

void _sock::send_packet(int socketId, const ERegisterPacket& packet) 
{
    Socket::Send(socketId, (u8*) &packet.PacketType, sizeof(EPacketType));
    Socket::Send(socketId, (u8*) &packet.ID, sizeof(ERegisterPacket::PackId));

    EString bodyString = packet.Body.dump();
    const char* data = bodyString.c_str();
    size_t dataLen = strlen(data) + 1;
    Socket::Send(socketId, (u8*)&dataLen, sizeof(size_t));
    Socket::Send(socketId, (u8*)data, dataLen);
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


void _sock::print_packet(const EString& name, const ERegisterPacket& packet) 
{
    E_INFO("------" + name + " PACKET -------");           
    E_INFO("| ID   =  " + std::to_string(packet.ID) + "  |");
    E_INFO(EString("| TYPE = | ") + GetPacketTypeString(packet.PacketType) + " |");
    E_INFO("---------------------");
    E_INFO("|       Body        |");
    E_INFO(packet.Body.dump());
    E_INFO("---------------------");
}