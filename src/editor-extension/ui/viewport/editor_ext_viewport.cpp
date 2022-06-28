#include "editor_extension.h"
 
#include "imgui_internal.h"

using namespace Engine;

EUIViewport::EUIViewport(const Renderer::RCamera& camera) 
    :   EUIField("VIEWPORT"), 
        fActiveTool(nullptr),
        fViewType(ViewType::DIFFUSE),
        fFrameBuffer(Graphics::Wrapper::CreateFrameBuffer(100, 100)), 
        fRenderer(Graphics::Wrapper::GetMainContext(), fFrameBuffer),
        fCamera(camera),
        fCameraControls(new EUIBasic3DCameraControls(&fCamera))
{
    SetWidth(100);
    SetHeight(100);

    AddEventListener<events::EMouseDragEvent>([this](events::EMouseDragEvent event){
        if (this->fCameraControls && !ImGuizmo::IsUsing())
        {
            this->fCameraControls->OnMouseDrag(event);
        }
    });
    AddEventListener<events::EMouseScrollEvent>([this](events::EMouseScrollEvent event){
        if (this->fCameraControls && !ImGuizmo::IsUsing())
        {
            this->fCameraControls->OnMouseScroll(event);
        }
    });
    AddEventListener<events::EKeyDownEvent>([this](events::EKeyDownEvent event){
        if (this->fCameraControls && !ImGuizmo::IsUsing())
        {
            this->fCameraControls->OnKeyDown(event);
        }
    });
    AddEventListener<events::EKeyUpEvent>([this](events::EKeyUpEvent event){
        if (this->fCameraControls && !ImGuizmo::IsUsing())
        {
            this->fCameraControls->OnKeyUp(event);
        }
    });
}

EUIViewport::~EUIViewport() 
{
    ClearRegisteredTools();

    if (fFrameBuffer)
    {
        delete fFrameBuffer;
    }
}

Renderer::RScene& EUIViewport::GetScene() 
{
    return fScene;
}

const Renderer::RScene& EUIViewport::GetScene() const
{
    return fScene;
}

const Renderer::RCamera& EUIViewport::GetCamera() const
{
    return fCamera;
}

Renderer::RCamera& EUIViewport::GetCamera() 
{
    return fCamera;
}

EViewportTool* EUIViewport::AddTool(EViewportTool* newTool) 
{
    if (fActiveTool == nullptr)
    {
        fActiveTool = newTool;
    }
    fRegisteredTools.push_back(newTool);
    newTool->SetViewport(this);
    return newTool;
}

void EUIViewport::SetViewType(ViewType type)
{
    fViewType = type;
}


EVector<EViewportTool*> EUIViewport::GetRegisteredTools()
{
    return fRegisteredTools;
}

void EUIViewport::ClearRegisteredTools()
{
    for (EViewportTool* tool : fRegisteredTools)
    {
        delete tool;
    }
    fRegisteredTools.clear();
    fActiveTool = nullptr;
}

EViewportTool* EUIViewport::GetActiveTool()
{
    return fActiveTool;
}

void EUIViewport::SetActiveTool(const EString& toolName)
{
    for (EViewportTool* tool : fRegisteredTools)
    {
        if (tool->GetToolName() == toolName)
        {
            fActiveTool = tool;
            return;
        }
    }
    E_WARN("Could not find tool " + toolName);
}


bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImVec2(fWidthOverride, fHeightOverride);
    
    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fRenderer.Render(&fScene, &fCamera);
    u32 textureId = fFrameBuffer->GetColorAttachment();
    switch (fViewType)
    {
        case ViewType::DIFFUSE: textureId = fFrameBuffer->GetColorAttachment(); break;
        case ViewType::NORMAL: textureId = fFrameBuffer->GetNormalAttachment(); break;
        case ViewType::DEPTH: textureId = fFrameBuffer->GetDepthAttachment(); break;
    }

    // Show the render result
    ImGui::Image((ImTextureID)(unsigned long long)(unsigned long)textureId, {contentRegion.x, contentRegion.y}, {0, 1}, {1, 0});

    // Prepare Imguizmo
    ImGuiContext& g = *Graphics::Wrapper::GetCurrentImGuiContext();
    ImRect itemRect = g.LastItemData.Rect;
    

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetID(1);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(itemRect.GetTL().x, itemRect.GetTL().y, itemRect.GetWidth(), itemRect.GetHeight());



    if (fActiveTool && fActiveTool->IsVisible())
    {
        if (fActiveTool->Render())
        {
            fActiveTool->Finish();
            //fEventDispatcher.Enqueue<events::EViewportToolFinishEvent>({fActiveTool->GetToolName()});
        }
    }


    return true;
}

