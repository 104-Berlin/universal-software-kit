#include "editor_extension.h"

using namespace Engine;

EUIViewportManager::EUIViewportManager(EUIRegister* uiRegister)
    : fUIRegister(uiRegister)
{
    shared::Events().Connect<events::EExtensionUnloadEvent>([this](events::EExtensionUnloadEvent e){
        if (fViewportRenderMap.find(e.ExtensionName) != fViewportRenderMap.end())
        {
            fViewportRenderMap.erase(e.ExtensionName);
        }
    });
}

void EUIViewportManager::AddViewportDescription(const EViewportDescription& description, TRenderFunction renderFunction)
{
    fViewports[description.ExtensionName] = EUIViewportRenderFunction{ description, renderFunction };

    ERef<EUIViewport> viewport = nullptr;
    if (fViewports.find(description.Type.Value) != fViewports.end())
    {
        if (!fViewports[description.Type.Value].expired())
        {
            viewport = fViewports[description.Type.Value].lock();
        }
    }

    if (!viewport)
    {
        ERef<EUIPanel> panel = EMakeRef<EUIPanel>("Viewport");
        viewport = CreateViewport(description.Type);
        panel->AddChild(viewport);
    }
}

ERef<EUIViewport> EUIViewportManager::CreateViewport(const EViewportType& type)
{
    switch (type.Value)
    {
    case EViewportType::DEFAULT_3D:
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::PERSPECTIVE));
    case EViewportType::DEFAULT_2D:
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::ORTHOGRAPHIC));
    case EViewportType::FRONT_RIGHT_TOP_3D:
    {
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::PERSPECTIVE));
    }
    }
    return nullptr;
}
