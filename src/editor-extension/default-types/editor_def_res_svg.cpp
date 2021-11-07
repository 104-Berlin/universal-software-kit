#include "editor_extension.h"

using namespace Editor;
using namespace Engine;

Engine::EResourceDescription::ResBuffer ResSvg::ImportSvg(const Engine::EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer res;
    res.Data = new u8[data.Size];
    memcpy(res.Data, data.Data, data.Size);
    res.Size = data.Size;
    return res;
}
