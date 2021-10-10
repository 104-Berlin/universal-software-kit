#pragma once

namespace Editor {


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