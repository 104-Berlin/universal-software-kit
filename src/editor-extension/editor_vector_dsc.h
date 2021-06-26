#pragma once

static Engine::EValueDescription dsc_Vec2 = Engine::EValueDescription::CreateStruct("Vec2", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec3 = Engine::EValueDescription::CreateStruct("Vec3", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}});
static Engine::EValueDescription dsc_Vec4 = Engine::EValueDescription::CreateStruct("Vec4", {{"X", Engine::DoubleDescription}, {"Y", Engine::DoubleDescription}, {"Z", Engine::DoubleDescription}, {"W", Engine::DoubleDescription}});

