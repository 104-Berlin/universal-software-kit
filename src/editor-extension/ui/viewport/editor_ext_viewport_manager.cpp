#include "editor_extension.h"

using namespace Engine;

EUIViewportManager::EUIViewportManager(EUIRegister* uiRegister, EViewportRenderFunctionRegister* renderFunctionRegister)
    : fUIRegister(uiRegister), fViewportRenderFunctionRegister(renderFunctionRegister)
{
   shared::Events().Connect<EntityChangeEvent>([this](EntityChangeEvent event){
       HandleEntityChange(event);
   });
}

void EUIViewportManager::ReloadViewports()
{
    fRenderFunctions.clear();
    ESet<EViewportType::opts> viewportTypes;
    for (const EUIViewportRenderFunction& func : fViewportRenderFunctionRegister->GetAllItems())
    {
        fRenderFunctions[func.Description.Type.Value][func.ValueDescription.GetId()] = func;
        viewportTypes.insert(func.Description.Type.Value);
    }

    for (const EViewportType::opts& type : viewportTypes)
    {
        // Add default transform function
        EUIViewportRenderFunction transformRenderFunction;
        transformRenderFunction.Description.Type = type;
        transformRenderFunction.Description.ExtensionName = "CORE_VIEWPORTS";
        transformRenderFunction.Description.Name = "Transform";
        transformRenderFunction.ValueDescription = Editor::ETransform::_dsc;
        transformRenderFunction.NeedsOwnObject = false;
        transformRenderFunction.RenderFunction = [](Renderer::RObject* object, ERef<EProperty> property){
            Editor::ETransform transform;
            if (property->GetValue<Editor::ETransform>(&transform))
            {
                object->SetPosition(transform.Position);
                object->SetRotation(transform.Rotation);
                object->SetScale(transform.Scale);
            }
        };
        fRenderFunctions[type][Editor::ETransform::_dsc.GetId()] = transformRenderFunction;

        // Create the viewport
        if (fViewports.find(type) == fViewports.end())
        {
            ERef<EUIPanel> viewportPanel = EMakeRef<EUIPanel>(EString("Viewport") + EViewportType(type).ToString());
            ERef<EUIViewport> viewport = CreateViewport(type);
            // Add Grid to viewport
            Renderer::RGid* grid = new Renderer::RGid(100, 100, 1.0f, 1.0f);
            grid->SetRotation(EVec3(glm::radians(90.0), 0, 0));
            grid->SetPosition(EVec3(-50, 0, -50));
            viewport->GetScene().Add(grid);

            viewportPanel->AddChild(viewport);
            fUIRegister->RegisterItem("CORE-VIEWPORTS", viewportPanel);
            fViewports[type] = viewport;
        }
        else if (fViewports[type].expired())
        {
            fViewports.erase(type);
        }
    }
}


ERef<EUIViewport> EUIViewportManager::CreateViewport(const EViewportType& type) const
{
    switch (type.Value)
    {
    case EViewportType::DEFAULT_3D:
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::PERSPECTIVE));
    case EViewportType::DEFAULT_2D:
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::ORTHOGRAPHIC));
    case EViewportType::FRONT_RIGHT_TOP_3D:
    {
        return EMakeRef<EUIViewport>(Renderer::RCamera(Renderer::ECameraMode::PERSPECTIVE));
    }
    }
    return nullptr;
}

void EUIViewportManager::HandleEntityChange(EntityChangeEvent event)
{
    if (!event.Data.Value())
    {
        return;
    }
    
    ERef<EProperty> property = event.Data.Value();
    EValueDescription componentDescription = property->GetDescription();

    if (event.Type == EntityChangeType::COMPONENT_CHANGED)
    {
        ComponentChangeData changeData;
        if (property->GetValue(&changeData))
        {
            property = changeData.NewValue.Value();
            componentDescription = property->GetDescription();
        }
    }

    for (const auto& vp : fViewports)
    {
        if (vp.second.expired())
            continue;

        if (fRenderFunctions[vp.first].find(componentDescription.GetId()) == fRenderFunctions[vp.first].end())
            continue;

        EUIViewportRenderFunction func = fRenderFunctions[vp.first][componentDescription.GetId()];

        ERef<EUIViewport> viewport = vp.second.lock();


        Renderer::RScene& scene = viewport->GetScene();
        Renderer::RObject* entityObject = viewport->GetObjectFromEntity(event.Entity.Handle);


        if (!entityObject)
        {
            entityObject = new Renderer::RObject();
            scene.Add(entityObject);
            viewport->PushToEntityObjectMap(event.Entity.Handle, entityObject);
        }

        if (event.Type == EntityChangeType::COMPONENT_ADDED || event.Type == EntityChangeType::COMPONENT_CHANGED)
        {
            if (func.NeedsOwnObject)
            {
                if (func.ComponentObjectIndex.find(componentDescription.GetId()) == func.ComponentObjectIndex.end())
                {
                    Renderer::RObject* newObject = new Renderer::RObject();
                    entityObject->Add(newObject);
                    entityObject = newObject;
                    func.ComponentObjectIndex[componentDescription.GetId()] = entityObject->GetChildren().size() - 1;
                }
                else
                {
                    entityObject = entityObject->GetChildAt(func.ComponentObjectIndex[componentDescription.GetId()]);
                }
            }
            
            func.RenderFunction(entityObject, property);
        }
        else if (event.Type == EntityChangeType::COMPONENT_REMOVED)
        {
            scene.DeleteObject(entityObject);
        }
    }
}
