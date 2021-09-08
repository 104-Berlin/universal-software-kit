#include "editor_extension.h"

#include "imgui_internal.h"

using namespace Engine;

void intern::InitUI()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

EUIField::EUIField(const EString& label) 
    :   fLabel(label), 
        fID(next_ui_id()), 
        fVisible(true), 
        fDirty(false), 
        fWidthOverride(0),
        fHeightOverride(0),
        fCalculatedSize(),
        fLastMousePos(0.0f, 0.0f)
{
    
}

ERef<EUIField> EUIField::AddChild(const ERef<EUIField>& child) 
{
    fChildren.push_back(child);
    return fChildren.back();
}

void EUIField::Clear() 
{
    fChildren.clear();
}

void EUIField::SetCustomUpdateFunction(UpdateFunction function) 
{
    fCustomUpdateFunction = function;
}

void EUIField::Render() 
{
    if (!fVisible) { return; }

    if (fDirty && fCustomUpdateFunction)
    {
        fCustomUpdateFunction();
    }
    ImGui::PushID(fID);
    if (OnRender())
    {
        for (ERef<EUIField> uiField : fChildren)
        {
            uiField->Render();
        }
    }
    OnRenderEnd();
    ImGui::PopID();
}

bool EUIField::OnRender() 
{
    return true;
}

void EUIField::OnRenderEnd() 
{
    ImGuiContext& g = *Graphics::Wrapper::GetCurrentImGuiContext();
    ImGuiWindow* window = g.CurrentWindow;

    ImRect itemRect = window->DC.LastItemRect;
    
    fCalculatedSize.x = itemRect.GetWidth();
    fCalculatedSize.y = itemRect.GetHeight();

    if (ImGui::IsItemHovered())
    {
        EVec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        mousePos -= EVec2(itemRect.Min.x, itemRect.Min.y);

        ImVec2 md0 = ImGui::GetMouseDragDelta(0, 0.0f);
        EVec2 mouseDrag0(md0.x, md0.y);
        ImGui::ResetMouseDragDelta(0);
        ImVec2 md1 = ImGui::GetMouseDragDelta(1, 0.0f);
        EVec2 mouseDrag1(md1.x, md1.y);
        ImGui::ResetMouseDragDelta(1);
        ImVec2 md2 = ImGui::GetMouseDragDelta(2, 0.0f);
        EVec2 mouseDrag2(md2.x, md2.y);
        ImGui::ResetMouseDragDelta(2);


        EVec2 mouseDelta = mousePos - fLastMousePos;
        fLastMousePos = mousePos;
        if (glm::length(mouseDelta) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseMoveEvent>({mousePos, mouseDelta});
        }

        
        if (ImGui::IsMouseClicked(0))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 0});
        }
        if (ImGui::IsMouseClicked(1))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 1});
        }
        if (ImGui::IsMouseClicked(2))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 2});
        }
        if (glm::length(mouseDrag0) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag0, 0});
        }
        if (glm::length(mouseDrag1) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag1, 1});
        }
        if (glm::length(mouseDrag2) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag2, 2});
        }
    }
    
}

void EUIField::UpdateEventDispatcher() 
{
    OnUpdateEventDispatcher();
    fEventDispatcher.Update();

    for (ERef<EUIField> child : fChildren)
    {
        child->UpdateEventDispatcher();
    }
}

const EString& EUIField::GetLabel() const
{
    return fLabel;    
}

void EUIField::SetSize(float width, float height) 
{
    fWidthOverride = width;
    fHeightOverride = height;
}

void EUIField::SetSize(const EVec2& size) 
{
    fWidthOverride = size.x;
    fHeightOverride = size.y;
}

void EUIField::SetWidth(float width) 
{
    fWidthOverride = width;
}

void EUIField::SetHeight(float height) 
{
    fHeightOverride = height;
}

EVec2 EUIField::GetSize() const
{
    return fCalculatedSize;
}

float EUIField::GetWidth() const
{
    return fCalculatedSize.x;
}

float EUIField::GetHeight() const
{
    return fCalculatedSize.y;
}

EUIPanel::EUIPanel(const EString& title) 
    : EUIField(title), fOpen(true)
{
    
}

