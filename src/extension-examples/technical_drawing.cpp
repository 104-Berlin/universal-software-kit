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

static EUnorderedMap<EDataBase::Entity, EUnorderedMap<EValueDescription::t_ID, RMesh*>> meshes;
static EWeakRef<EUIViewport> drawingViewport;
static EBezierEditTool* bezierEdit = nullptr;
static ELineEditTool* lineEdit = nullptr;
static EDataBase::Entity currentEditCurveEntity = 0;
static EDataBase::Entity currentEditLineEntity = 0;

E_STORAGE_STRUCT(Plane,
    (EVec3, Position),
    (EVec3, Rotation),
    (EVec3, Scale)
)

E_STORAGE_STRUCT(LinePositions,
    (EVec3, Start, 200.0f, 200.0f, 0.0f),
    (EVec3, End, 200.0f, 500.0f, 0.0f)
)

E_STORAGE_STRUCT(Line, 
    (float, Thickness, 8.0f),
    (LinePositions, Positions)
)

E_STORAGE_STRUCT(CurvePositions,
    (EVec3, Start, 200.0f, 200.0f, 0.0f),
    (EVec3, End, 400.0f, 200.0f, 0.0f),
    (EVec3, Controll1, 200.0f, 100.0f, 0.0f),
    (EVec3, Controll2, 400.0f, 100.0f, 0.0f)
)

E_STORAGE_STRUCT(Curve,
    (float, Thickness, 5.0f),
    (CurvePositions, Positions),
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
    if (event.ToolName == EBezierEditTool::sGetName()) 
    { 
        EBezierEditTool* editTool = static_cast<EBezierEditTool*>(drawingViewport.lock()->GetActiveTool());
        if (editTool)
        {
            CurvePositions newPositions;
            newPositions.Start = editTool->GetCurve()->GetStartPos();
            newPositions.End = editTool->GetCurve()->GetEndPos();
            newPositions.Controll1 = editTool->GetCurve()->GetControll1();
            newPositions.Controll2 = editTool->GetCurve()->GetControll2();
            shared::SetValue<CurvePositions>(currentEditCurveEntity, "Curve.Positions", newPositions);
        }
    }
    else if (event.ToolName == ELineEditTool::sGetName()) 
    { 
        ELineEditTool* editTool = static_cast<ELineEditTool*>(drawingViewport.lock()->GetActiveTool());
        if (editTool)
        {
            LinePositions newPosition;
            newPosition.Start = editTool->GetLine()->GetStart();
            newPosition.End = editTool->GetLine()->GetEnd();
            shared::SetValue<LinePositions>(currentEditLineEntity, "Line.Positions", newPosition);
        }
    }
}


