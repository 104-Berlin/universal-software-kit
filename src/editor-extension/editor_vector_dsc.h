#pragma once

static Engine::EValueDescription dsc_Vec2 = Engine::EValueDescription::CreateStruct("Vec2", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec3 = Engine::EValueDescription::CreateStruct("Vec3", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec4 = Engine::EValueDescription::CreateStruct("Vec4", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}, {"W", Engine::DoubleDescription}});

namespace convert
{
    extern template E_EDEXAPI bool setter<EVec2>(Engine::EStructProperty*, const EVec2&);
    extern template E_EDEXAPI bool getter<EVec2>(const Engine::EStructProperty*, EVec2*);
    extern template E_EDEXAPI bool setter<EVec3>(Engine::EStructProperty*, const EVec3&);
    extern template E_EDEXAPI bool getter<EVec3>(const Engine::EStructProperty*, EVec3*);
    extern template E_EDEXAPI bool setter<EVec4>(Engine::EStructProperty*, const EVec4&);
    extern template E_EDEXAPI bool getter<EVec4>(const Engine::EStructProperty*, EVec4*);
}
