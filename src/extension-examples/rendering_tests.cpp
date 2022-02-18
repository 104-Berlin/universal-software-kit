#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

E_STORAGE_STRUCT(MeshComponent, 
    (EResourceLink, Mesh, "Mesh")
)

EUnorderedMap<EDataBase::Entity, RMesh*> fMeshes;


APP_ENTRY
{
    RCamera camera(ECameraMode::PERSPECTIVE);
    camera.SetPosition({10, 10, -10});

    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    ERef<EUIViewport> viewport = EMakeRef<EUIViewport>(camera);
    EWeakRef<EUIViewport> weakViewport = viewport;

    showPanel->AddChild(viewport);
    info.PanelRegister->RegisterItem(extensionName, showPanel);

    shared::Events().Connect<EntityChangeEvent>([weakViewport](EntityChangeEvent event){
        if (event.Type == EntityChangeType::COMPONENT_ADDED)
        {
            if (event.Data.Value())
            {
                MeshComponent meshComponent;
                if (convert::getter<MeshComponent>(event.Data.Value().get(), &meshComponent))
                {
                    RMesh* mesh = new RMesh();
                    fMeshes[event.Entity.Handle] = mesh;
                    weakViewport.lock()->GetScene().Add(mesh);
                    auto meshResource = shared::GetResource(meshComponent.Mesh.ResourceId);
                    
                    if (meshResource)
                    {
                        Editor::EMeshResource* meshData = meshResource->GetCPtr<Editor::EMeshResource>();
                        mesh->SetData(meshData->Vertices, meshData->Indices);
                    }
                }
            }
        }
        else if (event.Type == EntityChangeType::COMPONENT_CHANGED)
        {
            if (event.Data.Value())
            {
                ComponentChangeData changeData;
                if (convert::getter(event.Data.Value().get(), &changeData))
                {
                    MeshComponent meshComponent;
                    if (convert::getter<MeshComponent>(changeData.NewValue.Value().get(), &meshComponent))
                    {
                        RMesh* mesh = fMeshes[event.Entity.Handle];
                        if (!mesh) { return; }

                        auto meshResource = shared::GetResource(meshComponent.Mesh.ResourceId);
                        
                        if (meshResource)
                        {
                            Editor::EMeshResource* meshData = meshResource->GetCPtr<Editor::EMeshResource>();
                            mesh->SetData(meshData->Vertices, meshData->Indices);
                        }
                        else
                        {
                            mesh->SetData({}, {});
                        }
                    }
                }
            }
        }
        else if (event.Type == EntityChangeType::COMPONENT_REMOVED)
        {
            weakViewport.lock()->GetScene().DeleteObject(fMeshes[event.Entity.Handle]);
        }
    }, viewport.get());
}

EXT_ENTRY
{
   info.GetComponentRegister().RegisterStruct<MeshComponent>(extensionName);
}
