#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    
    

    info.PanelRegister->RegisterItem(extensionName, showPanel);
}

EXT_ENTRY
{

}