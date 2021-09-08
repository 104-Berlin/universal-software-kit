#include "editor_extension.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace Engine;
using namespace Graphics;
using namespace Renderer;

EResourceDescription::ResBuffer ImportImage(const EResourceDescription::ResBuffer data)
{
    EResourceDescription::ResBuffer result;
    int x, y, n;
    result.Data = stbi_load_from_memory(data.Data, data.Size, &x, &y, &n, 4);
    result.Size = x * y * 4;
    return result;
}

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    
    

    info.PanelRegister->RegisterItem(extensionName, showPanel);
}

EXT_ENTRY
{
    EResourceDescription imageDsc("Image",{"png", "jpeg", "bmp"});
    imageDsc.ImportFunction = &ImportImage;

    info.GetResourceRegister().RegisterItem(extensionName, imageDsc);
}