#include "engine_extension.h"

using namespace Engine;

static EComponentDescription dsc("ExmapleComponent", {
    {EValueType::DOUBLE, "Height"},
    {EValueType::STRING, "HeightType"}
});

EXT_ENTRY
{
    E_INFO("Init example 2");
    scene->RegisterComponent(dsc);
}