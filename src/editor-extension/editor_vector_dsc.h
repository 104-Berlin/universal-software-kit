#pragma once


#define EVec2_dsc Engine::EValueDescription::CreateStruct("EVec2", {{"X", DoubleDescription}, {"Y", DoubleDescription}})
#define EVec3_dsc Engine::EValueDescription::CreateStruct("EVec3", {{"X", DoubleDescription}, {"Y", DoubleDescription}, {"Z", DoubleDescription}})
#define EVec4_dsc Engine::EValueDescription::CreateStruct("EVec4", {{"X", DoubleDescription}, {"Y", DoubleDescription}, {"Z", DoubleDescription}, {"W", DoubleDescription}})


namespace ImConvert
{
    E_EDEXAPI ImVec2 GlmToImVec2(const EVec2& vector);
    E_EDEXAPI ImVec4 GlmToImVec4(const EVec4& vector);

    E_EDEXAPI EVec2 ImToGlmVec2(const ImVec2& vector);
    E_EDEXAPI EVec4 ImToGlmVec4(const ImVec4& vector);
}


#ifdef EWIN

namespace convert
{
    extern template E_EDEXAPI bool setter<EVec2>(Engine::EProperty*, const EVec2&);
    extern template E_EDEXAPI bool getter<EVec2>(const Engine::EProperty*, EVec2*);
    extern template E_EDEXAPI bool setter<EVec3>(Engine::EProperty*, const EVec3&);
    extern template E_EDEXAPI bool getter<EVec3>(const Engine::EProperty*, EVec3*);
    extern template E_EDEXAPI bool setter<EVec4>(Engine::EProperty*, const EVec4&);
    extern template E_EDEXAPI bool getter<EVec4>(const Engine::EProperty*, EVec4*);
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
    bool setter<EVec2>(Engine::EProperty*, const EVec2&);
    template <>
    bool getter<EVec2>(const Engine::EProperty*, EVec2*);
    template <>
    bool setter<EVec3>(Engine::EProperty*, const EVec3&);
    template <>
    bool getter<EVec3>(const Engine::EProperty*, EVec3*);
    template <>
    bool setter<EVec4>(Engine::EProperty*, const EVec4&);
    template <>
    bool getter<EVec4>(const Engine::EProperty*, EVec4*);

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
