#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

EUnorderedMap<EDataBase::Entity, RObject*> fMeshes;

E_STORAGE_STRUCT(Face, 
    (i32, ID),
    (EVector<EVec3>, Points),
    (EVec3, Normal)
)


E_STORAGE_STRUCT(Extrusion, 
    (i32, FaceID),
    (EVec3, ExtrudeDirection, {0.0, 0.0, 1.0}),
    (double, ExtrudeDistance, 1.0)
)

E_STORAGE_STRUCT(ExtrusionBody,
    (Face, StartFace, {0, { {-1.0f, 0.0f, -1.0f}, {-1.0f, 0.0f,  1.0f}, { 1.0f, 0.0f,  1.0f}, {1.0f, 0.0f, -1.0f}}, {0.0f, -1.0f, 0.0f} }),
    (EVector<Extrusion>, Extrusions)
)

typedef EVector<RTestMesh::Vertex> TVertexList;

struct MeshData
{
    TVertexList Vertices;
    
    void Push(const MeshData& data)
    {
        for (RTestMesh::Vertex vert : data.Vertices)
        {
            Vertices.push_back(vert);
        }
    }
};

MeshData FaceToMeshData(const Face& f)
{
    MeshData data;
    for (EVec3 p : f.Points)
    {
        RTestMesh::Vertex vert;
        vert.Position = p;
        vert.Normal = f.Normal;
        data.Vertices.push_back(vert);
    }
    return data;
}

EVector<Face> ExtrudeFace(Face f, Extrusion extrusion)
{
    EVector<Face> result;

    EVec3 centerPoint;
    for (size_t i = 0; i < f.Points.size(); i++)
    {
        centerPoint += f.Points[i];
    }
    centerPoint /= f.Points.size();
    centerPoint += extrusion.ExtrudeDirection * ((float)extrusion.ExtrudeDistance / 2.0f);
    
    for (size_t i = 0; i < f.Points.size(); i++)
    {
        EVec3 currentPoint = f.Points[i];
        EVec3 nextPoint = f.Points[(i + 1) % f.Points.size()];

        Face newFace;
        newFace.ID = f.ID + 1;

        newFace.Points.push_back(currentPoint);
        newFace.Points.push_back(currentPoint + (extrusion.ExtrudeDirection * (float)extrusion.ExtrudeDistance));
        newFace.Points.push_back(nextPoint + (extrusion.ExtrudeDirection * (float)extrusion.ExtrudeDistance));
        newFace.Points.push_back(nextPoint);

        EVec3 currentCenterPoint = (newFace.Points[0] + newFace.Points[1] + newFace.Points[2] + newFace.Points[3]) / 4.0f;
        EVec3 toCenterVector = currentCenterPoint - centerPoint;
        EVec3 possibleNormal = glm::cross(newFace.Points[1] - newFace.Points[0], newFace.Points[2] - newFace.Points[0]);
        float dot = glm::dot(possibleNormal, toCenterVector);
        if (dot < 0.0f)
        {
            newFace.Normal = -possibleNormal;
        }
        else
        {
            newFace.Normal = possibleNormal;
        }


        result.push_back(newFace);
    }


    Face lastFace;
    lastFace.Normal = -f.Normal;
    for (const EVec3& p : f.Points)
    {
        lastFace.Points.push_back(p + (extrusion.ExtrudeDirection * (float)extrusion.ExtrudeDistance));
    }
    result.push_back(lastFace);

    return result;
}

RTestMesh* CreateMeshFromFace(const Face& f)
{
    MeshData data = FaceToMeshData(f);
    RTestMesh* mesh = new RTestMesh();
    mesh->SetData(data.Vertices);
    return mesh;
}

void CreateMeshFromExtrusion(ExtrusionBody body, RObject* renderToOObject)
{
    if (!renderToOObject)
    {
        return;
    }
    renderToOObject->Clear();
    
    renderToOObject->Add(CreateMeshFromFace(body.StartFace));

    Face currentFace = body.StartFace;

    for (const Extrusion& extr : body.Extrusions)
    {
        EVector<Face> newFaces = ExtrudeFace(currentFace, extr);
        if (newFaces.size() == 0) { break; }

        for (const Face& f : newFaces)
        {
            renderToOObject->Add(CreateMeshFromFace(f));
        }

        currentFace = newFaces.back();
    }
}

APP_ENTRY
{
    ERef<EUIPanel> extrusionViewport = EMakeRef<EUIPanel>("Extrusion Viewport");
    ERef<EUIViewport> viewport = EMakeRef<EUIViewport>(RCamera(Renderer::ECameraMode::PERSPECTIVE));
    EWeakRef<EUIViewport> weakViewport = viewport;

    Renderer::RGrid* grid = new Renderer::RGrid(100, 100, 1.0f, 1.0f);
    grid->SetRotation(EVec3(glm::radians(90.0), 0, 0));
    grid->SetPosition(EVec3(-50, 0, -50));
    viewport->GetScene().Add(grid);

    shared::Events().Connect<EntityChangeEvent>([weakViewport](EntityChangeEvent e) {
        if (e.Type == EntityChangeType::COMPONENT_ADDED)
        {

        }
        else if (e.Type == EntityChangeType::COMPONENT_REMOVED)
        {

        }
        else if (e.Type == EntityChangeType::COMPONENT_CHANGED)
        {
            if (e.Data.Value())
            {
                ComponentChangeData changeData;
                if (convert::getter(e.Data.Value().get(), &changeData))
                {
                    ExtrusionBody extrusionBody;
                    if (convert::getter<ExtrusionBody>(changeData.NewValue.Value().get(), &extrusionBody))
                    {
                        RObject* meshContainer = fMeshes[e.Entity.Handle];
                        if (!meshContainer)
                        {
                            meshContainer = new RObject();
                            fMeshes[e.Entity.Handle] = meshContainer;
                            if (!weakViewport.expired())
                            {
                                weakViewport.lock()->GetScene().Add(meshContainer);
                            }
                        }
                        CreateMeshFromExtrusion(extrusionBody, meshContainer);
                    }
                }
            }
        }
    }, viewport.get());


    extrusionViewport->AddChild(viewport);

    info.PanelRegister->RegisterItem(extensionName, extrusionViewport);
}

EXT_ENTRY
{
    info.GetComponentRegister().RegisterStruct<ExtrusionBody>(extensionName);
}