#pragma once

namespace Engine {

    class E_API EResourceLink
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
    extern template E_API bool setter<Engine::EResourceLink>(Engine::EProperty*, const Engine::EResourceLink&);
    extern template E_API bool getter<Engine::EResourceLink>(const Engine::EProperty*, Engine::EResourceLink*);
#else
    template <>
    bool setter<Engine::EResourceLink>(Engine::EProperty*, const Engine::EResourceLink&);
    template <>
    bool getter<Engine::EResourceLink>(const Engine::EProperty*, Engine::EResourceLink*);
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