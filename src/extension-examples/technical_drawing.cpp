#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

EVector<RMesh::Vertex> planeVertices = {
    {{-50.0f,-50.0f, -1.0f}},
    {{ 50.0f,-50.0f, -1.0f}},
    {{ 50.0f, 50.0f, -1.0f}},
    {{-50.0f, 50.0f, -1.0f}},
};

EVector<u32> planeIndices = {
    0, 1, 2, 2, 3, 0
};

static EUnorderedMap<ERegister::Entity, RMesh*> meshes;

static EValueDescription TechnicalMeshDsc = EValueDescription::CreateStruct("TechnicalDrawing", {{"Positions", EVec3_dsc.GetAsArray()}});

static EValueDescription PlaneDescription = EValueDescription::CreateStruct("Plane", {{"Position", EVec3_dsc},
                                                                                      {"Rotation", EVec3_dsc},
                                                                                      {"Scale", EVec3_dsc}});

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
    /*ERegister::Entity newEntity = activeScene->CreateEntity();
    activeScene->AddComponent(newEntity, TechnicalMeshDsc);

    EStructProperty* technicalMesh = activeScene->GetComponent(newEntity, TechnicalMeshDsc);
    EArrayProperty* arrayProp = static_cast<EArrayProperty*>(technicalMesh->GetProperty("Positions"));
    EStructProperty* vector = static_cast<EStructProperty*>(arrayProp->AddElement());
    vector->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));
    data.CurrentEditPoint = static_cast<EStructProperty*>(arrayProp->AddElement());
    data.CurrentEditPoint->SetValue(EVec3(e.Position.x, e.Position.y, 0.0f));*/
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
    
    
    shared::StaticSharedContext::instance().Events().AddEntityChangeEventListener("Plane.Position", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Position"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetPosition(posVector);
        }
    });
    shared::StaticSharedContext::instance().Events().AddEntityChangeEventListener("Plane.Rotation", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Rotation"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetRotation(glm::vec3{glm::radians(posVector.x), glm::radians(posVector.y), glm::radians(posVector.z)});
        }
    });
    shared::StaticSharedContext::instance().Events().AddEntityChangeEventListener("Plane.Scale", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Scale"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetScale(posVector);
        }
    });

    shared::StaticSharedContext::instance().Events().AddComponentCreateEventListener(PlaneDescription, [drawingViewport](ERegister::Entity entity){
        // Create mesh in 3D Scene
        ERef<EStructProperty> mesh = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, PlaneDescription.GetId()));
        if (!mesh) { return; }
        if (drawingViewport.expired()) { return; }
        RMesh* gMesh = new RMesh();
        meshes[entity] = gMesh;
        gMesh->SetData(planeVertices, planeIndices);
        drawingViewport.lock()->GetScene().Add(gMesh);
        EProperty* pos = mesh->GetProperty("Position");
        EProperty* rot = mesh->GetProperty("Rotation");
        EProperty* sca = mesh->GetProperty("Scale");

        
        shared::SetValue<EVec3>(entity, "Plane.Scale", {1.0f, 1.0f, 1.0f});
    });




    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    info.GetTypeRegister().RegisterItem(extensionName, TechnicalMeshDsc);
    info.GetTypeRegister().RegisterItem(extensionName, PlaneDescription);
}