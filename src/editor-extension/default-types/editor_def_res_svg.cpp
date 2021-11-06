#include "editor_extension.h"

using namespace Editor;
using namespace Engine;

Engine::EResourceDescription::ResBuffer ImportSvg(const Engine::EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer res;
    res.Data = new u8[data.Size];
    res.Size = data.Size;
    return res;
}
