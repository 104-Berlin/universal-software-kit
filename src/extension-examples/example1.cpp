#define EXT_WITH_DEFAULT_INITS
#include "engine_extension.h"

EXT_ENTRY
{
    E_INFO(EString("Initiliazing ") + extensionName);
    ERef<Engine::EUIPanel> uiPanel = EMakeRef<Engine::EUIPanel>("First panel");

    extension->UIRegister->RegisterItem(extensionName, uiPanel);
}