#include "engine_extension.h"

EXTENSION_EXPORT_FUN void entry(const char* extensionName, Engine::EExtInitInfo* extension)
{
    ERef<Engine::EUIPanel> uiPanel = EMakeRef<Engine::EUIPanel>("First panel");

    extension->UIRegister->RegisterItem(extensionName, uiPanel);
}