APP_ENTRY
{
    ERef<EUIPanel> someDrawingPanel = EMakeRef<EUIPanel>("Drawing Canvas");
    drawingViewport = std::dynamic_pointer_cast<EUIViewport>(someDrawingPanel->AddChild(EMakeRef<EUIViewport>()).lock());
    drawingViewport.lock()->SetCameraControls<EUIBasic2DCameraControls>();
    someDrawingPanel->SetMenuBar(EMakeRef<EUIViewportToolbar>(drawingViewport));
    bezierEdit = static_cast<EBezierEditTool*>(drawingViewport.lock()->AddTool(new EBezierEditTool()));
    lineEdit = static_cast<ELineEditTool*>(drawingViewport.lock()->AddTool(new ELineEditTool()));
    
    drawingViewport.lock()->AddEventListener<events::EMouseDownEvent>(&ViewportClicked);
    drawingViewport.lock()->AddEventListener<events::EMouseMoveEvent>(&ViewportMouseMove);
    drawingViewport.lock()->AddEventListener<events::EMouseDragEvent>(&ViewportDrag);

    drawingViewport.lock()->AddEventListener<events::EViewportToolFinishEvent>(&ViewportToolFinish);
    
    shared::Events().Connect<EntityChangeEvent>([](EntityChangeEvent e){
        if (e.Type == EntityChangeType::COMPONENT_CHANGED)
        {
            ComponentChangeData data;
            if (convert::getter(e.Data.Value().get(), &data))
            {
                if (data.Identifier.rfind("Plane", 0) == 0)
                {
                    Plane plane;
                    if (convert::getter(data.NewValue.Value().get(), &plane))
                    {
                        RMesh* graphicsMesh = meshes[e.Entity.Handle][Plane::_dsc.GetId()];

                        graphicsMesh->SetPosition(plane.Position);
                        graphicsMesh->SetRotation(plane.Rotation);
                        graphicsMesh->SetScale(plane.Scale);
                    }
                }
                else if (data.Identifier.rfind("Line", 0) == 0)
                {
                    Line line;
                    if (convert::getter(data.NewValue.Value().get(), &line))
                    {
                        RLine* entityLine = (RLine*) meshes[e.Entity.Handle][Line::_dsc.GetId()];

                        entityLine->SetThickness(line.Thickness);
                        entityLine->SetStart(line.Positions.Start);
                        entityLine->SetEnd(line.Positions.End);
                    }
                }
                else if (data.Identifier.rfind("Curve", 0) == 0)
                {
                    Curve curve;
                    if (convert::getter(data.NewValue.Value().get(), &curve))
                    {
                        RBezierCurve* entityCurve = (RBezierCurve*) meshes[e.Entity.Handle][Curve::_dsc.GetId()];

                        entityCurve->SetThickness(curve.Thickness);
                        entityCurve->SetStartPos(curve.Positions.Start);
                        entityCurve->SetEndPos(curve.Positions.End);
                        entityCurve->SetControll1(curve.Positions.Controll1);
                        entityCurve->SetControll2(curve.Positions.Controll2);
                        entityCurve->SetSteps(curve.Steps);
                    }
                }
            }
        }
        else if (e.Type == EntityChangeType::COMPONENT_ADDED)
        {
            Line l;
            Curve c;
            Plane p;
            if (convert::getter(e.Data.Value().get(), &l))
            {
                if (meshes[e.Entity.Handle][Line::_dsc.GetId()])
                {
                    delete meshes[e.Entity.Handle][Line::_dsc.GetId()];
                }
                RLine* newLine = new RLine();
                meshes[e.Entity.Handle][Line::_dsc.GetId()] = newLine;
                newLine->SetStart(l.Positions.Start);
                newLine->SetEnd(l.Positions.End);
                newLine->SetThickness(l.Thickness);
                drawingViewport.lock()->GetScene().Add(newLine);
                currentEditLineEntity = e.Entity.Handle;
                lineEdit->SetLine(newLine);
            }
            else if (convert::getter(e.Data.Value().get(), &c))
            {
                if (meshes[e.Entity.Handle][Curve::_dsc.GetId()])
                {
                    delete meshes[e.Entity.Handle][Curve::_dsc.GetId()];
                }
                RBezierCurve* newCurve = new RBezierCurve();
                meshes[e.Entity.Handle][Curve::_dsc.GetId()] = newCurve;
                newCurve->SetStartPos(c.Positions.Start);
                newCurve->SetEndPos(c.Positions.End);
                newCurve->SetControll1(c.Positions.Controll1);
                newCurve->SetControll2(c.Positions.Controll2);
                newCurve->SetThickness(c.Thickness);
                newCurve->SetSteps(c.Steps);
                drawingViewport.lock()->GetScene().Add(newCurve);
                currentEditCurveEntity = e.Entity.Handle;
                bezierEdit->SetBezierCurve(newCurve);
            }
            else if (convert::getter(e.Data.Value().get(), &p))
            {
                if (meshes[e.Entity.Handle][Plane::_dsc.GetId()])
                {
                    delete meshes[e.Entity.Handle][Plane::_dsc.GetId()];
                }
                RMesh* newPlane = new RMesh();
                meshes[e.Entity.Handle][Plane::_dsc.GetId()] = newPlane;
                newPlane->SetPosition(p.Position);
                newPlane->SetRotation(p.Rotation);
                newPlane->SetScale(p.Scale);
                drawingViewport.lock()->GetScene().Add(newPlane);
            }
        }
        else if (e.Type == EntityChangeType::ENTITY_DESTROYED)
        {
            if (meshes[e.Entity.Handle][Line::_dsc.GetId()])
            {
                delete meshes[e.Entity.Handle][Line::_dsc.GetId()];
            }
            if (meshes[e.Entity.Handle][Curve::_dsc.GetId()])
            {
                delete meshes[e.Entity.Handle][Curve::_dsc.GetId()];
            }
            if (meshes[e.Entity.Handle][Plane::_dsc.GetId()])
            {
                delete meshes[e.Entity.Handle][Plane::_dsc.GetId()];
            }
            meshes.erase(e.Entity.Handle);
        }
        else if (e.Type == EntityChangeType::COMPONENT_REMOVED)
        {
            ComponentChangeData data;
            if (e.Data.Value() && convert::getter(e.Data.Value().get(), &data))
            {
                EString changeType = data.Identifier;
                if (meshes[e.Entity.Handle][changeType])
                {
                    delete meshes[e.Entity.Handle][changeType];
                    meshes[e.Entity.Handle][changeType] = nullptr;
                }
            }
        }
    }, drawingViewport.lock().get());

    
    

    info.PanelRegister->RegisterItem(extensionName, someDrawingPanel);
}

EXT_ENTRY
{
    info.GetComponentRegister().RegisterStruct<TechnicalMesh>(extensionName);
    info.GetComponentRegister().RegisterStruct<Plane>(extensionName);
    info.GetComponentRegister().RegisterStruct<Line>(extensionName);
    info.GetComponentRegister().RegisterStruct<Curve>(extensionName);
}
