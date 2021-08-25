#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

std::vector<Graphics::GMesh::Vertex> planeVertices = {
    {{-50.0f,-50.0f, -1.0f}},
    {{ 50.0f,-50.0f, -1.0f}},
    {{ 50.0f, 50.0f, -1.0f}},
    {{-50.0f, 50.0f, -1.0f}},
};

std::vector<u32> planeIndices = {
    0, 1, 2, 2, 3, 0
};

ERegister* activeScene = nullptr;

static EValueDescription TechnicalMeshDsc = EValueDescription::CreateStruct("TechnicalDrawing", {{"Positions", dsc_Vec3.GetAsArray()}});

static EValueDescription PlaneDescription = EValueDescription::CreateStruct("Plane", {{"Position", dsc_Vec3},
                                                                                      {"Rotation", dsc_Vec3},
                                                                                      {"Scale", dsc_Vec3}});

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
    EWeakRef<EUIViewport> drawingViewport = std::dynamic_pointer_cast<EUIViewport>(someDrawingPanel->AddChild(EMakeRef<EUIViewport>()));
    
    drawingViewport.lock()->AddEventListener<events::EMouseDownEvent>(&ViewportClicked);
    drawingViewport.lock()->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport.lock()->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);
    

    activeScene->AddComponentCreateEventListener(PlaneDescription, [drawingViewport](EStructProperty* mesh, ERegister::Entity entity){
        // Create mesh in 3D Scene
        if (drawingViewport.expired()) { return; }
        GMesh* gMesh = new GMesh();
        gMesh->SetData(planeVertices, planeIndices);
        drawingViewport.lock()->GetScene().Add(gMesh);
        EProperty* pos = mesh->GetProperty("Position");
        EProperty* rot = mesh->GetProperty("Rotation");
        EProperty* sca = mesh->GetProperty("Scale");
        if (pos)
        {
            /*pos->AddEventListener<events::EValueChangeEvent<EVec3>>([gMesh](const events::EValueChangeEvent<EVec3>& event){
                gMesh->SetPosition(event.NewValue);
            });*/
        }
        if (rot)
        {
            /*rot->AddEventListener<events::EValueChangeEvent<EVec3>>([gMesh](const events::EValueChangeEvent<EVec3>& event){
                glm::quat rot(event.NewValue);
                gMesh->SetRotation(rot);
            });*/
        }
        if (sca)
        {
            /*sca->AddEventListener<events::EValueChangeEvent<EVec3>>([gMesh](const events::EValueChangeEvent<EVec3>& event){
                gMesh->SetScale(event.NewValue);
            });*/
        }
    });



    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    activeScene = info.GetActiveScene();

    info.GetTypeRegister().RegisterItem(extensionName, TechnicalMeshDsc);
    info.GetTypeRegister().RegisterItem(extensionName, PlaneDescription);
}