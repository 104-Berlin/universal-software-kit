#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    ERef<EUIViewport> viewport = EMakeRef<EUIViewport>();

    RScene& scene = viewport->GetScene();
    RMesh* model = new RMesh();
    scene.Add(model);


    showPanel->AddChild(viewport);
    info.PanelRegister->RegisterItem(extensionName, showPanel);
}