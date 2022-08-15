#pragma once

namespace Engine {

    struct E_INTER_API ETxtResource
    {
        EString String;

        void FromBuffer(Engine::ESharedBuffer buffer);
    };
    
}
