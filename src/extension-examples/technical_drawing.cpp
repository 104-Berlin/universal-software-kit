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

void ViewportMouseMove(events::EMouseMoveEvent e)
{
}

void ViewportClicked(events::EMouseDownEvent e)
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
    
    drawingViewport->AddEventListener<events::EMouseDownEvent>(&ViewportClicked);
    drawingViewport->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);
    someDrawingPanel->AddChild(drawingViewport);

    activeScene->AddComponentCreateEventListener(TechnicalMeshDsc, [drawingViewport](EStructProperty* mesh){
        // Create mesh in 3D Scene
        drawingViewport->GetScene().Add(new GMesh());
    });


    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    activeScene = info.GetActiveScene();

    info.GetTypeRegister().RegisterItem(extensionName, TechnicalMeshDsc);
}