bool EUIPanel::OnRender() 
{
    if (fOpen)
    {
        ImGui::Begin(GetLabel().c_str(), &fOpen);
        if (!fOpen) 
        {
            fWasJustClosed = true;
        }
    }
    return fOpen;
}

void EUIPanel::OnRenderEnd() 
{
    if (fOpen || fWasJustClosed)
    {
        fWasJustClosed = false;
        ImGui::End();
    }
}

bool EUIPanel::IsOpen() const
{
    return fOpen;
}

void EUIPanel::Close() 
{
    fOpen = false;
}

void EUIPanel::Open() 
{
    fOpen = true;
}

EUIViewport::EUIViewport(const Renderer::RCamera& camera) 
    :   EUIField("VIEWPORT"), 
        fFrameBuffer(Graphics::Wrapper::CreateFrameBuffer(100, 100)), 
        fRenderer(Graphics::Wrapper::GetMainContext(), fFrameBuffer),
        fCamera(camera)
{
    
}

EUIViewport::~EUIViewport() 
{
    if (fFrameBuffer)
    {
        delete fFrameBuffer;
    }
}

Graphics::GScene& EUIViewport::GetScene() 
{
    return fScene;
}

const Graphics::GScene& EUIViewport::GetScene() const
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

bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fRenderer.Render(&fScene, &fCamera);

    ImGui::Image((ImTextureID)(unsigned long long)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});

    return true;
}


EUIButton::EUIButton(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIButton::OnRender() 
{
    if (ImGui::Button(GetLabel().c_str(), {fWidthOverride, fHeightOverride}))
    {
        fEventDispatcher.Enqueue<events::EButtonEvent>(events::EButtonEvent());
    }
    return true;
}

EUITextField::EUITextField(const EString& label, const EString& content)
    : EUIField(label), fContent(content)
{
    
}

bool EUITextField::OnRender() 
{
    char text[255];
    memcpy(text, fContent.c_str(), 255);
    if (ImGui::InputText(GetLabel().c_str(), text, 255, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        fContent = text;
        fEventDispatcher.Enqueue<ETextChangeEvent>({fContent});
    }
    return true;
}

// ----------------------------------------
// Main Menu Bar
EUIMainMenuBar::EUIMainMenuBar() 
    : EUIField("MainMenuBar")
{

}

bool EUIMainMenuBar::OnRender() 
{
    return fOpen = ImGui::BeginMainMenuBar();
}

void EUIMainMenuBar::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndMainMenuBar();
    }
}


// ----------------------------------------
// Menu
EUIMenu::EUIMenu(const EString& displayName) 
    : EUIField(displayName), fOpen(false)
{
    
}

bool EUIMenu::OnRender() 
{
    return fOpen = ImGui::BeginMenu(GetLabel().c_str());
}

void EUIMenu::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndMenu();
    }
    
}


// ----------------------------------------
// Context Menu
EUIContextMenu::EUIContextMenu(const EString& displayName) 
    : EUIField(displayName), fOpen(false)
{
    
}

bool EUIContextMenu::OnRender() 
{
    return fOpen = ImGui::BeginPopupContextWindow();
}

void EUIContextMenu::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndPopup();
    }
}

// ----------------------------------------
// Menu Item
EUIMenuItem::EUIMenuItem(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIMenuItem::OnRender() 
{
    if (ImGui::MenuItem(GetLabel().c_str()))
    {
        fEventDispatcher.Enqueue<events::EButtonEvent>(events::EButtonEvent());
    }
    return true;
}

EUIImageView::EUIImageView() 
    : EUIField("ImageView")
{
    fTexture = Graphics::Wrapper::CreateTexture();
}

EUIImageView::~EUIImageView() 
{
    delete fTexture;
}

void EUIImageView::SetTextureData(u8* data, size_t width, size_t height) 
{
    fTexture->SetData(data, width, height);
}

bool EUIImageView::OnRender() 
{
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (fWidthOverride != 0)
    {
        size.x = fWidthOverride;
    }
    if (fHeightOverride != 0)
    {
        size.y = fHeightOverride;
    }
    
    ImGui::Image((ImTextureID)(u64)fTexture->GetID(), size);
    return true;
}
