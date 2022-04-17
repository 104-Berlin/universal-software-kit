#include "editor_extension.h"
 
#include "imgui_internal.h"

using namespace Engine;

EUIViewport::EUIViewport(const Renderer::RCamera& camera) 
    :   EUIField("VIEWPORT"), 
        fTransformControls(this),
        fActiveTool(nullptr),
        fViewType(ViewType::DIFFUSE),
        fFrameBuffer(Graphics::Wrapper::CreateFrameBuffer(100, 100)), 
        fRenderer(Graphics::Wrapper::GetMainContext(), fFrameBuffer),
        fCamera(camera),
        fCameraControls(new EUIBasic3DCameraControls(&fCamera))
{
    fTransformControls.SetVisible(true);

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
    for (EViewportTool* tool : fRegisteredTools)
    {
        delete tool;
    }
    fRegisteredTools.clear();

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
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fRenderer.Render(&fScene, &fCamera);
    u32 textureId = fFrameBuffer->GetColorAttachment();
    switch (fViewType)
    {
        case ViewType::DIFFUSE: textureId = fFrameBuffer->GetColorAttachment(); break;
        case ViewType::NORMAL: textureId = fFrameBuffer->GetNormalAttachment(); break;
        case ViewType::DEPTH: textureId = fFrameBuffer->GetDepthAttachment(); break;
    }

    ImGui::Image((ImTextureID)(unsigned long long)(unsigned long)textureId, {contentRegion.x, contentRegion.y}, {0, 1}, {1, 0});

    if (fActiveTool && fActiveTool->IsVisible())
    {
        if (fActiveTool->Render())
        {
            fEventDispatcher.Enqueue<events::EViewportToolFinishEvent>({fActiveTool->GetToolName()});
        }
    }

    ImGuiContext& g = *Graphics::Wrapper::GetCurrentImGuiContext();
    ImRect itemRect = g.LastItemData.Rect;
    

    ImGuizmo::SetOrthographic(fCamera.GetMode() == Renderer::ECameraMode::ORTHOGRAPHIC);
    ImGuizmo::SetID(1);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(itemRect.GetTL().x, itemRect.GetTL().y, itemRect.GetWidth(), itemRect.GetHeight());
    fTransformControls.OnRender();


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

EUIViewportTransformControls& EUIViewport::GetTransformControls()
{
    return fTransformControls;
}

const EUIViewportTransformControls& EUIViewport::GetTransformControls() const
{
    return fTransformControls;
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


EUIViewportTransformControls::EUIViewportTransformControls(EUIViewport* viewport)
    : fViewport(viewport), fAttachedObject(nullptr), fVisible(false), fWasUsing(false)
{

}

void EUIViewportTransformControls::OnRender()
{
    if (!fVisible) { return; }
    if (!fAttachedObject) { return; }

    EMat4 viewMatrix = fViewport->GetCamera().GetViewMatrix();
    EMat4 projectionMatrix = fViewport->GetCamera().GetProjectionMatrix(fViewport->GetWidth(), fViewport->GetHeight());
    EMat4 transformMatrix = fAttachedObject->GetModelMatrix();
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(transformMatrix));
    
    if (fWasUsing && !ImGuizmo::IsUsing())
    {
        fWasUsing = false;
        if (fOnChange)
        {
            fOnChange(Editor::ETransform(fLastPosition, fLastRotation, fLastScale));
        }
    }

    if (ImGuizmo::IsUsing())
    {
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformMatrix), glm::value_ptr(fLastPosition), glm::value_ptr(fLastRotation), glm::value_ptr(fLastScale));
        fLastRotation = EVec3(glm::radians(fLastRotation.x), glm::radians(fLastRotation.y), glm::radians(fLastRotation.z));


        fAttachedObject->SetPosition(fLastPosition);
        fAttachedObject->SetRotation(fLastRotation);
        
        fAttachedObject->SetScale(fLastScale);
        fWasUsing = true;
    }
}

void EUIViewportTransformControls::SetAttachedObject(Renderer::RObject* object)
{
    fAttachedObject = object;
}

void EUIViewportTransformControls::SetVisible(bool visible)
{
    fVisible = visible;
}

bool EUIViewportTransformControls::IsVisible() const
{
    return fVisible;
}


void EUIViewportTransformControls::SetOnChange(TransformUpdateFunction func)
{
    fOnChange = func;
}
