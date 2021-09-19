#pragma once

namespace Engine {


    namespace _sock {
        void close(int socketId);
        int read(int socketId, u8* data, size_t data_size);
        int send(int socketId, const u8* data, size_t data_size);
    }


    enum class ESocketEvent : u32
    {
        CREATE_ENTITY = 0,
        CREATE_COMPONENT = 1,

        SET_VALUE = 2,

        GET_VALUE = 20,

        REGISTER_EVENT = 40,
    };

}
