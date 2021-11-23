#pragma once


#define EVec3_dsc Engine::EValueDescription::CreateStruct("EVec3", {{"X", DoubleDescription}, {"Y", DoubleDescription}, {"Z", DoubleDescription}})


namespace ImConvert
{
    E_EDEXAPI ImVec2 GlmToImVec2(const EVec3& vector);
    E_EDEXAPI ImVec4 GlmToImVec4(const EVec3& vector);

    E_EDEXAPI EVec3 ImToGlmVec2(const ImVec2& vector);
    E_EDEXAPI EVec3 ImToGlmVec4(const ImVec4& vector);
}


#ifdef EWIN

namespace convert
{
    extern template E_EDEXAPI bool setter<EVec3>(Engine::EProperty*, const EVec3&);
    extern template E_EDEXAPI bool getter<EVec3>(const Engine::EProperty*, EVec3*);
}

namespace Engine { namespace getdsc {

    extern template E_EDEXAPI EValueDescription GetDescription<EVec3>();

}}
#else
namespace convert
{
    template <>
    bool setter<EVec3>(Engine::EProperty*, const EVec3&);
    template <>
    bool getter<EVec3>(const Engine::EProperty*, EVec3*);

}
namespace Engine { namespace getdsc {
    template <>
    EValueDescription GetDescription<EVec3>();
} }
#endif
