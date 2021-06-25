#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

EScene* activeScene = nullptr;

static EValueDescription TechnicalMeshDsc = EValueDescription::CreateStruct("TechnicalDrawing", {{"Positions", dsc_Vec3.GetAsArray()}});

struct ExtensionData
{
    RCamera Camera = RCamera(ECameraMode::ORTHOGRAPHIC);
    EStructProperty* CurrentEditPoint = nullptr;

    EVector<RLine> drawingLines;
};

static ExtensionData data{};

void RenderSimpleViewport(GContext* context, GFrameBuffer* frameBuffer)
{
    data.drawingLines.clear();
    context->Clear();
    RRenderer3D renderer(context);
    renderer.Begin(frameBuffer, &(data.Camera));
    for (const auto& entry : activeScene->View(TechnicalMeshDsc))
    {
        EArrayProperty* pointArray = static_cast<EArrayProperty*>(entry.second->GetProperty("Positions"));
        EStructProperty* beginPoint = static_cast<EStructProperty*>(pointArray->GetElement(0));
        EStructProperty* endPoint = static_cast<EStructProperty*>(pointArray->GetElement(1));
        RLine line;
        line.SetStart(beginPoint->GetValue<EVec3>());
        line.SetEnd(endPoint->GetValue<EVec3>());
        data.drawingLines.push_back(line);
        renderer.Submit(&data.drawingLines.back());
    }
    renderer.End();
}

void ViewportMouseMove(EMouseMoveEvent e)
{
}

void ViewportClicked(EMouseClickEvent e)
{
    EScene::Entity newEntity = activeScene->CreateEntity();
    activeScene->InsertComponent(newEntity, TechnicalMeshDsc);

    EStructProperty* technicalMesh = activeScene->GetComponent(newEntity, TechnicalMeshDsc);
    EArrayProperty* arrayProp = static_cast<EArrayProperty*>(technicalMesh->GetProperty("Positions"));
    EStructProperty* vector = static_cast<EStructProperty*>(arrayProp->AddElement());
    vector->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));
    data.CurrentEditPoint = static_cast<EStructProperty*>(arrayProp->AddElement());
}

void ViewportDrag(EMouseDragEvent e)
{
    if (data.CurrentEditPoint)
    {
        data.CurrentEditPoint->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));    
    }
}


APP_ENTRY
{
    ERef<EUIPanel> someDrawingPanel = EMakeRef<EUIPanel>("Drawing Canvas");
    ERef<EUIViewport> drawingViewport = EMakeRef<EUIViewport>();
    
    drawingViewport->SetRenderFunction(&RenderSimpleViewport);
    drawingViewport->AddEventListener<EMouseClickEvent>(&ViewportClicked);
    drawingViewport->AddEventListener<EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport->AddEventListener<EMouseDragEvent>(&ViewportDrag);
    someDrawingPanel->AddChild(drawingViewport);


    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    activeScene = info.GetActiveScene();

    info.GetTypeRegister().RegisterItem(extensionName, TechnicalMeshDsc);
}