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
static EWeakRef<EUIViewport> drawingViewport;
static EBezierEditTool* bezierEdit = nullptr;
static ELineEditTool* lineEdit = nullptr;
static ERegister::Entity currentEditCurveEntity = 0;
static ERegister::Entity currentEditLineEntity = 0;

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

E_STORAGE_STRUCT(Curve,
    (float, Thickness, 5.0f),
    (EVec3, Start, 200.0f, 200.0f, 0.0f),
    (EVec3, End, 400.0f, 200.0f, 0.0f),
    (EVec3, Controll1, 200.0f, 100.0f, 0.0f),
    (EVec3, Controll2, 400.0f, 100.0f, 0.0f),
    (int, Steps, 10)
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

void ViewportToolFinish(events::EViewportToolFinishEvent event)
{
    if (!currentEditCurveEntity) { return; }
    Curve currentCurve;
    if (shared::GetValue<Curve>(currentEditCurveEntity, &currentCurve))
    {
        currentCurve.Start = bezierEdit->GetCurve()->GetStartPos();
        currentCurve.End = bezierEdit->GetCurve()->GetEndPos();
        currentCurve.Controll1 = bezierEdit->GetCurve()->GetControll1();
        currentCurve.Controll2 = bezierEdit->GetCurve()->GetControll2();
        shared::SetValue<Curve>(currentEditCurveEntity, "Curve", currentCurve);
    }
}


APP_ENTRY
{
    ERef<EUIPanel> someDrawingPanel = EMakeRef<EUIPanel>("Drawing Canvas");
    drawingViewport = std::dynamic_pointer_cast<EUIViewport>(someDrawingPanel->AddChild(EMakeRef<EUIViewport>()).lock());
    bezierEdit = static_cast<EBezierEditTool*>(drawingViewport.lock()->AddTool(new EBezierEditTool()));
    lineEdit = static_cast<ELineEditTool*>(drawingViewport.lock()->AddTool(new ELineEditTool()));
    
    drawingViewport.lock()->AddEventListener<events::EMouseDownEvent>(&ViewportClicked);
    drawingViewport.lock()->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport.lock()->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);

    drawingViewport.lock()->AddEventListener<events::EViewportToolFinishEvent>(&ViewportToolFinish);
    
    shared::Events().AddEntityChangeEventListener(Line::_dsc.GetId(), [](ERegister::Entity entity, const EString& nameIdent){
        Line l;
        if (shared::GetValue<Line>(entity, &l))
        {
            RLine* entityLine = (RLine*) meshes[entity][Line::_dsc.GetId()];
            if (entityLine)
            {
                entityLine->SetStart(l.Start);
                entityLine->SetEnd(l.End);
                entityLine->SetThickness(l.Thickness);
            }
        }
    }, drawingViewport.lock().get());

    shared::Events().AddComponentCreateEventListener(Line::_dsc, [](ERegister::Entity entity){
        Line line;
        if (shared::GetValue<Line>(entity, &line))
        {
            RLine* newLine = new RLine();
            meshes[entity][Line::_dsc.GetId()] = newLine;
            newLine->SetStart(line.Start);
            newLine->SetEnd(line.End);

            lineEdit->SetLine(newLine);
            currentEditLineEntity = entity;
            drawingViewport.lock()->GetScene().Add(newLine);
        }
    }, drawingViewport.lock().get());


    shared::Events().AddEntityChangeEventListener(Curve::_dsc.GetId(), [](ERegister::Entity entity, const EString& nameIdent){
        Curve l;
        if (shared::GetValue<Curve>(entity, &l))
        {
            RBezierCurve* entityLine = (RBezierCurve*) meshes[entity][Curve::_dsc.GetId()];
            if (entityLine)
            {
                entityLine->SetStartPos(l.Start);
                entityLine->SetEndPos(l.End);
                entityLine->SetThickness(l.Thickness);
                entityLine->SetControll1(l.Controll1);
                entityLine->SetControll2(l.Controll2);
                entityLine->SetSteps(l.Steps);
            }
        }
    }, drawingViewport.lock().get());

    shared::Events().AddComponentCreateEventListener(Curve::_dsc, [](ERegister::Entity entity){
        Curve line;
        if (shared::GetValue<Curve>(entity, &line))
        {
            RBezierCurve* newLine = new RBezierCurve();
            meshes[entity][Curve::_dsc.GetId()] = newLine;
            newLine->SetStartPos(line.Start);
            newLine->SetEndPos(line.End);
            newLine->SetControll1(line.Controll1);
            newLine->SetControll2(line.Controll2);
            newLine->SetSteps(line.Steps);
            
            bezierEdit->SetBezierCurve(newLine);
            currentEditCurveEntity = entity;
            drawingViewport.lock()->GetScene().Add(newLine);
        }
    }, drawingViewport.lock().get());

    
    shared::Events().AddEntityChangeEventListener("Plane.Position", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValueFromIdent(entity, "Plane.Position"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetPosition(posVector);
        }
    }, drawingViewport.lock().get());
    shared::Events().AddEntityChangeEventListener("Plane.Rotation", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValueFromIdent(entity, "Plane.Rotation"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetRotation(glm::vec3{glm::radians(posVector.x), glm::radians(posVector.y), glm::radians(posVector.z)});
        }
    }, drawingViewport.lock().get());
    shared::Events().AddEntityChangeEventListener("Plane.Scale", [](ERegister::Entity entity, const EString& ident){
        RMesh* graphicsMesh = meshes[entity][Plane::_dsc.GetId()];
        if (!graphicsMesh) { return; }
        ERef<EStructProperty> pos = std::dynamic_pointer_cast<EStructProperty>(shared::GetValueFromIdent(entity, "Plane.Scale"));
        EVec3 posVector;
        if (pos->GetValue<EVec3>(posVector))
        {
            graphicsMesh->SetScale(posVector);
        }
    }, drawingViewport.lock().get());

    shared::Events().AddComponentCreateEventListener(Plane::_dsc, [](ERegister::Entity entity){
        // Create mesh in 3D Scene
        if (drawingViewport.expired()) { return; }
        Plane mesh;
        if (shared::GetValue<Plane>(entity, &mesh)){
            RMesh* gMesh = new RMesh();
            meshes[entity][Plane::_dsc.GetId()] = gMesh;
            gMesh->SetData(planeVertices, planeIndices);
            drawingViewport.lock()->GetScene().Add(gMesh);

            if (mesh.Scale.length() == 0)
            {
                shared::SetValue<EVec3>(entity, "Plane.Scale", {1.0f, 1.0f, 1.0f});
            }
        }
    }, drawingViewport.lock().get());


    ERef<EUIPanel> toolPanel = EMakeRef<EUIPanel>("Toolbar");
    
    toolPanel->AddChild(EMakeRef<EUIViewportToolbar>(drawingViewport));


    info.PanelRegister->RegisterItem(extensionName, toolPanel);
    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    info.GetComponentRegister().RegisterStruct<TechnicalMesh>(extensionName);
    info.GetComponentRegister().RegisterStruct<Plane>(extensionName);
    info.GetComponentRegister().RegisterStruct<Line>(extensionName);
    info.GetComponentRegister().RegisterStruct<Curve>(extensionName);
}
