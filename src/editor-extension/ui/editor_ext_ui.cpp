#include "editor_extension.h"

#include "imgui_internal.h"

using namespace Engine;

void intern::InitUI()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

EUIField::EUIField(const EString& label) 
    : fLabel(label), fID(next_ui_id()), fVisible(true), fDirty(false), fLastMousePos(0.0f, 0.0f)
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
    if (ImGui::IsItemHovered())
    {
        ImGuiContext& g = *Graphics::Wrapper::GetCurrentImGuiContext();
        ImGuiWindow* window = g.CurrentWindow;

        ImRect itemRect = window->DC.LastItemRect;


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

EUIViewport::EUIViewport() 
    : EUIField("VIEWPORT")
{
    fFrameBuffer = Graphics::Wrapper::CreateFrameBuffer();
}

EUIViewport::~EUIViewport() 
{
    delete fFrameBuffer;   
}

void EUIViewport::SetRenderFunction(RenderFunction renderFunction) 
{
    fRenderFuntion = renderFunction;
}

Graphics::GScene& EUIViewport::GetScene() 
{
    return fScene;
}

const Graphics::GScene& EUIViewport::GetScene() const
{
    return fScene;
}

bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fFrameBuffer->Bind();
    if (fRenderFuntion)
    {
        fRenderFuntion(Graphics::Wrapper::GetMainContext(), fFrameBuffer);
    }
    fFrameBuffer->Unbind();

    ImGui::Image((ImTextureID)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});

    return true;
}


EUIButton::EUIButton(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIButton::OnRender() 
{
    if (ImGui::Button(GetLabel().c_str()))
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