EVec2 EUIViewport::Project(const EVec3& point) const
{
    EVec4 result = fCamera.GetProjectionMatrix(fFrameBuffer->GetWidth(), fFrameBuffer->GetHeight()) * fCamera.GetViewMatrix() * EVec4(point.x, point.y, point.z, 1.0f);
    return {((result.x + 1.0f) / 2.0f) * fFrameBuffer->GetWidth(), ((-result.y + 1.0f) / 2.0f) * fFrameBuffer->GetHeight()};
}

EVec3 EUIViewport::Unproject(const EVec3& point) const
{
    EVec4 result = glm::inverse(fCamera.GetProjectionMatrix(fFrameBuffer->GetWidth(), fFrameBuffer->GetHeight()) * fCamera.GetViewMatrix()) * EVec4(point.x / fFrameBuffer->GetWidth() * 2.0f - 1.0f, -(point.y / fFrameBuffer->GetHeight() * 2.0f - 1.0f), point.z, 1.0f);
    return {result.x, result.y, result.z};
}

float EUIViewport::GetWidth() const
{
    return fFrameBuffer->GetWidth();
}

float EUIViewport::GetHeight() const
{
    return fFrameBuffer->GetHeight();
}

const ESelectionContext& EUIViewport::GetSelectionContext() const
{
    return fSelectionCtx;
}

ESelectionContext& EUIViewport::GetSelectionContext()
{
    return fSelectionCtx;
}

void EUIViewport::PushToEntityObjectMap(EDataBase::Entity entity, Renderer::RObject* object)
{
    if (fEntityObjectMap.find(entity) != fEntityObjectMap.end())
    {
        E_WARN("Try pushing... Entity " + std::to_string(entity) + " already exists in the map");
        return;
    }
    fEntityObjectMap[entity] = object;
    fObjectEntityMap[object] = entity;
}

void EUIViewport::RemoveFromEntityObjectMap(EDataBase::Entity entity)
{
    if (fEntityObjectMap.find(entity) == fEntityObjectMap.end())
    {
        E_WARN("Try removing... Entity " + std::to_string(entity) + " does not exist in the map");
        return;
    }
    fObjectEntityMap.erase(fEntityObjectMap[entity]);
    fEntityObjectMap.erase(entity);
}

void EUIViewport::RemoveFromEntityObjectMap(Renderer::RObject* object)
{
    if (fObjectEntityMap.find(object) == fObjectEntityMap.end())
    {
        E_WARN("Try removing... Object does not exist in the map");
        return;
    }
    fEntityObjectMap.erase(fObjectEntityMap[object]);
    fObjectEntityMap.erase(object);
}

EDataBase::Entity EUIViewport::GetEntityFromObject(Renderer::RObject* object) const
{
    auto it = fObjectEntityMap.find(object);
    if (it == fObjectEntityMap.end())
    {
        E_WARN("Try getting... Object does not exist in the map");
        return 0;
    }
    return it->second;
}

Renderer::RObject* EUIViewport::GetObjectFromEntity(EDataBase::Entity entity) const
{
    auto it = fEntityObjectMap.find(entity);
    if (it == fEntityObjectMap.end())
    {
        E_WARN("Try getting... Entity " + std::to_string(entity) + " does not exist in the map");
        return nullptr;
    }
    return it->second;
}


EUIViewportToolbar::EUIViewportToolbar(EWeakRef<EUIViewport> viewport) 
    : EUIField("Viewport " + viewport.lock()->GetLabel()), fViewport(viewport)
{
    SetDirty();
    SetCustomUpdateFunction([this](){
        Regenerate();
    });
}

bool EUIViewportToolbar::OnRender() 
{
    return true;
}

void EUIViewportToolbar::Regenerate() 
{
    fChildren.clear();
    if (fViewport.expired()) { return; }
    EWeakRef<EUIField> grid = AddChild(EMakeRef<EUIGrid>());
    
    for (EViewportTool* tool : fViewport.lock()->GetRegisteredTools())
    {
        EString toolName = tool->GetToolName();
        EWeakRef<EUISelectable> tool_Selectable = std::dynamic_pointer_cast<EUISelectable>(grid.lock()->AddChild(EMakeRef<EUISelectable>(tool->GetIcon())).lock());
        tool_Selectable.lock()->SetStateControllFunction([this, toolName]()->bool{
            EViewportTool* activeTool = this->fViewport.lock()->GetActiveTool();
            if (!activeTool) { return false; }
            return activeTool->GetToolName() == toolName;
        });
        tool_Selectable.lock()->AddEventListener<events::ESelectableChangeEvent>([this, toolName](events::ESelectableChangeEvent event){
            // Activate tool
            fViewport.lock()->SetActiveTool(toolName);
        });
    }
}
