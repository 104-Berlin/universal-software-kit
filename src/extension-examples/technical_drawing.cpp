#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

ERegister* activeScene = nullptr;

static EValueDescription TechnicalMeshDsc = EValueDescription::CreateStruct("TechnicalDrawing", {{"Positions", dsc_Vec3.GetAsArray()}});

struct ExtensionData
{
    RCamera Camera = RCamera(ECameraMode::ORTHOGRAPHIC);
    EStructProperty* CurrentEditPoint = nullptr;
};

static ExtensionData data{};

void RenderSimpleViewport(GContext* context, GFrameBuffer* frameBuffer)
{
    context->Clear();
    RRenderer3D renderer(context);
    renderer.Begin(frameBuffer, &(data.Camera));
    for (const auto& entry : activeScene->View(TechnicalMeshDsc))
    {
        EArrayProperty* pointArray = static_cast<EArrayProperty*>(entry.second->GetProperty("Positions"));
        EStructProperty* beginPoint = static_cast<EStructProperty*>(pointArray->GetElement(0));
        EStructProperty* endPoint = static_cast<EStructProperty*>(pointArray->GetElement(1));
        GLine line;
        line.SetStart(beginPoint->GetValue<EVec3>());
        line.SetEnd(endPoint->GetValue<EVec3>());
        //renderer.Submit(&line);
    }
    renderer.End();
}

void ViewportMouseMove(events::EMouseMoveEvent e)
{
}

void ViewportClicked(events::EMouseClickEvent e)
{
    ERegister::Entity newEntity = activeScene->CreateEntity();
    activeScene->InsertComponent(newEntity, TechnicalMeshDsc);

    EStructProperty* technicalMesh = activeScene->GetComponent(newEntity, TechnicalMeshDsc);
    EArrayProperty* arrayProp = static_cast<EArrayProperty*>(technicalMesh->GetProperty("Positions"));
    EStructProperty* vector = static_cast<EStructProperty*>(arrayProp->AddElement());
    vector->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));
    data.CurrentEditPoint = static_cast<EStructProperty*>(arrayProp->AddElement());
    data.CurrentEditPoint->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));
}

void ViewportDrag(events::EMouseDragEvent e)
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
    drawingViewport->AddEventListener<events::EMouseClickEvent>(&ViewportClicked);
    drawingViewport->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);
    someDrawingPanel->AddChild(drawingViewport);


    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    activeScene = info.GetActiveScene();

    info.GetTypeRegister().RegisterItem(extensionName, TechnicalMeshDsc);
}