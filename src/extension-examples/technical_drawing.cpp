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
    (EVec2, Size)
)

E_STORAGE_STRUCT(LinePositions,
    (EVec3, Start, 200.0f, 200.0f, 0.0f),
    (EVec3, End, 200.0f, 500.0f, 0.0f)
)

E_STORAGE_STRUCT(Line, 
    (float, Thickness, 8.0f),
    (LinePositions, Positions)
)

E_STORAGE_STRUCT(Curve,
    (float, Thickness, 5.0f),
    (Editor::ECurveSegment, Positions),
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
            Editor::ECurveSegment segment;
            segment.Start = editTool->GetCurve()->GetStartPos();
            segment.End = editTool->GetCurve()->GetEndPos();
            segment.Controll1 = editTool->GetCurve()->GetControll1();
            segment.Controll2 = editTool->GetCurve()->GetControll2();
            shared::SetValue<Editor::ECurveSegment>(currentEditCurveEntity, "Curve.Position", segment);
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


    EUIViewportRenderFunction curveRenderFunc;
    curveRenderFunc.Description.Type = EViewportType::FRONT_RIGHT_TOP_3D;
    curveRenderFunc.NeedsOwnObject = true;
    curveRenderFunc.ValueDescription = Curve::_dsc;
    curveRenderFunc.RenderFunction = [](RObject* object, ERef<EProperty> value){
        RBezierCurve* bezierCurve = (RBezierCurve*)object->GetChildAt(0);
        if (!bezierCurve)
        {
            bezierCurve = new RBezierCurve();
            object->Add(bezierCurve);
        }
        Curve curveData;
        if (value->GetValue(&curveData))
        {
            bezierCurve->SetStartPos(curveData.Positions.Start);
            bezierCurve->SetEndPos(curveData.Positions.End);
            bezierCurve->SetControll1(curveData.Positions.Controll1);
            bezierCurve->SetControll2(curveData.Positions.Controll2);
            bezierCurve->SetThickness(curveData.Thickness);
            bezierCurve->SetSteps(curveData.Steps);
        }
    };
    curveRenderFunc.InitViewportTools = []() -> EVector<EViewportTool*> {
        EBezierEditTool* bezierTool = new EBezierEditTool();
        bezierTool->SetComponentIdentifier("Curve.Position");
        return {bezierTool};
    };
    
    EUIViewportRenderFunction planeRenderFunc;
    planeRenderFunc.Description.Type = EViewportType::FRONT_RIGHT_TOP_3D;
    planeRenderFunc.NeedsOwnObject = true;
    planeRenderFunc.ValueDescription = Plane::_dsc;
    planeRenderFunc.RenderFunction = [](RObject* object, ERef<EProperty> value){
        RMesh* planeMesh = (RMesh*) object->GetChildAt(0);
        if (!planeMesh)
        {
            planeMesh = new RMesh();
            object->Add(planeMesh);
        }
        Plane plane;
        if (value->GetValue(&plane))
        {
            float halfWidth = plane.Size.x / 2.0f;
            float halfHeight = plane.Size.y / 2.0f;
            EVector<RMesh::Vertex> vertices = {
                {{-halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}},
                {{halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}},
                {{halfWidth,  halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}},
                {{-halfWidth,  halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f}}};
            EVector<u32> indices = {
                0, 1, 2,
                2, 3, 0
            };
            planeMesh->SetData(vertices, indices);
        }
    };

    info.ViewportRenderFunctions->RegisterItem(extensionName, planeRenderFunc);
    info.ViewportRenderFunctions->RegisterItem(extensionName, curveRenderFunc);
}

EXT_ENTRY
{
    Plane::_dsc.AddDependsOn(Editor::ETransform::_dsc);

    info.GetComponentRegister().RegisterStruct<TechnicalMesh>(extensionName);
    info.GetComponentRegister().RegisterStruct<Plane>(extensionName);
    info.GetComponentRegister().RegisterStruct<Line>(extensionName);
    info.GetComponentRegister().RegisterStruct<Curve>(extensionName);
}
