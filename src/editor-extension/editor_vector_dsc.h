#pragma once


#define EVec2_dsc Engine::EValueDescription::CreateStruct("EVec2", {{"X", DoubleDescription}, {"Y", DoubleDescription}})
#define EVec3_dsc Engine::EValueDescription::CreateStruct("EVec3", {{"X", DoubleDescription}, {"Y", DoubleDescription}, {"Z", DoubleDescription}})
#define EVec4_dsc Engine::EValueDescription::CreateStruct("EVec4", {{"X", DoubleDescription}, {"Y", DoubleDescription}, {"Z", DoubleDescription}, {"W", DoubleDescription}})


#ifdef EWIN

namespace convert
{
    extern template E_EDEXAPI bool setter<EVec2>(Engine::EStructProperty*, const EVec2&);
    extern template E_EDEXAPI bool getter<EVec2>(const Engine::EStructProperty*, EVec2*);
    extern template E_EDEXAPI bool setter<EVec3>(Engine::EStructProperty*, const EVec3&);
    extern template E_EDEXAPI bool getter<EVec3>(const Engine::EStructProperty*, EVec3*);
    extern template E_EDEXAPI bool setter<EVec4>(Engine::EStructProperty*, const EVec4&);
    extern template E_EDEXAPI bool getter<EVec4>(const Engine::EStructProperty*, EVec4*);
}

namespace Engine { namespace getdsc {

    extern template E_EDEXAPI EValueDescription GetDescription<EVec2>();
    extern template E_EDEXAPI EValueDescription GetDescription<EVec3>();
    extern template E_EDEXAPI EValueDescription GetDescription<EVec4>();

}}
#else
namespace convert
{
    template <>
    bool setter<EVec2>(Engine::EStructProperty*, const EVec2&);
    template <>
    bool getter<EVec2>(const Engine::EStructProperty*, EVec2*);
    template <>
    bool setter<EVec3>(Engine::EStructProperty*, const EVec3&);
    template <>
    bool getter<EVec3>(const Engine::EStructProperty*, EVec3*);
    template <>
    bool setter<EVec4>(Engine::EStructProperty*, const EVec4&);
    template <>
    bool getter<EVec4>(const Engine::EStructProperty*, EVec4*);

}
namespace Engine { namespace getdsc {
    template <>
    EValueDescription GetDescription<EVec2>();
    template <>
    EValueDescription GetDescription<EVec3>();
    template <>
    EValueDescription GetDescription<EVec4>();
} }
#endif
