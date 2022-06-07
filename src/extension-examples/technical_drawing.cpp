#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;


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
    (float, Thickness, 0.1f),
    (LinePositions, Positions)
)

E_STORAGE_STRUCT(Curve,
    (float, Thickness, 0.1f),
    (Editor::ECurveSegment, Position),
    (int, Steps, 10)
)

E_STORAGE_STRUCT(TechnicalBox,
    (EVec3, Size)
)

E_STORAGE_STRUCT(TechnicalMesh,
    (EVector<EVec3>, Positions)
)





APP_ENTRY
{
    EUIViewportRenderFunction technicalBoxRenderFunc;
    technicalBoxRenderFunc.Description.Type = EViewportType::FRONT_RIGHT_TOP_3D;
    technicalBoxRenderFunc.NeedsOwnObject = true;
    technicalBoxRenderFunc.RenderFunction = [](RObject* object, ERef<EProperty> value){
        RMesh* mesh = (RMesh*)object->GetChildAt(0);
        if (!mesh)
        {
            mesh = new RMesh();
            object->Add(mesh);
        }

    };

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
            bezierCurve->SetStartPos(curveData.Position.Start);
            bezierCurve->SetEndPos(curveData.Position.End);
            bezierCurve->SetControll1(curveData.Position.Controll1);
            bezierCurve->SetControll2(curveData.Position.Controll2);
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
    TechnicalBox::_dsc.AddDependsOn(Editor::ETransform::_dsc);

    info.GetComponentRegister().RegisterStruct<TechnicalMesh>(extensionName);
    info.GetComponentRegister().RegisterStruct<Plane>(extensionName);
    info.GetComponentRegister().RegisterStruct<Line>(extensionName);
    info.GetComponentRegister().RegisterStruct<Curve>(extensionName);
}
