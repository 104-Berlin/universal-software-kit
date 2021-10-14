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

static EUnorderedMap<ERegister::Entity, EUnorderedMap<EValueDescription::t_ID, RMesh*>> meshes;

E_STORAGE_STRUCT(Plane,
    (EVec3, Position),
    (EVec3, Rotation),
    (EVec3, Scale)
)

E_STORAGE_STRUCT(Line, 
    (float, Thickness, 8.0f),
    (EVec3, Start),
    (EVec3, End)
)

E_STORAGE_STRUCT(TechnicalMesh,
    (EVector<EVec3>, Positions)
)

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
    EWeakRef<EUIViewport> drawingViewport = std::dynamic_pointer_cast<EUIViewport>(someDrawingPanel->AddChild(EMakeRef<EUIViewport>()).lock());
    
    drawingViewport.lock()->AddEventListener<events::EMouseDownEvent>(&ViewportClicked);
    drawingViewport.lock()->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport.lock()->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);
    
    shared::Events().AddEntityChangeEventListener(Line::_dsc.GetId(), [](ERegister::Entity entity, const EString& nameIdent){
        Line l = shared::GetValue<Line>(entity);
        RLine* entityLine = (RLine*) meshes[entity][Line::_dsc.GetId()];
        if (entityLine)
        {
            entityLine->SetStart(l.Start);
            entityLine->SetEnd(l.End);
            entityLine->SetThickness(l.Thickness);
        }
    }, drawingViewport.lock().get());

    shared::Events().AddComponentCreateEventListener(Line::_dsc, [drawingViewport](ERegister::Entity entity){
        Line line = shared::GetValue<Line>(entity);
        RLine* newLine = new RLine();
        meshes[entity][Line::_dsc.GetId()] = newLine;
        newLine->SetStart(line.Start);
        newLine->SetEnd(line.End);
        drawingViewport.lock()->GetScene().Add(newLine);
    }, drawingViewport.lock().get());

    
    shared::Events().AddEntityChangeEventListener("Plane.Position", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Position"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetPosition(posVector);
        }
    }, drawingViewport.lock().get());
    shared::Events().AddEntityChangeEventListener("Plane.Rotation", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Rotation"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetRotation(glm::vec3{glm::radians(posVector.x), glm::radians(posVector.y), glm::radians(posVector.z)});
        }
    }, drawingViewport.lock().get());
    shared::Events().AddEntityChangeEventListener("Plane.Scale", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValue(entity, "Plane.Scale"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetScale(posVector);
        }
    }, drawingViewport.lock().get());

    shared::Events().AddComponentCreateEventListener(Plane::_dsc, [drawingViewport](ERegister::Entity entity){
        // Create mesh in 3D Scene
        if (drawingViewport.expired()) { return; }
        Plane mesh = shared::GetValue<Plane>(entity);
        RMesh* gMesh = new RMesh();
        meshes[entity][Plane::_dsc.GetId()] = gMesh;
        gMesh->SetData(planeVertices, planeIndices);
        drawingViewport.lock()->GetScene().Add(gMesh);

        if (mesh.Scale.length() == 0)
        {
            shared::SetValue<EVec3>(entity, "Plane.Scale", {1.0f, 1.0f, 1.0f});
        }
    }, drawingViewport.lock().get());




    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    info.GetComponentRegister().RegisterStruct<TechnicalMesh>(extensionName);
    info.GetComponentRegister().RegisterStruct<Plane>(extensionName);
    info.GetComponentRegister().RegisterStruct<Line>(extensionName);
}