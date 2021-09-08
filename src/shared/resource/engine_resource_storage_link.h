#pragma once

namespace Engine {

    class EResourceLink
    {
    public:
        EString Type;
        EResourceData::t_ID ResourceId;

        EResourceLink(const EString& type = "");
        ~EResourceLink() = default;

        bool operator==(const EResourceLink& other) const
        {
            return Type == other.Type && ResourceId == other.ResourceId;
        }

        bool operator!=(const EResourceLink& other) const
        {
            return !((*this) == other);
        }
    };

}



namespace convert
{
#ifdef EWIN
    extern template E_API bool setter<Engine::EResourceLink>(Engine::EStructProperty*, const Engine::EResourceLink&);
    extern template E_API bool getter<Engine::EResourceLink>(const Engine::EStructProperty*, Engine::EResourceLink*);
#else
    template <>
    setter<Engine::EResourceLink>(Engine::EStructProperty*, const Engine::EResourceLink&);
    template <>
    getter<Engine::EResourceLink>(const Engine::EStructProperty*, Engine::EResourceLink*);
#endif
}

namespace Engine { namespace getdsc {
#ifdef EWIN
    extern template E_API EValueDescription GetDescription<Engine::EResourceLink>();
#else
    template <>
    EValueDescription GetDescription<Engine::EResourceLink>();
#endif
} }