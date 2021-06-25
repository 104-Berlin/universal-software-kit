#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

struct ExtensionData
{
    RMesh RenderingMesh;

    // Mesh data
    EVector<EVec3> currentPositions;
};

void RenderSimpleViewport(GContext* context, GFrameBuffer* frameBuffer)
{
    context->Clear();
}

void ViewportMouseMove(EMouseMoveEvent e)
{
    E_INFO("MouseMove: " + std::to_string(e.MouseDelta.x) + ", " + std::to_string(e.MouseDelta.y));
    E_INFO("MovePos: " + std::to_string(e.Position.x) + ", " + std::to_string(e.Position.y));
}

void ViewportClicked(EMouseClickEvent e)
{
    E_INFO("Button: " + std::to_string((int)e.MouseButton));
    E_INFO("Pos: " + std::to_string(e.Position.x) + ", " + std::to_string(e.Position.y));
}


APP_ENTRY
{
    ERef<EUIPanel> someDrawingPanel = EMakeRef<EUIPanel>("Drawing Canvas");
    ERef<EUIViewport> drawingViewport = EMakeRef<EUIViewport>();
    
    drawingViewport->SetRenderFunction(&RenderSimpleViewport);
    drawingViewport->AddEventListener<EMouseClickEvent>(&ViewportClicked);
    drawingViewport->AddEventListener<EMouseMoveEvent>(&ViewportMouseMove);
    someDrawingPanel->AddChild(drawingViewport);


    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}