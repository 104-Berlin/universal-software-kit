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
    TRenderFunctionMap oldRenderFunctions = fRenderFunctions;
    fRenderFunctions.clear();

    ESet<EViewportType::opts> viewportTypes;
    for (const EUIViewportRenderFunction& func : fViewportRenderFunctionRegister->GetAllItems())
    {
        fRenderFunctions[func.Description.Type.Value][func.ValueDescription.GetId()] = func;
        viewportTypes.insert(func.Description.Type.Value);
    }

    // Find which we unloaded
    for (const auto& entry : oldRenderFunctions)
    {
        if (fRenderFunctions.find(entry.first) == fRenderFunctions.end())
        {
            // All renderfunctions unloaded in this viewport type
            for (const auto& renderFunc : entry.second)
            {
                for (auto [entity, index] : renderFunc.second.ComponentObjectIndex)
                {
                    EWeakRef<EUIViewport> vp = fViewports[entry.first];
                    if (!vp.expired() && vp.lock())
                    {
                        Renderer::RObject* entityObject = vp.lock()->GetObjectFromEntity(entity);
                        if (entityObject)
                        {
                            entityObject->Clear();
                        }
                    }
                }
            }
            continue;
        }

        for (const auto& renderFunc : entry.second)
        {
            if (fRenderFunctions[entry.first].find(renderFunc.first) == fRenderFunctions[entry.first].end())
            {
                for (auto [entity, index] : renderFunc.second.ComponentObjectIndex)
                {
                    EWeakRef<EUIViewport> vp = fViewports[entry.first];
                    if (!vp.expired() && vp.lock())
                    {
                        Renderer::RObject* entityObject = vp.lock()->GetObjectFromEntity(entity);
                        if (entityObject)
                        {
                            entityObject->Clear();
                        }
                    }
                }
            }
        }
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
        transformRenderFunction.InitViewportTools = [](){
            EVector<EViewportTool*> result;
            result.push_back(new ETransformTool());
            return result;
        };
        transformRenderFunction.ToolFinished = [](events::EViewportToolFinishEvent event, EWeakRef<EUIViewport> viewport){
            if (viewport.expired()) { return; }
            if (event.ToolName == ETransformTool::sGetName())
            {
                for (EViewportTool* tool : viewport.lock()->GetRegisteredTools())
                {
                    if (tool->GetToolName() == event.ToolName)
                    {
                        Editor::ETransform newTransform = ((ETransformTool*)tool)->GetTransform();
                        Renderer::RObject* selectedObject = viewport.lock()->GetSelectionContext().GetSelectedObject();
                        EDataBase::Entity selectedEntity = 0; 
                        if (selectedObject && (selectedEntity = viewport.lock()->GetEntityFromObject(selectedObject)))
                        {
                            shared::SetValue(selectedEntity, "ETransform", newTransform);
                        }
                        break;
                    }
                }
            }
        };
        fRenderFunctions[type][Editor::ETransform::_dsc.GetId()] = transformRenderFunction;

        
        if (fViewports.find(type) == fViewports.end() || fViewports[type].expired())
        {
            fViewports.erase(type);
        }

        // Create the viewport
        if (fViewports.find(type) == fViewports.end())
        {
            ERef<EUIPanel> viewportPanel = EMakeRef<EUIPanel>(EString("Viewport") + EViewportType(type).ToString());
            ERef<EUIViewport> viewport = CreateViewport(type);
            viewportPanel->SetMenuBar(EMakeRef<EUIViewportToolbar>(viewport));
            // Add Grid to viewport
            Renderer::RGrid* grid = new Renderer::RGrid(100, 100, 1.0f, 1.0f);
            grid->SetRotation(EVec3(glm::radians(90.0), 0, 0));
            grid->SetPosition(EVec3(-50, 0, -50));
            viewport->GetScene().Add(grid);

            viewport->AddEventListener<events::EViewportToolFinishEvent>([this, type](events::EViewportToolFinishEvent event){
                ViewportToolFinished(event, type);
            });

            viewportPanel->AddChild(viewport);
            fUIRegister->RegisterItem("CORE-VIEWPORTS", viewportPanel);
            fViewports[type] = viewport;
        }

        ERef<EUIViewport> viewport = fViewports[type].lock();

        // Reload the tools
        viewport->ClearRegisteredTools();
        for (const auto& func : fRenderFunctions[type])
        {
            if (func.second.InitViewportTools)
            {
                EVector<EViewportTool*> tools = func.second.InitViewportTools();
                for (EViewportTool* t : tools)
                {
                    viewport->AddTool(t);
                }
            }
        }

        EVector<EDataBase::Entity> allEntities = shared::GetAllEntites();
        for (EDataBase::Entity entity : allEntities)
        {
            EVector<ERef<EProperty>> properties = shared::GetAllComponents(entity);
            for (ERef<EProperty> property : properties)
            {
                CallRenderFunctionForComponent(entity, property, type);
            }
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

        ERef<EUIViewport> viewport = vp.second.lock();
        Renderer::RScene& scene = viewport->GetScene();
        Renderer::RObject* entityObject = viewport->GetObjectFromEntity(event.Entity.Handle);


        if (event.Type == EntityChangeType::COMPONENT_ADDED || event.Type == EntityChangeType::COMPONENT_CHANGED)
        {
            CallRenderFunctionForComponent(event.Entity.Handle, property, vp.first);
        }
        else if (entityObject && event.Type == EntityChangeType::COMPONENT_REMOVED)
        {
            scene.DeleteObject(entityObject);
        }
    }
}

void EUIViewportManager::ViewportToolFinished(events::EViewportToolFinishEvent event, EViewportType::opts viewportType)
{
    for (const auto& entry : fRenderFunctions[viewportType])
    {
        if (entry.second.ToolFinished)
        {
            entry.second.ToolFinished(event, fViewports[viewportType]);
        }
    }
}

void EUIViewportManager::CallRenderFunctionForComponent(EDataBase::Entity entity, EWeakRef<EProperty> component, EViewportType::opts type)
{
    if (component.expired())
    {
        return;
    }
    if (fViewports.find(type) == fViewports.end() || fViewports.at(type).expired())
    {
        return;
    }

    EValueDescription componentDescription = component.lock()->GetDescription();
    
    if (fRenderFunctions[type].find(componentDescription.GetId()) == fRenderFunctions[type].end())
        return;

    EUIViewportRenderFunction& func = fRenderFunctions[type][componentDescription.GetId()];

    ERef<EUIViewport> viewport = fViewports.at(type).lock();


    Renderer::RScene& scene = viewport->GetScene();
    Renderer::RObject* entityObject = viewport->GetObjectFromEntity(entity);


    if (!entityObject)
    {
        entityObject = new Renderer::RObject();
        scene.Add(entityObject);
        viewport->GetSelectionContext().SetSelectedObject(entityObject);
        viewport->PushToEntityObjectMap(entity, entityObject);
    }

    if (func.NeedsOwnObject)
    {
        if (func.ComponentObjectIndex.find(entity) == func.ComponentObjectIndex.end())
        {
            Renderer::RObject* newObject = new Renderer::RObject();
            entityObject->Add(newObject);
            
            func.ComponentObjectIndex[entity] = entityObject->GetChildCount() - 1;

            
            entityObject = newObject;
        }
        else
        {
            entityObject = entityObject->GetChildAt(func.ComponentObjectIndex[entity]);
        }
    }
    
    func.RenderFunction(entityObject, component.lock());
}
