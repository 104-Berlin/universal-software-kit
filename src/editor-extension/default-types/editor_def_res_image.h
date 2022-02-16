#pragma once

namespace Editor {

    struct EImageResource
    {
        int Width;
        int Height;
        int Channels;
        u8* Data;

        void FromBuffer(Engine::ESharedBuffer buffer);
    };

    struct EImageUserData
    {
        int width;
        int height;
        int channels;
    };

    namespace ResImage {
        Engine::EResourceDescription::ResBuffer E_EDEXAPI ImportImage(const Engine::EResourceDescription::RawBuffer data);
    }

}