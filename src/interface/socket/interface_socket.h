#pragma once

namespace Engine {


    enum class EPacketType : u32
    {
        CREATE_ENTITY = 0,
        CREATE_COMPONENT = 1,

        SET_VALUE = 2,

        GET_VALUE = 20,
        GET_ALL_VALUES = 21,

        REGISTER_EVENT = 40,
    };

    static const char* GetPacketTypeString(EPacketType type)
    {
        switch (type)
        {
            case EPacketType::CREATE_ENTITY: return "CREATE_ENTITY";
            case EPacketType::CREATE_COMPONENT: return "CREATE_COMPONENT";
            case EPacketType::SET_VALUE: return "SET_VALUE";
            case EPacketType::GET_VALUE: return "GET_VALUE";
            case EPacketType::GET_ALL_VALUES: return "GET_ALL_VALUES";
            case EPacketType::REGISTER_EVENT: return "REGISTER_EVENT";
        }
        return "";
    }

    struct ERegisterPacket
    {
        using PackId = u32;
        EPacketType PacketType = EPacketType::REGISTER_EVENT;
        PackId      ID = 0;
        EJson       Body = EJson::object();
    };


    namespace _sock {
        void close(int socketId);
        int read(int socketId, u8* data, size_t data_size);
        int send(int socketId, const u8* data, size_t data_size);

        void send_packet(int socketId, const ERegisterPacket& packet);
        int read_packet(int socketId, ERegisterPacket* outPacket);

        void print_last_socket_error();
        void print_packet(const EString& name, const ERegisterPacket& packet);
    }
}
