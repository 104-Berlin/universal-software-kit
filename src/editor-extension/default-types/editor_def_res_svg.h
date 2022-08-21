#pragma once

namespace Editor {

    struct E_EDEXAPI ESvgResource
    {
        
    };

    namespace ResSvg {
        Engine::EResourceDescription::ResBuffer E_EDEXAPI ImportSvg(const Engine::EResourceDescription::RawBuffer data);
    }

}