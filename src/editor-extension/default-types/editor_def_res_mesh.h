#pragma once

namespace Editor {

    struct EMeshUserData
    {
        char Name[255];
        int VerticesCount;
        int IndicesCount;

    };

    namespace ResMesh {
        Engine::EResourceDescription::ResBuffer E_EDEXAPI ImportMesh(const Engine::EResourceDescription::RawBuffer data);
    }


}
