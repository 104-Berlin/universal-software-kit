#pragma once

namespace Editor {

    struct E_EDEXAPI EMeshResource
    {
        EVector<Renderer::RMesh::Vertex> Vertices;
        EVector<u32> Indices;

        void FromBuffer(Engine::ESharedBuffer buffer);
    };

    namespace ResMesh {
        Engine::EResourceDescription::ResBuffer E_EDEXAPI ImportMesh(const Engine::EResourceDescription::RawBuffer data);
    }


}
