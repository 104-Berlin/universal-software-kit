#include "editor_extension.h"

using namespace Engine;
using namespace Graphics;
using namespace Renderer;

E_STORAGE_STRUCT(MeshComponent, 
    (EResourceLink, Mesh, "Mesh")
)

EUnorderedMap<EDataBase::Entity, RMesh*> fMeshes;

EDataBase::Entity currentTransformObject = 0;

APP_ENTRY
{
    RCamera camera(ECameraMode::PERSPECTIVE);
    camera.SetPosition({0, 0, -100});
    //camera.LookAt({0, 0, 0});

    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    ERef<EUIViewport> viewport = EMakeRef<EUIViewport>(camera);

    viewport->GetTransformControls().SetOnChange([](Editor::ETransform transform) {
        shared::SetValue(currentTransformObject, "ETransform", transform);
    });

    EWeakRef<EUIViewport> weakViewport = viewport;

    ERef<EUIDropdown> viewTypeDropdown = EMakeRef<EUIDropdown>("View Type");
    viewTypeDropdown->AddOption("Diffuse");
    viewTypeDropdown->AddOption("Normal");
    viewTypeDropdown->AddOption("Depth");
    viewTypeDropdown->AddEventListener<events::ESelectChangeEvent>([weakViewport](events::ESelectChangeEvent e){
        if (e.Index == 0)
        {
            weakViewport.lock()->SetViewType(EUIViewport::ViewType::DIFFUSE);
        }
        else if (e.Index == 1)
        {
            weakViewport.lock()->SetViewType(EUIViewport::ViewType::NORMAL);
        }
        else if (e.Index == 2)
        {
            weakViewport.lock()->SetViewType(EUIViewport::ViewType::DEPTH);
        }
    });

    ERef<EUICheckbox> cameraMode = EMakeRef<EUICheckbox>("Perspective Camera");
    cameraMode->SetValue(true);
    cameraMode->AddEventListener<events::ECheckboxEvent>([weakViewport](events::ECheckboxEvent e){
        if (e.Checked)
        {
            weakViewport.lock()->GetCamera().SetMode(ECameraMode::PERSPECTIVE);
        }
        else
        {
            weakViewport.lock()->GetCamera().SetMode(ECameraMode::ORTHOGRAPHIC);
        }
    });

    showPanel->AddChild(viewTypeDropdown);
    showPanel->AddChild(EMakeRef<EUISameLine>());
    showPanel->AddChild(cameraMode);
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
                    weakViewport.lock()->GetTransformControls().SetAttachedObject(mesh);
                    currentTransformObject = event.Entity.Handle;
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
                    RMesh* mesh = fMeshes[event.Entity.Handle];
                    if (!mesh) { return; }

                    MeshComponent meshComponent;
                    Editor::ETransform transform;
                    if (convert::getter<MeshComponent>(changeData.NewValue.Value().get(), &meshComponent))
                    {
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
                    else if (convert::getter<Editor::ETransform>(changeData.NewValue.Value().get(), &transform))
                    {
                        mesh->SetPosition(transform.Position);
                        mesh->SetRotation(transform.Rotation);
                        mesh->SetScale(transform.Scale);
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
    MeshComponent::_dsc.AddDependsOn(Editor::ETransform::_dsc);
    info.GetComponentRegister().RegisterStruct<MeshComponent>(extensionName);
}
