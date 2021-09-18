#pragma once

namespace Engine {

    enum class ESocketEvent : u32
    {
        CREATE_ENTITY = 0,
        CREATE_COMPONENT = 1,

        SET_VALUE = 2,

        GET_VALUE = 20
    };

    struct ESocketEvent_CreateComponent
    {
        ERegister::Entity Entity;
        char              Component[128];
    };

}
