#pragma once

namespace Engine {


    enum class EPacketType : u32
    {
        CREATE_ENTITY = 0,
        CREATE_COMPONENT = 1,

        SET_VALUE = 2,

        GET_VALUE = 20,

        REGISTER_EVENT = 40,
    };

    struct ERegisterPacket
    {
        using PackId = u32;
        EPacketType PacketType;
        PackId      ID;
        EJson       Body;
    };


    namespace _sock {
        void close(int socketId);
        int read(int socketId, u8* data, size_t data_size);
        int send(int socketId, const u8* data, size_t data_size);

        void send_packet(int socketId, const ERegisterPacket& packet);
        void read_packet(int socketId, ERegisterPacket* outPacket);
    }
}
