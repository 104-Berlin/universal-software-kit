#pragma once

namespace Editor {

    struct E_EDEXAPI ESvgResource
    {
        EVector<ECurveSegment> CurveSegments;
        float                   Width;
        float                   Height;

        void FromBuffer(Engine::ESharedBuffer buffer);
    };

    namespace ResSvg {
        Engine::EResourceDescription::ResBuffer E_EDEXAPI ImportSvg(const Engine::EResourceDescription::RawBuffer data);
    }

}